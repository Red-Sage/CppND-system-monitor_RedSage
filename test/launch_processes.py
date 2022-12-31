from multiprocessing import Pool
import time

def f(x):
    y = ''
    for i in range(x):
        y += "x"*i
    return y


if __name__ == "__main__":
    with Pool(4) as p:
        p.map(f, list(range(2000, 3000, 1)))