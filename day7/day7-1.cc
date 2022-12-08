#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

class File;
class Dir;

using DirOrFile = std::variant<std::shared_ptr<File>, std::shared_ptr<Dir>>;

class File {
public:
  File(const std::string &n, size_t s) : name(n), size(s) {}

  size_t getSize() const { return size; }

private:
  std::string name;
  size_t size;
};

class Dir : public std::enable_shared_from_this<Dir> {
public:
  Dir(const std::string &n, std::shared_ptr<Dir> p) : name(n), parent(p) {}

  void addFile(const std::string &name, size_t size) {
    // need to recalculate total size after adding a file
    totalSize = 0;
    children[name] = DirOrFile{std::make_shared<File>(name, size)};
  }

  std::shared_ptr<Dir> goToRoot() {
    if (parent == nullptr) {
      return shared_from_this();
    }
    return parent->goToRoot();
  }

  std::shared_ptr<Dir> goUp() {
    if (parent == nullptr) {
      return shared_from_this();
    }
    return parent;
  }

  std::shared_ptr<Dir> goDown(const std::string &name) {
    auto it = children.find(name);
    if (it != children.end() &&
        std::holds_alternative<std::shared_ptr<Dir>>(it->second)) {
      return std::get<std::shared_ptr<Dir>>(it->second);
    }

    return addDir(name);
  }

  size_t calculateTotalSize() {
    if (totalSize == 0) {
      for (const auto &[name, dirOrFile] : children) {
        if (std::holds_alternative<std::shared_ptr<File>>(dirOrFile)) {
          totalSize += std::get<std::shared_ptr<File>>(dirOrFile)->getSize();
        } else {
          totalSize +=
              std::get<std::shared_ptr<Dir>>(dirOrFile)->calculateTotalSize();
        }
      }
    }
    return totalSize;
  }

  std::vector<std::shared_ptr<Dir>> findDecendantsSmallerThan(size_t max) {
    std::vector<std::shared_ptr<Dir>> result;
    for (const auto &[name, dirOrFile] : children) {
      if (std::holds_alternative<std::shared_ptr<File>>(dirOrFile)) {
        continue;
      }

      auto dir = std::get<std::shared_ptr<Dir>>(dirOrFile);
      if (dir->calculateTotalSize() <= max) {
        result.push_back(dir);
      }

      auto subDirs = dir->findDecendantsSmallerThan(max);
      result.insert(std::end(result), std::begin(subDirs), std::end(subDirs));
    }
    return std::move(result);
  }

  friend std::ostream &operator<<(std::ostream &os, Dir &dir);

  const std::string &getName() const { return name; }

private:
  std::ostream &printDirectory(std::ostream &os, size_t indent = 0) {
    os << std::string(indent, ' ') << "<" << (name != "/" ? name : "root")
       << " size='" << calculateTotalSize() << "'>" << std::endl;
    for (const auto &[name, dirOrFile] : children) {
      if (std::holds_alternative<std::shared_ptr<File>>(dirOrFile)) {
        os << std::string((indent + 2), ' ') << "<" << name << " size='"
           << std::get<std::shared_ptr<File>>(dirOrFile)->getSize() << "'/>"
           << std::endl;
      } else {
        std::get<std::shared_ptr<Dir>>(dirOrFile)->printDirectory(os,
                                                                  indent + 2);
      }
    }
    os << std::string(indent, ' ') << "</" << (name != "/" ? name : "root")
       << ">" << std::endl;
    return os;
  }

  std::shared_ptr<Dir> addDir(const std::string &name) {
    totalSize = 0;
    auto subDir = std::make_shared<Dir>(name, shared_from_this());
    children[name] = DirOrFile{subDir};
    return subDir;
  }

  std::string name;
  std::shared_ptr<Dir> parent;
  std::map<std::string, DirOrFile> children{};
  size_t totalSize{0};
};

std::ostream &operator<<(std::ostream &os, Dir &dir) {
  return dir.printDirectory(os);
}

std::shared_ptr<Dir> readAndCreateFilesystem(const std::string &filename,
                                             std::shared_ptr<Dir> currentDir) {
  std::ifstream file;
  file.open(filename, std::ios::in);

  std::regex numberRegex{R"((\d+)+)"};
  std::smatch numberMatch;

  std::regex fileNameRegex{R"(\w+)"};
  std::smatch fileNameMatch;

  bool hasTried = false;
  std::string line{};
  while (true) {
    if (line.empty()) {
      if (hasTried) {
        break;
      }
      if (!std::getline(file, line)) {
        break;
      } else {
        if (line.empty()) {
          hasTried = true;
        }
      }
    }

    if (line.starts_with("$ cd ")) {
      auto to = line.substr(5);
      if (to == "/") {
        currentDir = currentDir->goToRoot();
      } else if (to == "..") {
        currentDir = currentDir->goUp();
      } else {
        currentDir = currentDir->goDown(to);
      }
      line.clear();
    }

    if (line.starts_with("$ ls")) {
      try {
        while (true) {
          if (!std::getline(file, line)) {
            throw std::runtime_error("end of file");
          }
          if (line.empty()) {
            break;
          } else if (std::regex_search(line, numberMatch, numberRegex)) {
            auto size = std::stoul(numberMatch.str());
            if (std::regex_search(line, fileNameMatch, fileNameRegex)) {
              currentDir->addFile(fileNameMatch.str(), size);
            }
          } else if (line.starts_with("dir ")) {
            auto dirName = line.substr(4);
            currentDir->goDown(dirName);
          } else {
            break;
          }
        }
      } catch (const std::runtime_error &e) {
        break;
      }
    }
  }
  return currentDir->goToRoot();
}

int main() {
  auto dir = std::make_shared<Dir>("/", nullptr);

  dir = readAndCreateFilesystem("input.txt", dir);

  size_t limit{100000};

  auto dirsSmallerThanLimit = dir->findDecendantsSmallerThan(limit);

  // sum of all dirs smaller than limit
  size_t sumOfDirsSmallerThanLimit{0};
  for (const auto &dir : dirsSmallerThanLimit) {
    sumOfDirsSmallerThanLimit += dir->calculateTotalSize();
  }

  auto sizeOfRoot = dir->calculateTotalSize();
  if (sizeOfRoot < limit) {
    sumOfDirsSmallerThanLimit += sizeOfRoot;
  }

  std::cout << "Found " << dirsSmallerThanLimit.size()
            << " dirs smaller than limit" << std::endl;

  std::cout << sumOfDirsSmallerThanLimit << std::endl;

  std::ofstream file;
  file.open("output.xml", std::ios::out);
  file << *dir;

  return 0;
}
