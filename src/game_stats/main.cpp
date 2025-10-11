#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "../../chess-library/include/chess.hpp"
#include <emscripten/bind.h>
#include "../types/ShortMove.hpp"
#include "../types/GameData.hpp"

namespace std
{
    template <>
    struct hash<chess::Square>
    {
        size_t operator()(const chess::Square &sq) const
        {
            return sq.index();
        }
    };
}

using namespace std;
using namespace chess;
using namespace emscripten;

constexpr uint8_t endOpeningHalfMove = 50;

inline uint8_t pieceValue(PieceType pt)
{
    switch (pt)
    {
    case PAWN:
        return 1;
    case KNIGHT:
        return 3;
    case BISHOP:
        return 3;
    case ROOK:
        return 5;
    case QUEEN:
        return 9;
    default:
        return 0;
    }
}

template <typename T>
val unordered_map_to_js(const unordered_map<string, T> &map)
{
    static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
    val jsMap = val::object();
    for (const auto &pair : map)
    {
        jsMap.set(pair.first, pair.second);
    }
    return jsMap;
}

template <typename T>
val vector_to_js(const vector<T> &vec)
{
    static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
    val jsArray = val::array();
    for (const auto &v : vec)
    {
        jsArray.call<void>("push", v);
    }
    return jsArray;
}

template <typename T>
val pawn_struct_to_js(const unordered_map<string, T> &pawnStruct)
{
    static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
    val jsMap = val::object();
    for (const auto &pair : pawnStruct)
    {
        jsMap.set(pair.first, pair.second);
    }
    return jsMap;
}

unordered_map<string, uint32_t> computeGameHeatmap(const GameData &game, Color colorFilter)
{
    unordered_map<string, uint32_t> heatmap;
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (const ShortMove &sm : game.moves)
    {
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() == colorFilter)
        {
            continue;
        }

        heatmap[sm.to]++;
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    return heatmap;
}

vector<uint8_t> computeGameMobility(const GameData &game, Color colorFilter)
{
    vector<uint8_t> mobility;
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (const ShortMove &sm : game.moves)
    {
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() == colorFilter)
        {
            continue;
        }
        Movelist moves;
        movegen::legalmoves(moves, curr);
        mobility.push_back(static_cast<uint8_t>(moves.size()));
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    return mobility;
}

vector<uint8_t> computeGameAttackedPieces(const GameData &game, Color colorFilter)
{
    vector<uint8_t> attacked_pieces;
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (const ShortMove &sm : game.moves)
    {
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() != colorFilter)
        {
            continue;
        }
        Movelist moves;
        movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, curr);
        unordered_map<Square, uint8_t> destCount;
        for (const Move &m : moves)
        {
            destCount[m.to()] += 1;
        }
        uint8_t cnt = 0;
        for (auto &kv : destCount)
        {
            Piece pc = curr.at(kv.first);
            if (pc != Piece::NONE && pc.color() != curr.sideToMove())
                cnt++;
        }
        attacked_pieces.push_back(cnt);
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    return attacked_pieces;
}

vector<int8_t> computeGameMaterialBalances(const GameData &game, Color colorFilter)
{
    vector<int8_t> material_balances;
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (const ShortMove &sm : game.moves)
    {
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() != colorFilter)
        {
            continue;
        }
        int8_t white = 0, black = 0;
        for (Square sq = Square::SQ_A1; sq <= Square::SQ_H8; ++sq)
        {
            Piece pc = curr.at(sq);
            if (pc == Piece::NONE)
                continue;
            int8_t val = -(pc.type());
            if (pc.color() == Color::WHITE)
                white += val;
            else
                black += val;
        }
        material_balances.push_back(white - black);
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    return material_balances;
}

vector<int8_t> computeGameCenterControl(const GameData &game, Color colorFilter)
{
    vector<int8_t> center_control;
    vector<Square> center_sq = {
        Square::SQ_D4, Square::SQ_E4,
        Square::SQ_D5, Square::SQ_E5};
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (size_t i = 0; i < game.moves.size(); ++i)
    {
        const ShortMove &sm = game.moves[i];
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() != colorFilter)
        {
            continue;
        }
        int8_t cc = 0;
        for (const Square &sq : center_sq)
        {
            Piece piece = curr.at(sq);
            if (piece != Piece::NONE && piece.color() != curr.sideToMove() && piece.type() == PieceType::PAWN)
            {
                cc++;
            }
        }
        center_control.push_back(cc);
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    return center_control;
}

vector<int8_t> computeGameExtendedCenterControl(const GameData &game, Color colorFilter)
{
    vector<int8_t> ext_center_control;
    vector<Square> ext_sq = {
        Square::SQ_C3, Square::SQ_D3, Square::SQ_E3, Square::SQ_F3,
        Square::SQ_C4, Square::SQ_D4, Square::SQ_E4, Square::SQ_F4,
        Square::SQ_C5, Square::SQ_D5, Square::SQ_E5, Square::SQ_F5,
        Square::SQ_C6, Square::SQ_D6, Square::SQ_E6, Square::SQ_F6};
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (size_t i = 0; i < game.moves.size(); ++i)
    {
        const ShortMove &sm = game.moves[i];
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() != colorFilter)
        {
            continue;
        }
        int8_t ecc = 0;
        for (const Square &sq : ext_sq)
        {
            Piece piece = curr.at(sq);
            if (piece != Piece::NONE && piece.color() != curr.sideToMove() && piece.type() == PieceType::PAWN)
            {
                ecc++;
            }
        }
        ext_center_control.push_back(ecc);
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    return ext_center_control;
}

unordered_map<string, uint8_t> computeGamePawnStruct(const GameData &game, Color colorFilter)
{
    uint8_t isolated_pawns = 0;
    uint8_t doubled_pawns = 0;
    uint8_t passed_pawns = 0;
    uint8_t fianchetto_pawns = 0;
    Board board;
    board.setFen(constants::STARTPOS);
    Board curr = board;
    uint8_t counter = 0;
    for (const ShortMove &sm : game.moves)
    {
        string uci = sm.from + sm.to;
        if (sm.promotion.has_value())
            uci += *(sm.promotion);
        Move mv = uci::uciToMove(curr, uci);
        curr.makeMove(mv);
        if (curr.sideToMove() != colorFilter)
        {
            continue;
        }
        if (++counter > endOpeningHalfMove)
        {
            break;
        }
    }
    auto pawnOnFile = [&curr](Color side, uint8_t file) -> uint8_t
    {
        uint8_t c = 0;
        for (uint8_t rank = 0; rank < 8; ++rank)
        {
            Square sq = Square(File(file), Rank(rank));
            Piece pc = curr.at(sq);
            if (pc != Piece::NONE && pc.type() == PAWN && pc.color() == side)
                c++;
        }
        return c;
    };
    for (uint8_t sideIdx = 0; sideIdx < 2; ++sideIdx)
    {
        Color side = (sideIdx == 0 ? Color::WHITE : Color::BLACK);
        for (uint8_t f = 0; f < 8; ++f)
        {
            uint8_t cnt = pawnOnFile(side, f);
            if (cnt >= 2)
                doubled_pawns += (cnt - 1);
            if (cnt == 1)
            {
                bool left = (f > 0 && pawnOnFile(side, f - 1) > 0);
                bool right = (f < 7 && pawnOnFile(side, f + 1) > 0);
                if (!left && !right)
                    isolated_pawns++;
            }
            if (cnt >= 1)
            {
                bool passed = true;
                for (uint8_t ef = std::max(0, f - 1); ef <= std::min(7, f + 1); ++ef)
                {
                    for (uint8_t r = 0; r < 8; ++r)
                    {
                        Square esq = Square(File(ef), Rank(r));
                        Piece epc = curr.at(esq);
                        if (epc != Piece::NONE && epc.type() == PAWN && epc.color() != side)
                        {
                            passed = false;
                            break;
                        }
                    }
                    if (!passed)
                        break;
                }
                if (passed)
                    passed_pawns += cnt;
            }
        }
        if (side == Color::WHITE)
        {
            if (curr.at(Square::SQ_B3).type() == PAWN)
                fianchetto_pawns++;
            if (curr.at(Square::SQ_G3).type() == PAWN)
                fianchetto_pawns++;
        }
        else
        {
            if (curr.at(Square::SQ_B6).type() == PAWN)
                fianchetto_pawns++;
            if (curr.at(Square::SQ_G6).type() == PAWN)
                fianchetto_pawns++;
        }
    }
    return {{"isolated", isolated_pawns}, {"doubled", doubled_pawns}, {"passed", passed_pawns}, {"fianchetto", fianchetto_pawns}};
}

val computeHeatmap(const vector<GameData> &games, Color colorFilter)
{
    unordered_map<string, uint32_t> total_heatmap;
    for (const auto &game : games)
    {
        auto game_heatmap = computeGameHeatmap(game, colorFilter);
        for (const auto &kv : game_heatmap)
        {
            total_heatmap[kv.first] += kv.second;
        }
    }
    return unordered_map_to_js(total_heatmap);
}

val computeMobility(const vector<GameData> &games, Color colorFilter)
{
    vector<uint8_t> total_mobility;
    for (const auto &game : games)
    {
        auto game_mobility = computeGameMobility(game, colorFilter);
        total_mobility.insert(total_mobility.end(), game_mobility.begin(), game_mobility.end());
    }
    return vector_to_js(total_mobility);
}

val computeAttackedPieces(const vector<GameData> &games, Color colorFilter)
{
    vector<uint8_t> total_attacked_pieces;
    for (const auto &game : games)
    {
        auto game_attacked_pieces = computeGameAttackedPieces(game, colorFilter);
        total_attacked_pieces.insert(total_attacked_pieces.end(), game_attacked_pieces.begin(), game_attacked_pieces.end());
    }
    return vector_to_js(total_attacked_pieces);
}

val computeMaterialBalances(const vector<GameData> &games, Color colorFilter)
{
    vector<int8_t> total_material_balances;
    for (const auto &game : games)
    {
        auto game_material_balances = computeGameMaterialBalances(game, colorFilter);
        total_material_balances.insert(total_material_balances.end(), game_material_balances.begin(), game_material_balances.end());
    }
    return vector_to_js(total_material_balances);
}

val computeCenterControl(const vector<GameData> &games, Color colorFilter)
{
    vector<int8_t> total_center_control;
    for (const auto &game : games)
    {
        auto game_center_control = computeGameCenterControl(game, colorFilter);
        total_center_control.insert(total_center_control.end(), game_center_control.begin(), game_center_control.end());
    }
    return vector_to_js(total_center_control);
}

val computeExtendedCenterControl(const vector<GameData> &games, Color colorFilter)
{
    vector<int8_t> total_ext_center_control;
    for (const auto &game : games)
    {
        auto game_ext_center_control = computeGameExtendedCenterControl(game, colorFilter);
        total_ext_center_control.insert(total_ext_center_control.end(), game_ext_center_control.begin(), game_ext_center_control.end());
    }
    return vector_to_js(total_ext_center_control);
}

val computePawnStruct(const vector<GameData> &games, Color colorFilter)
{
    unordered_map<string, uint8_t> total_pawn_struct = {
        {"isolated", 0},
        {"doubled", 0},
        {"passed", 0},
        {"fianchetto", 0}};
    for (const auto &game : games)
    {
        auto game_pawn_struct = computeGamePawnStruct(game, colorFilter);
        for (const auto &kv : game_pawn_struct)
        {
            total_pawn_struct[kv.first] += kv.second;
        }
    }
    return pawn_struct_to_js(total_pawn_struct);
}

EMSCRIPTEN_BINDINGS(chess_stats_module)
{
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

    class_<Color>("Color")
        .constructor<>()
        .constructor<std::string>()
        .function("longStr", &Color::longStr);

    emscripten::function("computeHeatmap", &computeHeatmap);
    emscripten::function("computeMobility", &computeMobility);
    emscripten::function("computeAttackedPieces", &computeAttackedPieces);
    emscripten::function("computeMaterialBalances", &computeMaterialBalances);
    emscripten::function("computeCenterControl", &computeCenterControl);
    emscripten::function("computeExtendedCenterControl", &computeExtendedCenterControl);
    emscripten::function("computePawnStruct", &computePawnStruct);

    register_vector<ShortMove>("VectorMove");
    register_vector<GameData>("VectorGameData");
}
