#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <array>
#include <iostream>
#include <vector>

namespace GapBuffer {
  using namespace std;
  vector<char> createFileBuffer(const string path);
  void printBuffer(vector<char>& buffer);
  void insert(vector<char>& buffer, int pos, string text);
  void remove(vector<char>& buffer, int pos);
} // namespace GapBuffer
#endif
