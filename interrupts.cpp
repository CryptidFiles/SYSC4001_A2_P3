/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include "interrupts.hpp"
#include <cstdlib>
#include <cmath>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/

    int current_time = 0;                    // Current simulation time in ms

    std::vector<std::string> execution_log;  // Store execution events

    // Constants for interrupt processing times
    const int SWITCH_MODE_TIME = 1;          // Switch to/from kernel mode
    const int CONTEXT_SAVE_RESTORE_TIME = 10; // Save/restore context 
    const int ISR_ACTIVITY_TIME = 40;        // Execute activities in ISR
    const int IRET_TIME = 1;                 // IRET instruction? Ask prof on what that is?

    // State tracking variables
    bool in_user_mode = true;  // user or kernel mode
    bool processing_interrupt = false;  // interrupt processing flag
    int device_number = -1;  // current device

    // CPU speed multiplier
    double CPU_SPEED_MULT = 1.0;
	if (const char* e = std::getenv("CPU_SPEED")) {
		try { CPU_SPEED_MULT = std::stod(e); } catch (...) { CPU_SPEED_MULT = 1.0; }
		if (CPU_SPEED_MULT <= 0.0) CPU_SPEED_MULT = 1.0;
	}


    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
        
        /******************ADD YOUR SIMULATION CODE HERE*************************/

        if (activity == "CPU") {
            int scaled = (int) std::llround((double)duration_intr / CPU_SPEED_MULT);
            execution += std::to_string(current_time) + ", " + 
                std::to_string(scaled) + ", CPU execution\n\n";
            current_time += scaled;

        } else if (activity == "SYSCALL") {
            device_number = duration_intr;
            processing_interrupt = true;
            in_user_mode = false; // enter kernel mode by switching mode bit to 0 (false) 

            // Use the boilerplate function for standard interrupt steps
            auto [boilerplate_log, new_current_time] = intr_boilerplate(current_time, device_number, CONTEXT_SAVE_RESTORE_TIME, vectors);
           
            // Append the execution logging to the string   
            execution += boilerplate_log;

            // Adjust current time with the ISR activities duration
            current_time = new_current_time;
              
            // Delay of device direclty taken from device table 
            int device_delay = delays[device_number];

            // Execute ISR body activities
            // First Activity: Run the device driver
            execution += std::to_string(current_time) + ", " + std::to_string(ISR_ACTIVITY_TIME) + ", " + 
                            "SYSCALL: run the ISR (call device driver)\n";
            current_time += ISR_ACTIVITY_TIME;

            // Second Activity: transfer data
            execution += std::to_string(current_time) + ", " + std::to_string(ISR_ACTIVITY_TIME) + ", " + 
                            "transfer data from device to memory\n";
            current_time += ISR_ACTIVITY_TIME;


            // Calculate remaining time determined to carry out rest of device delay for error handling purposes
            int remaining_time = device_delay - (2 * ISR_ACTIVITY_TIME);
            if(remaining_time > 0){
                execution += std::to_string(current_time) + ", " + std::to_string(remaining_time) + 
                            ", check for errors\n";
                current_time += remaining_time;
            }

            // Execute IRET (return from enterrupt) instruction
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(IRET_TIME) + ", IRET\n";
            current_time += IRET_TIME;
            
            // Restore context that occurs as we switch from OS services to user program
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(CONTEXT_SAVE_RESTORE_TIME) + ", context restored\n";
            current_time += CONTEXT_SAVE_RESTORE_TIME;
            
            // Switch back to user mode 
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(SWITCH_MODE_TIME) + ", switch to user mode\n\n";
            current_time += SWITCH_MODE_TIME;

            // Update state
            in_user_mode = true;
            processing_interrupt = false;
            device_number = -1;

        } else if (activity == "END_IO") {  
            processing_interrupt = true;
            device_number = duration_intr;
            in_user_mode = false; // enter kernel mode by switching mode bit to 0 (false) 
            
            // Use the boilerplate function for standard interrupt steps
            auto [boilerplate_log, new_current_time] = intr_boilerplate(current_time, device_number, CONTEXT_SAVE_RESTORE_TIME, vectors);

            // Append the execution logging to the string   
            execution += boilerplate_log;

            // Adjust current time with the ISR activities duration
            current_time = new_current_time;

            // Delay of device direclty taken from device table 
            int device_delay = delays[device_number];

            // Execute ISR body activities
            // First Activity: Run the device driver
            execution += std::to_string(current_time) + ", " + std::to_string(ISR_ACTIVITY_TIME) + ", " + 
                            "ENDIO: run the ISR (device driver)\n";
            current_time += ISR_ACTIVITY_TIME;


            // Calculate remaining time determined to carry out rest of device delay for error handling purposes
            int remaining_time = device_delay - ISR_ACTIVITY_TIME;
            if(remaining_time > 0){
                execution += std::to_string(current_time) + ", " + std::to_string(remaining_time) + 
                            ", check device status\n";
                current_time += remaining_time;
            }

            // Execute IRET (return from enterrupt) instruction
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(IRET_TIME) + ", IRET\n";
            current_time += IRET_TIME;
            
            // Restore context that occurs as we switch from OS services to user program
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(CONTEXT_SAVE_RESTORE_TIME) + ", context restored\n";
            current_time += CONTEXT_SAVE_RESTORE_TIME;

            // Switch back to user mode 
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(SWITCH_MODE_TIME) + ", switch to user mode\n\n";
            current_time += SWITCH_MODE_TIME;

            // Update state
            in_user_mode = true;
            processing_interrupt = false;
            device_number = -1;
            
        } else {
            // Command read in line isn't recognized as a CPU or I/O burst
            execution += activity + " is not recognized as a valid input\n\n";

        }
        /************************************************************************/
    }

    input_file.close();
    write_output(execution);
    return 0;
}
