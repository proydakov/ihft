import sys
import matplotlib.pyplot as plt
import numpy as np

for x in range(1, len(sys.argv)):
    fname = sys.argv[x]

    x = np.loadtxt(fname, delimiter="\n")

    q = [50, 75, 80, 95, 99, 99.9, 100]
    p = np.percentile(x, q = q)

    fig, ax = plt.subplots()
    fig.canvas.set_window_title('Latency hist for: ' + fname)

    print(f"file: {fname}")
    plt.hist(x, density=True, bins=1024)  # density=False would make counts
    plt.ylabel('Samples')
    plt.xlabel('Latency (nanoseconds)');

    print(f"samples: {len(x)}")
    for i in range(len(q)):
        perc = q[i]
        cycl = p[i]
        print(f"percentile[{perc}]: {cycl}")

plt.show()
