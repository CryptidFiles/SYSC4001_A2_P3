# SYSC 4001 – Assignment 1: Interrupts Simulator
Carleton University – Fall 2025  
Group Members:
- Aryan Kumar Singh (101299776)
- Srivathsan Murali (101287534)

___________________________________________________________________________________________________________
📘 OVERVIEW
-----------------------------------------------------------------------------------------------------------
This repository contains the **Interrupts Simulator** developed for **SYSC 4001 – Operating Systems (Fall 2025)**.  
The goal is to simulate the operation of interrupts and analyze how timing parameters affect overall system performance.

___________________________________________________________________________________________________________
🧩 PROJECT STRUCTURE
-----------------------------------------------------------------------------------------------------------
```
SYSC4001_A1/
 ┣ .vscode/                         # Optional editor settings
 ┣ bin/                             # Compiled executable (after running build.sh)
 ┣ input_files/                     # Contains all input traces (trace.txt, test_trace2–6.txt)
 ┣ output_files/                    # Contains output files for all 20 test cases
 ┣ README.md                        # Full project documentation and run instructions
 ┣ build.sh                         # Build script to compile the simulator
 ┣ device_table.txt                 # Device number vs. I/O delay table
 ┣ execution.txt                    # Temporary execution output (auto-overwritten each run)
 ┣ experiments_plan_final.csv       # Defines 20 experiment configurations
 ┣ interrupts.cpp                   # Main simulator implementation
 ┣ interrupts.hpp                   # Header with helper functions and data structures
 ┣ report.pdf                       # Final written report of test case results
 ┣ run.sh                           # Batch automation script (runs all test cases)
 ┗ vector_table.txt                 # Vector table mapping device numbers to ISR addresses
```
___________________________________________________________________________________________________________
🚀 HOW TO RUN THE SIMULATOR
-----------------------------------------------------------------------------------------------------------

1️⃣  BUILD THE PROJECT
-------------------------------------
Open a terminal inside the repository root and run:
    source build.sh

This compiles the simulator and produces the executable:
    ./bin/interrupts_sim

2️⃣  RUN ALL TEST CASES
-------------------------------------
Execute the batch script:
    bash run.sh

The script automatically:
 • Reads configurations from experiments_plan_final.csv  
 • Uses the correct trace file for each test:
       - Cases 1–11 → input_files/trace.txt  
       - Cases 12–20 → input_files/test_traceN.txt  
 • Runs the simulator for each configuration  
 • Generates a temporary file named execution.txt  
 • Copies and renames execution.txt into the appropriate case file inside output_files/

Resulting structure:
output_files/
 ├── execution_case_01.txt
 ├── execution_case_02.txt
 ├── ...
 └── execution_case_20.txt

3️⃣  ABOUT execution.txt
-------------------------------------
⚠️ IMPORTANT:
 • execution.txt is a temporary working file created by the simulator for each run.  
 • It is overwritten every time a new test executes — this is expected behavior.  
 • The run.sh script automatically copies/renames it into the output_files/ folder.  
 • Do not modify or delete execution.txt manually — it is managed by the workflow.

4️⃣  VIEWING RESULTS
-------------------------------------
After all runs complete, check:
    output_files/
Each text file contains the complete interrupt trace and timing breakdown for its case.

___________________________________________________________________________________________________________
⚠️ NOTES & COMMON PITFALLS
-----------------------------------------------------------------------------------------------------------

🗂 INPUT FILES
 • Ensure all trace files (trace.txt, test_trace2.txt, etc.) are inside input_files/.  
 • If renamed or moved, update their paths in experiments_plan_final.csv.

🐧 ENVIRONMENT
 • Run only using Bash (Linux, macOS, or Git Bash/WSL on Windows).  
 • CMD and PowerShell will NOT work with run.sh.

🔨 BUILD FIRST
 • Always compile before testing:
       source build.sh

💬 CONSOLE MESSAGES
 • "File content overwritten successfully." → Normal confirmation.  
 • "Malformed input line" → Check the trace file formatting (extra commas/spaces/comments).

🔠 LINE ENDING WARNINGS
 • "LF will be replaced by CRLF" → Safe to ignore; Windows formatting notice.

🌿 GIT WORKFLOW
 • To safely sync and push your changes:
       git add .
       git commit -m "Updated scripts and outputs"
       git pull --rebase origin main
       git push origin main

___________________________________________________________________________________________________________
📊 PARAMETER SWEEPS (20 CASES)
-----------------------------------------------------------------------------------------------------------
The simulator was executed 20 times to study the influence of various parameters:

| Group | Parameter Varied | Values Tested |
|--------|------------------|----------------|
| 1 | Context save/restore time | 10 ms, 20 ms, 30 ms |
| 2 | Vector address size | 2 bytes, 4 bytes, 8 bytes |
| 3 | CPU speed multiplier | 0.5×, 1.0×, 2.0×, 4.0× |
| 4 | ISR activity time | 40 ms, 80 ms, 120 ms, 160 ms, 200 ms |
| 5 | Mixed edge cases | Fast context + slow ISR, Slow context + fast ISR, Worst and Best overall scenarios |

___________________________________________________________________________________________________________
📄 INPUT FILES
-----------------------------------------------------------------------------------------------------------

trace.txt  
Represents one program execution trace:

trace.txt
-------------------------------------
# Represents one program execution trace:
CPU, 50
SYSCALL, 7
END_IO, 7
CPU, 100
SYSCALL, 12
END_IO, 12
CPU, 20


vector_table.txt
-------------------------------------
# Example:
7   0x0E
12  0x1B
20  0x28
22  0x16


device_table.txt
-------------------------------------
# Example:
7   110
12  600

