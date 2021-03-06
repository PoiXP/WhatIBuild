#include "Build.h"
#include "Logging.h"
#include <cassert>

namespace WhatIBuild
{
//-----------------------------------------------------------------------------
Unit::Unit(const std::string& filename, const std::string& path)
  : m_Filename(filename)
  , m_Path(path)
{

}

Unit::~Unit()
{

}
//-----------------------------------------------------------------------------
Property::Property()
{
}

Property::Property(const std::string& name, const std::string& value)
  : m_Name(name)
  , m_Value(value)
{
}

Property::~Property()
{

}
//-----------------------------------------------------------------------------

const size_t Module::INVALID_INDEX = std::numeric_limits<size_t>::max();

Module::Module(const std::string& name, const std::string& path)
  : m_Name(name)
  , m_Path(path)
{

}

Module::~Module()
{

}

size_t Module::Lookup(const std::string& path) const
{
  auto it = m_Lookup.find(path);
  if (it != m_Lookup.end())
  {
    return it->second;
  }
  return INVALID_INDEX;
}

void Module::AddUnit(const Unit& unit)
{
 if (Lookup(unit.GetPath()) != INVALID_INDEX)
 {
   char buffer[1024];
   sprintf_s(buffer, sizeof(buffer)-1, "Name collision. Unit with name %s already exists", unit.GetPath());
   WIBLOG_ERROR(buffer);
 }

  m_Lookup[unit.GetPath()] = m_Units.size();
  m_Units.push_back(unit);
}

void Module::AddProperty(Module::PropertyEnum propertyId, const Property& property)
{
  m_Properties[propertyId].AddProperty(property);
}

//-----------------------------------------------------------------------------
const size_t Build::INVALID_INDEX = std::numeric_limits<size_t>::max();

Build::Build()
{

}

Build::~Build()
{

}

size_t Build::Lookup(const std::string& name) const
{
  auto it = m_Lookup.find(name);
  if (it != m_Lookup.end())
  {
    return it->second;
  }
  return INVALID_INDEX;
}

void Build::AddModule(const Module& module)
{
  assert(Lookup(module.GetName()) == INVALID_INDEX);

  m_Lookup[module.GetName()] = m_Modules.size();
  m_Modules.push_back(module);
}

//-----------------------------------------------------------------------------
}