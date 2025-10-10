#include <emscripten/bind.h>
#include <unordered_map>
#include <vector>
#include <string>

#include "./MoveData.hpp"

#ifndef FENDATA_H
#define FENDATA_H
struct FenData
{
    std::unordered_map<std::string, MoveData> moves;
    std::vector<int> indexes;

    emscripten::val getMovesJS() const
    {
        emscripten::val result = emscripten::val::object();
        for (const auto &pair : moves)
        {
            emscripten::val moveObj = emscripten::val::object();
            moveObj.set("games", pair.second.games);
            moveObj.set("points", pair.second.points);

            emscripten::val yearsArray = emscripten::val::array();
            for (size_t i = 0; i < pair.second.years.size(); ++i)
            {
                yearsArray.set(i, pair.second.years[i]);
            }
            moveObj.set("years", yearsArray);

            emscripten::val statsMap = emscripten::val::object();
            for (const auto &stat : pair.second.stats)
            {
                emscripten::val statObj = emscripten::val::object();
                statObj.set("count", stat.second.count);
                statObj.set("points", stat.second.points);
                statsMap.set(std::to_string(stat.first), statObj);
            }
            moveObj.set("stats", statsMap);

            result.set(pair.first, moveObj);
        }
        return result;
    }
    emscripten::val getIndexesJS() const
    {
        emscripten::val arr = emscripten::val::array();
        size_t i = 0;
        for (const auto &index : indexes)
        {
            arr.set(i++, index);
        }
        return arr;
    }
};

#endif