#include "includes.h"

string to_hex(uint16_t val)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "%04X", val);
    return string(buf);
}

int ascii_art()
{
    cout << R"( __  __           _ ______              
|  \/  |         | |___  /              
| \  / | ___   __| |  / / ___ _ __ ___  
| |\/| |/ _ \ / _` | / / / _ \ '__/ _ \
| |  | | (_) | (_| |/ /_|  __/ | | (_) |
|_|  |_|\___/ \__,_/_____\___|_|  \___/

)" << "ModBus Exploration          by TryH4ckM3" << endl << endl;
    return 0;
}