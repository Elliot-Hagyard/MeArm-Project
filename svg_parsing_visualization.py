import matplotlib.pyplot as plt
import numpy as np
    
mySVG = open("/Users/elliothagyard/Desktop/untitled/cmake-build-debug/example.csv")
n_arr = []
fig = plt.figure()

for index, line in enumerate(mySVG):
    if(line[0] == 'X'):
        continue
    if(line[0] == 'e' or not n_arr):
        n_arr.append([[],[]])
        continue
    xy = line.strip().split(',')
    n_arr[len(n_arr)-1][0].append(xy[1])
    n_arr[len(n_arr)-1][1].append(xy[2])
ax = fig.add_subplot(1, 1, 1)
plt.gca().invert_yaxis()
for i in n_arr:
    ax = fig.add_subplot(1, 1, 1)
    ax.plot(i[0],i[1])
plt.show()