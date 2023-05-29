// item.cpp
#include "item.h"

Item::Item(){
  x = 0;
  y = 0;
  isO = false;
}

Item::Item(int x, int y, bool isO) {
  x = x;
  y = y;
  isO = isO;
}

void Item::goLeft() {
  if (x > 0) {
    x--;
  }
}

void Item::goRight() {
  if (x < 2) {
    x++;
  }
}

void Item::goDown() {
  if (y < 2) {
    y++;
  }
}
void Item::goUp() {
  if (y > 0) {
    y--;
  }
}
