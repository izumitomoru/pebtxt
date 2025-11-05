#include "functions.h"

// TODO:
// REFORM CODEBASE!!!
// consolidate stuff into reusable functions to reduce redundancy (this will look something like moving the main loop to main() instead of using mianLoop)
// add mouse scrolling (maybe not, this looks awful on ncurses)
// fix extra newline sometimes saving at the end of the file
// possibly implement scrolling past final line, not difficult i imagine but likely needlessly time consuming
// add more command mode features
// add auto indent and auto scope close
// add ask for filename when saved if no filename previously provided

// TODO: important!!!
// use buffers instead of vectors, or make multiple vectors to handle large files; right now loading large files loads into a single vector and is super slow
// add undo and redo
// add rebinding keys
// add text wrapping or side scrolling

namespace Functions {

  // read file and write to buffer
  vector<char> createFileBuffer(const string& path) {
    vector<char> buffer{};
    string line{};
    int charSum{};
    ifstream sfile(path);

    // if file doesn't exist, create a blank buffer with a single newline
    if (sfile.fail()) {
      buffer.push_back('\n');
      sfile.close();
      return buffer;
    }

    // write to buffer
    while (sfile.is_open()) {
      // extract lines
      while (getline(sfile, line)) {
        for (int i{}; i < line.size(); ++i) {
          buffer.push_back(line[i]);
          charSum++;
        }
        buffer.push_back('\n');
      }
      sfile.close();
    }

    return buffer;
  }

  void saveFile(const vector<char>& buffer, const string& path) {
    fstream outputfile(path, fstream::in | fstream::out | fstream::trunc);
    for (int i{}; i < buffer.size(); ++i) {
      outputfile << buffer[i];
    }
  }

  lineInfo getLineInfo(vector<char>& buffer, const int lineNum, const int linestart) {
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

  //// scrolling ////

  // screen
  void scrollscr(const int& amount, const bufferInfo& bufinfo) {
    // catch going past first line
    if (bufinfo.topline + amount <= 1) {
      bufinfo.topline = 1;
    }
    // catch going past last line
    else if (bufinfo.bottomline + amount >= bufinfo.linesum) {
      bufinfo.topline += bufinfo.linesum - bufinfo.bottomline;
    } else {
      bufinfo.topline += amount;
    }
  }

  // cursor
  void scrollcur(const int& amount, const bufferInfo& bufinfo) {
    // most of this code is pretty messy

    // catch going past first line
    if (bufinfo.cursorlinenum + amount <= 1) {
      scrollscr(amount, bufinfo);
      bufinfo.cursorlinenum = 1;
      bufinfo.cur_y         = 0;
      return;
    }
    // catch going past last line
    else if (bufinfo.cursorlinenum + amount > bufinfo.linesum) {
      bufinfo.cursorlinenum = bufinfo.linesum;
      if (bufinfo.bottomline == bufinfo.linesum && bufinfo.bottomline < bufinfo.scrheight)
        bufinfo.cur_y = bufinfo.bottomline - 1;
      else
        bufinfo.cur_y = bufinfo.scrheight - 1;
      scrollscr(amount, bufinfo);
      return;
    }

    // going past top line
    if (bufinfo.cursorlinenum + amount < bufinfo.topline) {
      scrollscr((bufinfo.cursorlinenum + amount) - bufinfo.topline, bufinfo);
      bufinfo.cursorlinenum += amount;
      bufinfo.cur_y = 0;
      return;
    }
    // going past bottom line
    else if (bufinfo.cursorlinenum + amount > bufinfo.bottomline) {
      scrollscr((bufinfo.cursorlinenum + amount) - bufinfo.bottomline, bufinfo);
      bufinfo.cur_y = bufinfo.scrheight - 1;
      bufinfo.cursorlinenum += amount;
      return;
    }
    bufinfo.cursorlinenum += amount;
    bufinfo.cur_y += amount;
  }

  //// text manipulation ////

  void insert(vector<char>& buffer, const int& pos, const char ch) {
    buffer.insert(buffer.begin() + pos, ch);
  }

  void remove(vector<char>& buffer, const int& pos) {
    buffer.erase(buffer.begin() + pos);
  }

  array<char, 3> open{ '\(', '\[', '\{' };
  array<char, 3> close{ ')', ']', '}' };

  // autoPair for parentheses, quotes etc.
  void autoPair(vector<char>& buffer, int pos, char ch) {
    char pairch{};

    // switch (ch) {
    // case open[0]: {
    //  pairch = ')';
    //}
    //}
    //// if new pair
    // if (buffer[pos] != ch && buffer[pos - 1] != ch) {
    //  insert(buffer, pos, ch);
    //  insert(buffer, pos - 1, pairch);
    //}
  }

  void mainLoop(const string& path) {
    // create file buffer
    vector<char> buffer{ createFileBuffer(path) };

    // create screen fitting the terminal
    initscr();
    if (has_colors()) {
      start_color();
      init_color(8, 0, 90, 255); // first parameter is the ID
    }

    // give all input control to the program
    raw();
    set_escdelay(0);

    // disable automatically echoing keyboard input
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

    int cur_x_mem{};

    int cursorlinenum{ 1 };

    move(0, 0);
    getyx(stdscr, cur_y, cur_x);

    // scrolling
    int scrolldist{ 7 };

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

    const bufferInfo bufinfo{
      buffer,
      toplinenum,
      bottomlinenum,
      linesum,
      currentline,
      linestart,
      cursorlinenum,
      cur_y,
      cur_x,
      LINES,
      COLS,
      cmdmode,
    };

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
        }
      }
      maxspacenum = static_cast<int>(log10(linesum));
      // + 2 is for the space between line number and text
      linestart = maxspacenum + 2;

      // print buffer

      // keep track of the topmost visible line, and print from it

      linenum = toplinenum;

      // i failed to make a single line that handles both cases, i just don't have the tools for it
      if (toplinenum == 1 && linesum <= LINES)
        bottomlinenum = linesum;
      else
        bottomlinenum = toplinenum + LINES - 1;

      lineInfo line{ getLineInfo(buffer, toplinenum, linestart) };
      // reset buffer printing
      buf_y = 0;
      buf_x = 0;

      for (int i{ line.offset }; i < buffer.size(); ++i) {
        // if not newline
        if (buffer[i] != '\n') {
          mvprintw(buf_y, linestart + buf_x, "%c", buffer[i]);

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

      cur_x = cur_x_mem;

      if (cmdmode) {
        if (cur_x > check_line.lineend)
          cur_x = check_line.lineend;
      } else {
        if (cur_x > check_line.lineendtrue)
          cur_x = check_line.lineendtrue;
      }
      if (cur_x < linestart)
        cur_x = linestart;

      lineInfo currentline = getLineInfo(buffer, cursorlinenum, linestart);
      textcurpos           = currentline.offset + (cur_x - linestart);
      lineend              = currentline.lineend;
      lineendtrue          = currentline.lineendtrue;

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

      midpoint   = (LINES) / 2;
      difference = cursorlinenum + (midpoint - bottomlinenum);

      //// PRINT INFO PRINT ////
      // mvprintw(LINES / 2, 100, "linesum: %d, currentline: %d, cursorlinenum: %d, log10 linesum: %d, buffer size: %d, bottomlinenum: %d, toplinenum: %d", linesum, currentline.linenum, cursorlinenum, static_cast<int>(log10(linesum)), static_cast<int>(buffer.size()), bottomlinenum, toplinenum);
      // mvprintw(LINES / 2 + 1, 100, "cur_y: %d, cur_x: %d", cur_y, cur_x);
      // mvprintw(LINES / 2 + 2, 100, "difference: %d", difference);
      // mvprintw(LINES / 2 + 3, 100, "thing: %d, thing2: %d", linesum - toplinenum, linesum - 1);
      // mvprintw(5, 70, "toplinenum: %d", toplinenum);
      // mvprintw(6, 70, "LINES: %d", LINES);
      // mvprintw(7, 70, "linesum: %d", linesum);
      // mvprintw(8, 70, "bottomlinenum: %d", bottomlinenum);
      // mvprintw(9, 70, "testing: %d", testing);

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
          saveFile(buffer, path);

          break;
        }

        //// vim stuff

        // enable input mode
        case 'i': {
          cmdmode = false;
          break;
        }
        case 'I': {
          cmdmode   = false;
          cur_x     = linestart;
          cur_x_mem = cur_x;
          break;
        }
        case 'a': {
          cmdmode = false;
          if (atlineendtrue)
            break;

          ++cur_x;
          cur_x_mem = cur_x;
          break;
        }
        case 'A': {
          cmdmode   = false;
          cur_x     = lineendtrue;
          cur_x_mem = cur_x;
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
        case 'O': {
          saved   = false;
          cmdmode = false;

          insert(buffer, currentline.offset, '\n');
          cur_x = linestart;
          break;
        }

        // center line
        case 'z': {
          if (difference == 0 || linesum <= LINES)
            break;

          scrollscr(difference, bufinfo);

          if (toplinenum == 1)
            cur_y = cursorlinenum - 1;
          else if (bottomlinenum + difference >= linesum)
            cur_y += -(linesum - bottomlinenum);
          else
            cur_y += -difference;

          break;
        }

        /// navigation keys
        case '0': {
          cur_x     = linestart;
          cur_x_mem = cur_x;
          break;
        }
        case '$': {
          cur_x     = lineend;
          cur_x_mem = cur_x;
          break;
        }
        case 'g': {
          if (getch() == 'g')
            scrollcur(-linesum, bufinfo);
          else
            break;
          break;
        }
        case 'G': {
          scrollcur(linesum, bufinfo);
          break;
        }

        // fast scroll
        case ctrl('u'): {
          scrollcur(-scrolldist, bufinfo);
          break;
        }
        case ctrl('d'): {
          scrollcur(scrolldist, bufinfo);
          break;
        }

        // directional
        case 'h': {
          if (atlinestart)
            break;
          --cur_x;
          cur_x_mem = cur_x;
          break;
        }
        case 'j': {
          scrollcur(1, bufinfo);
          break;
        }
        case 'k': {
          scrollcur(-1, bufinfo);
          break;
        }
        case 'l': {
          // if (cur_x > lineend - 2)
          if (atlineend)
            break;
          ++cur_x;
          cur_x_mem = cur_x;
          break;
        }
        }
      }

      /// input mode
      else {
        switch (ch) {
          // exit input mode
        case esc: { // i guess you can't use '0x1b' for escape, it has to be 27
          cmdmode = true;
          if (atlinestart)
            break;
          --cur_x;
          cur_x_mem = cur_x;
          break;
        }

        // navigation (arrow keys etc.)
        case KEY_UP: {
          scrollcur(-1, bufinfo);
          break;
        }
        case KEY_DOWN: {
          scrollcur(1, bufinfo);
          break;
        }
        case KEY_RIGHT: {
          if (atlineendtrue)
            break;
          ++cur_x;
          cur_x_mem = cur_x;
          break;
        }
        case KEY_LEFT: {
          if (atlinestart) {
            break;
          }
          --cur_x;
          cur_x_mem = cur_x;
          break;
        }

        case '\n': {
          saved = false;
          insert(buffer, textcurpos, '\n');

          if (atbottomline && !lastlinevisible) {
            ++cursorlinenum;
            ++toplinenum;
            break;
          } else
            ++cursorlinenum;
          cur_x     = linestart;
          cur_x_mem = cur_x;

          ++cur_y;

          break;
        }
        case KEY_BACKSPACE: {
          if (atfirstline && atlinestart)
            break;

          saved = false;

          // regular char
          if (!atlinestart) {
            remove(buffer, textcurpos - 1);
            --cur_x;
            cur_x_mem = cur_x;
            break;
          }

          // backspacing on blankline
          else if (atlinestart) {
            // get length of line above it
            lineInfo above{ getLineInfo(buffer, cursorlinenum - 1, linestart) };
            remove(buffer, textcurpos - 1);

            cur_x     = linestart + above.lengthtrue;
            cur_x_mem = cur_x;

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
          if (atlineend && atlastline)
            break;

          saved = false;

          remove(buffer, textcurpos);

          if (!atlineend)
            break;

          if (!firstlinevisible && lastlinevisible) {
            --toplinenum;
            ++cur_y;
          }

          cur_x_mem = cur_x;

          break;
        }

        /// autoclose
        case '\(': {
          autoPair(buffer, textcurpos, ch);

          break;

          ++cur_x;
          cur_x_mem = cur_x;
          break;
        }
        // case ')': {
        //  if ()
        //    break;
        //}

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
          cur_x_mem = cur_x;
          break;
        }
        }
      }
    }
  }

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

  void writeDummyLines(string path, int linenum) {
    // thing to write dummy lines
    ofstream sfile(path, fstream::out | fstream::trunc);
    for (int i{ 1 }; i <= linenum; ++i) {
      sfile << "line " + to_string(i) << '\n';
    }
    sfile.close();
  }
} // namespace Functions
