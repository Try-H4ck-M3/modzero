#include "recon.h"

void discover_mei(modbus_t* ctx)
{
    log_step("Attempting Device Identification Discovery (FC 43 / MEI 0x0E)");

    // MEI discovery request
    // FC 43 / MEI 0x0E / Read Device ID (basic) / Object ID 0x00
    uint8_t req[] = { 0x2B, 0x0E, 0x01, 0x00 };
    int req_length = sizeof(req);

    // Send raw Modbus request
    int send_status = modbus_send_raw_request(ctx, req, req_length);
    if (send_status == -1)
    {
        log_error("Failed to send MEI discovery request: " + string(modbus_strerror(errno)));
        return;
    }

    // Receive response
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH] = {0};
    int rsp_length = modbus_receive_confirmation(ctx, rsp);

    if (rsp_length == -1)
    {
        log_error("Failed to receive MEI discovery response: " + string(modbus_strerror(errno)));
        return;
    }

    // Parsing with MBAP layer Indexes
    // rsp[0..5] = MBAP Header
    // rsp[6]    = Unit ID (Slave ID)
    // rsp[7]    = Function Code / Exception Code
    // rsp[8]    = Data / Exception Reason

    if (rsp_length >= 9)
    {
        uint8_t function_or_error = rsp[7];
        
        // Bit 7 set indicates a Modbus exception response
        if (function_or_error == 0x8E || (function_or_error & 0x80))
        {
            uint8_t exception_code = rsp[8];
            string reason = "Unknown Exception";

            // Translate common Modbus exception codes
            switch (exception_code)
            {
                case 0x01: reason = "Illegal Function (FC 43 unsupported)"; break;
                case 0x02: reason = "Illegal Data Address (MEI Type 0x0E or Object ID is invalid)"; break;
                case 0x03: reason = "Illegal Data Value"; break;
                case 0x04: reason = "Slave Device Failure"; break;
                default:   reason = "Modbus Exception Code 0x" + to_string(exception_code); break;
            }

            log_error("Device identification rejected: " + reason);
            return;
        }
    }

    // Validate response structure
    // Expected: FC 0x2B, MEI type 0x0E, ReadDevId code 0x01
    if (rsp_length < 14 || rsp[7] != 0x2B || rsp[8] != 0x0E)
    {
        log_error("Received unexpected or malformed packet response layout");
        return;
    }

    // Parse TLV objects from response payload
    // Byte 13 = Number of Objects, byte 14+ = TLV entries
    int num_objects = rsp[13];
    int idx = 14;

    log_info("Discovered " + to_string(num_objects) + " device identification objects:");

    int successful_reads = 0;

    // Iterate through each TLV entry
    for (int i = 0; i < num_objects; i++)
    {
        // Bounds check before reading TLV header
        if (idx + 2 > rsp_length)
        {
            break;
        }

        // Read Object ID and Length
        // Each TLV entry: Object ID (1 byte) + Length (1 byte) + Value (Length bytes)
        uint8_t obj_id = rsp[idx];
        uint8_t obj_len = rsp[idx + 1];
        idx += 2;

        // Bounds check before reading value
        if (idx + obj_len > rsp_length)
        {
            break;
        }

        // Extract value string
        string obj_val(reinterpret_cast<char*>(&rsp[idx]), obj_len);
        idx += obj_len;

        // Map Object ID to human-readable label
        string label;
        switch (obj_id)
        {
            case 0x00: label = "Vendor Name"; break;
            case 0x01: label = "Product Code"; break;
            case 0x02: label = "Major/Minor Revision"; break;
            case 0x03: label = "Vendor URL"; break;
            case 0x04: label = "Product Name"; break;
            case 0x05: label = "Model Name"; break;
            case 0x06: label = "User Application Name"; break;
            default:   label = "Custom Object (0x" + to_string(obj_id) + ")"; break;
        }

        log_info("  [+] " + label + ": " + obj_val);
        successful_reads++;
    }

    // Report result
    if (successful_reads == 0)
    {
        log_error("Failed to extract valid string definitions from payload structure");
    }
    else
    {
        log_info("Device identification complete (" + to_string(successful_reads) + " objects)");
    }
}


void bruteforce_slave_id(modbus_t* ctx)
{
    log_step("Brute-forcing Unit/Slave IDs (1-247)");

    // Track discovered slaves and ranges
    int active_slaves = 0;
    uint16_t reg;
    int range_start = -1;
    string current_state_desc = "";

    // Probe each possible slave ID
    for (int slave = 1; slave <= 247; slave++)
    {
        // Set target slave ID for this probe
        modbus_set_slave(ctx, slave);

        // Read holding register 0 as a test
        int rc = modbus_read_registers(ctx, 0, 1, &reg);

        string new_state_desc = "";
        bool is_active = false;

        if (rc > 0)
        {
            // Slave responded with valid data
            new_state_desc = "responded successfully (HR:0 = " + to_string(reg) + ")";
            is_active = true;
        }
        else if (rc == -1)
        {
            // Check if error is an exception (means slave exists but rejected the request)
            string err = modbus_strerror(errno);
            if (err.find("Illegal") != string::npos || err.find("Exception") != string::npos)
            {
                new_state_desc = "responded with Exception: " + err;
                is_active = true;
            }
        }
        
        if (is_active)
        {
            active_slaves++;
            
            if (range_start == -1)
            {
                // Initialize first range
                range_start = slave;
                current_state_desc = new_state_desc;
            }
            else if (new_state_desc != current_state_desc)
            {
                // Print previous range, start new one
                if (range_start == slave - 1)
                {
                    log_info("Slave ID  [" + to_string(range_start) + "] " + current_state_desc);
                }
                else
                {
                    log_info("Slave IDs [" + to_string(range_start) + " to " + to_string(slave - 1) + "] " + current_state_desc);
                }
                
                range_start = slave;
                current_state_desc = new_state_desc;
            }
        }
        else
        {
            // Close range on timeout
            if (range_start != -1)
            {
                if (range_start == slave - 1)
                {
                    log_info("Slave ID  [" + to_string(range_start) + "] " + current_state_desc);
                }
                else
                {
                    log_info("Slave IDs [" + to_string(range_start) + " to " + to_string(slave - 1) + "] " + current_state_desc);
                }
                range_start = -1;
                current_state_desc = "";
            }
        }
    }
    
    // Print remaining range after loop ends
    if (range_start != -1)
    {
        if (range_start == 247)
        {
            log_info("Slave ID  [" + to_string(range_start) + "] " + current_state_desc);
        }
        else
        {
            log_info("Slave IDs [" + to_string(range_start) + " to 247] " + current_state_desc);
        }
    }
    
    // Summarize findings
    if (active_slaves == 0)
    {
        log_warn("No active slaves detected on the gateway");
    }
    else
    {
        log_info("Found " + to_string(active_slaves) + " active slave(s)");
    }
}