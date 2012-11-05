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
