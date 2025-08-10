#include "functions.h"

// TODO:
// make slightly less bad structs
// implement scrolling and proper file info fetching

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
    while (getline(sfile, line)) {
      lineSum++;
    }
    sfile.close();

    fileInfo info{
      path,
      cachepath,
      path,
      lineSum,
      log10(lineSum),
      0,
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

  void gotoLine(string path, int lineNum) {
    fileInfo file{ getFileInfo(path) };
    ifstream sfile(file.path);

    string line;
    if (sfile.is_open()) {
      // for (int i{}; i <= lineNum; ++i) {
      //  getline(sfile, line);
      //}
      int currentLine{};
      while (getline(sfile, line)) {
        ++currentLine;
        if (currentLine == lineNum)
          break;
      }
      cout << lineNum << ' ' << line << '\n';
    }
  }

  void readFile(const string path) {
    // get file info and open file and cache file
    fileInfo file{ getFileInfo(path) };
    ifstream sfile(file.path);
    // currently will not handle ~ and / paths
    ofstream cachefile(file.cachepath, fstream::out | fstream::trunc);

    if (sfile.is_open()) {
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

      // line to read
      string line{};
      // track line number manually
      int linenum{};

      // getline() automatically moves to next line
      while (getline(sfile, line)) {
        ++linenum;

        // spaces
        if (log10(linenum) == static_cast<int>(log10(linenum))) {
          spacestr.resize(spacenum);
          --spacenum;
        }

        // print spaces, line number and line
        cout << spacestr << linenum << ' ' << line << '\n';

        // info
        // cout << file.lineLog10 << ' ' << log10(linenum) << ' ' << spacenum << ' ' << spacestr << linenum << ' ' << line << '\n';

        // write to cache file
        cachefile << line << '\n';
      }

      // cache handler
      cachefile.close();

      // enter before closing file or exiting program
      // awful solution probably
      string enter{};
      cin >> enter;

      remove(file.cachepath.c_str());

      sfile.close();

      // one possible (shitty) solution i found
      // while (!file.eof()) {}
    }
  }

  void openExistingFile(string path) {
    // just wrote over my whole functions.cpp file lol thank god for undo
    // changing this shit to burger.txt ONLY
    // writeDummyLines("./files/burger.txt", 200);

    fileInfo file{ getFileInfo(path) };
    cout << "Path is " << file.path << '\n';

    readFile(file.path);

    // file >> line;

    // assuming you need a loop for all lines, i'll experiment with that

    // writing to file goes here i guess
  }
} // namespace Functions
