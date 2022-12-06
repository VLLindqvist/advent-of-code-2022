#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using Buffer = std::deque<char>;

Buffer readAndBufferize(const std::string &filename) {
  std::ifstream file;
  file.open(filename, std::ios::in);

  std::string line;
  if (!std::getline(file, line)) {
    throw std::runtime_error("Invalid input");
  }

  return {std::begin(line), std::end(line)};
}

bool haveDuplicate(const std::array<char, 14> &bufferSubset) {
  // find duplicate in bufferSubset
  for (size_t i = 0; i < bufferSubset.size(); ++i) {
    for (size_t j = i + 1; j < bufferSubset.size(); ++j) {
      if (bufferSubset[i] == bufferSubset[j]) {
        return true;
      }
    }
  }

  return false;
}

size_t parseBuffer(Buffer &buffer) {
  size_t index = 0;

  while (!buffer.empty() && buffer.size() > 13) {
    if (!haveDuplicate({buffer[0], buffer[1], buffer[2], buffer[3], buffer[4],
                        buffer[5], buffer[6], buffer[7], buffer[8], buffer[9],
                        buffer[10], buffer[11], buffer[12], buffer[13]})) {
      index += 14;
      break;
    }
    index++;
    buffer.pop_front();
  }

  return index;
}

int main() {
  auto buffer = readAndBufferize("input.txt");

  auto index = parseBuffer(buffer);

  std::cout << index << std::endl;

  return 0;
}
