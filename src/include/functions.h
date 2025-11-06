#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

// cool snippet i stole from stack overflow
#define ctrl(x) ((x) & 0x1f)
#define esc 27

namespace Functions
{

  using namespace std;

  struct lineInfo {
    int    linenum{};
    int    offset{};
    int    length{};
    int    lengthtrue{};
    int    lineend{};
    int    lineendtrue{};
    string str{};
  };

  lineInfo getLineInfo(vector<char>&, int, int);

  vector<char> createFileBuffer(const string& path);
  void         insert(vector<char>& buffer, const int& pos, const char ch);
  void         remove(vector<char>& buffer, const int& pos);

  void writeDummyLines(string, int);
  void mainLoop(const string& path);

} // namespace Functions
#endif
