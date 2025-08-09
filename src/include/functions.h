#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <streambuf>
#include <string>
#include <vector>

namespace Functions {
  using namespace std;
  struct fileInfo {
    string path{};
    string cachepath;
    string filename{};
    int lineSum{};
    int charSum{};
  };
  fileInfo getFileInfo(const std::string);
  void readFile(const std::string, const std::string);
  int getLineSum(std::string);
  void writeDummyLines(std::string, int);
  void openExistingFile(std::string);
  void gotoLine(std::string);
} // namespace Functions
#endif
