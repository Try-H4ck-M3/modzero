#include "logger.h"

// Define colors
const string COLOR_RESET   = "\033[0m" ;
const string COLOR_MAGENTA = "\033[35m";
const string COLOR_BLUE    = "\033[34m";
const string COLOR_RED     = "\033[31m";
const string COLOR_YELLOW  = "\033[33m";
const string COLOR_GREEN   = "\033[32m";

// Blue info
int log_info(string message)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_c);
    cout << COLOR_MAGENTA << put_time(local_time, "%H:%M:%S") << COLOR_RESET << " | " << COLOR_BLUE << "INFO" << COLOR_RESET << " | " << COLOR_RESET << message << endl;
    
    return 0;
}

// Green step
int log_step(string message)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_c);
    cout << COLOR_MAGENTA << put_time(local_time, "%H:%M:%S") << COLOR_RESET << " | " << COLOR_GREEN << "STEP" << COLOR_RESET << " | " << COLOR_RESET << message << endl;
    
    return 0;
}

// Yellow warning
int log_warn(string message)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_c);
    cout << COLOR_MAGENTA << put_time(local_time, "%H:%M:%S") << COLOR_RESET << " | " << COLOR_YELLOW << "WARN" << COLOR_RESET << " | " << COLOR_RESET << message << endl;
    
    return 0;
}

// Red error
int log_error(string message)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_c);
    cerr << COLOR_MAGENTA << put_time(local_time, "%H:%M:%S") << COLOR_RESET << " | " << COLOR_RED << "ERR " << COLOR_RESET << " | " << COLOR_RESET << message << endl;
    
    return 0;
}