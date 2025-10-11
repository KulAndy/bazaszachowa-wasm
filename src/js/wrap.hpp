#include <unordered_map>
#include <vector>
#include <emscripten/bind.h>

#ifndef WRAPJS_H
#define WRAPJS_H

template <typename T, typename V>
emscripten::val unordered_map_to_js(const std::unordered_map<V, T> &map)
{
    static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
    emscripten::val jsMap = emscripten::val::object();
    for (const auto &pair : map)
    {
        jsMap.set(pair.first, pair.second);
    }
    return jsMap;
}

template <typename T>
emscripten::val vector_to_js(const std::vector<T> &vec)
{
    static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
    emscripten::val jsArray = emscripten::val::array();
    for (const auto &v : vec)
    {
        jsArray.call<void>("push", v);
    }
    return jsArray;
}

#endif