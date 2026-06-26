#include "includes.h"
#include "logger.h"
#include "recon.h"
#include "read.h"


int main(int argc, char** argv)
{
    // :D
    ascii_art();

    try
    {
        TCLAP::CmdLine cmd("ModZero | ModBus Exploration Tool (By TryH4ckM3)", ' ', "0.1", false);
        
        // Add all agrs
        TCLAP::ValueArg<string> nameArg("i", "ip", "Target PLC IP address", true, "", "string");
        cmd.add(nameArg);

        TCLAP::ValueArg<int> portArg("p", "port", "Target PLC port", false, 502, "int");
        cmd.add(portArg);
        
        TCLAP::ValueArg<int> slaveArg("s", "slave-id", "Modbus Unit/Slave ID", false, 1, "int");
        cmd.add(slaveArg);

        TCLAP::ValueArg<int> timeoutArg("t", "timeout", "Milliseconds timeout when connecting", false, 500, "int");
        cmd.add(timeoutArg);

        TCLAP::ValueArg<int> delayArg("d", "delay", "Milliseconds delay between modbus packets", false, 100, "int");
        cmd.add(delayArg);

        TCLAP::SwitchArg helpSwitch("h", "help", "Display manual", cmd, false);
        TCLAP::SwitchArg bruteforceslaveidSwitch("b", "bruteforce-slave-id", "Brute-force Unit/Slave IDs (1-247)", cmd, false);  
        TCLAP::SwitchArg reconSwitch("r", "recon", "Attempt to get device info", cmd, false);  
        
        TCLAP::SwitchArg dumpallSwitch("a", "dump-all", "Dump everything possible", cmd, false);
        TCLAP::SwitchArg dumpcoilsSwitch("c", "dump-coils", "Dump coil values", cmd, false);
        TCLAP::SwitchArg dumpdiscreteinputsSwitch("n", "dump-discrete-inputs", "Dump descrete input values", cmd, false);
        TCLAP::SwitchArg dumpinputregistersSwitch("e", "dump-input-registers", "Dump input register values", cmd, false);
        TCLAP::SwitchArg dumpdholdingregistersSwitch("g", "dump-holding-registers", "Dump holding register values", cmd, false);

        cmd.parse(argc, argv);
        
        // Get values
        string target_ip = nameArg.getValue();
        int target_port = portArg.getValue();
        int slave_id = slaveArg.getValue();

        int timeout = timeoutArg.getValue();
        //int delay = delayArg.getValue();
        bool bruteforceslaveid = bruteforceslaveidSwitch.getValue();
        bool recon = reconSwitch.getValue();
        
        bool dumpall = dumpallSwitch.getValue();
        bool dumpcoils = dumpcoilsSwitch.getValue();
        bool dumpdiscreteinputs = dumpdiscreteinputsSwitch.getValue();
        bool dumpinputregisters = dumpinputregistersSwitch.getValue();
        bool dumpdholdingregisters = dumpdholdingregistersSwitch.getValue();

        // Check help
        if (helpSwitch.getValue())
        {
            // Manually trigger TCLAP's usage output
            TCLAP::StdOutput().usage(cmd); 
            return 0;
        }

        // Check for any action
        if (!dumpall && !dumpcoils && !dumpdiscreteinputs && !dumpinputregisters && !dumpdholdingregisters && !bruteforceslaveid && !recon)
        {
            log_info("Action required. Check --help");
            exit(1);
        }

        log_info("ModZero started");

        // Init modbus
        modbus_t* ctx = modbus_new_tcp(target_ip.c_str(), target_port);
        if (ctx == nullptr)
        {
            log_error("Failed to create Modbus context");
            return 1;
        }

        modbus_set_slave(ctx, slave_id);
        modbus_set_response_timeout(ctx, 0, timeout * 1000);
        
        // Setup connection
        log_step("Connecting to modbus://" + target_ip + ":" + to_string(target_port));
        if (modbus_connect(ctx) == -1)
        {
            log_error("Connection failed: " + string(modbus_strerror(errno)));
            modbus_free(ctx);
            return 1;
        }
        log_info("Connected successfully");

        // Bruteforce slave ID
        if (bruteforceslaveid)
        {
            bruteforce_slave_id(ctx);
        }

        // Discover vendor data
        if (recon)
        {
            discover_mei(ctx);
        }

        // Dumping logic
        int max_dump_addr = 65535;
        if (dumpall)
        {
            dump_coils(ctx, max_dump_addr);
            log_info("Finished dumping coils");
            
            dump_discrete_inputs(ctx, max_dump_addr);
            log_info("Finished dumping descrete inputs");

            dump_input_registers(ctx, max_dump_addr);
            log_info("Finished dumping input registers");

            dump_holding_registers(ctx, max_dump_addr);
            log_info("Finished dumping holding registers");
        }
        else
        {
            if (dumpcoils)
            {
                dump_coils(ctx, max_dump_addr);
                log_info("Finished dumping coils");
            }
            
            if (dumpdiscreteinputs)
            {
                dump_discrete_inputs(ctx, max_dump_addr);
                log_info("Finished dumping discrete inputs");
            }

            if (dumpinputregisters)
            {
                dump_input_registers(ctx, max_dump_addr);
                log_info("Finished dumping input registers");
            }

            if (dumpdholdingregisters)
            {
                dump_holding_registers(ctx, max_dump_addr);
                log_info("Finished dumping holding registers");
            }
        }

        // Clean
        modbus_close(ctx);
        modbus_free(ctx);
        log_info("Disconnected");

    }
    catch (TCLAP::ArgException &e)
	{
        log_error(e.error() + " for arg " + e.argId());
    }
}