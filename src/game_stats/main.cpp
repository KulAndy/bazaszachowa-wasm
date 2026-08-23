#include <cstdint>
#include <emscripten/bind.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "attacked_pieces.hpp"
#include "center_control.hpp"
#include "heatmap.hpp"
#include "main.hpp"
#include "material.hpp"
#include "mobility.hpp"
#include "pawn_structure.hpp"

using namespace std;
using namespace chess;
using namespace emscripten;

template <typename T, typename Hash, typename KeyEqual>
val pawn_struct_to_js(
    const unordered_map<string, T, Hash, KeyEqual> &pawnStruct) {
  static_assert(std::is_arithmetic_v<T>, "T must be a numeric type");

  val jsMap = val::object();

  for (const auto &[key, value] : pawnStruct) {
    jsMap.set(key, value);
  }

  return jsMap;
}

val computeHeatmap(const vector<GameData> &games, Color colorFilter) {
  StringUint32Map total_heatmap;

  for (const auto &game : games) {
    auto game_heatmap = computeGameHeatmap(game, colorFilter);

    for (const auto &kv : game_heatmap) {
      total_heatmap[kv.first] += kv.second;
    }
  }

  return unordered_map_to_js(total_heatmap);
}

val computeMobility(const vector<GameData> &games, Color colorFilter) {
  vector<uint8_t> total_mobility;

  for (const auto &game : games) {
    auto game_mobility = computeGameMobility(game, colorFilter);

    total_mobility.insert(total_mobility.end(), game_mobility.begin(),
                          game_mobility.end());
  }

  return vector_to_js(total_mobility);
}

val computeAttackedPieces(const vector<GameData> &games, Color colorFilter) {
  vector<uint8_t> total_attacked_pieces;

  for (const auto &game : games) {
    auto game_attacked_pieces = computeGameAttackedPieces(game, colorFilter);

    total_attacked_pieces.insert(total_attacked_pieces.end(),
                                 game_attacked_pieces.begin(),
                                 game_attacked_pieces.end());
  }

  return vector_to_js(total_attacked_pieces);
}

val computeMaterialBalances(const vector<GameData> &games, Color colorFilter) {
  vector<int8_t> total_material_balances;

  for (const auto &game : games) {
    auto game_material_balances =
        computeGameMaterialBalances(game, colorFilter);

    total_material_balances.insert(total_material_balances.end(),
                                   game_material_balances.begin(),
                                   game_material_balances.end());
  }

  return vector_to_js(total_material_balances);
}

val computeCenterControl(const vector<GameData> &games, Color colorFilter) {
  vector<int8_t> total_center_control;

  for (const auto &game : games) {
    auto game_center_control = computeGameCenterControl(game, colorFilter);

    total_center_control.insert(total_center_control.end(),
                                game_center_control.begin(),
                                game_center_control.end());
  }

  return vector_to_js(total_center_control);
}

val computeExtendedCenterControl(const vector<GameData> &games,
                                 Color colorFilter) {
  vector<int8_t> total_ext_center_control;

  for (const auto &game : games) {
    auto game_ext_center_control =
        computeGameExtendedCenterControl(game, colorFilter);

    total_ext_center_control.insert(total_ext_center_control.end(),
                                    game_ext_center_control.begin(),
                                    game_ext_center_control.end());
  }

  return vector_to_js(total_ext_center_control);
}

val computePawnStruct(const vector<GameData> &games, Color colorFilter) {
  StringUint8Map total_pawn_struct = {
      {"isolated", 0}, {"doubled", 0}, {"passed", 0}, {"fianchetto", 0}};

  for (const auto &game : games) {
    auto game_pawn_struct = computeGamePawnStruct(game, colorFilter);

    for (const auto &kv : game_pawn_struct) {
      total_pawn_struct[kv.first] += kv.second;
    }
  }

  return pawn_struct_to_js(total_pawn_struct);
}

EMSCRIPTEN_BINDINGS(chess_stats_module) {
  register_vector<string>("VectorString");

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

  class_<Color>("Color").constructor<>().constructor<std::string>().function(
      "longStr", &Color::longStr);

  emscripten::function("computeHeatmap", &computeHeatmap);
  emscripten::function("computeMobility", &computeMobility);
  emscripten::function("computeAttackedPieces", &computeAttackedPieces);
  emscripten::function("computeMaterialBalances", &computeMaterialBalances);
  emscripten::function("computeCenterControl", &computeCenterControl);
  emscripten::function("computeExtendedCenterControl",
                       &computeExtendedCenterControl);
  emscripten::function("computePawnStruct", &computePawnStruct);

  register_vector<ShortMove>("VectorMove");
  register_vector<GameData>("VectorGameData");
}
