#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <emscripten/bind.h>

#include "../js/wrap.hpp"

using namespace emscripten;

namespace stats {
template <typename T> double avg(const std::vector<T> &data) {
  if (data.empty()) {
    return 0.0;
  }

  double sum = std::accumulate(data.begin(), data.end(), 0.0);
  return sum / static_cast<double>(data.size());
}

template <typename T> double variance(const std::vector<T> &data) {
  if (data.size() < 2) {
    return 0.0;
  }

  double mean = avg(data);
  double var = 0.0;

  for (const auto &x : data) {
    var += (x - mean) * (x - mean);
  }

  return var / static_cast<double>(data.size());
}

template <typename T> double stddev(const std::vector<T> &data) {
  return std::sqrt(variance(data));
}

template <typename T> double median(std::vector<T> data) {
  if (data.empty()) {
    return 0.0;
  }

  std::ranges::sort(data);
  size_t n = data.size();

  if (n % 2 == 0) {
    return (data[n / 2 - 1] + data[n / 2]) / 2.0;
  }

  return data[n / 2];
}

template <typename T> double q1(std::vector<T> data) {
  if (data.empty()) {
    return 0.0;
  }

  std::ranges::sort(data);
  size_t n = data.size();
  size_t idx = n / 4;

  return data[idx];
}

template <typename T> double q3(std::vector<T> data) {
  if (data.empty()) {
    return 0.0;
  }

  std::ranges::sort(data);
  size_t n = data.size();
  size_t idx = (3 * n) / 4;

  return data[idx];
}

template <typename T> std::optional<T> dominant(const std::vector<T> &data) {
  if (data.empty()) {
    return std::nullopt;
  }

  std::map<T, size_t> freq;

  for (const auto &x : data) {
    freq[x]++;
  }

  T mode = data.front();
  size_t maxCount = 0;

  for (const auto &[value, count] : freq) {
    if (count > maxCount) {
      maxCount = count;
      mode = value;
    }
  }

  return mode;
}

template <typename T> val histogram(const std::vector<T> &data) {
  std::unordered_map<T, int> freq;

  for (const auto &x : data) {
    freq[x]++;
  }

  return unordered_map_to_js(freq);
}
} // namespace stats

EMSCRIPTEN_BINDINGS(chess_stats_module) {
  register_vector<uint32_t>("VectorUint32");
  register_vector<uint8_t>("VectorUint8");
  register_vector<int8_t>("VectorInt8");

  function("avg_Uint32", &stats::avg<uint32_t>);
  function("variance_Uint32", &stats::variance<uint32_t>);
  function("stddev_Uint32", &stats::stddev<uint32_t>);
  function("median_Uint32", &stats::median<uint32_t>);
  function("q1_Uint32", &stats::q1<uint32_t>);
  function("q3_Uint32", &stats::q3<uint32_t>);
  function("dominant_Uint32", &stats::dominant<uint32_t>);
  function("histogram_Uint32", &stats::histogram<uint32_t>);

  function("avg_Uint8", &stats::avg<uint8_t>);
  function("variance_Uint8", &stats::variance<uint8_t>);
  function("stddev_Uint8", &stats::stddev<uint8_t>);
  function("median_Uint8", &stats::median<uint8_t>);
  function("q1_Uint8", &stats::q1<uint8_t>);
  function("q3_Uint8", &stats::q3<uint8_t>);
  function("dominant_Uint8", &stats::dominant<uint8_t>);
  function("histogram_Uint8", &stats::histogram<uint8_t>);

  function("avg_Int8", &stats::avg<int8_t>);
  function("variance_Int8", &stats::variance<int8_t>);
  function("stddev_Int8", &stats::stddev<int8_t>);
  function("median_Int8", &stats::median<int8_t>);
  function("q1_Int8", &stats::q1<int8_t>);
  function("q3_Int8", &stats::q3<int8_t>);
  function("dominant_Int8", &stats::dominant<int8_t>);
  function("histogram_Int8", &stats::histogram<int8_t>);
}
