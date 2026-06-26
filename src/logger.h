#pragma once
#include "includes.h"

extern const string COLOR_RESET;
extern const string COLOR_MAGENTA;
extern const string COLOR_BLUE;
extern const string COLOR_RED;
extern const string COLOR_YELLOW;
extern const string COLOR_GREEN;

int log_info(string message);
int log_step(string message);
int log_warn(string message);
int log_error(string message);
