#ifndef FUNCTIONS_H
#define FUNCTIONS_H

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

  fileInfo getFileInfo(const string&);
  lineInfo getLineInfo(vector<char>&, int, int);

  vector<char> createFileBuffer(const string& path);
  void printBuffer(vector<char>& buffer);
  void insert(vector<char>& buffer, const int& pos, const char ch);
  void remove(vector<char>& buffer, const int& pos);

  void readFile(const string);
  void readftxui(const string);
  void writeDummyLines(string, int);
  void openExistingFile(string);
  void mainLoop(const string& path);
  string getLine(const string path, int lineNum);
} // namespace Functions
#endif
