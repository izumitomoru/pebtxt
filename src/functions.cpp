#include "functions.h"
#include <ncurses.h>

// TODO:
// REFACTOR CODEBASE!!!
// add mouse scrolling (maybe not, this looks awful on ncurses)
// fix extra newline sometimes saving at the end of the file
// possibly implement scrolling past final line, not difficult i imagine but likely needlessly time consuming
// add more command mode features
// add auto indent and auto scope close
// add ask for filename when saved if no filename previously provided

// TODO: important!!!
// use buffers instead of vectors, or make multiple vectors to handle large files; right now loading large files loads into a single vector and is super slow
// add undo and redo
// add rebinding keys add text wrapping or side scrolling

// TODO: priority
// rework initial setup to allow for better manipulation

namespace Functions
{

  // read file and write to buffer
  vector<char> createFileBuffer(const string& path) {
    // create buffer and open file
    vector<char> buffer{};
    ifstream     sfile(path);

    // if file doesn't exist, create blank buffer with one \n
    if (sfile.fail()) {
      buffer.push_back('\n');
      return buffer;
    }
    // if file exists, read contents and write to buffer
    else {
      string line{};
      while (getline(sfile, line)) {
        for (int i{}; i < line.size(); ++i) {
          buffer.push_back(line[i]);
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
    int    currentlinenum{ 1 };
    int    offset{};

    if (buffer.size() > 1) {
      for (int i{}; currentlinenum <= lineNum; ++i) {
        // add to string if line is proper, when next line found, stop
        if (buffer[i] != '\n' && currentlinenum == lineNum) {
          linestr += buffer[i];
        }
        else if (buffer[i] == '\n') { // if nl, increment line number
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
        lineend,
        lineendtrue,
        linestr,
      };
      return line;
    }
    else {
      lineInfo line{
        1,
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

  //// text manipulation ////

  void insert(vector<char>& buffer, const int& pos, const char ch) {
    buffer.insert(buffer.begin() + pos, ch);
  }

  void remove(vector<char>& buffer, const int& pos) {
    buffer.erase(buffer.begin() + pos);
  }

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

  // just so we're clear, i know this whole thing is terrible. i just wanna experiment with classes and stuff so i literally converted my whole program into a class
  class Buffer {
private:
    vector<char> buffer{};
    int          printlinenum{ 0 };
    int          linesum{};

    int printcur_y{};
    int printcur_x{};

    // lines
    lineInfo toplineinfo{};

    int topline{ 1 };
    int bottomline{};
    int linestart{};

    int    linenum_padding{};
    string spacestr{};

    // loop stuff
    bool& running;
    int   ch{};

    bool saved{ true };

    // user cursor info
    int cur_y{};
    int cur_x{};
    int cur_x_mem{};
    int textcurpos{};

    int midpoint{};
    int height{};
    int difference{};

    int lineend{};
    int lineendtrue{};
    int cursorlinenum{ 1 };

    const string& path;

    // program info
    int&  scrheight;
    int&  scrwidth;
    bool& cmdmode;
    int&  scrolldist;

public:
    //// constructor ////
    // not bothering with an init() function; i think every time you initialize this you'll want a buffer to be created
    Buffer(const string& path, int& scrheight, int& scrwidth, bool& cmdmode, int& scrolldist, bool& running)
        : path{ path },
          scrheight{ scrheight },
          scrwidth{ scrwidth },
          cmdmode{ cmdmode },
          scrolldist{ scrolldist },
          running{ running } {
      buffer = createFileBuffer(path);
    };

    //// scrolling ////
    // the way i have it set up is that everything is printed starting from the topmost visible line, just be aware of that
    void scrollscr(const int& amount) {
      // catch going up past line 1
      // if calculated top line is less than 1, set to 1
      if (topline + amount <= 1) {
        topline = 1;
      }
      // catch going down past final line
      // if calculated bottom line is greater than linesum, set bottom line to linesum
      else if (bottomline + amount > linesum) {
        topline += linesum - bottomline;
      }
      // otherwise, continue with calculated amount
      else {
        topline += amount;
      }
    }

    void scrollcur(const int& amount) {
      // most of this code is pretty messy

      // catch going up past first line
      // if calculated line number is less than 1, set cursor at line 1
      if (cursorlinenum + amount <= 1) {
        scrollscr(amount);
        cursorlinenum = 1;
        cur_y         = 0;
        return;
      }
      // catch going down past last line
      // if calculated line number is greater than final line, set cursor at final line
      else if (cursorlinenum + amount > linesum) {
        cursorlinenum = linesum;
        // if bottom line is final line and doesn't go past screen height max, set cursor at bottom line
        if (bottomline == linesum && bottomline < scrheight)
          cur_y = bottomline - 1;
        // otherwise, set cursor at screen height max
        else
          cur_y = scrheight - 1;
        scrollscr(amount);
        return;
      }

      // going past top line
      else if (cursorlinenum + amount < topline) {
        scrollscr((cursorlinenum + amount) - topline);
        cursorlinenum += amount;
        cur_y = 0;
        return;
      }
      // going past bottom line
      else if (cursorlinenum + amount > bottomline) {
        scrollscr((cursorlinenum + amount) - bottomline);
        cur_y = scrheight - 1;
        cursorlinenum += amount;
        return;
      }
      cursorlinenum += amount;
      cur_y += amount;
    }

    //// printing ////
    void print() {
      /// reset loop variables ///

      // with this system, if the buffer has no newlines, everything fails to print, so i made it that vectors always have at least one newline
      // increment linesum every newline
      linesum = 0;
      for (int i{}; i < buffer.size(); ++i) {
        if (buffer[i] == '\n' || buffer.size() == 1) {
          ++linesum;
        }
      }

      linestart = static_cast<int>(log10(linesum)) + 2; // make sure lines always start after line number

      // this prevents some funky stuff from happening that SHOULD be getting caught by scroll functions and such, this is a bandaid
      if (topline == 1 && linesum <= LINES)
        bottomline = linesum;
      else
        bottomline = topline + LINES - 1;

      // reset buffer print cursor
      printcur_y = 0;
      printcur_x = linestart;

      // keep track of the topmost visible line, and print from it
      lineInfo toplineinfo = getLineInfo(buffer, topline, linestart);

      // make sure line number is correct
      printlinenum = topline;

      /// print start ///

      // print starting from topmost visible line
      for (int i{ toplineinfo.offset }; i < buffer.size(); ++i) {
        // iterate through line printing each character until line end
        if (buffer[i] != '\n') {
          mvprintw(printcur_y, printcur_x, "%c", buffer[i]);

          ++printcur_x;
        }
        // if at line end, print line number, padding, then increment line number
        else if (buffer[i] == '\n') {
          // idk what this math is doing and i'm in no state to even try understanding
          // right to left line number alignment
          linenum_padding = -static_cast<int>(log10(printlinenum)) + static_cast<int>(log10(linesum));
          mvprintw(printcur_y, linenum_padding, "%d", printlinenum);

          printcur_x = linestart; // reset print cursor position
          ++printcur_y;           // move down one line
          ++printlinenum;         // increment print line number
        }
      }
    }

    void usercursorcorrect() {
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
      }
      else {
        if (cur_x > check_line.lineendtrue)
          cur_x = check_line.lineendtrue;
      }
      if (cur_x < linestart)
        cur_x = linestart;

      // some bullshit probably i forgot
      lineInfo currentline = getLineInfo(buffer, cursorlinenum, linestart);
      textcurpos           = currentline.offset + (cur_x - linestart);
      lineend              = currentline.lineend;
      lineendtrue          = currentline.lineendtrue;

      midpoint   = (LINES) / 2;
      difference = cursorlinenum + (midpoint - bottomline);

      move(cur_y, cur_x);
    }

    void debugprint() {
      lineInfo currentline = getLineInfo(buffer, cursorlinenum, linestart);
      mvprintw(LINES / 2, 100, "linesum: %d, currentline: %d, cursorlinenum: %d, log10 linesum: %d, buffer size: %d, bottomlinenum: %d, topline: %d", linesum, currentline.linenum, cursorlinenum, static_cast<int>(log10(linesum)), static_cast<int>(buffer.size()), bottomline, topline);
      mvprintw(LINES / 2 + 1, 100, "cur_y: %d, cur_x : % d ", cur_y, cur_x);
      mvprintw(LINES / 2 + 2, 100, " difference : % d ", difference);
      mvprintw(LINES / 2 + 3, 100, "thing: %d, thing2: %d", linesum - topline, linesum - 1);
      mvprintw(5, 70, "topline: %d", topline);
      mvprintw(6, 70, "LINES: %d", LINES);
      mvprintw(7, 70, "linesum: %d", linesum);
      mvprintw(8, 70, "bottomlinenum: %d", bottomline);
      // mvprintw(9, 70, "testing: %d", testing);

      move(cur_y, cur_x);
    }

    void input() {
      lineInfo currentline = getLineInfo(buffer, cursorlinenum, linestart);
      // line check booleans
      bool firstlinevisible = (topline == 1);
      bool attopline        = (cursorlinenum == topline);
      bool atbottomline     = (cursorlinenum == bottomline);
      bool atfirstline      = (cursorlinenum == 1);
      bool atlastline       = (cursorlinenum == linesum);
      bool lastlinevisible  = (bottomline == linesum);
      bool atlinestart      = (cur_x == linestart);
      bool atlineend        = (cur_x >= lineend);
      bool atlineendtrue    = (cur_x >= lineendtrue);
      bool screenfull       = (bottomline >= LINES);
      ch                    = getch();

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
          }
          else {
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
            ++topline;
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

          scrollscr(difference);

          if (topline == 1)
            cur_y = cursorlinenum - 1;
          else if (bottomline + difference >= linesum)
            cur_y += -(linesum - bottomline);
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
            scrollcur(-linesum);
          else
            break;
          break;
        }
        case 'G': {
          scrollcur(linesum);
          break;
        }

        // fast scroll
        case ctrl('u'): {
          scrollcur(-scrolldist);
          break;
        }
        case ctrl('d'): {
          scrollcur(scrolldist);
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
          scrollcur(1);
          break;
        }
        case 'k': {
          scrollcur(-1);
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
          scrollcur(-1);
          break;
        }
        case KEY_DOWN: {
          scrollcur(1);
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
            ++topline;
            break;
          }
          else
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
              --topline;
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
              --topline;
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
            --topline;
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
  };

  void mainLoop(const string& path) {
    //// ncurses init ////
    initscr();            // create screen fitting the terminal
    raw();                // give all input control to the program
    set_escdelay(0);      // forgot what this is used for but it's important i swear
    noecho();             // disable automatically echoing keyboard input
    keypad(stdscr, TRUE); // enable keypad and function keys

    // color stuff (bullshit awful evil terrible system)
    if (has_colors()) {
      start_color();
      init_color(8, 0, 90, 255); // first parameter is the ID
    }

    //// main program variables ////

    int& scrheight{ LINES };
    int& scrwidth{ COLS };
    bool cmdmode{ true };

    /// config ///

    // scrolling
    int scrolldist{ 7 };

    bool running{ true };

    // create buffer object
    Buffer buffer{
      path,
      scrheight,
      scrwidth,
      cmdmode,
      scrolldist,
      running,
    };

    //// loop start ////
    while (running) {
      // for the sake of readability and experience i'm refactoring this whole thing to make use of classes and member functions
      // i wanna have a flow like this:
      // 1. clear screen
      // 2. reset loop variables
      // 3. print line number
      // 4. print buffer
      // 5. correct user cursor position according to previous input and or errors
      // 6. handle input

      // what i'm trying to do now is very funny and improper but i'm gonna continue anyways

      clear();                    // reset screen
      buffer.print();             // print buffer
      buffer.usercursorcorrect(); // correct cursor position
      buffer.debugprint();        // debug print
      refresh();                  // refresh screen
      buffer.input();             // input
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
