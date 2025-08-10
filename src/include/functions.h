#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// #include "cli/cli.h"
#include "cpp-terminal/terminal.hpp" // will be trying to use this
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
    double lineLog10{};
    int charSum{};
  };
  fileInfo getFileInfo(const string);
  void readFile(const string);
  int getLineSum(string);
  void writeDummyLines(string, int);
  void openExistingFile(string);
  void gotoLine(string, int);
} // namespace Functions
#endif
