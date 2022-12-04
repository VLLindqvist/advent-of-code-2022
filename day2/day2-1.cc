#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

enum Gesture : size_t { Rock = 1, Paper = 2, Scissors = 3 };

class RPS {
public:
  struct Round {
    const size_t yourPoints;
    const size_t theirPoints;

    Round(size_t you, size_t they) : yourPoints{you}, theirPoints{they} {}
  };

  static const std::map<char, Gesture> gestures;

  void play() {
    for (const auto &uniqueRound : uniqueRounds) {
      const auto &yourPlayChar = uniqueRound.first[1];
      const auto &theirPlayChar = uniqueRound.first[0];

      const auto &yourPlay = gestures.at(yourPlayChar);
      const auto &theirPlay = gestures.at(theirPlayChar);

      if (yourPlay == theirPlay) {
        // Tie
        yourScore += ((3 + yourPlay) * uniqueRound.second);
        theirScore += ((3 + theirPlay) * uniqueRound.second);
      } else if (yourPlay == Gesture::Rock && theirPlay == Gesture::Scissors ||
                 yourPlay == Gesture::Paper && theirPlay == Gesture::Rock ||
                 yourPlay == Gesture::Scissors && theirPlay == Gesture::Paper) {
        // You win
        yourScore += ((6 + yourPlay) * uniqueRound.second);
        theirScore += (theirPlay * uniqueRound.second);
      } else {
        // They win
        yourScore += (yourPlay * uniqueRound.second);
        theirScore += ((6 + theirPlay) * uniqueRound.second);
      }
    }
  }

  void addRound(char yourPlay, char theirPlay) {
    std::string permutation = std::string{} + theirPlay + yourPlay;
    uniqueRounds.at(permutation)++;
  }

  size_t getYourScore() const { return yourScore; }

  std::map<std::string, size_t> uniqueRounds = {
      {"AX", 0}, {"AY", 0}, {"AZ", 0}, {"BX", 0}, {"BY", 0},
      {"BZ", 0}, {"CX", 0}, {"CY", 0}, {"CZ", 0},
  };

private:
  std::vector<std::shared_ptr<Round>> rounds{};

  size_t yourScore{0};
  size_t theirScore{0};
};

// initialize static members gesture in RPS with 'A','X' -> Rock, 'B','Y' ->
// Paper, 'C','Z' -> Scissors
const std::map<char, Gesture> RPS::gestures = {
    {'A', Gesture::Rock},  {'X', Gesture::Rock},     {'B', Gesture::Paper},
    {'Y', Gesture::Paper}, {'C', Gesture::Scissors}, {'Z', Gesture::Scissors}};

int main() {
  std::ifstream inputFile;
  inputFile.open("input.txt", std::ios::in);

  RPS rps{};

  std::string line;
  int id = 0;
  while (inputFile) {
    std::getline(inputFile, line);
    if (line.empty() || line.size() != 3) {
      break;
    }

    const char theirPlay = line[0];
    const char yourPlay = line[2];
    rps.addRound(yourPlay, theirPlay);
  }

  rps.play();

  inputFile.close();

  auto yourScore = rps.getYourScore();

  std::cout << yourScore << std::endl;

  return 0;
}