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

void test() {
}

int main() {
  // getinput();

  // test();

  using namespace ftxui;

  // Create a simple document with three text elements.
  Element document = hbox({
      text("left") | border,
      text("middle") | border | flex,
      text("right") | border,
  });

  // Create a screen with full width and height fitting the document.
  auto screen = Screen::Create(
      Dimension::Full(),       // Width
      Dimension::Fit(document) // Height
  );

  // Render the document onto the screen.
  Render(screen, document);

  // Print the screen to the console.
  screen.Print();

  return 0;
}
