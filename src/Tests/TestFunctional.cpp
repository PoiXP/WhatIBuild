#include <UnitTest++\src\UnitTest++.h>
#include "Build.h"
#include "BuildParser.h"
#include "ReturnCode.h"

using namespace WhatIBuild;

SUITE(Test_ParseMSVCSolution)
{
  TEST(testFileNotFound)
  {
    Build build;
    CHECK_EQUAL(e_Return_FileNotFound,  Parser::ParseVSSolution("invlid_path", build));
  }
  TEST(testFunctional)
  {
    Build build;
    CHECK_EQUAL(e_Return_OK,  Parser::ParseVSSolution("..\\src\\Tests\\TestData\\Test.sln", build));
    CHECK_EQUAL(2, build.GetModulesCount());
    {
      const Module& module = build.GetModule(0u);
      CHECK_EQUAL("ProjectA", module.GetName());
      CHECK_EQUAL("ProjectA\\ProjectA.vcxproj", module.GetPath());
      CHECK_EQUAL(6, module.GetUnitsCount());
      // CHECK_EQUAL("src\\CutsomBuilt1.file", module.GetUnit(0).GetFileName());
      CHECK_EQUAL("ExcludedFile.cpp",  module.GetUnit(0).GetFileName());
      CHECK_EQUAL("File1.cpp",         module.GetUnit(1).GetFileName());
      CHECK_EQUAL("File2.cpp",         module.GetUnit(2).GetFileName());
      CHECK_EQUAL("common.h",          module.GetUnit(3).GetFileName());
      CHECK_EQUAL("File1.h",           module.GetUnit(4).GetFileName());
      CHECK_EQUAL("File2.h",           module.GetUnit(5).GetFileName());

      CHECK_EQUAL(2, module.GetProperty(Module::e_ProjectConfigurations).GetCount());
      {
        const Property& prop = module.GetProperty(Module::e_ProjectConfigurations).GetProperty(0);
        CHECK_EQUAL("ProjectConfiguration", prop.GetName());
        CHECK_EQUAL("Debug|Win32", prop.GetValue());
        CHECK_EQUAL(2, prop.GetCount());
      }
      {
        const Property& prop = module.GetProperty(Module::e_ProjectConfigurations).GetProperty(1);
        CHECK_EQUAL("ProjectConfiguration", prop.GetName());
        CHECK_EQUAL("Release|Win32", prop.GetValue());
        CHECK_EQUAL(2, prop.GetCount());
      }
      CHECK_EQUAL(1, module.GetProperty(Module::e_Globals).GetCount());
      {
        const Property& prop = module.GetProperty(Module::e_Globals).GetProperty(0);
        CHECK_EQUAL("Globals", prop.GetName());
        CHECK_EQUAL("", prop.GetValue());
        CHECK_EQUAL(3, prop.GetCount());
        {
          struct Props
          {
            char name[64];
            char value[64];
          } props[] =
          {
            {"ProjectGuid",       "{06D38399-0152-4053-B8A5-228AE55CC1BC}" },
            {"Keyword",           "Win32Proj" },
            {"RootNamespace",     "ProjectA" },
          };
          const size_t count = sizeof(props)/sizeof(props[0]);
          CHECK_EQUAL(count, prop.GetCount());
          for (size_t i = 0; i < count; ++i)
          {
            CHECK_EQUAL(props[i].name, prop.GetProperty(i).GetName());
            CHECK_EQUAL(props[i].value, prop.GetProperty(i).GetValue());
          }
        }
      }
      CHECK_EQUAL(2, module.GetProperty(Module::e_ConfigurationSettings).GetCount());
      {
        const Property& prop = module.GetProperty(Module::e_ConfigurationSettings).GetProperty(0);
        CHECK_EQUAL("Condition", prop.GetName());
        CHECK_EQUAL("'$(Configuration)|$(Platform)'=='Debug|Win32'", prop.GetValue());
        CHECK_EQUAL(2, prop.GetCount());
        {
          const Property& compileProps = prop.GetProperty(0);
          CHECK_EQUAL("ClCompile", compileProps.GetName());
          CHECK_EQUAL("", compileProps.GetValue());
          struct Props
          {
            char name[64];
            char value[64];
          } props[] =
          {
            {"PrecompiledHeader",       "" },
            {"WarningLevel",            "Level3" },
            {"Optimization",            "Disabled" },
            {"PreprocessorDefinitions", "WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)" },
          };
          const size_t count = sizeof(props)/sizeof(props[0]);
          CHECK_EQUAL(count, compileProps.GetCount());
          for (size_t i = 0; i < count; ++i)
          {
            CHECK_EQUAL(props[i].name, compileProps.GetProperty(i).GetName());
            CHECK_EQUAL(props[i].value, compileProps.GetProperty(i).GetValue());
          }
        }
        {
          const Property& linkerProps = prop.GetProperty(1);
          CHECK_EQUAL("Link", linkerProps.GetName());
          CHECK_EQUAL("", linkerProps.GetValue());
          struct Props
          {
            char name[64];
            char value[64];
          } props[] =
          {
            {"SubSystem",               "Console" },
            {"GenerateDebugInformation","true" },
          };
          const size_t count = sizeof(props)/sizeof(props[0]);
          CHECK_EQUAL(count, linkerProps.GetCount());
          for (size_t i = 0; i < count; ++i)
          {
            CHECK_EQUAL(props[i].name, linkerProps.GetProperty(i).GetName());
            CHECK_EQUAL(props[i].value, linkerProps.GetProperty(i).GetValue());
          }
        }
      }

      {
        const Property& prop = module.GetProperty(Module::e_ConfigurationSettings).GetProperty(1);
        CHECK_EQUAL("Condition", prop.GetName());
        CHECK_EQUAL("'$(Configuration)|$(Platform)'=='Release|Win32'", prop.GetValue());
        CHECK_EQUAL(2, prop.GetCount());
        {
          const Property& compileProps = prop.GetProperty(0);
          CHECK_EQUAL("ClCompile", compileProps.GetName());
          CHECK_EQUAL("", compileProps.GetValue());
          struct Props
          {
            char name[64];
            char value[64];
          } props[] =
          {
            {"WarningLevel",            "Level3" },
            {"PrecompiledHeader",       "" },
            {"Optimization",            "MaxSpeed" },
            {"FunctionLevelLinking",    "true" },
            {"IntrinsicFunctions",      "true" },
            {"PreprocessorDefinitions", "WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)" },
          };
          const size_t count = sizeof(props)/sizeof(props[0]);
          CHECK_EQUAL(count, compileProps.GetCount());
          for (size_t i = 0; i < count; ++i)
          {
            CHECK_EQUAL(props[i].name, compileProps.GetProperty(i).GetName());
            CHECK_EQUAL(props[i].value, compileProps.GetProperty(i).GetValue());
          }
        }
        {
          const Property& linkerProps = prop.GetProperty(1);
          CHECK_EQUAL("Link", linkerProps.GetName());
          CHECK_EQUAL("", linkerProps.GetValue());
          struct Props
          {
            char name[64];
            char value[64];
          } props[] =
          {
            {"SubSystem",               "Console" },
            {"GenerateDebugInformation","true" },
            {"EnableCOMDATFolding",     "true" },
            {"OptimizeReferences",      "true" },
          };
          const size_t count = sizeof(props)/sizeof(props[0]);
          CHECK_EQUAL(count, linkerProps.GetCount());
          for (size_t i = 0; i < count; ++i)
          {
            CHECK_EQUAL(props[i].name, linkerProps.GetProperty(i).GetName());
            CHECK_EQUAL(props[i].value, linkerProps.GetProperty(i).GetValue());
          }
        }
      }
      CHECK_EQUAL(2, module.GetProperty(Module::e_PropertySheets).GetCount());
      {
        const Property& prop = module.GetProperty(Module::e_PropertySheets).GetProperty(0);
        CHECK_EQUAL("PropertySheets", prop.GetName());
        CHECK_EQUAL("'$(Configuration)|$(Platform)'=='Debug|Win32'", prop.GetValue());
        CHECK_EQUAL(1, prop.GetCount());
        CHECK_EQUAL("Import", prop.GetProperty(0).GetName());
        CHECK_EQUAL("$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props", prop.GetProperty(0).GetValue());
      }
      {
        const Property& prop = module.GetProperty(Module::e_PropertySheets).GetProperty(1);
        CHECK_EQUAL("PropertySheets", prop.GetName());
        CHECK_EQUAL("'$(Configuration)|$(Platform)'=='Release|Win32'", prop.GetValue());
        CHECK_EQUAL(1, prop.GetCount());
        CHECK_EQUAL("Import", prop.GetProperty(0).GetName());
        CHECK_EQUAL("$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props", prop.GetProperty(0).GetValue());
      }
    }

    CHECK_EQUAL(2, build.GetModulesCount());
    {
      const Module& module = build.GetModule(1u);
      CHECK_EQUAL("ProjectB", module.GetName());
      CHECK_EQUAL("ProjectB\\ProjectB.vcxproj", module.GetPath());
      CHECK_EQUAL(5, module.GetUnitsCount());
      CHECK_EQUAL("File1.cpp",  module.GetUnit(0).GetFileName());
      CHECK_EQUAL("File2.cpp",  module.GetUnit(1).GetFileName());
      CHECK_EQUAL("common.h",   module.GetUnit(2).GetFileName());
      CHECK_EQUAL("File1.h",    module.GetUnit(3).GetFileName());
      CHECK_EQUAL("File2.h",    module.GetUnit(4).GetFileName());
    }

  }
}
