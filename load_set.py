import numpy as np

def load_set(suffix=''):
    X = []
    y = []

    with open('processed_data/dataset5k' + suffix + '.csv', 'r') as f:
        for line in f:
            line = line.rstrip()
            if not line: continue
            values = line.split(',')
            X.append([float(val) for val in values[1:]])
            y.append(int(values[0]))

    return np.array(X), np.array(y)
