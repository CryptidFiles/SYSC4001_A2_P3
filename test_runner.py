#!/usr/bin/env python3

import os
import csv
import subprocess
import shutil
from datetime import datetime

class CSVTestRunner:
    def __init__(self):
        self.scenarios_file = "test_scenarios.csv"
        self.results_dir = "test_results_csv"
        os.makedirs(self.results_dir, exist_ok=True)
        os.makedirs("input_files", exist_ok=True)
        os.makedirs("programs", exist_ok=True)
        os.makedirs("output_files", exist_ok=True)
        
    def compile_simulator(self):
        """Compile the C++ simulator"""
        print("Compiling simulator...")
        result = subprocess.run([
            "g++", "-std=c++17", "-o", "simulator", 
            "interrupts.cpp"
        ], capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"Compilation failed: {result.stderr}")
            return False
        print("Compilation successful!")
        return True
    
    def load_scenarios(self):
        """Load all test scenarios from CSV"""
        scenarios = []
        with open(self.scenarios_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                scenarios.append(row)
        return scenarios
    
    def parse_programs(self, programs_str):
        """Parse programs from string format to dictionary"""
        programs = {}
        if programs_str:
            program_entries = programs_str.split(';')
            for entry in program_entries:
                if ':' in entry:
                    prog_name, prog_content = entry.split(':', 1)
                    programs[prog_name.strip()] = prog_content.replace('\\n', '\n')
        return programs
    
    def setup_scenario(self, scenario):
        """Set up input files for a scenario"""
        # Create trace file
        with open("input_files/trace.txt", "w") as f:
            f.write(scenario['trace_content'])
        
        # Create external files
        with open("input_files/external_files.txt", "w") as f:
            f.write(scenario['external_files_content'])
        
        # Create program files
        programs = self.parse_programs(scenario['programs'])
        for prog_name, prog_content in programs.items():
            with open(f"programs/{prog_name}.txt", "w") as f:
                f.write(prog_content)
        
        return programs
    
    def run_scenario(self, scenario):
        """Run a single test scenario"""
        scenario_id = scenario['scenario_id']
        description = scenario['description']
        
        print(f"\n{'='*50}")
        print(f"Running scenario: {scenario_id}")
        print(f"Description: {description}")
        print(f"{'='*50}")
        
        # Setup files
        programs = self.setup_scenario(scenario)
        
        # Debug info
        print("Created files:")
        print(f"  Programs: {list(programs.keys())}")
        
        # Run simulator
        print("Executing simulator...")
        result = subprocess.run([
            "./simulator", 
            "input_files/trace.txt",
            "input_files/vector_table.txt", 
            "input_files/device_table.txt",
            "input_files/external_files.txt"
        ], capture_output=True, text=True)
        
        print(f"Return code: {result.returncode}")
        
        # Capture ALL output for debugging
        if result.stdout:
            print("Simulator stdout:")
            print(result.stdout)
        if result.stderr:
            print("Simulator stderr:")
            print(result.stderr)
        
        if result.returncode != 0:
            print(f"❌ Scenario {scenario_id} FAILED")
            
            # Save error log
            error_log = f"stdout:\n{result.stdout}\n\nstderr:\n{result.stderr}"
            with open(f"{self.results_dir}/{scenario_id}_error.log", "w") as f:
                f.write(error_log)
            
            return False
        
        # Check if output files were created
        if not os.path.exists("output_files/execution.txt"):
            print("❌ execution.txt was not created!")
            return False
        if not os.path.exists("output_files/system_status.txt"):
            print("❌ system_status.txt was not created!")
            return False
        
        # Save results
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        result_dir = f"{self.results_dir}/{scenario_id}_{timestamp}"
        os.makedirs(result_dir, exist_ok=True)
        
        # Copy all relevant files
        shutil.copy("output_files/execution.txt", f"{result_dir}/execution.txt")
        shutil.copy("output_files/system_status.txt", f"{result_dir}/system_status.txt")
        shutil.copy("input_files/trace.txt", f"{result_dir}/trace.txt")
        shutil.copy("input_files/external_files.txt", f"{result_dir}/external_files.txt")
        
        # Copy program files
        programs_dir = f"{result_dir}/programs"
        os.makedirs(programs_dir, exist_ok=True)
        for prog_name in programs.keys():
            shutil.copy(f"programs/{prog_name}.txt", f"{programs_dir}/{prog_name}.txt")
        
        print(f"✅ Scenario {scenario_id} completed successfully")
        print(f"📁 Results saved to: {result_dir}")
        return True
    
    def run_all_scenarios(self):
        """Run all test scenarios"""
        if not self.compile_simulator():
            return
        
        scenarios = self.load_scenarios()
        print(f"Loaded {len(scenarios)} test scenarios")
        
        successful = 0
        failed = 0
        
        for scenario in scenarios:
            if self.run_scenario(scenario):
                successful += 1
            else:
                failed += 1
        
        print(f"\n{'='*50}")
        print("TEST SUMMARY:")
        print(f"✅ Successful: {successful}")
        print(f"❌ Failed: {failed}")
        print(f"📊 Total: {len(scenarios)}")
        print(f"📁 All results saved to: {self.results_dir}/")
    
    def run_single_scenario(self, scenario_id):
        """Run a specific scenario by ID"""
        scenarios = self.load_scenarios()
        scenario = None
        
        for s in scenarios:
            if s['scenario_id'] == scenario_id:
                scenario = s
                break
        
        if not scenario:
            print(f"Scenario '{scenario_id}' not found!")
            available = [s['scenario_id'] for s in scenarios]
            print(f"Available scenarios: {available}")
            return
        
        if not self.compile_simulator():
            return
        
        self.run_scenario(scenario)

if __name__ == "__main__":
    import sys
    
    runner = CSVTestRunner()
    
    if len(sys.argv) > 1:
        # Run specific scenario
        runner.run_single_scenario(sys.argv[1])
    else:
        # Run all scenarios
        runner.run_all_scenarios()