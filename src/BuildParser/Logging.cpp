#include "Logging.h"
#include <iostream>

static void Info_Fallback(const char* text)
{
  std::cout << "INFO: " << text << std::endl;
}

static void Warning_Fallback(const char* text)
{
  std::cout << "WARNING: " << text << std::endl;
}

static void Error_Fallback(const char* text)
{
  std::cout << "ERROR: " << text << std::endl;
}

static WhatIBuild::LoggingCallbacks g_LogCallbacks = 
{
  &Info_Fallback,
  &Warning_Fallback,
  &Error_Fallback
};

void WhatIBuild::LogManager::Inititalize(const LoggingCallbacks& callbacks)
{
  g_LogCallbacks = callbacks;
}

void WhatIBuild::LogManager::Info(const char* text)
{
  (*g_LogCallbacks.logInfo)(text);
}

void WhatIBuild::LogManager::Warning(const char* text)
{
  (*g_LogCallbacks.logWarning)(text);
}

void WhatIBuild::LogManager::Error(const char* text)
{
  (*g_LogCallbacks.logError)(text);
}
