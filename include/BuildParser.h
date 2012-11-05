#ifndef WIB_BUILD_PARSER_H
#define WIB_BUILD_PARSER_H
#pragma once

namespace WhatIBuild
{
  class Build;
  class Parser
  {
  public:
    static int ParseVSSolution(const char* path, Build& build);
  };
}


#endif // WIB_BUILD_PARSER_H