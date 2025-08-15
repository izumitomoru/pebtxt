#include "gap_buffer.h"
#include "functions.h"

namespace GapBuffer {
  using namespace std;
  using namespace Functions;

  // using vectors really sucks but i gotta get something tangible here
  vector<char> createFileBuffer(const string path) {
    fileInfo file{ getFileInfo(path) };

    ifstream sfile(file.path);

    vector<char> buffer{};

    while (sfile.is_open()) {
      // line to read
      string line{};
      // track line number manually
      [[maybe_unused]] int linenum{};

      while (getline(sfile, line)) {
        for (int i{}; i < line.size(); ++i) {
          buffer.push_back(line[i]);
        }
        buffer.push_back('\n');
        // test.push_back(line += '\n');
        // GapBuffer::createTestBuffer();
      }
      sfile.close();
    }
    return buffer;
  }

  void insert(vector<char>& buffer, int pos, string text) {
    for (int i{}; i < text.length(); ++i, ++pos) {
      buffer.insert(buffer.begin() + pos, text[i]);
    }

    for (int i{}; i < buffer.size(); ++i) {
      cout << buffer[i];
    }
  }

} // namespace GapBuffer
