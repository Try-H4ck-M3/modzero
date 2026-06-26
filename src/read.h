#pragma once
#include "includes.h"

void dump_coils(modbus_t* ctx, int max_addr);
void dump_discrete_inputs(modbus_t* ctx, int max_addr);
void dump_holding_registers(modbus_t* ctx, int max_addr);
void dump_input_registers(modbus_t* ctx, int max_addr);
