import sys
import matplotlib.pyplot as plt

def pack_tuples(run_file) -> []:
    result_tuples = []
    n_value = 0
    next_line_is_time_line = False
    for line in run_file.readlines():
        if next_line_is_time_line:
            real_time_value = float(line)
            result_tuples.append((int(n_value), float(real_time_value)))
            n_value = 0
            real_time_value = 0
            next_line_is_time_line = False
        if "Test For" in line or "Computing for" in line:
            n_value = line.split()[2]
        if "Multiples per thread" in line or "Running in Series" in line:
            next_line_is_time_line = True
        if "TotalSeconds" in line:
            real_time_value = float(line.split()[2])
            result_tuples.append((int(n_value), real_time_value))
            n_value = 0
            real_time_value = 0
            next_line_is_time_line = False

    return result_tuples

parallel_run = sys.argv[1]
serial_run = sys.argv[2]

#parallel_run_data = []
#serial_run_data = []

parallel_run_file = open(parallel_run, "r")
serial_run_file = open(serial_run, "r")

plottable_data = []

parallel_tuples = pack_tuples(parallel_run_file)
parallel_run_file.close()
serial_tuples = pack_tuples(serial_run_file)
serial_run_file.close()
serial_tuple_index = 0

print(parallel_tuples)
print(serial_tuples)

result_diff_tuples = []
for t in parallel_tuples:
    n = t[0]
    diff = float(serial_tuples[serial_tuple_index][1]) / float(t[1])
    result_diff_tuples.append((n, diff))    
    serial_tuple_index += 1

#print(result_diff_tuples)

plt.xlabel("N Value")
plt.ylabel("Parallelization Speedup x")
plt.title("Parallelization speedup to value of N")
plottable_data = (sys.argv[3], result_diff_tuples)
#print(result_diff_tuples)
plt.plot(*zip(*plottable_data[1][1:]),label=plottable_data[0])

plt.legend()
plt.show()