#pragma once

#include <iostream>
#include <type_traits>

enum class IManagerHandler
{
  RESOURCE_MANAGER
};

template <typename T>
std::ostream &operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type &stream, const T &e)
{
  return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

class IManager
{
public:
  virtual ~IManager(){};

  virtual IManagerHandler getHandler() const = 0;

  virtual bool init() = 0;

  virtual bool destroy() = 0;
};
