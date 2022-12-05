#include <deque>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using Stack = std::deque<char>;
using Stacks = std::vector<Stack>;

Stacks readStacks(std::ifstream &file) {
  Stacks stacks{};
  std::string line;

  while (std::getline(file, line)) {
    if (!line.empty() && line[1] == '1') {
      if (!std::getline(file, line)) {
        throw std::runtime_error("Invalid input");
      }
      break;
    }

    if (stacks.empty()) {
      size_t numberOfStacks = ((line.size() - 1) / 4) + 1;
      stacks = {numberOfStacks, Stack{}};
    }

    std::regex crateRegex("[A-Z]");
    auto cratesInRowBegin =
        std::sregex_iterator(std::begin(line), std::end(line), crateRegex);
    auto cratesInRowEnd = std::sregex_iterator();

    for (std::sregex_iterator i = cratesInRowBegin; i != cratesInRowEnd; ++i) {
      std::smatch match = *i;
      auto crate = match.str()[0];

      // find index of crate in line
      size_t index = (match.position() - 1) / 4;
      stacks[index].push_back(crate);
    }
  }

  return std::move(stacks);
}

std::string readAndExecuteInstructions(std::ifstream &file,
                                       const Stacks &_stacks) {
  Stacks stacks{_stacks};
  std::string topCrates{};

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) {
      break;
    }

    // create regex that matches a number
    std::regex numberRegex("[0-9]+");

    auto numbersInRowBegin =
        std::sregex_iterator(std::begin(line), std::end(line), numberRegex);
    auto numbersInRowEnd = std::sregex_iterator();

    if (std::distance(numbersInRowBegin, numbersInRowEnd) != 3) {
      throw std::runtime_error("Invalid input");
    }

    auto i = numbersInRowBegin;
    std::smatch match = *i;
    size_t numberOfCrates = std::stoi(match.str());
    i++;
    match = *i;
    size_t fromIndex = std::stoi(match.str()) - 1;
    i++;
    match = *i;
    size_t destinationIndex = std::stoi(match.str()) - 1;

    size_t k = 0;
    while (k < numberOfCrates) {
      k++;
      if (stacks[fromIndex].empty()) {
        throw std::runtime_error("Invalid input");
      }
      auto crate = stacks[fromIndex].front();
      stacks[fromIndex].pop_front();
      stacks[destinationIndex].push_front(crate);
    }
  }

  for (auto &stack : stacks) {
    if (!stack.empty()) {
      topCrates += stack.front();
    }
  }

  return topCrates;
}

int main() {
  std::ifstream file;
  file.open("input.txt", std::ios::in);

  auto stack = readStacks(file);

  auto topOfStacks = readAndExecuteInstructions(file, stack);

  file.close();

  std::cout << topOfStacks << std::endl;

  return 0;
}
