import os
import subprocess

def create_script_file(filename, script_dir, commands):
    script_path = os.path.join(script_dir, filename)
    with open(script_path, 'w') as script_file:
        for command in commands:
            script_file.write(command + '\n')
    return script_path

def run_script(script_path, phyLS_executable):
    result = subprocess.run([phyLS_executable, '-f', script_path], capture_output=True, text=True)
    return result.stdout, result.stderr

def main():
    phyLS_executable = './build/bin/phyLS'
    benchmark_dir = './benchmarks/'
    script_dir = './script/'
    result_file = 'results.txt'
    
    os.makedirs(script_dir, exist_ok=True)
    
    with open(result_file, 'w') as res_file:
        for filename in os.listdir(benchmark_dir):
            if filename.endswith(".aig"):
                file_path = os.path.join(benchmark_dir, filename)
                res_file.write(f"Processing file: {filename}\n")
                
                # Create the script with commands
                commands = [f"read_aiger {file_path}", "ps -a"]
                commands.extend(["balance"] * 3)
                commands.extend(["rewrite"] * 10)
                
                script_filename = f"{os.path.splitext(filename)[0]}_script.txt"
                script_path = create_script_file(script_filename, script_dir, commands)
                
                # Run the script using phyLS
                stdout, stderr = run_script(script_path, phyLS_executable)
                if stderr:
                    res_file.write(f"Error processing file {filename}: {stderr}\n")
                else:
                    res_file.write(f"Results for {filename}:\n{stdout}\n")

if __name__ == "__main__":
    main()
