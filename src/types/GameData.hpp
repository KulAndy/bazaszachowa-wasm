#include <string>
#include <vector>

#include "./ShortMove.hpp"

#ifndef GAMEDATA_H
#define GAMEDATA_H

class GameData {
public:
  std::vector<ShortMove> moves;
  std::string result;
  int year;
  int id;

  GameData(int id, const std::string &result, int year)
      : result(result), year(year), id(id) {}
};

#endif
