# A tool to combine two data files from windows into a single readable run information
# usage: win-data-parser.py run-file run-time-file output-file-name
import sys

run_file = open(sys.argv[1], "r")
time_file = open(sys.argv[2], "r")
output_file = open(sys.argv[3], "w+")

run_lines = run_file.readlines()
time_lines = time_file.readlines()
time_index = 0
time_line_count = len(time_lines)

for run_line in run_lines:
    output_file.write(run_line)
    output_file.write(time_lines[time_index])
    output_file.write("\n")
    time_index += 1

run_file.close()
time_file.close()
output_file.close()