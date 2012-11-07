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
  const char*  GetPath() const      { return m_Path.c_str(); }
private:
  std::string m_Filename;
  std::string m_Path;
};

class Property
{
public:
  Property();
  Property(const std::string& name, const std::string& value);
  ~Property();

  const char*       GetName()   const { return m_Name.c_str(); }
  const char*       GetValue() const  { return m_Value.c_str(); }
  size_t            GetCount() const  { return m_Properties.size(); }
  const Property&   GetProperty(size_t index) const
  {
    return m_Properties[index];
  }
  void        AddProperty(const Property& property)
  {
    m_Properties.push_back(property);
  }

private:
  std::string           m_Name;
  std::string           m_Value;
  std::vector<Property> m_Properties;
};

/// Module contains information about project
class Module
{
public:
  /// Project properties
	enum PropertyEnum
  {
    e_ProjectConfigurations,  /// ItemGroup Label="ProjectConfigurations"
    e_Globals,                /// PropertyGroup Label="Globals"
    e_ConfigurationSettings,  /// PropertyGroup Condition="..." Label="Configuration"
    e_PropertySheets,         /// ImportGroup Label="PropertySheets" Condition="..."
    e_PropertiesCount
  };

  static const size_t INVALID_INDEX;

  Module(const std::string& name, const std::string& path);
  ~Module();

  const char*     GetName() const             { return m_Name.c_str(); }
  const char*     GetPath() const             { return m_Path.c_str(); }
  const Unit&     GetUnit(size_t index) const { return m_Units[index]; }
  size_t          GetUnitsCount() const       { return m_Units.size(); }
  const Property& GetProperty(PropertyEnum propertyId) const
  { 
    return m_Properties[propertyId];
  }
  size_t          Lookup(const std::string& path) const;
  void            AddUnit(const Unit& unit);
  void            AddProperty(PropertyEnum propertyId, const Property& property);
private:
  std::vector<Unit> m_Units;
  std::map<std::string, size_t> m_Lookup;
  Property          m_Properties[e_PropertiesCount];
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