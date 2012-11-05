#include "BuildParser.h"
#include "Build.h"
#include "ReturnCode.h"
#include "Logging.h"
#include "File.h"

#include <string>
#include <tinyxml/tinyxml.h>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
//-----------------------------------------------------------------------------

namespace
{
class VSProjectParser
{
public:
  VSProjectParser(const char* path);
  WhatIBuild::ReturnCodeEnum Parse(const std::string& solutionPath, WhatIBuild::Module& module);
private:
  std::string m_Path;

};

VSProjectParser::VSProjectParser(const char* path)
  : m_Path(path)
{

}

WhatIBuild::ReturnCodeEnum VSProjectParser::Parse(const std::string& solutionPath, WhatIBuild::Module& module)
{
  boost::filesystem::path projectPath = boost::filesystem::path(solutionPath).parent_path();
  projectPath /= module.GetPath();

  boost::filesystem::path directory = projectPath.parent_path();

  if (!boost::filesystem::exists(projectPath))
  {
    WIBLOG_ERROR( (boost::format("Project file: \"%s\" not found") % projectPath.string()).str().c_str() );
    return WhatIBuild::e_Return_FileNotFound;
  }
  TiXmlDocument doc( projectPath.string().c_str() );
  if (!doc.LoadFile())
  {
    WIBLOG_ERROR( (boost::format("Not able to parse XML: %s") % projectPath.string()).str().c_str() );
    return WhatIBuild::e_Return_ParseError;
  }
  TiXmlElement* project = doc.FirstChildElement("Project");
  if (project == NULL)
  {
    return WhatIBuild::e_Return_ParseError;
  }

  TiXmlNode* itemGroup = project->FirstChild("ItemGroup");
  while (itemGroup)
  {
    TiXmlNode* node = itemGroup->FirstChild("ClCompile");
    while (node)
    {
      TiXmlElement* element = node->ToElement();
      if (element && element->Attribute("Include"))
      {
        boost::filesystem::path filePath = element->Attribute("Include");
        boost::filesystem::path completePath = directory;
        completePath /= filePath;
        WhatIBuild::Unit unit(filePath.filename().string(), completePath.string());
        module.AddUnit(unit);
      }
      node = itemGroup->IterateChildren(node);
    }
    node = itemGroup->FirstChild("ClInclude");
    while (node)
    {
      TiXmlElement* element = node->ToElement();
      if (element && element->Attribute("Include"))
      {
        boost::filesystem::path filePath = element->Attribute("Include");
        boost::filesystem::path completePath = directory;
        completePath /= filePath;
        WhatIBuild::Unit unit(filePath.filename().string(), completePath.string());
        module.AddUnit(unit);
      }
      node = itemGroup->IterateChildren(node);
    }
    itemGroup = project->IterateChildren(itemGroup);
  }

  return WhatIBuild::e_Return_OK;
}

//-----------------------------------------------------------------------------
class VSSolutionParser
{
public:
  VSSolutionParser(const char* path);
  WhatIBuild::ReturnCodeEnum Parse(WhatIBuild::Build& build);
private:
  static const int SUPPORTED_VERSION = 2010;
  WhatIBuild::ReturnCodeEnum ParseInternal(WhatIBuild::Build& build);
  WhatIBuild::ReturnCodeEnum ParseProject(std::string& name, std::string& path);
  const std::string& GetCurrentLine() const;
  void ReadNextLine();

  std::string m_Path;
  std::string m_CurrentLine;
  File        m_File;
};

VSSolutionParser::VSSolutionParser(const char* path)
  : m_Path(path)
{

}

const std::string& VSSolutionParser::GetCurrentLine() const
{
  return m_CurrentLine;
}

void VSSolutionParser::ReadNextLine()
{
  if (!m_File.EndOfFile())
  {
    m_File.ReadLine(m_CurrentLine);
  }
}


WhatIBuild::ReturnCodeEnum VSSolutionParser::Parse(WhatIBuild::Build& build)
{
  if (m_File.Open(m_Path.c_str()))
  {
    WIBLOG_INFO( (boost::format("Parsing solution: %s") % m_Path).str().c_str() );
    return ParseInternal(build);
  }
  else
  {
    WIBLOG_ERROR( (boost::format("Solution file not found on path: %s") % m_Path).str().c_str() );
    return WhatIBuild::e_Return_FileNotFound;
  }
}

WhatIBuild::ReturnCodeEnum VSSolutionParser::ParseInternal(WhatIBuild::Build& build)
{
  enum ParseStateEnum
  {
    e_Parse_FileVersion,
    e_Parse_ProjectSection,
    e_Parse_GlobalSection,
    e_Parse_Error,
  };

  WhatIBuild::ReturnCodeEnum resultCode = WhatIBuild::e_Return_OK;
  ParseStateEnum parseState = e_Parse_FileVersion;
  std::string line;
  while(!m_File.EndOfFile() && parseState != e_Parse_Error)
  {
    switch(parseState)
    {
    case e_Parse_FileVersion:
      {
        int version;
        ReadNextLine();
        if (sscanf_s(GetCurrentLine().c_str(), "# Visual Studio %d", &version) == 1)
        {
          if (version == SUPPORTED_VERSION)
          {
            WIBLOG_INFO( (boost::format("Solution file version: %u is supported") % version).str().c_str() );
            ReadNextLine();
            parseState = e_Parse_ProjectSection;
          }
          else
          {
            WIBLOG_ERROR( (boost::format("Not supported solution version: %u, expected %u") % version % SUPPORTED_VERSION).str().c_str() );
            parseState = e_Parse_Error;
            resultCode = WhatIBuild::e_Return_NotSupportedVersion;
          }
        }
      }
      break;
    case e_Parse_ProjectSection:
      {
        if(GetCurrentLine() != "Global")
        {
          std::string name, path;
          if (ParseProject(name, path) == WhatIBuild::e_Return_OK)
          {
            // skip solution folders
            boost::filesystem::path filename(path);
            if (!filename.has_extension())
            {
              WIBLOG_INFO( (boost::format("Found solution folder: %s") % name).str().c_str() );
              continue;
            }
            WIBLOG_INFO( (boost::format("Parsing project: %s on path: %s") % name % m_Path).str().c_str() );

            WhatIBuild::Module module(name, path);
            VSProjectParser projectParser(path.c_str());
            WhatIBuild::ReturnCodeEnum returnCode = projectParser.Parse(m_Path, module);
            if (returnCode == WhatIBuild::e_Return_OK)
            {
              WIBLOG_INFO( (boost::format("%u file(s) found") % module.GetUnitsCount()).str().c_str() );
              build.AddModule(module);
            }
            else
            {
              WIBLOG_ERROR( (boost::format("Project parsing error: %d") % returnCode).str().c_str() );
              parseState = e_Parse_Error;
              resultCode = returnCode;
            }
          }
          else
          {
            WIBLOG_ERROR( (boost::format("Failed parsing solution file on line: %s") % GetCurrentLine()).str().c_str() );
            parseState = e_Parse_Error;
            resultCode = WhatIBuild::e_Return_ParseError;
          }
        }
        else
        {
          WIBLOG_INFO( (boost::format("Solution parsed! %u project(s) found") % build.GetModulesCount()).str().c_str() );
          parseState = e_Parse_GlobalSection;
        }
      }
      break;
    case e_Parse_GlobalSection:
      {
        // Not implemented
        ReadNextLine();
      }
      break;
    };
  }

  m_File.Close();
  return resultCode;
}

WhatIBuild::ReturnCodeEnum VSSolutionParser::ParseProject(std::string& name, std::string& path)
{
  const char reProjectDefinition[] = "^Project[^=]*=[ ]*\"([^\"]*)\"[ ]*,[ ]*\"([^\"]*)\".*$";
  boost::regex projectDefinition(reProjectDefinition);
  boost::match_results<std::string::const_iterator> query;

  if (regex_match(GetCurrentLine(), query, projectDefinition))
  {
    name = std::string(query[1].first, query[1].second);
    path = std::string(query[2].first, query[2].second);
  }
  else
  {
    WIBLOG_ERROR( (boost::format("Not valid project definition: %s") % GetCurrentLine()).str().c_str() );
    WhatIBuild::e_Return_ParseError;
  }

  while(!m_File.EndOfFile())
  {
    ReadNextLine();
    if (GetCurrentLine() == "EndProject")
    {
      ReadNextLine();
      return WhatIBuild::e_Return_OK;
    }
  }
  return WhatIBuild::e_Return_ParseError;
}


}

//-----------------------------------------------------------------------------
namespace WhatIBuild
{

int Parser::ParseVSSolution(const char* path, Build& build)
{
  VSSolutionParser parser(path);
  return parser.Parse(build);
}

}