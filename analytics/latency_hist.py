import sys
import matplotlib.pyplot as plt
import numpy as np

for x in range(1, len(sys.argv)):
    fname = sys.argv[x]

    x = np.loadtxt(fname)

    q = [50, 75, 80, 95, 99, 99.9, 100]
    p = np.percentile(x, q = q)

    fig, ax = plt.subplots()
    fig.canvas.manager.set_window_title('Latency hist for: ' + fname)

    print(f"file: {fname}")
    plt.hist(x, density=True, bins=256, range=(0, p[3] * 2))
    plt.xlabel('Latency (nanoseconds)');

    print(f"samples: {len(x)}")
    for i in range(len(q)):
        perc = q[i]
        cycl = int(p[i])
        print(f"percentile[{perc}]: {cycl}us")

plt.show()
