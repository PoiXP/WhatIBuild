#ifndef WIB_BUILD_H
#define WIB_BUILD_H
#pragma once

#include <vector>
#include <map>
#include <string>

namespace WhatIBuild
{
class Unit
{
public:
  Unit(const std::string& filename, const std::string& path);
  ~Unit();
  const char*  GetFileName() const  { return m_Filename.c_str(); }
  const char*  GetPath() const         { return m_Path.c_str(); }
private:
  std::string m_Filename;
  std::string m_Path;
};

class Module
{
public:
  static const size_t INVALID_INDEX;

  Module(const std::string& name, const std::string& path);
  ~Module();

  const char* GetName() const             { return m_Name.c_str(); }
  const char* GetPath() const             { return m_Path.c_str(); }
  const Unit& GetUnit(size_t index) const { return m_Units[index]; }
  size_t      GetUnitsCount() const       { return m_Units.size(); }
  size_t      Lookup(const std::string& path) const;
  void        AddUnit(const Unit& unit);
private:
  std::vector<Unit> m_Units;
  std::map<std::string, size_t> m_Lookup;
  std::string       m_Name;
  std::string       m_Path;

};

class Build
{
public:
  static const size_t INVALID_INDEX;

  Build();
  ~Build();
  const Module& GetModule(size_t index) const { return m_Modules[index]; }
  size_t        GetModulesCount() const       { return m_Modules.size(); }
  size_t        Lookup(const std::string& name) const;
  void          AddModule(const Module& module);
private:
  std::vector<Module> m_Modules;
  std::map<std::string, size_t> m_Lookup;
};

}

#endif // WIB_BUILD_H