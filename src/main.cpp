#include "functions.h"

std::string parse_args(const int&, char* argv[]);

int main(int argc, char* argv[]) {
  using namespace Functions;
  // getinput();
  // writeDummyLines("./files/burger.txt", 125);
  string path{ parse_args(argc, argv) };

  mainLoop(path);
  // testinputs();
  return 0;
}

// command line argument parser
std::string parse_args(const int& argc, char* argv[]) {
  std::string path{};

  // if no extra args, get path post run
  if (argc == 1) {
    std::cout << "Enter path: ";
    std::cin >> path;
  } else {
    path = argv[1];
  }

  // trim path (probably should add a check for quotations but whatever)
  if (path[0] != '/' && path[0] != '~' && path[0] != '.')
    path = "./" + path; // default to current directory

  return path;
}
