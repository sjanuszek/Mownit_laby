import numpy as np
import time
N = 10**7

def kahan(arr):
    sum = np.float32(0)
    err = np.float32(0)

    for i in range(len(arr)):
        y = arr[i] - err
        temp = sum + y
        err = (temp - sum) - y
        sum = temp

    return sum

def rek_add(arr):
    if len(arr) == 1:
        return arr[0]
    mid = len(arr) // 2
    return rek_add(arr[:mid]) + rek_add(arr[mid:])

def main():
    val = 0.53125
    true_val = val * N
    to_sum = np.full(N,np.float32(val))
    
    t1_start = time.time()
    summ = kahan(to_sum)
    t1_end = time.time()

    t2_start = time.time()
    rek_sum = rek_add(to_sum)
    t2_end = time.time()

    err_abs = true_val - summ
    print(f"Błąd bezwzględny: {err_abs}")
    err_rel = err_abs/true_val * 100
    print(f"Błąd względny: {err_rel} %")
    
    print(f"Czas wykonania dodawania algorytm Kahana: {t1_end - t1_start}")
    print(f"Czas wykonania dodawania rekurencyjnie: {t2_end - t2_start}")

main()
