/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

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
    const int CONTEXT_SAVE_RESTORE_TIME = 10; // Save/restore context    (CONFUSED, ON EXAMPLE IT SHOWS 1 and 2 seconds, is it suppose to be 10 for two context switches?)
    const int FIND_VECTOR_TIME = 1;          // Find vector (ISR start address) in memory
    const int GET_ISR_TIME = 1;              // Obtain ISR address from vector table
    const int ISR_ACTIVITY_TIME = 40;        // Execute activities in ISR
    const int IRET_TIME = 1;                 // IRET instruction? Ask prof on what that is?

    // State tracking variables
    bool in_user_mode = true;  // user or kernel mode
    bool processing_interrupt = false;  // interrupt processing flag
    int device_number = -1;  // current device

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        if (activity == "CPU") {
            // Handle CPU burst - simple execution in user mode
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(duration_intr) + ", CPU execution\n";
            current_time += duration_intr;

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
            
            // Execute ISR body (device driver activities)
            execution += std::to_string(current_time) + ", " + std::to_string(ISR_ACTIVITY_TIME) + ", " + "call device driver" + "\n";
            current_time += ISR_ACTIVITY_TIME;
                
            
        } else if (activity == "END_IO") { 
            // Set the device number used as the input command's second argument
            int end_device_number = duration_intr;
            
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
                        std::to_string(SWITCH_MODE_TIME) + ", switch to user mode\n";
            current_time += SWITCH_MODE_TIME;

            // Log end of I/O
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(delays[end_device_number]) + ", end of I/O " + std::to_string(end_device_number) + ": interrupt\n\n";
            current_time += delays[end_device_number];
            
            // Update state
            in_user_mode = true;
            processing_interrupt = false;
            device_number = -1;
        }
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
