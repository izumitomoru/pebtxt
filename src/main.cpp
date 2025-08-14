#include "functions.h"
#include <ftxui/dom/elements.hpp>

// i like to section off most of my code in functions (inefficient as fuck) for my sanity
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
    // Functions::readftxui(path);
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

// terminal testing stuff
void basicscreen() {
  using namespace ftxui;
  auto screen = Screen::Create(
      Dimension::Full(),
      Dimension::Fixed(5));

  auto& pixel = screen.PixelAt(10, 2);

  pixel.character        = U'X';
  pixel.foreground_color = Color::Red;
  pixel.background_color = Color::RGB(0, 255, 0);
  pixel.bold             = true;
  Terminal::Size();
  screen.Print();
  screen.ResetPosition(/*clear=*/true);
}

// try to make a simple screen to output on
void testscreen() {
  using namespace ftxui;
  // Element doc = hbox({ text("Test"), text("test2") });
  // Render(screen, doc);

  Element test      = text("test") | center;
  Element testbox   = vbox({ text("test"), text("test2") }) | center;
  Element testframe = frame(paragraph("burger bob\ntest"));

  Element winBorder = window(text(""), testframe);

  auto screen = Screen::Create(Dimension::Full());
  Render(screen, winBorder);

  screen.ResetPosition(true);
  screen.Print();
  while (true)
    ;
}

int main() {
  getinput();

  // test();
  // basicscreen();
  // testscreen();

  return 0;
}
