#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <array>
#include <iostream>
#include <vector>

namespace GapBuffer {
  using namespace std;
  vector<char> createFileBuffer(const string path);
  void insert(vector<char>& buffer, int pos, string text);
} // namespace GapBuffer
#endif
