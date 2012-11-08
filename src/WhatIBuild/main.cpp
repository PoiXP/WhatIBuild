#include "Build.h"
#include "BuildParser.h"
#include "ReturnCode.h"
#include "Logging.h"

#include <cstdarg>
#include <iostream>
#include <boost/program_options.hpp>

class Output
{
public:
  virtual ~Output() {}
  virtual void Print(const char* format, ...) = 0;
};

class LogOutput : public Output
{
public:
  void Print(const char* format, ...)
  {
    char line[1024];
    va_list args;
    va_start(args, format);
    vsprintf_s(line, sizeof(line), format, args);
    va_end(args);
    WIBLOG_INFO(line);
  }
};

class FileOutput : public Output
{
public:
  FileOutput() {}
  ~FileOutput()
  { 
    fclose(m_File);
  }
  bool Open(const char* filename)
  {
    fopen_s(&m_File, filename, "w");
    return m_File != 0;
  }
  void Print(const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    vfprintf_s(m_File, format, args);
    va_end(args);
    fprintf(m_File, "\n");
  }
private:
  FILE* m_File;
};

static Output* g_Output = NULL;
#define OUTPUT(...) g_Output->Print(##__VA_ARGS__);


namespace po = boost::program_options;

enum ReturnCode
{
  e_OK = 0,
  e_InvalidParameters,
  e_CantFindFile,
};

void OutputPropertyContent(const WhatIBuild::Property& property, int indent = 1)
{
  char format[64];
  sprintf_s(format, sizeof(format), "%%%ds%%s:%%s", 2*indent);
  size_t propertiesCount = property.GetCount();
  for (size_t index = 0; index < propertiesCount; ++index)
  {
    const WhatIBuild::Property& child = property.GetProperty(index);
    OUTPUT(format, "",child.GetName(), child.GetValue());
    OutputPropertyContent(child, indent + 1);
  }
}


int main(int ArgCount, char** Args)
{
  std::string reportType;
  std::string outputFile;
  std::string configurationSection;
  std::string solutionPath;

  po::options_description options("Allowed options");
  options.add_options()
    ("help", "produce help message")
    ("solution", po::value<std::string>(&solutionPath), "Path to Visual Studio solution file.")
    ("out", po::value<std::string>(&outputFile), "Output file name")
    ("do", po::value<std::string>(&reportType), "Type of report: print_files, print_configuration")
    ("section", po::value<std::string>(&configurationSection)->default_value("all"), "For --do=print_configuration a section to print: all, configurations, globals, settings, property_sheets");

  po::positional_options_description positionOptions;
  positionOptions.add("solution", 1);

  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(ArgCount, Args). options(options).positional(positionOptions).run(), vm);
    po::notify(vm);
  }
  catch(po::error& genericError)
  {
    WIBLOG_ERROR(genericError.what());
    return WhatIBuild::e_Return_ParseError;
  }

  if (vm.count("help"))
  {
    std::cout << options << "\n";
    return WhatIBuild::e_Return_OK;
  }

  if (vm.count("out"))
  {
    FileOutput* output = new FileOutput();
    if (output->Open(vm["out"].as<std::string>().c_str()))
    {
      g_Output = output;
    }
    else
    {
      delete output;
      WIBLOG_ERROR("Can't open open file.")
      return WhatIBuild::e_Return_FileNotFound;
    }
  }
  else
  {
    g_Output = new LogOutput;
  }



  WhatIBuild::Build build;
  int returnCode = WhatIBuild::Parser::ParseVSSolution(solutionPath.c_str(), build);
  if (returnCode != WhatIBuild::e_Return_OK)
  {
    return returnCode;
  }
  if (reportType == "print_files")
  {
    size_t moduleCount = build.GetModulesCount();
    for (size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx)
    {
      const WhatIBuild::Module& module = build.GetModule(moduleIdx);
      size_t unitCount = module.GetUnitsCount();
      OUTPUT("%s:%d", module.GetName(), unitCount);
      for (size_t unitIdx = 0; unitIdx < unitCount; ++unitIdx)
      {
        OUTPUT("%s", module.GetUnit(unitIdx).GetPath());
      }
    }
  }
  else if(reportType == "print_configuration")
  {
    struct SectionsMapping
    {
      char  name[32];
      int   sections;
    } static Mapping[] =
    {
      {"all",             -1},
      {"configurations",  WhatIBuild::Module::e_ProjectConfigurations},
      {"globals",         WhatIBuild::Module::e_Globals},
      {"settings",        WhatIBuild::Module::e_ConfigurationSettings},
      {"property_sheets", WhatIBuild::Module::e_PropertySheets}
    };
    static int MAPPING_COUNT = sizeof(Mapping) / sizeof(Mapping[0]);

    bool found = false;
    int sectionsToPrint;
    for (int index = 0; index < MAPPING_COUNT; ++index)
    {
      if (configurationSection == Mapping[index].name)
      {
        sectionsToPrint = Mapping[index].sections;
        found = true;
        break;
      }
    }
    if (!found)
    {
      WIBLOG_ERROR("Unknown configuration name");
      return WhatIBuild::e_Return_ParseError;
    }

    size_t moduleCount = build.GetModulesCount();
    for (size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx)
    {
      const WhatIBuild::Module& module = build.GetModule(moduleIdx);
      OUTPUT("%s", module.GetName());
      for (int i = 0; i < WhatIBuild::Module::e_PropertiesCount; ++i)
      {
        if (sectionsToPrint == -1 || sectionsToPrint == i)
        {
          OutputPropertyContent(module.GetProperty(static_cast<WhatIBuild::Module::PropertyEnum>(i)) );
        }
      }
    }
  }
  else
  {
    WIBLOG_ERROR("Unknown report type");
    return WhatIBuild::e_Return_ParseError;
  }

  return WhatIBuild::e_Return_OK;
}