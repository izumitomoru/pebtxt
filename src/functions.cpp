#include "functions.h"
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ncurses.h>
#include <string>

// TODO:
// improve read function so it can print just the size of the terminal, and from a chosen position in the file
// get started on learning ftxui elements and input making it work with buffers for initial edit functions and stuff

// model of what should happen when opening a file
// 1. create screen to fit terminal size
// 2. get path input and create file buffer
// 3. loop to print buffer on and wait for user inputs (scroll, hit key or backspace)
// 4. at end of loop, overwrite file contents with buffer contents

namespace Functions {

  // default path for files
  const string defaultpath{ "./files/" };
  const string defaultcachepath{ "./cache/" };
  // const string defaultpath{ "/run/media/pebarch/pebdrive/_Code/C++/filetests/files/" };
  // const string cachepath{ "/run/media/pebarch/pebdrive/_Code/C++/filetests/cache/" };

  // making very inefficient copies at the moment, but it's workable
  fileInfo getFileInfo(string path) {
    // slightly flawed, but whatever
    string cachepath{};
    if (path[0] != '/' && path[0] != '~' && path[0] != '.') {
      cachepath = defaultcachepath + path;
      path      = defaultpath + path;
    } else if (path[0] == '/' || path[0] == '~' || path[0] == '.') {
    }

    // get total number of lines, allowing for proper padding
    string line;
    ifstream sfile(path);
    int lineSum{};
    int charSum{};
    while (getline(sfile, line)) {
      lineSum++;
      charSum += line.length();
    }
    sfile.close();

    fileInfo info{
      path,
      path,
      cachepath,
      lineSum,
      log10(lineSum),
      charSum,
    };

    return info;
  }

  void writeDummyLines(string path, int linenum) {
    // thing to write dummy lines
    ofstream sfile(path, fstream::out | fstream::trunc);
    for (int i{ 1 }; i <= linenum; ++i) {
      sfile << "line " + to_string(i) << '\n';
    }
    sfile.close();
  }

  // rewrote this on so much sleep deprivation, i tried to fix the 1 number difference in this state but failed
  string getLine(const string path, int lineNum) {
    using namespace GapBuffer;
    fileInfo file{ getFileInfo(path) };
    vector<char> buffer{ createFileBuffer(file.path) };

    string line{};
    int currentlinenum{ 1 };

    // print buffer contents
    for (int i{}; currentlinenum < lineNum + 1; ++i) {
      // cout << "iter: " << i << '\n';
      if (buffer[i] == '\n') {
        ++currentlinenum;
      }

      if (currentlinenum == lineNum && buffer[i] != '\n') {
        // cout << "test\n";
        line += buffer[i];
      }
    }

    // cout << "Line " << currentlinenum - 1 << ": " << line;
    return line;
  }

  void readFile(const string path) {
    using namespace GapBuffer;
    // get file info and create gap buffer
    fileInfo file{ getFileInfo(path) };
    // const string path{ file.path };
    cout << "Path: " << file.path << '\n';
    vector<char> buffer{ createFileBuffer(file.path) };

    // get spaces
    int spacenum = file.lineLog10;
    string spacestr{};
    for (int i{ 0 }; i < spacenum; ++i) {
      // idk which one of these is the most efficient
      spacestr += ' ';
      // spacestr = spacestr + ' ';
      // spacestr.append(" ");
    }

    // horrific system i will change when the first line onscreen isn't the first
    int linenum{ 1 };
    // print for first line specifically
    cout << spacestr << linenum << ' ';

    // print buffer contents
    for (int i{}; i < buffer.size(); ++i) {
      // if not newline, print character
      if (buffer[i] != '\n') {
        cout << buffer[i];
      } else {
        // print newline
        cout << buffer[i];

        // increment line number
        ++linenum;

        // check if line number is a power of 10
        if (log10(linenum) == static_cast<int>(log10(linenum))) {
          // cout << "is int\n";
          // decrement spacenum BEFORE resizing, this took 20 - 30 minutes to figure out, my brain's so fried
          --spacenum;
          spacestr.resize(spacenum);
        }

        // print line
        cout << spacestr << linenum << ' ';
      }
    }
  }

  string getPath() {
    cout << "Enter path: ";
    string path{};
    cin >> path;

    return path;
  }

  void mainLoop() {
    using namespace GapBuffer;
    using namespace ftxui;

    // get file info
    // fileInfo file(getFileInfo(getPath()));

    //// create file buffer
    // vector<char> buffer{ createFileBuffer(file.path) };

    // create screen fitting the terminal
    initscr();

    // raw() makes all input raw, cbreak() returns everything else directly
    // also apparently for ctrl+q you NEED to use raw()
    // cbreak();
    raw();

    // don't echo during getch()
    noecho();
    // enable keypad and function keys
    keypad(stdscr, TRUE);

    // loop variables

    // line count, cursor position, screen dimensions, vertical half point
    int currentline{ 1 };
    int cursorpos_x{};
    int cursorpos_y{};
    int screenY{ LINES };
    int screenX{ COLS };
    int screenVerticalHalfpoint{ screenY / 2 };

    // calculate topmost line
    // maybe obsolete now that i have ncurses; it provides a scroll thing for me but i may make a custom function
    int highestVisibleLine{ (screenVerticalHalfpoint - screenVerticalHalfpoint) + 1 };

    // screen elements

    // window snippet
    WINDOW* win;
    win = newwin(COLS, LINES, 5, 5); // height, width, start_y, start_x
    box(win, 0, 0);                  // Draw a box around the window

    // scroll window snippet
    WINDOW* scroll_win = newwin(10, 30, 0, 0);
    scrollok(scroll_win, TRUE); // Enable scrolling for this window
    for (int i = 0; i < 20; ++i) {
      wprintw(scroll_win, "Line %d\n", i);
    }
    wrefresh(scroll_win);

    bool running{ true };

    char ch{};
    while (running) {
      wrefresh(win); // Refresh to show the box
      mvprintw(1, 0, "KEY NAME : %s - 0x%02x\n", keyname(ch), ch);
      switch (ch = getch()) {
      case ctrl('q'): {
        mvprintw(0, 0, "exiting");
        endwin();
        running = false;
      }
      case ctrl('a'): {
        mvprintw(0, 0, "ctrl+a");
      }
      }
      // switch (ch = getch()) {
      // case '0x1B': {
      //  endwin();
      //  running = false;
      //}
      //}
      refresh();
    }

    endwin();

    // screen size of 10, cursor is at y pos 6,
  }

  void mainLoopftxui() {
    using namespace GapBuffer;
    using namespace ftxui;

    // get file info
    fileInfo file(getFileInfo(getPath()));

    // create file buffer
    vector<char> buffer{ createFileBuffer(file.path) };

    // create screen fitting the terminal
    auto screen = Screen::Create(Dimension::Full());
    // screen.
    // ScreenInteractive screen = ScreenInteractive::TerminalOutput();

    // loop variables

    // line count, cursor position, screen dimensions, vertical half point
    int currentline{ 1 };
    int cursorpos_x{};
    int cursorpos_y{};
    int screenDimY{ screen.dimy() };
    int screenDimX{ screen.dimx() };
    int screenVerticalHalfpoint{ screenDimY / 2 };

    // calculate topmost line
    int highestVisibleLine{ (screenVerticalHalfpoint - screenVerticalHalfpoint) + 1 };

    // screen elements
    Element bufferDisplay{};
    Element winBorder = window(text("window title"), frame(text("test")));

    bool running{ true };

    int i{ 0 };
    // while (running != false) {
    for (i; i < 500; ++i) {
      // screen.Active();
      // Render(screen, winBorder);
      screen.ResetPosition(true);
      Render(screen, text(to_string(i)));
      screen.Print();
      // screen.Clear();
    }

    // screen size of 10, cursor is at y pos 6,

    // idk
    Element thing = paragraph("");

    // Element testframe = frame(thing);
    // Element winBorder = window(text(""), testframe);
    Element testtext = text("test thing");

    // Render(screen, winBorder);
    // screen.Print();

    // screen.ResetPosition(true);

    // print screen
    // Render(screen, thing);
    // Render(screen, contents);
  }

  // probably not even needed
  // void openExistingFile(string path) {
  //  // just wrote over my whole functions.cpp file lol thank god for undo
  //  // changing this shit to burger.txt ONLY
  //  // writeDummyLines("./files/burger.txt", 105);

  //  fileInfo file{ getFileInfo(path) };
  //  cout << "Path is " << file.path << '\n';
  //  cout << "Cache path is " << file.cachepath << '\n';

  //  readFile(file.path);

  //  // file >> line;

  //  // assuming you need a loop for all lines, i'll experiment with that

  //  // writing to file goes here i guess
  //}

} // namespace Functions
