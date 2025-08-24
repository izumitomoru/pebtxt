#include "functions.h"
#include <ncurses.h>

// TODO:
// fix line number log10 check on first 9 lines (some dumb shit i forgot probably)
// handle creating new files, and invalid filenames
// add mouse scrolling (maybe not, this looks awful on ncurse)
// fix extra newline saving sometimes at the end of the file
// possibly implement scrolling past final line, not difficult i imagine but likely needlessly time consuming

// model of what should happen when opening a file
// 1. create screen to fit terminal size
// 2. get path input and create file buffer
// 3. loop to print buffer on and wait for user inputs (scroll, hit key or backspace)
// 4. on save, overwrite file contents with buffer contents

// scrolling model (for now) gonna make it just when you reach the bottom screen part
// 1. track line number, when line number exceeds screen lines, downshift everything
//

namespace Functions {

  // default path for files
  const string defaultpath{ "./files/" };
  const string defaultcachepath{ "./cache/" };

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

  lineInfo getLineInfo(vector<char>& buffer, int lineNum) {
    string linestr{};
    int currentlinenum{ 1 };
    int offset{};

    if (buffer.size() > 1) {
      for (int i{}; currentlinenum <= lineNum; ++i) {
        // add to string if line is proper, when next line found, stop
        if (buffer[i] != '\n' && currentlinenum == lineNum) {
          linestr += buffer[i];
        } else if (buffer[i] == '\n') { // if nl, increment line number
          ++currentlinenum;
        }

        // separate this
        if (buffer[i] == '\n' && currentlinenum == lineNum) {
          // line number matches up,
          offset = i + 1;
        }
      }

      // works
      int length = linestr.length();

      lineInfo line{
        linestr,
        lineNum,
        offset,
        length,
      };
      return line;
    } else {
      lineInfo line{
        "",
        1,
        0,
        0,
      };
      return line;
    }
  }

  // using vectors really sucks but i gotta get something tangible here
  vector<char> createFileBuffer(fileInfo file) {
    // cout << "Path: " << path;
    ifstream sfile(file.path);

    vector<char> buffer{};

    if (file.charSum > 0) {
      while (sfile.is_open()) {
        // line to read
        string line{};
        // track line number manually
        [[maybe_unused]] int linenum{};

        // add file lines to buffer
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
    } else {
      if (file.lineSum > 1) {
        for (int i{}; i < file.lineSum; ++i) {
          buffer.push_back('\n');
        }
        sfile.close();
      } else {
        buffer.push_back('\n');
      }
    }
    // test to see if buffer was created successfully
    // cout << buffer[3] << '\n';
    return buffer;
  }

  void insert(vector<char>& buffer, int pos, char ch) {
    // for (int i{}; i < text.length(); ++i, ++pos) {
    //  buffer.insert(buffer.begin() + pos, text[i]);
    //}
    buffer.insert(buffer.begin() + pos, ch);
  }

  void remove(vector<char>& buffer, int pos) {
    buffer.erase(buffer.begin() + pos);
  }

  void printBuffer(vector<char>& buffer) {
    for (int i{}; i < buffer.size(); ++i) {
      cout << buffer[i];
    }
  }

  void saveFile(vector<char>& buffer, string path) {
    fstream outputfile(path, fstream::in | fstream::out | fstream::trunc);
    for (int i{}; i < buffer.size(); ++i) {
      outputfile << buffer[i];
    }
  }

  string getPath() {
    cout << "Enter path: ";
    string path{};
    cin >> path;

    return path;
  }

  void mainLoop() {
    using namespace ftxui;

    // get file info
    fileInfo file(getFileInfo(getPath()));
    // fileInfo file(getFileInfo("burger.txt"));

    // create file buffer
    vector<char> buffer{ createFileBuffer(file) };

    // create screen fitting the terminal
    initscr();
    if (has_colors()) {
      start_color();
      init_color(8, 0, 90, 255); // first parameter is the ID
    }

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
    set_escdelay(0);
    // qiflush();

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
    int highestVisibleLine{ (scrVerticalHalfpoint - scrVerticalHalfpoint) + 1 };

    // spaces (calculate this every time backspace or enter is pressed)
    int maxspacenum{};
    string spacestr{};

    // line number
    int linenum{ 0 };
    int linesum{ 0 };
    int topmostlinenum{ 1 };
    int bottommostlinenum{};
    int bufferlineoffset{};

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
    int cursorlinenum{ 1 };

    // vim-like command mode so i can stop fucking using arrow keys
    bool command_mode{ true };

    string forbiddenchars{ "asdf" };
    bool saved{ true };

    // ofstream outputFile(file.path);

    // main loop
    while (running) {
      clear();

      // calculate line sum, spaces, line number, then print buffer
      maxspacenum = 0;
      linestart   = 0;
      // line sum
      linesum = 0;
      for (int i{}; i < buffer.size(); ++i) {
        if (buffer[i] == '\n' || buffer.size() == 1) {
          ++linesum;
          // if (log10(linesum) == static_cast<int>(log10(linesum))) {
          //  ++maxspacenum;
          //}
        }
      }
      maxspacenum = static_cast<int>(log10(linesum));
      // + 2 is for the space between line number and text
      linestart = maxspacenum + 2;

      // spacenum = log10(linesum) / 10;

      // print buffer

      // keep track of the topmost visible line, and print from it

      linenum           = topmostlinenum;
      bottommostlinenum = topmostlinenum + LINES - 1;
      lineInfo line{ getLineInfo(buffer, topmostlinenum) }; // problem

      // reset buffer printing
      buf_y = 0;
      buf_x = 0;

      for (int i{ line.offset }; i < buffer.size(); ++i) {
        // if not newline
        if (buffer[i] != '\n') {
          mvprintw(buf_y, linestart + buf_x, "%c", buffer[i]);
          // usleep(10000);
          // refresh();

          ++buf_x;
        } else if (buffer[i] == '\n') {
          // spaces
          mvprintw(buf_y, -static_cast<int>(log10(linenum)) + static_cast<int>(log10(linesum)), "%d", linenum);

          // line number
          init_pair(1, 8, COLOR_BLACK);
          attron(8);
          // this gets offset when topmostline is a power of 10
          attroff(COLOR_PAIR(8));

          buf_x = 0;
          ++buf_y; // in place of a \n, it keeps things smoother i think
          ++linenum;
        }
      }

      // text edge cursor position checks
      lineInfo check_line{ getLineInfo(buffer, cursorlinenum) };
      // top border
      if (cursorlinenum < 1 && cur_y < 0) {
        //++cursorlinenum;
        //++cur_y;
        cursorlinenum = 1;
        cur_y         = 0;
      }
      // line number border
      if (cur_x < linestart) {
        cur_x = linestart;
      }
      // line end border
      if (command_mode) {
        if (cur_x > linestart + check_line.length) {
          cur_x = linestart + check_line.length - 1;
        } else if (check_line.length == 1) {
          cur_x = linestart;
        }
        if (cur_x < linestart) {
          cur_x = linestart;
        }

      } else {
        if (cur_x > linestart + check_line.length) {
          cur_x = linestart + check_line.length;
        }
      }

      lineInfo currentline{ getLineInfo(buffer, cursorlinenum) };
      int textcurpos = currentline.offset + (cur_x - linestart);

      mvprintw(cur_y, 100, "linesum: %d, currentline: %d, cursorlinenum: %d, log10 linesum: %d, buffer size: %d, bottommostlinenum: %d, topmostlinenum: %d", linesum, currentline.linenum, cursorlinenum, static_cast<int>(log10(linesum)), static_cast<int>(buffer.size()), bottommostlinenum, topmostlinenum);

      move(cur_y, cur_x);
      refresh();

      // input
      ch = getch();

      // command mode
      if (command_mode) {
        switch (ch) {
        //// application/file functions
        case ctrl('q'): {
          if (saved == true) {
          thing:
            // clean up
            endwin();
            running = false;
          } else {
            clear();
            printw("Quit without saving? y/n ");
            switch (getch()) {
            case 'y': {
              endwin();
              running = false;
              break;
            }
            case 'n': {
              break;
            }
            default: {
              goto thing;
            }
            }
          }
          break;
        }
        case ctrl('s'): {
          // save
          saved = true;
          saveFile(buffer, file.path);

          break;
        }

        //// vim stuff

        // enable input mode
        case 'i': {
          command_mode = false;
          break;
        }
        case 'I': {
          command_mode = false;
          cur_x        = linestart;
          break;
        }
        case 'a': {
          command_mode = false;
          if (cur_x > linestart + currentline.length)
            break;

          ++cur_x;
          break;
        }
        case 'A': {
          command_mode = false;
          if (cur_x > linestart + currentline.length) {
            break;
          }
          cur_x = linestart + currentline.length;
          break;
        }

        // center line
        case 'z': {
        }

        // movement keys
        case 'h': {
          if (cur_x == linestart)
            break;
          --cur_x;
          break;
        }
        case 'j': {
          if (cursorlinenum == linesum && bottommostlinenum == linesum)
            break;

          // if (cursorlinenum + 1 == LINES) {
          //  ++topmostlinenum;
          //  ++cursorlinenum;
          //  break;
          //}

          // if (cursorlinenum + 1 > LINES /*/ 2*/ && bottommostlinenum != linesum) {
          if (cur_y + 1 == LINES) {
            ++topmostlinenum;
            ++cursorlinenum;
            break;
          }
          ++cursorlinenum;
          ++cur_y;
          break;
        }
        case 'k': {
          if (cursorlinenum == 1)
            break;

          // scroll up
          // middle of the screen scroll
          // if (cur_y > LINES / 2 || topmostlinenum == 1) {
          //  --cursorlinenum;
          //  --cur_y;
          //  break;
          //}
          if (cur_y == 0 && cursorlinenum /*> LINES / 2*/) {
            --topmostlinenum;
            --bottommostlinenum;
            --cursorlinenum;
            break;
          }
          --cursorlinenum;
          --cur_y;
          break;
        }
        case 'l': {
          if (cur_x > linestart + currentline.length - 2)
            break;
          ++cur_x;
          break;
        }
        }
      } else { // not command mode (crazy)
        switch (ch) {
          // exit input mode
        case 27: { // i guess you can't use '0x1b' for escape, it has to be 27
          command_mode = true;
          if (cur_x == linestart)
            break;
          --cur_x;
          break;
        }

        // navigation (arrow keys etc.)
        case KEY_UP: {
          // all this code is pretty messy
          if (cursorlinenum == 1)
            break;

          // scroll up
          // middle of the screen scroll
          // if (cur_y > LINES / 2 || topmostlinenum == 1) {
          //  --cursorlinenum;
          //  --cur_y;
          //  break;
          //}
          if (cur_y == 0 /*&& cursorlinenum > LINES / 2*/) {
            --topmostlinenum;
            --bottommostlinenum;
            --cursorlinenum;
            break;
          }
          --cursorlinenum;
          --cur_y;
          break;
        }
        case KEY_DOWN: {
          if (cursorlinenum == linesum)
            break;

          // scroll down

          if (cursorlinenum + 1 > LINES /*/ 2*/ && bottommostlinenum != linesum) {
            ++topmostlinenum;
            ++bottommostlinenum;
            ++cursorlinenum;
            break;
          }
          ++cursorlinenum;
          ++cur_y;
          break;
        }
        case KEY_RIGHT: {
          if (cur_x > linestart + currentline.length)
            break;

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
          // mouse testing

        case '\n': {
          // testing

          saved = false;
          insert(buffer, textcurpos, '\n');
          if (bottommostlinenum == cursorlinenum) {
            ++cursorlinenum;
            ++topmostlinenum;
            break;
          }
          ++cursorlinenum;
          cur_x = linestart;
          // had some stuff here for scroll on enter but i won't keep it probably

          // if (bottommostlinenum == cursorlinenum || topmostlinenum == 1) {
          //   ++topmostlinenum;
          //   break;
          // }

          // if (topmostlinenum > 1) {
          //   ++topmostlinenum;
          //   ++bottommostlinenum;
          //   break;
          // }

          ++cur_y;

          // insert(buffer, textcurpos, ' ');
          break;
          // insert(buffer, 5, '\n');
        }
        case KEY_BACKSPACE: {
          // do nothing on 0,0
          if (cursorlinenum == 1 && cur_x == linestart)
            break;

          saved = false;

          // regular char
          if (cur_x != linestart) {
            remove(buffer, textcurpos - 1);
            --cur_x;
            break;
          }

          // backspacing on blankline
          else if (cur_x == linestart) {
            // get length of line above it
            lineInfo above{ getLineInfo(buffer, cursorlinenum - 1) };
            remove(buffer, textcurpos - 1);

            if (cursorlinenum == topmostlinenum && topmostlinenum != 1) {
              --topmostlinenum;
              --cursorlinenum;
              break;
            }

            cur_x = linestart + above.length;

            --cursorlinenum;

            // if you don't want the ability to scroll past final line
            if (bottommostlinenum == linesum) {
              --topmostlinenum;
              break;
            }

            // if (cursorlinenum==topmostlinenum&&top)

            // if (topmostlinenum != 1) {
            //  --topmostlinenum;
            //  //--bottommostlinenum;
            //  break;
            //}

            --cur_y;

            break;
          }

          // regular char removal
          break;
        }
        case '0x7F': {
          if (textcurpos == line.length && cursorlinenum == linesum)
            break;

          if (cur_x != linestart) {
            remove(buffer, textcurpos + 1);
            --cur_x;
            break;
          }
        }

        // standard input
        default: {
          // for (int i{}; i < forbiddenchars.length(); ++i) {
          //  if (ch == forbiddenchars[i]) {
          //    break;
          //  }
          //}

          saved = false;
          insert(buffer, textcurpos, ch);
          ++cur_x;
          break;
        }
        }
      }
    }
  }

  void writeDummyLines(string path, int linenum) {
    // thing to write dummy lines
    ofstream sfile(path, fstream::out | fstream::trunc);
    for (int i{ 1 }; i <= linenum; ++i) {
      sfile << "line " + to_string(i) << '\n';
    }
    sfile.close();
  }

#if 0
  void readFile(const string path) {
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
#endif

#if 0
  void mainLoopftxui() {
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
#endif

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
