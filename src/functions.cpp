#include "functions.h"

// TODO:
/// REFORM CODEBASE!!!
// add mouse scrolling (maybe not, this looks awful on ncurses)
// fix extra newline sometimes saving at the end of the file
// possibly implement scrolling past final line, not difficult i imagine but likely needlessly time consuming
// add more command mode features
// add auto indent and auto scope close
// add cursor x position memory
// add ask for filename when saved if no filename previously provided

namespace Functions {

  const string defaultpath{ "./" };

  // making very inefficient copies at the moment, but it's workable
  fileInfo getFileInfo(string path) {
    // slightly flawed, but whatever
    string cachepath{};
    if (path[0] != '/' && path[0] != '~' && path[0] != '.') {
      // cachepath = defaultcachepath + path;
      path = defaultpath + path;
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

  lineInfo getLineInfo(vector<char>& buffer, int lineNum, int linestart) {
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
      int lengthtrue = linestr.length();
      int length{ lengthtrue - 1 };

      int lineend     = linestart + length;
      int lineendtrue = linestart + lengthtrue;

      lineInfo line{
        lineNum,
        offset,
        length,
        lengthtrue,
        linestart,
        lineend,
        lineendtrue,
        linestr,
      };
      return line;
    } else {
      lineInfo line{
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        "",
      };
      return line;
    }
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

  void saveFile(vector<char>& buffer, string path) {
    fstream outputfile(path, fstream::in | fstream::out | fstream::trunc);
    for (int i{}; i < buffer.size(); ++i) {
      outputfile << buffer[i];
    }
  }

  // scroll
  // you don't need to know the current cursor num to scroll up properly, that will be handled as it happens
  void scrollUp(int& toplinenum, int& cursorlinenum) {
    if (toplinenum == 1)
      return;
    if (toplinenum != 1 || cursorlinenum != 1) {
      --toplinenum;
      --cursorlinenum;
    } else {
      return;
    }
  }

  void scrollDown(int& bottomlinenum, int& toplinenum, int& linesum) {
    if (bottomlinenum != linesum && toplinenum > 1) {
      ++toplinenum;
    } else {
      return;
    }
  }

  string getPath() {
    cout << "Enter path: ";
    string path{};
    cin >> path;

    return path;
  }

  void mainLoop(string path) {
    // get file info
    fileInfo file(getFileInfo(path));
    // fileInfo file(getFileInfo("burger.txt"));

    // create file buffer
    vector<char> buffer{ createFileBuffer(file) };

    // create screen fitting the terminal
    initscr();
    if (has_colors()) {
      start_color();
      init_color(8, 0, 90, 255); // first parameter is the ID
    }

    raw();
    set_escdelay(0);

    // disable echoing keyboard input directly
    noecho();
    // enable keypad and function keys
    keypad(stdscr, TRUE);

    /// loop variables

    // spaces
    int maxspacenum{};
    string spacestr{};

    // lines
    int linenum{ 0 };

    int linesum{ 0 };
    int toplinenum{ 1 };
    int bottomlinenum{};
    int bufferlineoffset{};

    int linestart{};
    int lineend{};
    int lineendtrue{};

    int midpoint{};
    int height{};
    int difference{};

    // cursor
    int textcurpos{};

    int cur_x{};
    int cur_y{};

    int cursorlinenum{ 1 };

    move(0, 0);
    getyx(stdscr, cur_y, cur_x);

    // scrolling
    int scrollamount{ 12 };

    int buf_x{};
    int buf_y{};

    // vim-like command mode so i can stop fucking using arrow keys
    bool cmdmode{ true };

    lineInfo check_line{};
    lineInfo currentline{};

    //  loop stuff
    bool running{ true };
    int ch{};

    string forbiddenchars{ "asdf" };
    bool saved{ true };

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

      linenum       = toplinenum;
      bottomlinenum = toplinenum + LINES - 1;
      lineInfo line{ getLineInfo(buffer, toplinenum, linestart) };
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
      lineInfo check_line = getLineInfo(buffer, cursorlinenum, linestart);
      // top border
      if (cursorlinenum < 1 && cur_y < 0) {
        cursorlinenum = 1;
        cur_y         = 0;
      }

      // checking before every refresh is probably better but it does annoy me
      if (cmdmode) {
        if (cur_x > check_line.lineend)
          cur_x = check_line.lineend;
      } else {
        if (cur_x > check_line.lineendtrue)
          cur_x = check_line.lineendtrue;
      }
      if (cur_x < linestart)
        cur_x = linestart;

      // i really overcomplicated this for practically no gain, and this is likely even slower than it was. but i LOVE IT!!!!!!! also i'm like really fucking out of it and this shit looks awesome
      // nah but i'm probably gonna undo like an hour and a half of work cuz i'm neurotic

      lineInfo currentline = getLineInfo(buffer, cursorlinenum, linestart);
      // lineInfo aboveLine   = getLineInfo(buffer, cursorlinenum - 1, linestart);
      // lineInfo belowLine   = getLineInfo(buffer, cursorlinenum + 1, linestart);
      textcurpos  = currentline.offset + (cur_x - linestart);
      lineend     = currentline.lineend;
      lineendtrue = currentline.lineendtrue;
      // if (currentline.length != 0) {
      //  lineend = linestart + currentline.length; // - 1 for newline character
      //} else
      //  lineend = linestart + currentline.length;

      // line check booleans
      bool firstlinevisible = (toplinenum == 1);
      bool attopline        = (cursorlinenum == toplinenum);
      bool atbottomline     = (cursorlinenum == bottomlinenum);
      bool atfirstline      = (cursorlinenum == 1);
      bool atlastline       = (cursorlinenum == linesum);
      bool lastlinevisible  = (bottomlinenum == linesum);
      bool atlinestart      = (cur_x == linestart);
      bool atlineend        = (cur_x >= lineend);
      bool atlineendtrue    = (cur_x >= lineendtrue);
      bool screenfull       = (bottomlinenum >= LINES);
      bool y0               = (cur_y == 0);
      bool yfull            = (cur_y + 1 == LINES);

      // midpoint = (bottomlinenum > toplinenum) / 2;
      midpoint   = (LINES) / 2;
      difference = abs(midpoint - cur_y);

      //// PRINT INFO PRINT ////
      mvprintw(LINES / 2, 100, "linesum: %d, currentline: %d, cursorlinenum: %d, log10 linesum: %d, buffer size: %d, bottomlinenum: %d, toplinenum: %d", linesum, currentline.linenum, cursorlinenum, static_cast<int>(log10(linesum)), static_cast<int>(buffer.size()), bottomlinenum, toplinenum);
      mvprintw(LINES / 2 + 1, 100, "cur_y: %d, cur_x: %d", cur_y, cur_x);

      move(cur_y, cur_x);
      refresh();

      //// input
      ch = getch();

      /// command mode
      if (cmdmode) {
        switch (ch) {
        //// application/file functions
        case ctrl('q'): {
        thing:
          if (saved == true) {
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
          cmdmode = false;
          break;
        }
        case 'I': {
          cmdmode = false;
          cur_x   = linestart;
          break;
        }
        case 'a': {
          cmdmode = false;
          if (atlineendtrue)
            break;

          ++cur_x;
          break;
        }
        case 'A': {
          cmdmode = false;
          if (atlineend) {
            break;
          }
          cur_x = lineendtrue;
          break;
        }

        // create newline and move cursor to it
        case 'o': {
          saved   = false;
          cmdmode = false;
          insert(buffer, currentline.offset + currentline.lengthtrue, '\n');

          cur_x = linestart;

          if (atbottomline && screenfull) {
            ++cursorlinenum;
            ++toplinenum;
            break;
          }
          ++cursorlinenum;
          if (atbottomline && screenfull)
            break;
          ++cur_y;
          break;
        }
        // same thing but above (inefficient i knowt)
        case 'O': {
          saved   = false;
          cmdmode = false;

          insert(buffer, currentline.offset, '\n');
          cur_x = linestart;
          break;
        }

        // center line
        case 'z': {
          //          if (firstlinevisible || !screenfull || atlastline)
          //            break;

          if (difference >= 0) {
            toplinenum    = difference;
            cursorlinenum = difference;
            cur_y         = midpoint;
          } // else if (difference < 0) {
          //  toplinenum -= difference;
          //  cursorlinenum -= difference;
          //  cur_y = midpoint;
          //} else {
          //}

          break;
        }

        /// navigation keys
        case '0': {
          cur_x = linestart;
          break;
        }
        case '$': {
          cur_x = lineend;
          break;
        }

        case ctrl('u'): {
          if (cursorlinenum - scrollamount < 1) {
            cur_y         = 0;
            toplinenum    = 1;
            cursorlinenum = 1;
            break;
          }

          lineInfo scrollline = getLineInfo(buffer, cursorlinenum - scrollamount, linestart);

          if (cursorlinenum - scrollamount >= toplinenum) {
            cursorlinenum -= scrollamount;
            cur_y -= scrollamount;
          }

          else if (cursorlinenum - scrollamount < toplinenum) {
            toplinenum += scrollline.linenum - toplinenum;
            cursorlinenum -= scrollamount; // keep
            cur_y = 0;
          }

          if (cur_x > scrollline.lineend)
            cur_x = scrollline.lineend;
          break;
        }

        case ctrl('d'): {
          if (cursorlinenum + scrollamount > linesum) {
            cur_y = LINES - 1;
            toplinenum += linesum - bottomlinenum;
            cursorlinenum = linesum;
            break;
          }

          lineInfo scrollline = getLineInfo(buffer, cursorlinenum + scrollamount, linestart);

          if (cursorlinenum + scrollamount <= bottomlinenum) {
            cursorlinenum += scrollamount;
            cur_y += scrollamount;
          }

          else if (cursorlinenum + scrollamount > bottomlinenum) {
            toplinenum += scrollline.linenum - bottomlinenum;
            cursorlinenum += scrollamount;
            cur_y = LINES - 1;
          }

          if (cur_x > scrollline.lineend)
            cur_x = scrollline.lineend;

          break;
        }

        case 'h': {
          if (atlinestart)
            break;
          --cur_x;
          break;
        }
        case 'j': {
          if (atlastline)
            break;

          if (yfull) {
            ++toplinenum;
            ++cursorlinenum;
            break;
          }

          ++cursorlinenum;
          ++cur_y;
          break;
        }
        case 'k': {
          if (atfirstline)
            break;

          if (y0) {
            --toplinenum;
            --cursorlinenum;
            break;
          }

          --cursorlinenum;
          --cur_y;
          break;
        }
        case 'l': {
          // if (cur_x > lineend - 2)
          if (atlineend)
            break;
          ++cur_x;
          break;
        }
        }
      }

      /// input mode
      else {
        switch (ch) {
          // exit input mode
        case 27: { // i guess you can't use '0x1b' for escape, it has to be 27
          cmdmode = true;
          if (atlinestart)
            break;
          --cur_x;
          break;
        }

        // navigation (arrow keys etc.)
        case KEY_UP: {
          if (atfirstline)
            break;

          if (cur_y == 0) {
            --toplinenum;
            --bottomlinenum;
            --cursorlinenum;
            break;
          }

          --cursorlinenum;
          --cur_y;
          break;
        }
        case KEY_DOWN: {
          if (atlastline)
            break;

          if (cur_y + 1 == LINES) {
            ++toplinenum;
            ++cursorlinenum;
            break;
          }

          ++cursorlinenum;
          ++cur_y;
          break;
        }
        case KEY_RIGHT: {
          if (atlineendtrue)
            break;
          ++cur_x;
          break;
        }
        case KEY_LEFT: {
          if (atlinestart) {
            break;
          }
          --cur_x;
          break;
        }

        case '\n': {
          // testing

          saved = false;
          insert(buffer, textcurpos, '\n');
          if (atbottomline) {
            ++cursorlinenum;
            ++toplinenum;
            break;
          }
          ++cursorlinenum;
          cur_x = linestart;

          ++cur_y;

          break;
        }
        case KEY_BACKSPACE: {
          // do nothing on 0,0
          if (atfirstline && atlinestart)
            break;

          saved = false;

          // regular char
          if (!atlinestart) {
            remove(buffer, textcurpos - 1);
            --cur_x;
            break;
          }

          // backspacing on blankline
          else if (atlinestart) {
            // get length of line above it
            lineInfo above{ getLineInfo(buffer, cursorlinenum - 1, linestart) };
            remove(buffer, textcurpos - 1);

            cur_x = linestart + above.lengthtrue;

            if (attopline && !firstlinevisible) {
              --toplinenum;
              --cursorlinenum;
              break;
            }

            --cursorlinenum;

            if (firstlinevisible) {
              --cur_y;
              break;
            }

            // if you don't want the ability to scroll past final line
            if (lastlinevisible) {
              --toplinenum;
              break;
            }

            --cur_y;

            break;
          }

          // regular char removal
          break;
        }
        // delete
        case KEY_DC: { // can't be 0x7f, must be 127
          saved = false;

          if (atlineend && atlastline)
            break;

          remove(buffer, textcurpos);

          if (!atlineend)
            break;

          if (!firstlinevisible && lastlinevisible) {
            --toplinenum;
            ++cur_y;
          }

          break;
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
} // namespace Functions
