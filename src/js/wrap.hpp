#include <emscripten/bind.h>
#include <type_traits>
#include <unordered_map>
#include <vector>

#ifndef WRAPJS_H
#define WRAPJS_H

template <typename T, typename V, typename Hash, typename KeyEqual>
emscripten::val
unordered_map_to_js(const std::unordered_map<V, T, Hash, KeyEqual> &map) {
  static_assert(std::is_arithmetic_v<T>, "T must be a numeric type");

  emscripten::val jsMap = emscripten::val::object();

  for (const auto &[key, value] : map) {
    jsMap.set(key, value);
  }

  return jsMap;
}

template <typename T> emscripten::val vector_to_js(const std::vector<T> &vec) {
  static_assert(std::is_arithmetic_v<T>, "T must be a numeric type");

  emscripten::val jsArray = emscripten::val::array();

  for (const auto &value : vec) {
    jsArray.call<void>("push", value);
  }

  return jsArray;
}

#endif
