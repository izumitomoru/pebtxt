#include "functions.h"
#include <string>

// TODO:
// get started on learning ftxui input and making it work with buffers for initial edit functions and stuff

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
      cachepath,
      path,
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
  void gotoLine(string path, int lineNum) {
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

    cout << "Line " << currentlinenum - 1 << ": " << line;
  }

  void readFile(const string path) {
    using namespace GapBuffer;
    // get file info and create gap buffer
    fileInfo file{ getFileInfo(path) };
    // const string path{ file.path };
    cout << "Path: " << file.path << '\n';
    vector<char> buffer{ createFileBuffer(file.path) };

    // test
    // cout << "buffer size: " << buffer.size() << " buffer fourth char: " << buffer[3] << '\n';

    // get spaces
    // double spacenum = static_cast<int>(file.lineLog10);
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
      // if not newline, simply print character
      if (buffer[i] != '\n') {
        cout << buffer[i];
      } else {
        // print \n
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

  // awful code, really just trying to understand this library atm
  void readftxui(const string path) {
    using namespace std;
    fileInfo file{ getFileInfo(path) };
    ifstream sfile(file.path);
    ofstream scachefile(file.cachepath, fstream::out | fstream::trunc);

    while (sfile.is_open()) {
      int spacenum = file.lineLog10;
      string spacestr{};

      for (int i{ 0 }; i < spacenum; ++i) {
        spacestr += ' ';
      }

      string line{};
      int linenum{};

      using namespace ftxui;

      string testcontents{};
      tempText contents{};

      while (getline(sfile, line)) {
        ++linenum;

        contents.text.push_back(line += '\n');
        if (std::log10(linenum) == static_cast<int>(std::log10(linenum))) {
          spacestr.resize(spacenum);
          --spacenum;
        }
        // line += '\n';
        testcontents += spacestr + to_string(linenum) + ' ' + line + '\n';
        // contents.text += spacestr + to_string(linenum) + ' ' + line;

        // Element thing = text(line + '\n');
      }

      // idk
      Element thing = paragraph(testcontents);
      // Element contents = paragraph("");

      Element testframe = frame(thing);
      Element winBorder = window(text(""), testframe);
      Element testtext  = text("test thing");

      auto screen = Screen::Create(Dimension::Full());

      screen.ResetPosition(true);
      Render(screen, winBorder);

      // print screen
      // Render(screen, thing);
      // Render(screen, contents);
      screen.Print();

      string b;
      cin >> b;

      scachefile.close();
      sfile.close();
    }
  }

  // probably not even needed
  void openExistingFile(string path) {
    // just wrote over my whole functions.cpp file lol thank god for undo
    // changing this shit to burger.txt ONLY
    // writeDummyLines("./files/burger.txt", 105);

    fileInfo file{ getFileInfo(path) };
    cout << "Path is " << file.path << '\n';
    cout << "Cache path is " << file.cachepath << '\n';

    readFile(file.path);

    // file >> line;

    // assuming you need a loop for all lines, i'll experiment with that

    // writing to file goes here i guess
  }

} // namespace Functions
