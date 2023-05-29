// item.h
#ifndef item_h
#define item_h

#include <Arduino.h>

class Item {
    public: 
    
    int x;
    int y;
    bool isO;
    Item();
    Item(int x, int y, bool isO);
    void goLeft();
    void goRight();
    void goDown();
    void goUp();
};

#endif
