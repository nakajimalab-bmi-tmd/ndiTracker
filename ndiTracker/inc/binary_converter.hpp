#ifndef __ml_conversion_binary_converter_hpp
#define __ml_conversion_binary_converter_hpp

#include <vector>

namespace device {

  template<template <typename T, typename A = std::allocator<T>> class Container = std::vector>
  class to_binary
  {
  public:
    typedef typename Container<char> container_type;

    template <typename ... types>
    static container_type convert(types ... others)
    {
      to_binary converter;
      converter.to_binary_impl(others...);
      return converter.res;
    }

  private:
    template<typename T, typename ... types>
    typename std::enable_if<(sizeof ...(types) > 0)>::type to_binary_impl(T val, types ... others)
    {
      char* pointer = reinterpret_cast<char*>(&val);
      res.insert(res.end(), pointer, pointer + sizeof(T));
      to_binary_impl(others...);
    }

    template<typename T>
    void to_binary_impl(T val)
    {
      char* pointer = reinterpret_cast<char*>(&val);
      res.insert(res.end(), pointer, pointer + sizeof(T));
    }

    container_type res;
  };

  class from_binary
  {
  public:
    template<template <typename T, typename A = std::allocator<T>> class Container = std::vector, typename ... types>
    static std::tuple<types...> convert(Container<char> const& binary_data)
    {
      return exec<types...>(binary_data.begin());
    }

    template <typename InputIterator, typename ... types>
    static std::tuple<types...> convert(InputIterator& first)
    {
      return exec<types...>(first);
    }

  private:
    template <typename T, typename InputIterator, typename ... types>
    static typename std::enable_if<(sizeof...(types) > 0), std::tuple<T, types...>>::type 
      exec(InputIterator& first)
    {
      T val;
      std::copy(first, first + sizeof(T), reinterpret_cast<char*>(&val));
      first += sizeof(T);
      return std::tuple_cat(std::tuple<T>(std::move(val)), exec<types...>(first));
    }

    template <typename T, typename InputIterator>
    static std::tuple<T> exec(InputIterator& first)
    {
      T val;
      std::copy(first, first + sizeof(T), reinterpret_cast<char*>(&val));
      first += sizeof(T);
      return std::tuple<T>(std::move(val));
    }
  };

  template<typename T, typename InputIterator>
  T binary_cast(InputIterator first)
  {
    return std::get<0>(typename from_binary::convert<InputIterator, T>(first));
  }
}


#endif
