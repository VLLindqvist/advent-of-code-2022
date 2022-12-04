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

const std::map<char, uint_fast8_t> priority{
    {'a', 1},  {'b', 2},  {'c', 3},  {'d', 4},  {'e', 5},  {'f', 6},  {'g', 7},
    {'h', 8},  {'i', 9},  {'j', 10}, {'k', 11}, {'l', 12}, {'m', 13}, {'n', 14},
    {'o', 15}, {'p', 16}, {'q', 17}, {'r', 18}, {'s', 19}, {'t', 20}, {'u', 21},
    {'v', 22}, {'w', 23}, {'x', 24}, {'y', 25}, {'z', 26}, {'A', 27}, {'B', 28},
    {'C', 29}, {'D', 30}, {'E', 31}, {'F', 32}, {'G', 33}, {'H', 34}, {'I', 35},
    {'J', 36}, {'K', 37}, {'L', 38}, {'M', 39}, {'N', 40}, {'O', 41}, {'P', 42},
    {'Q', 43}, {'R', 44}, {'S', 45}, {'T', 46}, {'U', 47}, {'V', 48}, {'W', 49},
    {'X', 50}, {'Y', 51}, {'Z', 52},
};

using Rucksack = std::pair<std::string, std::string>;
using Rucksacks = std::vector<std::shared_ptr<Rucksack>>;
using DuplicateItems = std::unordered_map<char, uint_fast16_t>;

void removeDuplicateItems(std::string &compartment) {
  // remove duplicate items from compartment
  DuplicateItems duplicateItems;
  for (const auto &item : compartment) {
    duplicateItems[item]++;
  }

  // remove duplicate items from compartment
  compartment.erase(std::remove_if(std::begin(compartment),
                                   std::end(compartment),
                                   [&duplicateItems](const char &item) {
                                     if (duplicateItems[item] > 1) {
                                       duplicateItems[item]--;
                                       return true;
                                     }
                                     return false;
                                   }),
                    std::end(compartment));
}

char findDuplicate(const std::string &firstCompartment,
                   const std::string &secondCompartment) {
  for (const auto &ch : firstCompartment) {
    if (secondCompartment.find(ch) != std::string::npos) {
      return ch;
    }
  }

  return '\0';
}

std::shared_ptr<DuplicateItems> readFile(const std::string &filename) {
  std::ifstream file;
  file.open(filename, std::ios::in);
  std::string line;

  auto duplicateItems = std::make_shared<DuplicateItems>();

  while (std::getline(file, line)) {
    if (line.empty()) {
      break;
    }

    size_t splitIndex = (line.size() / 2);

    auto firstCompartment = line.substr(0, splitIndex);
    auto secondCompartment = line.substr(splitIndex);

    removeDuplicateItems(firstCompartment);
    removeDuplicateItems(secondCompartment);

    // find duplicate item
    auto duplicate = findDuplicate(firstCompartment, secondCompartment);
    if (duplicate == '\0') {
      throw std::runtime_error("No duplicate item found");
      continue;
    }

    if (duplicateItems->find(duplicate) == std::end(*duplicateItems)) {
      duplicateItems->insert({duplicate, 1});
    } else {
      duplicateItems->at(duplicate)++;
    }
  }

  return duplicateItems;
}

size_t
calculateSumOfPriorities(const std::shared_ptr<DuplicateItems> &duplicates) {
  return std::accumulate(std::begin(*duplicates), std::end(*duplicates), 0,
                         [](size_t sum, const auto &duplicate) {
                           return sum + (priority.at(duplicate.first) *
                                         duplicate.second);
                         });
}

int main() {
  auto duplicateItems = readFile("input.txt");

  std::cout << calculateSumOfPriorities(duplicateItems) << std::endl;

  return 0;
}

// Wrong answers
// 6486 -> too low
// 8188 -> too low
// 18588 -> too high
// 7981