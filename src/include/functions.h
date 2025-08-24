#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// #include "cpp-terminal/terminal.hpp" // another option
// okay so i was gonna use ftxui but frankly i have no idea how to make it work so FUCK IT!!!! c++ ncurses

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <memory>
#include <ncurses.h>
#include <stdio.h>
#include <streambuf>
#include <string>
#include <unistd.h>
#include <vector>

// cool snippet i stole from stack overflow
#define ctrl(x) ((x) & 0x1f)

namespace Functions {
  using namespace std;

  struct fileInfo {
    string path{};
    string filename{};
    string cachepath;
    int lineSum{};
    double lineLog10{};
    int charSum{};
  };

  struct lineInfo {
    string str{};
    int linenum{};
    int offset{};
    int length{};
  };

  fileInfo getFileInfo(const string);
  lineInfo getLineInfo(vector<char>&, int);

  vector<char> createFileBuffer(fileInfo);
  void printBuffer(vector<char>& buffer);
  void insert(vector<char>& buffer, int pos, char ch);
  void remove(vector<char>& buffer, int pos);

  string getPath();

  void readFile(const string);
  void readftxui(const string);
  void writeDummyLines(string, int);
  void openExistingFile(string);
  void mainLoop(string path);
  string getLine(const string path, int lineNum);
} // namespace Functions
#endif
