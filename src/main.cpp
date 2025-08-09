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
  }
  case 2: {
    Functions::openExistingFile(path);
  }
  case 3: {
    Functions::gotoLine(path);
  }
  case 4: {
    cout << "Filename: " << file.path << '\n'
         << "Filename: " << file.filename << '\n'
         << "Lines: " << file.lineSum << '\n'
         << "Characters: " << file.charSum << '\n';
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
