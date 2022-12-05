#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

using Range = std::pair<uint_fast32_t, uint_fast32_t>;
using ElfPair = std::pair<Range, Range>;
using ElfPairs = std::vector<ElfPair>;

bool doOverlap(const Range &r1, const Range &r2) {
  return (r1.first <= r2.first && r1.second >= r2.first) ||
         (r2.first <= r1.first && r2.second >= r1.first);
}

size_t readAndExecute(const std::string &filename) {
  size_t numberOfOverlaps = 0;

  std::ifstream file;
  file.open(filename, std::ios::in);
  std::string line;

  while (std::getline(file, line)) {
    if (line.empty()) {
      break;
    }

    // find comma
    auto commaIndex = line.find(',');
    if (commaIndex == std::string::npos) {
      throw std::runtime_error("Invalid input file");
    }

    std::string pairStrings[2]{line.substr(0, commaIndex),
                               line.substr(commaIndex + 1)};

    auto elfPair = std::make_pair(Range{0, 0}, Range{0, 0});
    for (size_t i = 0; i < 2; ++i) {
      auto elfRangeStr = pairStrings[i];

      // find dash
      auto dashIndex = elfRangeStr.find('-');
      if (dashIndex == std::string::npos) {
        throw std::runtime_error("Invalid input file");
      }

      uint_fast32_t rangeFrom = std::stoi(elfRangeStr.substr(0, dashIndex));
      uint_fast32_t rangeTo = std::stoi(elfRangeStr.substr(dashIndex + 1));

      if (i == 0) {
        elfPair.first.first = rangeFrom;
        elfPair.first.second = rangeTo;
      } else {
        elfPair.second.first = rangeFrom;
        elfPair.second.second = rangeTo;
      }
    }

    if (doOverlap(elfPair.first, elfPair.second) ||
        doOverlap(elfPair.second, elfPair.first)) {
      ++numberOfOverlaps;
    }
  }

  return numberOfOverlaps;
}

int main() {
  auto numberOfOverlaps = readAndExecute("input.txt");

  std::cout << numberOfOverlaps << std::endl;

  return 0;
}
