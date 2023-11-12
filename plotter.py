import sys
import matplotlib.pyplot as plt

run_info_files = []
plot_data = []
for arg in sys.argv[1:]:
    run_info_file = open(arg, mode='r')
    contents = run_info_file.readlines()

    data_to_be_plotted = []

    n_value = 0
    real_time_value = 0
    next_line_is_time_line = False

    for line in contents:
    
        if next_line_is_time_line:
            real_time_value = float(line)
            data_to_be_plotted.append((int(n_value), float(real_time_value)))
            n_value = 0
            real_time_value = 0
            next_line_is_time_line = False
        if "Test For" in line or "Computing for" in line:
            n_value = line.split()[2]
        if "Multiples per thread" in line or "Running in Series" in line:
            next_line_is_time_line = True
        if "TotalSeconds" in line:
            real_time_value = float(line.split()[2])
            data_to_be_plotted.append((int(n_value), real_time_value))
            n_value = 0
            real_time_value = 0
            next_line_is_time_line = False

    plot_data.append((arg, data_to_be_plotted))

plt.xlabel("N Value")
plt.ylabel("Time to Compute Primes")
plt.title("N to Time to Compute")

for plot in plot_data:
    x_val = [x[0] for x in plot[1]]
    y_val = [x[1] for x in plot[1]]
    plt.plot(x_val, y_val, label=plot[0])

plt.legend()
plt.show()