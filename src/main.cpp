#include "functions.h"

void getinput() {
  using namespace std;
  cout << "1. New file 2. Existing file ";
  int choice;
  cin >> choice;

  // Open new or existing file
  switch (choice) {
  // new file
  case 1: {
  }

  case 2: {
    cout << "Enter path: ";
    string path;
    cin >> path;
    Functions::openExistingFile(path);
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
