#include <iostream>
#include "BuffPoolMgr.h"


int main()
{
    BuffPoolMgr mgr("test.txt", 3, 4);
    bool exit = false;
    while (!exit)
    {
        mgr.ui(exit);
    }
    
    return 0;
}
