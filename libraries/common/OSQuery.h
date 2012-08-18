#pragma once

#include <cstdlib>

class OSQuery{
public:
    static bool isMac(){
        #ifdef Q_WS_MAC
                return true;
        #endif
        return false;
    }
    
    static bool isWin(){
        #ifdef Q_WS_WIN
                return true;
        #endif
        return false;
    }
    
    static void clear_screen(){
        #ifdef Q_WS_WIN
            std::system ( "CLS" );
        #else // Assume POSIX
            std::system ( "clear" );
        #endif
    }
};
