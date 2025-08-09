#include "functions.h"

namespace Functions {

  void openNewFile() {
  }

  // default path for files
  const std::string defaultpath{ "./files/" };
  const std::string cachepath{ "./cache/" };
  // const std::string defaultpath{ "/run/media/pebarch/pebdrive/_Code/C++/filetests/files/" };
  // const std::string cachepath{ "/run/media/pebarch/pebdrive/_Code/C++/filetests/cache/" };

  // get total number of lines, allowing for proper padding
  int getLineSum(std::string path) {
    using namespace std;
    string line;
    ifstream file(path);
    int linesum{};
    while (getline(file, line)) {
      linesum++;
    }
    file.close();
    return linesum;
  }

  // uses line sum to calculate spaces (highly inefficient right now)
  void getSpaces(int& linenum, int& spacenum, std::string& spacestr) {
    using namespace std;
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

  void writeDummyLines(std::string path, int linenum) {
    using namespace std;
    // thing to write dummy lines
    ofstream file(path, fstream::out | fstream::trunc);
    for (int i{ 1 }; i <= linenum; ++i) {
      file << "line " + to_string(i) << '\n';
    }
    file.close();
  }

  void readFile(const std::string path, const std::string cachefilepath) {
    using namespace std;
    // open file and get line number
    ifstream file(path);
    const int linesum{ getLineSum(path) };
    file.close();

    // reopen file (required)
    file.open(path);

    // create and open cache file
    // currently will not handle ~ and / paths
    ofstream cachefile(cachefilepath, fstream::out | fstream::trunc);

    // space counter
    int spacenum = log10(linesum);
    // string for the spaces
    string spacestr;
    // line to read
    string line;

    /// reading file

    // good solution i found
    // don't use file.eof() as it only returns true after reading the end of the stream, it doesn't indicate that the next read will be end
    if (file.is_open()) {
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
  }

  void openExistingFile(std::string path) {
    using namespace std;

    // read file
    // slightly flawed, but whatever
    string cachefilepath{};
    if (path[0] != '/' && path[0] != '~' && path[0] != '.') {
      cachefilepath = cachepath + path;
      path          = defaultpath + path;
    } else if (path[0] == '/' || path[0] == '~' || path[0] == '.') {
    }

    writeDummyLines(path, 1000);

    cout << "Path is " << path << '\n';

    readFile(path, cachefilepath);

    // file >> line;

    // assuming you need a loop for all lines, i'll experiment with that

    // writing to file goes here i guess
  }
} // namespace Functions
