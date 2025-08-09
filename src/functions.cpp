#include "functions.h"

namespace Functions {

  void openNewFile() {
  }

  // default path for files
  const std::string defaultpath{ "/run/media/pebarch/pebdrive/_Code/C++/filetests/files/" };
  const std::string cachepath{ "/run/media/pebarch/pebdrive/_Code/C++/filetests/cache/" };

  int getLineSum(std::string path) {
    using namespace std;
    string line;
    ifstream file(path);
    // track lines manually
    int linesum{};
    while (getline(file, line)) {
      linesum++;
    }
    // redundant but keeping for good measure
    file.close();
    return linesum;
  }

  void getSpaces(int& linenum, int& spacenum, std::string& spacestr) {
    using namespace std;
    // memory inefficient but it works for now
    spacestr = "";
    // if whole number, decrement the number of spaces, this means each time it becomes a power of 10 it can recognize the change without needing a shitton of if statements
    if (log10(linenum) == static_cast<int>(log10(linenum))) {
      --spacenum;
    }
    for (int i{}; i <= spacenum; ++i) {
      // idk which one of these is the most efficient
      // spacestr = spacestr + ' ';
      spacestr += ' ';
      // spacestr.append(" ");
    }
  }

  // i think setting up something to view the file first would be ideal for learning, so i'll do that instead
  void openExistingFile(std::string path) {
    using namespace std;

    // read file
    // slightly flawed, but whatever
    string cachefilepath{};
    if (path[0] != '/' && path[0] != '~') {
      cachefilepath = cachepath + path;
      path          = defaultpath + path;
    } else if (path[0] == '/' || path[0] == '~') {
      // cout << "returning";

      // cache path testing
      // cachefilepath = path;
      // cout << "cache file path test" << cachefilepath;
    }

    cout << "Filename is " << path << '\n';

    // setting it to whatever amount of lines blah blah blah
    //    ofstream filething(path, fstream::out | fstream::trunc);
    //    for (int i{ 1 }; i <= 100; ++i) {
    //      filething << "line " + to_string(i) << '\n';
    //    }
    //    filething.close();

    ifstream file(path);

    // get line number
    int linesum{ getLineSum(path) };
    file.close();

    int spacenum = log10(linesum);

    string spacestr;

    file.open(path);

    string line;

    /// reading file tests

    // good solution i found
    // don't use file.eof() as it only returns true after reading the end of the stream, it doesn't indicate that the next read will be end
    if (file.is_open()) {
      // create and open cache file
      // currently will not handle ~ and / paths
      ofstream cachefile(cachefilepath, fstream::out | fstream::trunc);

      // track line number manually
      int linenum{};

      // getline() automatically moves to next line
      while (getline(file, line)) {
        // print lines
        ++linenum;

        // calculate number of spaces (super inefficiently)
        getSpaces(linenum, spacenum, spacestr);

        // print spaces, line number and line
        cout << spacestr << linenum << ' ' << line << '\n';

        // write to cache file
        cachefile << line << '\n';
      }

      // i wanna be able to get a line number and print just that line
      // void getspecificline(vector<ifstream*> file) {
      //  getline(*file[45], line);

      //  cout << "line 30 " << line;
      //}

      // cache handler
      cachefile.close();

      // enter before closing file or exiting program
      // awful solution probably
      string enter{};
      cin >> enter;

      remove(cachefilepath.c_str());

      file.close();

      // one possible (shitty) solution i found
      // while (!file.eof()) {}
    }

    // file >> line;

    // assuming you need a loop for all lines, i'll experiment with that

    // writing to file goes here i guess
  }
} // namespace Functions
