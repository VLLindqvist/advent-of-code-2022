#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

namespace RPS {
using UniqueRounds = std::map<std::string, size_t>;

struct Round {
  const size_t yourPoints;
  const size_t theirPoints;

  Round(size_t you, size_t they) : yourPoints{you}, theirPoints{they} {}
};
using Rounds = std::vector<std::shared_ptr<Round>>;

enum Gesture : size_t { Rock = 1, Paper = 2, Scissors = 3 };
enum Outcome : size_t { Lose = 1, Draw = 2, Win = 3 };

namespace {
constexpr Gesture mapCharToGesture(char c) {
  switch (c) {
  case 'A':
    return Rock;
  case 'B':
    return Paper;
  case 'C':
  default:
    return Scissors;
  }
}

constexpr Outcome mapCharToOutcome(char c) {
  switch (c) {
  case 'X':
    return Lose;
  case 'Y':
    return Draw;
  case 'Z':
  default:
    return Win;
  }
}
} // namespace

std::shared_ptr<UniqueRounds> init() {
  auto uniqueRounds = std::make_shared<UniqueRounds>(UniqueRounds{
      {"AX", 0},
      {"AY", 0},
      {"AZ", 0},
      {"BX", 0},
      {"BY", 0},
      {"BZ", 0},
      {"CX", 0},
      {"CY", 0},
      {"CZ", 0},
  });

  return std::move(uniqueRounds);
}

std::shared_ptr<Rounds>
play(const std::shared_ptr<RPS::UniqueRounds> uniqueRounds) {
  auto rounds = std::make_shared<Rounds>();

  for (const auto &[permutation, numberOfRounds] : *uniqueRounds) {
    const auto &theirPlay = mapCharToGesture(permutation[0]);
    const auto &wantedOutcome = mapCharToOutcome(permutation[1]);

    switch (wantedOutcome) {
    case Win: {
      auto yourPlay = Gesture::Paper;
      if (theirPlay == Gesture::Paper) {
        yourPlay = Gesture::Scissors;
      } else if (theirPlay == Gesture::Scissors) {
        yourPlay = Gesture::Rock;
      }
      std::fill_n(std::back_inserter(*rounds), numberOfRounds,
                  std::move(std::make_shared<Round>(yourPlay + 6, theirPlay)));
      break;
    }
    case Draw: {
      auto yourPlay = Gesture::Paper;
      if (theirPlay == Gesture::Rock) {
        yourPlay = Gesture::Rock;
      } else if (theirPlay == Gesture::Scissors) {
        yourPlay = Gesture::Scissors;
      }
      std::fill_n(
          std::back_inserter(*rounds), numberOfRounds,
          std::move(std::make_shared<Round>(yourPlay + 3, theirPlay + 3)));
      break;
    }
    case Lose:
    default: {
      auto yourPlay = Gesture::Paper;
      if (theirPlay == Gesture::Rock) {
        yourPlay = Gesture::Scissors;
      } else if (theirPlay == Gesture::Paper) {
        yourPlay = Gesture::Rock;
      }
      std::fill_n(std::back_inserter(*rounds), numberOfRounds,
                  std::move(std::make_shared<Round>(yourPlay, theirPlay + 6)));
      break;
    }
    }
  }

  return rounds;
}

size_t calculateYourScore(const std::shared_ptr<Rounds> rounds) {
  return std::accumulate(
      std::begin(*rounds), std::end(*rounds), 0,
      [](size_t i, std::shared_ptr<Round> r) { return i + r->yourPoints; });
}
} // namespace RPS

void readFile(const std::string &filename,
              std::shared_ptr<RPS::UniqueRounds> uniqueRounds) {
  std::ifstream file;
  file.open(filename, std::ios::in);
  std::string line;

  while (std::getline(file, line)) {
    if (line.empty() || line.size() != 3) {
      break;
    }

    std::string permutation = std::string{} + line[0] + line[2];
    uniqueRounds->at(permutation)++;
  }
}

int main() {
  auto uniqueRounds = RPS::init();

  readFile("input.txt", uniqueRounds);

  auto rounds = RPS::play(uniqueRounds);

  auto yourScore = RPS::calculateYourScore(rounds);

  std::cout << yourScore << std::endl;

  return 0;
}

// wrong answers
// 14281