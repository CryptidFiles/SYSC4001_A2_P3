# SYSC 4001 – Assignment 1: Interrupts Simulator  
**Carleton University – Fall 2025**  
**Group Members:**  
- Aryan Kumar Singh (101299776)  
- Srivathsan Murali (101287534)
___________________________________________________________________________________________________________
## 📘 Overview

This repository contains the **Interrupts Simulator** developed for **SYSC 4001 – Operating Systems (Fall 2025)**.  
The goal is to simulate the operation of interrupts and analyze how timing parameters affect overall system performance.

___________________________________________________________________________________________________________
## 🧩 Project Structure

```plaintext
SYSC4001_A1/
├── .vscode/                     # Optional editor settings
├── bin/                         # Compiled executable (after running build.sh)
├── README.md                    # Project documentation
├── build.sh                     # Build script to compile the simulator
├── device_table.txt             # Device number vs. I/O delay table
├── execution.txt                # Sample output (used for debugging)
├── experiments_plan_final.csv   # CSV file summarizing 20 test case parameters
├── interrupts.cpp               # Main simulator implementation
├── interrupts.hpp               # Helper functions and data structures
├── output_files/                # Folder containing execution_case_01.txt … execution_case_20.txt
├── run.sh                       # Alternative run script for quick execution
├── trace.txt                    # Input trace (single program execution trace)
└── vector_table.txt             # Vector table (device → ISR address)
```
___________________________________________________________________________________________________________
## ⚙️ How to Build

1. Open a terminal inside the repository root.  
2. Run:
   ```bash
   source build.sh
   ```
   This compiles `interrupts.cpp` and produces the binary:
   ```
   ./bin/interrupts_sim

___________________________________________________________________________________________________________
## 📊 Parameter Sweeps (20 Cases)

The simulator was executed **20 times** to study the influence of various parameters:

| Group | Parameter Varied | Values Tested |
|--------|------------------|----------------|
| 1 | Context save/restore time | 10 ms, 20 ms, 30 ms |
| 2 | Vector address size | 2 bytes, 4 bytes, 8 bytes |
| 3 | CPU speed multiplier | 0.5×, 1.0×, 2.0×, 4.0× |
| 4 | ISR activity time | 40 ms, 80 ms, 120 ms, 160 ms, 200 ms |
| 5 | Mixed edge cases | Fast context + slow ISR, Slow context + fast ISR, Worst and Best overall scenarios |

___________________________________________________________________________________________________________
## 📄 Input Files

### `trace.txt`
Represents one program execution trace:

CPU, 50
SYSCALL, 7
END_IO, 7
CPU, 100
SYSCALL, 12
END_IO, 12
CPU, 20

### `vector_table.txt`
Example:

7   0x0E
12  0x1B
20  0x28
22  0x16

### `device_table.txt`
Example:

7   110
12  600
___________________________________________________________________________________________________________




