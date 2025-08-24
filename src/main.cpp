#include "functions.h"

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

int main(int argc, char* argv[]) {
  using namespace Functions;
  // getinput();
  // writeDummyLines("./files/burger.txt", 125);
  string path{};
  if (argc == 1) {
    path = getPath();
  } else {
    path = argv[1];
  }
  mainLoop(path);
  // testinputs();
  return 0;
}
