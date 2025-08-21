#include "functions.h"
#include <algorithm>
#include <charconv>
#include <cstring>
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
// 4. on save, overwrite file contents with buffer contents

// print loop
// clear screen (if needed, check if text in a line was updated and only render that one (speed efficient but annoying for right now))
// print buffer contents, making sure everything is in the same place last frame, as well as current line number

// will need:
// checks for line number, a loop to get to iterate to a certain line number when adding and removing lines
//

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
    fileInfo file(getFileInfo("burger.txt"));

    // create file buffer
    vector<char> buffer{ createFileBuffer(file.path) };

    // create screen fitting the terminal
    initscr();

    // window snippet
    // WINDOW* win;
    // win = newwin(COLS, LINES, 0, 0); // height, width, start_y, start_x
    // box(win, 5, 5);                  // Draw a box around the window

    // scroll window snippet
    WINDOW* scroll_win = newwin(COLS, LINES, 0, 0);
    scrollok(scroll_win, TRUE); // Enable scrolling for this window
    refresh();                  // i think this puts the actual window onscreen

    wrefresh(scroll_win);

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

    int scrY{ LINES };
    int scrX{ COLS };
    int scrVerticalHalfpoint{ scrY / 2 };

    // calculate topmost line
    // maybe obsolete now that i have ncurses; it provides a scroll thing for me but i may make a custom function
    // definitely doing a custom function. unironically too lazy to look up how to use windows rn so i'm gonna do like 4 times the effort
    int highestVisibleLine{ (scrVerticalHalfpoint - scrVerticalHalfpoint) + 1 };

    // spaces (calculate this every time backspace or enter is pressed)
    int maxspacenum{};
    int currentspacenum{};
    string spacestr{};

    // line number
    int linenum{ 0 };
    int linesum{ 0 };

    int buf_x{};
    int buf_y{};

    // pre-loop stuff

    bool running{ true };
    int ch{};

    int linestart{};
    int testy{};

    int cur_x{};
    int cur_y{};
    move(0, 0);
    getyx(stdscr, cur_y, cur_x);
    int cursorLine{};

    // main loop
    while (running) {
      // mvprintw(1, 0, "KEY NAME : %s - 0x%02x\n", keyname(ch), ch);

      // read and update file buffer (this will be TERRIBLE btw since i just wanna make a shitty version before the 22nd if possible)
      clear();

      // calculate spaces, line number, then print buffer
      maxspacenum = 0;
      linestart   = 0;
      linesum     = 0;
      for (int i{}; i < buffer.size(); ++i) {
        if (buffer[i] == '\n') {
          ++linesum;
          if (log10(linesum) == static_cast<int>(log10(linesum))) {
            ++maxspacenum;
          }
        }
      }
      maxspacenum = static_cast<int>(log10(linesum));
      linestart   = maxspacenum + 2;
      // mvprintw(testy, 20, "print line start: %d max space num: %d", linestart, maxspacenum);
      // refresh();

      // spacenum = log10(linesum) / 10;

      // print buffer (can't handle scrolling yet)
#if 1
      linenum         = 1;
      currentspacenum = maxspacenum;
      for (int i{}; i < buffer.size(); ++i) {
        // if not newline
        if (buffer[i] != '\n') {
          // spaces and line num must print before other stuff
          for (int i{}; i < maxspacenum; ++i) {
            mvprintw(buf_y, i, " ");
          }
          for (int i{}; i < maxspacenum; ++i) {
            mvprintw(buf_y, currentspacenum, "%d", linenum);
          }

          // i lack knowledge! anyways, get your printf format specifiers straight or it's just like yeahhhh print the whole vector instantly (???????) somehow
          mvprintw(buf_y, linestart + buf_x, "%c", buffer[i]);
          // usleep(10000);
          // refresh();

          ++buf_x;
        } else if (buffer[i] == '\n') {
          // it can't print newline in the window for obvious reasons, idk how i'll really do this aside from manual line tracking
          // mvprintw(buf_y, linestart + buf_x, "%c", buffer[i]);
          mvprintw(buf_y, linestart + buf_x, "%c", '\'');
          buf_x = 0;
          ++buf_y; // in place of a \n, it keeps things smoother i think
          ++linenum;
          // this may fuck up when printing from a direct power of ten, but i'm not sure yet
          if (log10(linenum) == static_cast<int>(log10(linenum)) && linenum != 1) {
            --currentspacenum;
          }
        }
      }
#endif

      // failed attempt to reposition cursor where it should be post print
      // move(cur_y, linestart);
      if (cur_x < linestart) {
        cur_x = linestart;
      }
      move(cur_y, cur_x);

      refresh();
      buf_y = 0;
      buf_x = 0;

      // input
      switch (ch = getch()) {
      // application functions
      case ctrl('q'): {
        endwin();
        running = false;
        break;
      }
      case ctrl('s'): {
        // save
        break;
      }

      // arrow keys
      case KEY_UP: {
        --cur_y;
        break;
      }
      case KEY_DOWN: {
        ++cur_y;
        break;
      }
      case KEY_RIGHT: {
        if (inch() == '\'') {
          break;
        }

        ++cur_x;
        break;
      }
      case KEY_LEFT: {
        // idk if i really need two checks for this but whatever
        if (cur_x < linestart) {
          cur_x = linestart;
          break;
        }
        --cur_x;
        break;
      }
      }
    }
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

    [[maybe_unused]] int i{ 0 };
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
