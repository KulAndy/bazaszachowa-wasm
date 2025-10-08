#include "../../chess-library/include/chess.hpp"
#include "main.hpp"
#include <algorithm>
#include <emscripten/bind.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace emscripten;
using namespace chess;
using namespace std;

constexpr int firstBatchLimit = 40;

string cutStringToPenultimateSpace(const string &inputString)
{
  size_t lastSpaceIndex = inputString.rfind(' ');
  if (lastSpaceIndex == string::npos)
    return inputString;
  lastSpaceIndex = inputString.substr(0, lastSpaceIndex).rfind(' ');
  if (lastSpaceIndex == string::npos)
    return inputString;
  return inputString.substr(0, lastSpaceIndex);
}

unordered_map<string, FenData> getFENsFirstBatch(const GameData &row)
{
  unordered_map<string, FenData> fensMap;
  float points = (row.result == "1-0")   ? 1.0f
                 : (row.result == "0-1") ? 0.0f
                                         : 0.5f;
  Board board;
  size_t length = min(static_cast<size_t>(firstBatchLimit), row.moves.size());
  for (size_t i = 0; i < length; ++i)
  {
    const ShortMove &move = row.moves[i];
    float sidePoints = (i % 2 == 0) ? points : 1.0 - points;
    string fen = cutStringToPenultimateSpace(board.getFen());
    string uci = move.from + move.to;
    if (move.promotion)
    {
      uci += move.promotion.value();
    }
    Move moveObj = uci::uciToMove(board, uci);
    string san = uci::moveToSan(board, moveObj);
    board.makeMove(moveObj);
    FenData &fenData = fensMap[fen];
    MoveData &moveData = fenData.moves[san];
    moveData.games += 1;
    moveData.points += sidePoints;
    moveData.years.push_back(row.year);
    Stats &yearStats = moveData.stats[row.year];
    yearStats.count += 1;
    yearStats.points += sidePoints;
    if (find(fenData.indexes.begin(), fenData.indexes.end(), row.id) ==
        fenData.indexes.end())
    {
      fenData.indexes.push_back(row.id);
    }
  }
  return fensMap;
}

val convertFenMapToJS(const unordered_map<string, FenData> &fenMap)
{
  val result = val::object();
  for (const auto &pair : fenMap)
  {
    val fenData = val::object();
    fenData.set("moves", pair.second.getMovesJS());
    fenData.set("indexes", pair.second.getIndexesJS());
    result.set(pair.first, fenData);
  }
  return result;
}

val getFENsFirstBatchJS(const GameData &row)
{
  unordered_map<string, FenData> fenMap = getFENsFirstBatch(row);
  return convertFenMapToJS(fenMap);
}

EMSCRIPTEN_BINDINGS(ChessProcessor)
{
  value_object<Stats>("Stats")
      .field("count", &Stats::count)
      .field("points", &Stats::points);

  value_object<MoveData>("MoveData")
      .field("games", &MoveData::games)
      .field("points", &MoveData::points)
      .field("years", &MoveData::years)
      .field("stats", &MoveData::stats);

  class_<FenData>("FenData")
      .constructor<>()
      .function("getMovesJS", &FenData::getMovesJS)
      .function("getIndexesJS", &FenData::getIndexesJS);

  class_<ShortMove>("Move")
      .constructor<string, string, optional<string>>()
      .property("from", &ShortMove::from)
      .property("to", &ShortMove::to)
      .property("promotion", &ShortMove::promotion);

  class_<GameData>("GameData")
      .constructor<int, string, int>()
      .property("moves", &GameData::moves)
      .property("result", &GameData::result)
      .property("year", &GameData::year)
      .property("id", &GameData::id);

  emscripten::function("getFENsFirstBatch", &getFENsFirstBatch);
  emscripten::function("getFENsFirstBatchJS", &getFENsFirstBatchJS);

  register_map<string, MoveData>("MapStringMoveData");
  register_map<int, Stats>("MapIntStats");
  register_map<string, FenData>("MapStringFenData");
  register_vector<int>("VectorInt");
  register_vector<ShortMove>("VectorMove");
  register_vector<GameData>("VectorGameData");
  register_optional<string>();
}
