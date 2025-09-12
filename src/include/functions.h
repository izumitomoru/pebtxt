#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// #include "cpp-terminal/terminal.hpp" // another option
// okay so i was gonna use ftxui but frankly i have no idea how to make it work so FUCK IT!!!! c++ ncurses

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

// cool snippet i stole from stack overflow
#define ctrl(x) ((x) & 0x1f)
#define esc 27

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
    int linenum{};
    int offset{};
    int length{};
    int lengthtrue{};
    int linestart{};
    int lineend{};
    int lineendtrue{};
    string str{};
  };

  struct bufferInfo {
    vector<char>& buffer;
    int& topline;
    int& bottomline;
    int& linesum;
    lineInfo& currentline;
    int& linestart;
    int& cursorlinenum;
    int& cur_y;
    int& cur_x;
    int& scrheight;
    int& scrwidth;
    bool& cmdmode;
  };

  fileInfo getFileInfo(const string);
  lineInfo getLineInfo(vector<char>&, int, int);

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
