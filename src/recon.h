#pragma once
#include "includes.h"
#include "logger.h"
#include "utils.h"

void discover_mei(modbus_t* ctx);
void bruteforce_slave_id(modbus_t* ctx);
