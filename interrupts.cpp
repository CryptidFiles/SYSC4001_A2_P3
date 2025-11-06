/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include "interrupts.hpp"
#include <cstdlib>
#include <cmath>
#include <queue>

// Global variables for process management
//std::vector<PCB> PCB_table;
unsigned int next_pid = 1;

// Constants for interrupt processing times
const int SWITCH_MODE_TIME = 1;          // Switch to/from kernel mode
const int CONTEXT_SAVE_RESTORE_TIME = 10; // Save/restore context 
const int ISR_ACTIVITY_TIME = 40;        // Execute activities in ISR
const int IRET_TIME = 1;                 // IRET instruction? Ask prof on what that is?

// State tracking variables
bool in_user_mode = true;  // user or kernel mode
bool processing_interrupt = false;  // interrupt processing flag
int device_number = -1;  // current device



PCB create_child_pcb(const PCB& parent, const std::string& program_name = "") {
    PCB child(next_pid++, parent.PID, 
              program_name.empty() ? parent.program_name : program_name,
              parent.size, parent.partition_number);
    
    // If this is for EXEC, we need to find new memory
    if (!program_name.empty()) {
        child.partition_number = -1; // Will be allocated later
    }
    
    return child;
}

/**  Function to add process to ready queue
void add_to_ready_queue(PCB& process) {
    ready_queue.push(&process);
}

// Function to get next process from ready queue
PCB* get_next_process() {
    if (ready_queue.empty()) return nullptr;
    PCB* process = ready_queue.front();
    ready_queue.pop();
    return process;
}*/





std::tuple<std::string, std::string, int> simulate_trace(std::vector<std::string> trace_file, int time, std::vector<std::string> vectors, std::vector<int> delays, std::vector<external_file> external_files, PCB current, std::vector<PCB> wait_queue) {

    std::string trace;      //!< string to store single line of trace file
    std::string execution = "";  //!< string to accumulate the execution output
    std::string system_status = "";  //!< string to accumulate the system status output
    int current_time = time;

    //parse each line of the input trace file. 'for' loop to keep track of indices.
    for(size_t i = 0; i < trace_file.size(); i++) {
        auto trace = trace_file[i];

        auto [activity, duration_intr, program_name] = parse_trace(trace);

        if(activity == "CPU") { 
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU Burst\n\n";
            current_time += duration_intr;

        } else if(activity == "SYSCALL") { 
            device_number = duration_intr;
            processing_interrupt = true;
            in_user_mode = false; // enter kernel mode by switching mode bit to 0 (false) 

            auto [intr, time] = intr_boilerplate(current_time, duration_intr, 10, vectors);
            
            // Append the execution logging to the string   
            execution += intr;
            // Adjust current time with the ISR activities duration
            current_time = time;

            execution += std::to_string(current_time) + ", " + std::to_string(delays[duration_intr]) + ", SYSCALL ISR (ADD STEPS HERE)\n";
            current_time += delays[duration_intr];

            execution += std::to_string(current_time) + ", " + std::to_string(IRET_TIME) + ", IRET\n\n";
            current_time += IRET_TIME;

            // Update state
            in_user_mode = true;
            processing_interrupt = false;
            device_number = -1;

        } else if(activity == "END_IO") {
            device_number = duration_intr;
            processing_interrupt = true;
            in_user_mode = false; // enter kernel mode by switching mode bit to 0 (false) 

            auto [intr, time] = intr_boilerplate(current_time, duration_intr, 10, vectors);
            current_time = time;
            execution += intr;

            execution += std::to_string(current_time) + ", " + std::to_string(delays[duration_intr]) + ", ENDIO ISR(ADD STEPS HERE)\n";
            current_time += delays[duration_intr];

            execution += std::to_string(current_time) + ", " + std::to_string(IRET_TIME) + ", IRET\n\n";
            current_time += IRET_TIME;

            // Update state
            in_user_mode = true;
            processing_interrupt = false;
            device_number = -1;

        } else if(activity == "FORK") {
            auto [intr, time] = intr_boilerplate(current_time, 2, 10, vectors);
            execution += intr;
            current_time = time;

            ///////////////////////////////////////////////////////////////////////////////////////////
            //Add your FORK output here
            // Clone PCB for child
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", cloning the PCB\n";
            current_time += duration_intr;

            // Create child process
            PCB child = create_child_pcb(current);

            if (allocate_memory(&child)) {
                // Remove any existing processes with same PIDs
                wait_queue.erase(std::remove_if(wait_queue.begin(), wait_queue.end(),
                    [&](const PCB& p) { 
                        return p.PID == current.PID || p.PID == child.PID; 
                    }), 
                wait_queue.end());

                // Add parent to the waiting queue as we assume the child runs first with no preemption
                wait_queue.push_back(current);   

                execution += std::to_string(current_time) + ", 0, scheduler called\n";
                // Note: scheduler is empty as per requirements
                
                execution += std::to_string(current_time) + ", " + std::to_string(IRET_TIME) + ", IRET\n\n";
                current_time += IRET_TIME;

                // Add system status output
                system_status += "time: " + std::to_string(current_time) + "; current trace: " + trace + "\n";
                system_status += print_PCB(child, wait_queue);


                ///////////////////////////////////////////////////////////////////////////////////////////

                //The following loop helps you do 2 things:
                // * Collect the trace of the child (and only the child, skip parent)
                // * Get the index of where the parent is supposed to start executing from
                std::vector<std::string> child_trace;
                bool skip = true;
                bool exec_flag = false;
                int parent_index = 0;

                for(size_t j = i; j < trace_file.size(); j++) {
                    auto [_activity, _duration, _pn] = parse_trace(trace_file[j]);
                    if(skip && _activity == "IF_CHILD") {
                        skip = false;
                        continue;
                    } else if(_activity == "IF_PARENT"){
                        skip = true;
                        parent_index = j;
                        if(exec_flag) {
                            break;
                        }
                    } else if(skip && _activity == "ENDIF") {
                        skip = false;
                        continue;
                    } else if(!skip && _activity == "EXEC") {
                        skip = true;
                        child_trace.push_back(trace_file[j]);
                        exec_flag = true;
                    }

                    if(!skip) {
                        child_trace.push_back(trace_file[j]);
                    }
                }

                ///////////////////////////////////////////////////////////////////////////////////////////
                //With the child's trace, run the child (HINT: think recursion)
                if(!child_trace.empty()) {
                    auto [child_exec, child_status, child_time] = simulate_trace(child_trace, current_time, 
                                                                                vectors, delays, external_files, 
                                                                                child, wait_queue);
                    execution += child_exec;
                    system_status += child_status;
                    current_time = child_time;
                }

                i = parent_index; // Continue with parent from IF_PARENT


                ///////////////////////////////////////////////////////////////////////////////////////////
            } else {
                std::cerr << "ERROR: Memory allocation failed for child process!" << std::endl;
                execution += std::to_string(current_time) + ", 0, memory allocation failed for child\n\n";
            }

        } else if(activity == "EXEC") {
            auto [intr, time] = intr_boilerplate(current_time, 3, 10, vectors);
            current_time = time;
            execution += intr;

            ///////////////////////////////////////////////////////////////////////////////////////////
            //Add your EXEC output here
            // Get program size from external files
            unsigned int program_size = get_size(program_name, external_files);
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", Program is " + 
                        std::to_string(program_size) + " Mb large\n";
            current_time += duration_intr;


            // Create temporary PCB to check memory allocation
            PCB temp_pcb = current;
            temp_pcb.program_name = program_name;
            temp_pcb.size = program_size;
            temp_pcb.partition_number = -1;

            // UPDATE PCB TABLE - Remove old entries for this PID
            wait_queue.erase(std::remove_if(wait_queue.begin(), wait_queue.end(),
            [&](const PCB& p) { return p.PID == current.PID; }), 
            wait_queue.end());


            // Use existing allocate_memory function to find and allocate memory
            if (allocate_memory(&temp_pcb)) {
                // Free old memory if different from current (only if partition changed)
                if(current.partition_number != temp_pcb.partition_number && current.partition_number != -1) {
                    free_memory(&current);
                }

                // Loading program into memory (15ms per Mb)
                int load_time = program_size * 15;
                execution += std::to_string(current_time) + ", " + std::to_string(load_time) + ", loading program into memory\n";
                current_time += load_time;

                // Mark partition as occupied and update PCB
                execution += std::to_string(current_time) + ", 3, marking partition as occupied\n";
                current_time += 3;

                // Update current process with new program information
                current.program_name = program_name;
                current.size = program_size;
                current.partition_number = temp_pcb.partition_number;

                execution += std::to_string(current_time) + ", 6, updating PCB\n";
                current_time += 6;

                execution += std::to_string(current_time) + ", 0, scheduler called\n";
                // Note: scheduler is empty as per requirements
                
                execution += std::to_string(current_time) + ", " + std::to_string(IRET_TIME) + ", IRET\n\n";
                current_time += IRET_TIME;

                // Add system status output
                system_status += "time: " + std::to_string(current_time) + "; current trace: " + trace + "\n";
                system_status += print_PCB(current, wait_queue);

                // Load and execute the external program
                std::ifstream exec_trace_file(program_name + ".txt");

                if(!exec_trace_file.is_open()) {
                    std::cerr << "ERROR: Cannot open program file " << program_name + ".txt" << std::endl;
                    continue;
                }

                std::vector<std::string> exec_traces;
                std::string exec_trace;
                while(std::getline(exec_trace_file, exec_trace)) {
                    exec_traces.push_back(exec_trace);
                }
                exec_trace_file.close();

                // Execute the external program recursively
                auto [exec_exec, exec_status, exec_time] = simulate_trace(exec_traces, current_time, 
                                                                         vectors, delays, external_files, 
                                                                         current, wait_queue);
                execution += exec_exec;
                system_status += exec_status;
                current_time = exec_time;

                // Important: After EXEC, the current process is replaced
                break; 

            } else {
                std::cerr << "ERROR: Cannot allocate memory for program " << program_name << std::endl;
                execution += std::to_string(current_time) + ", 0, memory allocation failed for program " + program_name + "\n\n";
            }

        } else if(activity == "IF_CHILD" || activity == "IF_PARENT" || activity == "ENDIF") {
            // These are handled in FORK processing, just skip here
            continue;

        } else {
            // Command read in line isn't recognized as a CPU or I/O burst
            execution += activity + " is not recognized as a valid input\n\n";

        }
    }

    return {execution, system_status, current_time};
}




int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays, external_files] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    //Just a sanity check to know what files you have
    print_external_files(external_files);

    //Make initial PCB (notice how partition is not assigned yet)
    PCB current(0, -1, "init", 1, -1);

    //Update memory (partition is assigned here, you must implement this function)
    if(!allocate_memory(&current)) {
        std::cerr << "ERROR! Memory allocation failed!" << std::endl;
    }

    std::vector<PCB> wait_queue;

    wait_queue.push_back(current);

    /******************ADD YOUR VARIABLES HERE*************************/

    int current_time = 0;                    // Current simulation time in ms

    std::vector<std::string> execution_log;  // Store execution events



    /******************************************************************/


    //Converting the trace file into a vector of strings.
    std::vector<std::string> trace_file;
    std::string trace;
    while(std::getline(input_file, trace)) {
        trace_file.push_back(trace);
    }

    auto [execution, system_status, _] = simulate_trace(   trace_file, 
                                            0, 
                                            vectors, 
                                            delays,
                                            external_files, 
                                            current, 
                                            wait_queue);

    input_file.close();

    write_output(execution, "execution.txt");
    write_output(system_status, "system_status.txt");

    return 0;
}
