# Script used to plot statistics about algorithm execution

import matplotlib.pyplot as plt

def update_query_time_graph(init_percentage, step, times, ylabel, filename, i):
    x = []
    for _ in range(len(times)):
        x.append(init_percentage + step)
        init_percentage += step
    plt.clf()
    plt.plot(x, times)
    plt.xlabel("Percentage data used")
    plt.ylabel(ylabel)
    plt.savefig(f"./plots/{filename}_{i}.png")

def real_approx_graph(init_percentage, step, real_approx, i):
    x = []
    for _ in range(len(real_approx)):
        x.append(init_percentage + step)
        init_percentage += step
    real_y = [ra[0] for ra in real_approx]
    approx_y = [ra[1] for ra in real_approx]
    plt.clf()
    plt.plot(x, real_y, label="Real shapes")
    plt.plot(x, approx_y, label="Approx shapes")
    plt.xlabel("Percentage data used")
    plt.ylabel("# shapes")
    plt.legend()
    plt.savefig(f"./plots/real_approx_graph_{i}.png")


if __name__ == "__main__":
    i = 0
    init_percentages = []
    percentage_steps = []
    ttbg = []
    update_times = []
    query_times = []
    real_approx = []
    with open("Test_results.txt", "r") as f:
        for line in f.readlines():
            if i == 0:
                # Test case R(5), d(2), eps(0.01), init_percentage(80), percentage_to_split(10)
                i += 1
                parameters = line.strip().split('(')
                tmp_init = int(parameters[4].split(')')[0])
                tmp_to_split = int(parameters[5].split(')')[0])
                tmp = ((100 - tmp_init)*tmp_to_split) / 100
                init_percentages.append(tmp_init)
                percentage_steps.append(tmp)
            elif i == 1:
                # Time to build the grid (80%) = 13.888s
                i += 1
                ttbg.append(float(line.strip().split(' = ')[1].split('s')[0]))
            elif i == 2:
                # Update time after each update: 0.00414286s 0.00397076s 0.00459355s 0.00440845s 0.00484553s 0.00478704s 0.00480769s 0.004925s 0.00474667s 0.00582s
                i += 1
                list_tmp = line.strip().split(': ')[1]
                times = list_tmp.split("s")
                to_append = []
                for time in times:
                    if time != '':
                        to_append.append(float(time))
                update_times.append(to_append)
            elif i == 3:
                # Query time after each update: 1.186s 1.236s 1.271s 1.359s 1.543s 1.437s 1.519s 1.693s 1.494s 1.731s
                i += 1
                list_tmp = line.strip().split(': ')[1]
                times = list_tmp.split("s")
                to_append = []
                for time in times:
                    if time != '':
                        to_append.append(float(time))
                query_times.append(to_append)
            elif i == 4:
                # (Real, approx) shapes after each update: (2561, 0) - (2743, 0) - (2918, 0) - (3090, 0) - (3226, 0) - (3223, 0) - (3237, 0) - (3272, 0) - (3395, 0) - (3526, 0) -
                i = 0
                list_tmp = line.strip().split(': ')[1]
                real_approx_points = list_tmp.split(" - ")
                to_append = []
                for r_a in real_approx_points:
                    real = int(r_a.split(", ")[0].split("(")[1])
                    approx = int(r_a.split(", ")[1].split(")")[0])
                    to_append.append((real, approx))
                real_approx.append(to_append)

    for i in range(len(init_percentages)):
        update_query_time_graph(init_percentages[i], percentage_steps[i], update_times[i], "Update time [s]", "update_time_graph", i)
        update_query_time_graph(init_percentages[i], percentage_steps[i], query_times[i], "Query time [s]", "query_time_graph", i)
        real_approx_graph(init_percentages[i], percentage_steps[i], real_approx[i], i)
