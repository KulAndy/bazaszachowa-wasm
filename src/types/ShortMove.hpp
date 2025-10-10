#include <optional>
#include <string>

#ifndef SHORTMOVE_H
#define SHORTMOVE_H
class ShortMove
{
public:
    std::string from;
    std::string to;
    std::optional<std::string> promotion;

    ShortMove(const std::string &from, const std::string &to,
              const std::optional<std::string> &promotion = std::nullopt)
        : from(from), to(to), promotion(promotion) {}
};

#endif