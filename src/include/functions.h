#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// #include "cli/cli.h"
// doesn't work i guess
// #include "cpp-terminal/terminal.hpp" // will be trying to use this if ftxui sucks (unlikely)

#include "gap_buffer.h"
#include <cmath>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <streambuf>
#include <string>

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
  struct tempText {
    vector<string> text{};
    int lineSum{};
    int charSum{};
  };
  fileInfo getFileInfo(const string);
  void readFile(const string);
  void readftxui(const string);
  int getLineSum(string);
  void writeDummyLines(string, int);
  void openExistingFile(string);
  void gotoLine(string, int);
} // namespace Functions
#endif
