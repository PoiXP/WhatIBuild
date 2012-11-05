#include <stdio.h>
#include "Build.h"
#include "BuildParser.h"
#include "ReturnCode.h"


int main(int ArgCount, char** Args)
{
  if (ArgCount < 2)
  {
    return -1;
  }
  WhatIBuild::Build build;
  int returnCode = WhatIBuild::Parser::ParseVSSolution(Args[1], build);
  if (returnCode != WhatIBuild::e_Return_OK)
  {
    return returnCode;
  }

  size_t moduleCount = build.GetModulesCount();
  for (size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx)
  {
    const WhatIBuild::Module& module = build.GetModule(moduleIdx);
    size_t unitCount = module.GetUnitsCount();
    printf("%s:%d\n", module.GetName(), unitCount);
    for (size_t unitIdx = 0; unitIdx < unitCount; ++unitIdx)
    {
      printf("%s\n", module.GetUnit(unitIdx).GetPath());
    }
  }

  return WhatIBuild::e_Return_OK;
}