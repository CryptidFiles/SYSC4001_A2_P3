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
    const int CONTEXT_SAVE_RESTORE_TIME = 10; // Save/restore context
    const int FIND_VECTOR_TIME = 1;          // Find vector (ISR start address) in memory
    const int GET_ISR_TIME = 1;              // Obtain ISR address from vector table
    const int ISR_ACTIVITY_TIME = 40;        // Execute activities in ISR
    const int IRET_TIME = 1;                 // IRET instruction? Ask prof on what that is?

    // State tracking variables
    bool in_kernel_mode = false;  // user of kernel mode
    bool processing_interrupt = false;  // interrupt processing flag
    int device_number = -1;  // current device

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        //log_event(current_time, duration_intr, "CPU execution"); helper function needed
        current_time += duration_intr;

        if(activity == "CPU"){

        } 

        if(activity == "SYSCALL"){
            in_kernel_mode = true; // switch to kernel mode
            //


        }

        if(activity == "END_IO"){
            
        }
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
