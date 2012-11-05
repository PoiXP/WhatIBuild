#ifndef WIB_RETURN_CODE_H
#define WIB_RETURN_CODE_H
#pragma once

namespace WhatIBuild
{
  enum ReturnCodeEnum
  {
    e_Return_OK = 0,
    e_Return_FileNotFound,
    e_Return_ParseError,
    e_Return_NotSupportedVersion,
  };
}


#endif // WIB_RETURN_CODE_H