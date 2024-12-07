import os
import matplotlib.pyplot as plt
from datetime import datetime

def parse_memory_maps(directory):
    
    data = []

    for filename in sorted(os.listdir(directory)):

        # print(filename)
        timestamp = datetime.strptime(filename.split('_', 2)[2], "%Y-%m-%d_%H-%M-%S")
        # print(timestamp)

        mem = 0

        with open(os.path.join(directory, filename)) as file:
            lines = file.readlines()
            mem = lines[-1].split()[3]
            
        data.append((timestamp, mem)) # В мегабайтах

    return data

dir = "./outputs/"

data = parse_memory_maps(dir)

# print(data)
timestamps, mems = zip(*data)
# print(mems)

plt.plot(timestamps, mems, marker="o", label="Mem (RSS)")
plt.title("Mem Over Time")
plt.xlabel("Time")
plt.ylabel("Mem (RSS)")
plt.grid()
plt.legend()
plt.tight_layout()
plt.savefig("./graph.png") # Исправлен вызов savefig