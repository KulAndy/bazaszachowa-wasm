#ifndef TRANSPARENT_STRING_HASH_H
#define TRANSPARENT_STRING_HASH_H

struct TransparentStringHash {
  using is_transparent = void;

  std::size_t operator()(std::string_view value) const noexcept {
    return std::hash<std::string_view>{}(value);
  }

  std::size_t operator()(const std::string &value) const noexcept {
    return operator()(std::string_view{value});
  }

  std::size_t operator()(const char *value) const noexcept {
    return operator()(std::string_view{value});
  }
};

#endif
