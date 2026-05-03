import numpy as np
import matplotlib.pyplot as plt 
import time
N = 10**7

def main():
    val = 0.53125
    true_val = val * N
    to_sum = np.full(N,np.float32(val))
    err_rel_points = []
    x_points = []
    summ = 0

    t1_start = time.time()
    for i in range(len(to_sum)):
        summ += to_sum[i]
        if i % 25000 == 0:
            expected_sum = val * i
            current_err_abs = expected_sum - summ
            x_points.append(i)
            err_rel_points.append(current_err_abs/expected_sum * 100)
    t1_end = time.time()

    err_abs = true_val - summ
    print(f"Błąd bezwzględny: {err_abs}")
    err_rel = err_abs/true_val * 100
    print(f"Błąd względny: {err_rel} %")

    plt.plot(x_points,err_rel_points)
    plt.show()
    
    t2_start = time.time()
    rek_sum = rek_add(to_sum)
    t2_end = time.time()

    err_abs = true_val - rek_sum 
    print(f"Błąd bezwzględny (rekurencyjnie): {err_abs}")
    err_rel = err_abs/true_val * 100
    print(f"Błąd względny (rekurencyjnie): {err_rel} %")

    print(f"Czas wykonania dodawania w pętli: {t1_end - t1_start}")
    print(f"Czas wykonania dodawania rekurencyjnie: {t2_end - t2_start}")

def rek_add(arr):
    if len(arr) == 1:
        return arr[0]
    mid = len(arr) // 2
    return rek_add(arr[:mid]) + rek_add(arr[mid:])
    
main()
