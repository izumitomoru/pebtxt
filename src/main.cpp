#include "functions.h"

void getinput() {
  using namespace std;
  cout << "1. New file 2. Existing file 3. Read line 4. Get file info ";
  int choice;
  cin >> choice;

  cout << "Enter path: ";
  string path;
  cin >> path;

  Functions::fileInfo file{ Functions::getFileInfo(path) };

  // Open new or existing file
  switch (choice) {
  // new file
  case 1: {
    break;
  }
  case 2: {
    Functions::openExistingFile(path);
    break;
  }
  case 3: {
    cout << "Go to line: ";
    int lineNum{};
    cin >> lineNum;
    Functions::gotoLine(path, lineNum);
    break;
  }
  case 4: {
    cout << "Path: " << file.path << '\n'
         << "Filename: " << file.filename << '\n'
         << "Lines: " << file.lineSum << '\n'
         << "Line sum log 10: " << file.lineLog10 << '\n'
         << "Characters: " << file.charSum << '\n';
    break;
  }
  }
}

int main() {
  getinput();

  // inputfiletest();

  // ofstream file("./files/file.txt");
  // fstream file("./files/file.txt", fstream::out | fstream::app);
  // fstream file;
  // file.open("./files/file.txt", fstream::out);

  // fstream file("./files/file.txt", fstream::app);

  // file << "test text";

  // string text;
  // getline(file, text);
  // cout << text;
  // file.close();

  return 0;
}
