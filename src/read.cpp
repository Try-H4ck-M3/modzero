#include "read.h"
#include "logger.h"
#include "utils.h"

// Dump coils
void dump_coils(modbus_t* ctx, int max_addr)
{
    log_step("Dumping Coils (0 to " + to_string(max_addr) + ")");
    int chunk = 100;
    
    int range_start = 0;
    int current_state = -1;
    bool bounds_found = false;
    
    for (int addr = 0; addr < max_addr && !bounds_found; addr += chunk)
    {
        int count = std::min(chunk, max_addr - addr);
        uint8_t* dest = (uint8_t*)malloc(count * sizeof(uint8_t));
        
        int rc = modbus_read_bits(ctx, addr, count, dest);
        if (rc > 0)
        {
            for (int i = 0; i < rc; i++)
            {
                int state = dest[i] ? 1 : 0;
                int current_addr = addr + i;
                
                if (current_state == -1)
                {
                    current_state = state;
                    range_start = current_addr;
                }
                else if (state != current_state)
                {
                    if (range_start == current_addr - 1)
                    {
                        log_info("Coil  [0x" + to_hex(range_start) + "]           - " + (current_state ? "ON  (1)" : "OFF (0)"));
                    }
                    else
                    {
                        log_info("Coils [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + (current_state ? "ON  (1)" : "OFF (0)"));
                    }
                    current_state = state;
                    range_start = current_addr;
                }
            }
        }
        else
        {
            // Chunk failed! Fallback to single reads to find exact bounds.
            log_info("Chunk read failed at 0x" + to_hex(addr) + "");
            log_info("Probing single coils to find exact bounds");
            for (int i = 0; i < count; i++)
            {
                uint8_t single_val;
                if (modbus_read_bits(ctx, addr + i, 1, &single_val) > 0)
                {
                    int state = single_val ? 1 : 0;
                    int current_addr = addr + i;
                    if (current_state == -1)
                    {
                        current_state = state;
                        range_start = current_addr;
                    }
                    else if (state != current_state)
                    {
                        if (range_start == current_addr - 1)
                        {
                            log_info("Coil  [0x" + to_hex(range_start) + "]           - " + (current_state ? "ON  (1)" : "OFF (0)"));
                        }
                        else
                        {
                            log_info("Coils [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + (current_state ? "ON  (1)" : "OFF (0)"));
                        }
                        current_state = state;
                        range_start = current_addr;
                    }
                }
                else
                {
                    int last_valid = addr + i - 1;
                    if (last_valid >= 0)
                    {
                        log_info("Coils memory bounds detected [0x0 - 0x" + to_hex(last_valid) + "]");
                    }
                    else
                    {
                        log_info("PLC rejected the first coil address (0x0)");
                    }
                    max_addr = addr + i;
                    bounds_found = true;
                    break;
                }
            }
        }
        free(dest);
    }
    
    if (current_state != -1)
    {
        log_info("Coils [0x" + to_hex(range_start) + " to 0x" + to_hex(max_addr - 1) + "] - " + (current_state ? "ON  (1)" : "OFF (0)"));
    }
}

// Dump discrete inputs
void dump_discrete_inputs(modbus_t* ctx, int max_addr)
{
    log_step("Dumping Discrete Inputs (0 to " + to_string(max_addr) + ")");
    int chunk = 100;
    
    int range_start = 0;
    int current_state = -1;
    bool bounds_found = false;
    
    for (int addr = 0; addr < max_addr && !bounds_found; addr += chunk)
    {
        int count = std::min(chunk, max_addr - addr);
        uint8_t* dest = (uint8_t*)malloc(count * sizeof(uint8_t));
        
        int rc = modbus_read_input_bits(ctx, addr, count, dest);
        if (rc > 0)
        {
            for (int i = 0; i < rc; i++)
            {
                int state = dest[i] ? 1 : 0;
                int current_addr = addr + i;
                
                if (current_state == -1)
                {
                    current_state = state;
                    range_start = current_addr;
                }
                else if (state != current_state)
                {
                    if (range_start == current_addr - 1)
                    {
                        log_info("Discrete Input  [0x" + to_hex(range_start) + "]           - " + (current_state ? "ON  (1)" : "OFF (0)"));
                    }
                    else
                    {
                        log_info("Discrete Inputs [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + (current_state ? "ON  (1)" : "OFF (0)"));
                    }
                    current_state = state;
                    range_start = current_addr;
                }
            }
        }
        else
        {
            log_info("Chunk read failed at 0x" + to_hex(addr));
            log_info("Probing single discrete inputs to find exact bounds");
            for (int i = 0; i < count; i++)
            {
                uint8_t single_val;
                if (modbus_read_input_bits(ctx, addr + i, 1, &single_val) > 0)
                {
                    int state = single_val ? 1 : 0;
                    int current_addr = addr + i;
                    if (current_state == -1)
                    {
                        current_state = state;
                        range_start = current_addr;
                    }
                    else if (state != current_state)
                    {
                        if (range_start == current_addr - 1)
                        {
                            log_info("Discrete Input  [0x" + to_hex(range_start) + "]           - " + (current_state ? "ON  (1)" : "OFF (0)"));
                        }
                        else
                        {
                            log_info("Discrete Inputs [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + (current_state ? "ON  (1)" : "OFF (0)"));
                        }
                        current_state = state;
                        range_start = current_addr;
                    }
                }
                else
                {
                    int last_valid = addr + i - 1;
                    if (last_valid >= 0)
                    {
                        log_info("Discrete inputs memory bounds detected [0x0 - 0x" + to_hex(last_valid) + "]");
                    }
                    else
                    {
                        log_info("PLC rejected the first discrete input address (0x0)");
                    }
                    max_addr = addr + i;
                    bounds_found = true;
                    break;
                }
            }
        }
        free(dest);
    }
    
    if (current_state != -1)
    {
        log_info("Discrete Inputs [0x" + to_hex(range_start) + " to 0x" + to_hex(max_addr - 1) + "] - " + (current_state ? "ON  (1)" : "OFF (0)"));
    }
}

// Dump holding registers
void dump_holding_registers(modbus_t* ctx, int max_addr)
{
    log_step("Dumping Holding Registers (0 to " + to_string(max_addr) + ")");
    int chunk = 100;
    
    int start_addr = 0;
    uint16_t test_val;
    
    if (modbus_read_registers(ctx, 0, 1, &test_val) == -1)
    {
        log_info("Address 0x0 rejected, probing for valid start address");
        for (int probe = 1; probe <= 32; probe++)
        {
            if (modbus_read_registers(ctx, probe, 1, &test_val) > 0)
            {
                log_info("Found valid register at 0x" + to_hex(probe) + ". Value: " + to_string(test_val) + " (0x" + to_hex(test_val) + ")");
                start_addr = probe;
                break;
            }
        }
        if (start_addr == 0)
        {
            log_error("Could not find any valid holding registers (tried 0x0-0x20)");
            return;
        }
    }
    else
    {
        log_info("Address 0x0 is valid. Starting scan from 0x0");
    }

    int range_start = start_addr;
    uint16_t current_value = 0;
    bool is_first = true;
    bool bounds_found = false;
    
    for (int addr = start_addr; addr < max_addr && !bounds_found; addr += chunk)
    {
        int count = std::min(chunk, max_addr - addr);
        uint16_t* dest = (uint16_t*)malloc(count * sizeof(uint16_t));
        
        int rc = modbus_read_registers(ctx, addr, count, dest);
        if (rc > 0)
        {
            for (int i = 0; i < rc; i++)
            {
                uint16_t value = dest[i];
                int current_addr = addr + i;
                
                if (is_first)
                {
                    current_value = value;
                    range_start = current_addr;
                    is_first = false;
                }
                else if (value != current_value)
                {
                    if (range_start == current_addr - 1)
                    {
                        log_info("Holding Register  [0x" + to_hex(range_start) + "]           - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                    }
                    else
                    {
                        log_info("Holding Registers [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                    }
                    current_value = value;
                    range_start = current_addr;
                }
            }
        }
        else
        {
            log_info("Chunk read failed at 0x" + to_hex(addr) + ". Probing single registers to find exact bounds");
            for (int i = 0; i < count; i++)
            {
                uint16_t single_val;
                if (modbus_read_registers(ctx, addr + i, 1, &single_val) > 0)
                {
                    int current_addr = addr + i;
                    if (is_first)
                    {
                        current_value = single_val;
                        range_start = current_addr;
                        is_first = false;
                    }
                    else if (single_val != current_value)
                    {
                        if (range_start == current_addr - 1)
                        {
                            log_info("Holding Register  [0x" + to_hex(range_start) + "]           - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                        }
                        else
                        {
                            log_info("Holding Registers [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                        }
                        current_value = single_val;
                        range_start = current_addr;
                    }
                }
                else
                {
                    int last_valid = addr + i - 1;
                    if (last_valid >= start_addr)
                    {
                        log_info("Holding register memory bounds detected [0x" + to_hex(start_addr) + " - 0x" + to_hex(last_valid) + "]");
                    }
                    else
                    {
                        log_info("PLC rejected holding register address (0x" + to_hex(addr + i) + ")");
                    }
                    max_addr = addr + i;
                    bounds_found = true;
                    break;
                }
            }
        }
        free(dest);
    }
    
    if (!is_first)
    {
        if (range_start == max_addr - 1)
        {
            log_info("Holding Register  [0x" + to_hex(range_start) + "]           - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
        }
        else
        {
            log_info("Holding Registers [0x" + to_hex(range_start) + " to 0x" + to_hex(max_addr - 1) + "] - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
        }
    }
}

// Dump input registers
void dump_input_registers(modbus_t* ctx, int max_addr)
{
    log_step("Dumping Input Registers (0 to " + to_string(max_addr) + ")");
    int chunk = 100;

    int start_addr = 0;
    uint16_t test_val;
    
    if (modbus_read_input_registers(ctx, 0, 1, &test_val) == -1)
    {
        log_info("Address 0x0 rejected, probing for valid start address");
        for (int probe = 1; probe <= 10; probe++)
        {
            if (modbus_read_input_registers(ctx, probe, 1, &test_val) > 0)
            {
                log_info("Found valid input register at 0x" + to_hex(probe) + "! Value: " + to_string(test_val) + " (0x" + to_hex(test_val) + ")");
                start_addr = probe;
                break;
            }
        }
        if (start_addr == 0)
        {
            log_error("Could not find any valid input registers (tried 0x0-0x9)");
            return;
        }
    }
    else
    {
        log_info("Address 0x0 is valid. Starting scan from 0x0S");
    }

    int range_start = start_addr;
    uint16_t current_value = 0xFFFF;
    bool is_first = true;
    bool bounds_found = false;
    
    for (int addr = start_addr; addr < max_addr && !bounds_found; addr += chunk)
    {
        int count = std::min(chunk, max_addr - addr);
        uint16_t* dest = (uint16_t*)malloc(count * sizeof(uint16_t));
        
        int rc = modbus_read_input_registers(ctx, addr, count, dest);
        if (rc > 0)
        {
            for (int i = 0; i < rc; i++)
            {
                uint16_t value = dest[i];
                int current_addr = addr + i;
                
                if (is_first)
                {
                    current_value = value;
                    range_start = current_addr;
                    is_first = false;
                }
                else if (value != current_value)
                {
                    if (range_start == current_addr - 1)
                    {
                        log_info("Input Register  [0x" + to_hex(range_start) + "]           - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                    }
                    else
                    {
                        log_info("Input Registers [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                    }
                    current_value = value;
                    range_start = current_addr;
                }
            }
        }
        else
        {
            log_info("Chunk read failed at 0x" + to_hex(addr) + ". Probing single registers to find exact bounds");
            for (int i = 0; i < count; i++)
            {
                uint16_t single_val;
                if (modbus_read_input_registers(ctx, addr + i, 1, &single_val) > 0)
                {
                    int current_addr = addr + i;
                    if (is_first)
                    {
                        current_value = single_val;
                        range_start = current_addr;
                        is_first = false;
                    }
                    else if (single_val != current_value)
                    {
                        if (range_start == current_addr - 1)
                        {
                            log_info("Input Register  [0x" + to_hex(range_start) + "]           - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                        }
                        else
                        {
                            log_info("Input Registers [0x" + to_hex(range_start) + " to 0x" + to_hex(current_addr - 1) + "] - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
                        }
                        current_value = single_val;
                        range_start = current_addr;
                    }
                }
                else
                {
                    int last_valid = addr + i - 1;
                    if (last_valid >= start_addr)
                    {
                        log_info("Input register memory bounds detected [0x" + to_hex(start_addr) + " - 0x" + to_hex(last_valid) + "]");
                    }
                    else
                    {
                        log_info("PLC rejected input register address (0x" + to_hex(addr + i) + ")");
                    }
                    max_addr = addr + i;
                    bounds_found = true;
                    break;
                }
            }
        }
        free(dest);
    }
    
    if (!is_first)
    {
        if (range_start == max_addr - 1)
        {
            log_info("Input Register  [0x" + to_hex(range_start) + "]           - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
        }
        else
        {
            log_info("Input Registers [0x" + to_hex(range_start) + " to 0x" + to_hex(max_addr - 1) + "] - " + to_string(current_value) + " (0x" + to_hex(current_value) + ")");
        }
    }
}