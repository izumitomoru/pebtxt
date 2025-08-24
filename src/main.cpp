#include "functions.h"

// i like to section off most of my code in functions (inefficient as fuck) for my sanity
#if 0
void getinput() {
  using namespace std;
  using namespace Functions;
  cout << "1. New file 2. Existing file 3. Read line 4. Get file info ";
  int choice;
  cin >> choice;

  cout << "Enter path: ";
  string path{};
  cin >> path;
  // string pathanswer;
  // cin >> pathanswer;
  // const string& path{ pathanswer };

  fileInfo file{ Functions::getFileInfo(path) };

  // Open new or existing file
  switch (choice) {
  // new file
  case 1: {
    break;
  }
  case 2: {
    // readFile(path);
    mainLoop();
    break;
  }
  case 3: {
    cout << "Go to line: ";
    int lineNum{};
    cin >> lineNum;
    // cout << "Line " << lineNum << ": " << getLine(path, lineNum);
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
#endif

void testinputs() {
  initscr();
  raw();
  while (true) {
    int ch = getch();
    mvprintw(1, 0, "KEY NAME : %s - 0x%02x\n", keyname(ch), ch);
    if (ch == 'q') {
      return;
    }
    refresh();
  }
}

int main(int argc, char* argv[]) {
  using namespace Functions;
  // getinput();
  // writeDummyLines("./files/burger.txt", 125);
  string path{};
  if (argc == 1) {
    path = getPath();
  } else {
    path = argv[1];
  }
  mainLoop(path);
  // testinputs();
  return 0;
}
