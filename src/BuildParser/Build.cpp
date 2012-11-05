#include "Build.h"
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
   //TODO write a log message
 }

  m_Lookup[unit.GetPath()] = m_Units.size();
  m_Units.push_back(unit);
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