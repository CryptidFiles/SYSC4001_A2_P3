#!/usr/bin/env python3

import csv

scenarios = [
    {
        'scenario_id': 'scenario1',
        'description': 'Basic fork and exec',
        'trace_content': """FORK,10
IF_CHILD,0
EXEC,program1,50
IF_PARENT,0
EXEC,program2,25
ENDIF,0
CPU,100""",
        'external_files_content': """program1,10
program2,15
program3,8""",
        'programs': """program1:CPU,100;program2:SYSCALL,4
CPU,50;program3:CPU,80
SYSCALL,7
CPU,30"""
    },
    {
        'scenario_id': 'scenario2', 
        'description': 'Nested fork',
        'trace_content': """FORK,17
IF_CHILD,0
EXEC,calc,16
IF_PARENT,0
ENDIF,0
EXEC,editor,33
CPU,150""",
        'external_files_content': """calc,12
editor,18
browser,25""",
        'programs': """calc:FORK,15
IF_CHILD,0
CPU,30
IF_PARENT,0
CPU,40
ENDIF,0
CPU,20;editor:CPU,80
SYSCALL,7
CPU,30;browser:CPU,100
SYSCALL,12
CPU,50"""
    },
    {
        'scenario_id': 'scenario3',
        'description': 'IO Intensive', 
        'trace_content': """FORK,20
IF_CHILD,0
IF_PARENT,0
EXEC,io_program,60
ENDIF,0
EXEC,cpu_intensive,45
CPU,75""",
        'external_files_content': """io_program,8
cpu_intensive,20
utility,5""",
        'programs': """io_program:CPU,50
SYSCALL,6
CPU,15
END_IO,6;cpu_intensive:CPU,200
CPU,100;utility:CPU,25
SYSCALL,7
CPU,15"""
    }
]

with open('test_scenarios.csv', 'w', newline='', encoding='utf-8') as csvfile:
    fieldnames = ['scenario_id', 'description', 'trace_content', 'external_files_content', 'programs']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    
    writer.writeheader()
    for scenario in scenarios:
        writer.writerow(scenario)

print("✅ Created proper test_scenarios.csv")
print("📋 Format: scenario_id,description,trace_content,external_files_content,programs")