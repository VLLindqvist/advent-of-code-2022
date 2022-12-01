#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

struct Elf {
  std::vector<int> calories;

  Elf() : calories{} {}

  int totalCalories() const {
    return std::accumulate(std::begin(calories), std::end(calories), 0);
  }
};

bool largerThan(std::shared_ptr<Elf> a, std::shared_ptr<Elf> b) {
  return a->totalCalories() > b->totalCalories();
}

bool smallerThan(std::shared_ptr<Elf> a, std::shared_ptr<Elf> b) {
  return a->totalCalories() < b->totalCalories();
}

int main() {
  std::ifstream inputFile;
  inputFile.open("input.txt", std::ios::in);

  std::vector<std::shared_ptr<Elf>> elves{std::make_unique<Elf>()};

  std::string line;
  int id = 0;
  while (inputFile) {
    std::getline(inputFile, line);
    if (line.empty()) {
      elves.push_back(std::make_unique<Elf>());
      ++id;
      continue;
    }

    elves[id]->calories.push_back(std::stoi(line));
  }

  inputFile.close();

  std::sort(std::begin(elves), std::end(elves), largerThan);

  auto topThree = std::accumulate(
      std::begin(elves), std::begin(elves) + 3, 0,
      [](int i, std::shared_ptr<Elf> e) { return i + e->totalCalories(); });

  std::cout << topThree << std::endl;

  return 0;
}