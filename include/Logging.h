#ifndef WIB_LOGGING_H
#define WIB_LOGGING_H
#pragma once

namespace WhatIBuild
{
  typedef void (LogFunc)(const char*);

  struct LoggingCallbacks
  {
    LogFunc* logInfo;
    LogFunc* logWarning;
    LogFunc* logError;
  };
  class LogManager
  {
  public:
    static void Inititalize(const LoggingCallbacks& callbacks);
    static void Info(const char* text);
    static void Warning(const char* text);
    static void Error(const char* text);
  };
}

#define WIBLOG_INFO(text) WhatIBuild::LogManager::Info( (text) );
#define WIBLOG_WARNING(text) WhatIBuild::LogManager::Warning( (text) );
#define WIBLOG_ERROR(text) WhatIBuild::LogManager::Error( (text) );

#endif // WIB_LOGGING_H