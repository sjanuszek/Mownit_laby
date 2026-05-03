import numpy as np
import matplotlib.pyplot as plt

x0_vals = [0.1253,0.19999]
n = 1000

def logistic(x,r):
    return r * x * (1-x)

def a():
    r_vals = np.linspace(1,4,1000)
    x_vals = []
    r_points = []
    for x in x0_vals:
        for r in r_vals:
            for _ in range(n):
                x = logistic(x,r)
                x_vals.append(x)
                r_points.append(r)

        plt.scatter(r_points,x_vals,s=0.05)
        plt.show()

def b():
    r_vals = np.linspace(3.75,3.8,1000)
    x_vals_f32 = []
    x_vals_f64 = []
    r_points = []
    for x in x0_vals:
        xf32 = np.float32(x)
        xf64 = np.float64(x)
        for r in r_vals:
            for _ in range(n):
                xf32 = np.float32(logistic(np.float32(xf32),np.float32(r)))
                xf64 = np.float64(logistic(np.float64(xf64),np.float64(r)))

                x_vals_f32.append(xf32)
                x_vals_f64.append(xf64)
                r_points.append(r)

        plt.scatter(r_points,x_vals_f32,s=0.005,color='blue')
        plt.show()
        plt.scatter(r_points,x_vals_f64,s=0.005,color='red')
        plt.show()

def c():
    r = 4
    limit = 10000
    x_vals = np.linspace(0,1,100)
    to_zero_vals = []

    for x0 in x_vals:
        x = np.float32(x0)
        counter = 0
        while x > 1e-6 and counter<limit:
            x = np.float32(logistic(x,np.float32(r)))
            counter+=1
        to_zero_vals.append(counter)

    plt.plot(x_vals,to_zero_vals)
    plt.show()
