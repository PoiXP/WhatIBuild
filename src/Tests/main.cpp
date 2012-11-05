#include <UnitTest++\src\UnitTest++.h>
#include "Logging.h"

static void NoLog(const char* text) { }

int main()
{
  WhatIBuild::LoggingCallbacks callbacks = {&NoLog, &NoLog, &NoLog};
  WhatIBuild::LogManager::Inititalize(callbacks);

  try
  {
    return UnitTest::RunAllTests();
  }
  catch( ... )
  {
    return -1;
  }
}