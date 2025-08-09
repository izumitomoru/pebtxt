#include "functions.h"

void inputfiletest() {
  using namespace std;
  cout << "Filename: ";
  string filename;
  getline(cin >> ws, filename);
  // cin >> filename;

  fstream file("./files/" + filename, fstream::out | fstream::trunc);

  cout << "File contents: ";
  string contents;
  getline(cin >> ws, contents);
  file << contents;
  file.close();
}

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
    cout << "Filename: ";
    string filename;
    cin >> filename;
    Functions::openExistingFile(filename);
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
