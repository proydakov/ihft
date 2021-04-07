import sys
import matplotlib.pyplot as plt
import numpy as np

x = np.loadtxt(sys.argv[1], delimiter="\n")

q = [50, 75, 80, 95, 99, 99.9, 100]
p = np.percentile(x, q = q)

plt.hist(x, density=True, bins=1024, label=['Eastasia', 'Eurasia', 'Oceania'])  # density=False would make counts
plt.ylabel('Probability')
plt.xlabel('Data (cycles)');

for i in range(len(q)):
    perc = q[i]
    cycl = p[i]
    print(f"percentile[{perc}]: {cycl}")

plt.show()
