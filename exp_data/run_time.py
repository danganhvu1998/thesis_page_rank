import re
from datetime import datetime

# get data set name, and number of round
commands = open("../script/runSpark.sh", "r").readlines()
logs = open("experiments_log.txt", "r").readlines()

start_times = []
end_times = []
files = []
round_counts = []
runtimes = []
is_no_lost = []
total_run_time = {}
total_run_round = {}
avg_round_time = {}

for line in commands:
    line = line.strip()
    if not len(line): continue
    file = re.findall(r"data/(.+)\.out", line)[0]
    round_count = int(re.findall(r"--numIter=(\d+)", line)[0])
    files.append(file)
    total_run_time[file] = 0.0
    total_run_round[file] = 0.0
    avg_round_time[file] = 0.0
    round_counts.append(round_count)
    print(file, round_count)

curr_cmd = 0
for line in logs:
    line = line.strip()
    if not len(line): continue
    if "WARN NativeCodeLoader" in line: # Start
        start_time = datetime.strptime(
            re.findall(r"(\d\d/\d\d/\d\d \d\d:\d\d:\d\d)", line)[0],
            '%y/%m/%d %H:%M:%S'
        )
        start_times.append(start_time)
        end_times.append(start_time)
        is_no_lost.append(True)
    
    if "WARN TaskSetManager: Lost task" in line: # Lost task
        is_no_lost[-1] = False
    
    if "INFO ShutdownHookManager: Deleting directory" in line: # Done
        end_time = datetime.strptime(
            re.findall(r"(\d\d/\d\d/\d\d \d\d:\d\d:\d\d)", line)[0],
            '%y/%m/%d %H:%M:%S'
        )
        end_times[-1] = end_time

for i in range( len(start_times) ):
    runtime = (end_times[i] - start_times[i]).total_seconds()
    runtimes.append(runtime)

for i in range( len(start_times) ):
    if is_no_lost[i] == False or is_no_lost[i - i%5] == False: continue
    diff_time = 0
    diff_time_per_round = 0
    if( round_counts[i] != 1 ):
        diff_time = runtimes[i] - runtimes[i-i%5]
        diff_round = round_counts[i] - round_counts[i-i%5]
        diff_time_per_round = diff_time / diff_round
        total_run_time[files[i]] += diff_time
        total_run_round[files[i]] += diff_round
        avg_round_time[files[i]] = total_run_time[files[i]] / total_run_round[files[i]]
    else:
        print("---------------------------------------")
    # exp_result = f"{files[i]} {round_counts[i]} {runtime} {diff_time} {diff_time_per_round}"
    exp_result = f"{files[i]} {round_counts[i]} {runtimes[i]} {diff_time_per_round}"
    print(exp_result)

print("\n\n\n\n--------------------------------\n\n\n\n")
print(avg_round_time)
