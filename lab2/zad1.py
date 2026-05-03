import numpy as np
from time import time

N = [5,10,50,100,200,500]

def gauss_jordan(A,B):
    n = len(A)
    A_ext = np.concatenate((A,np.resize(B,(n,1))),axis=1)
    for i in range(n):
        max_factor_id = np.argmax(A_ext[i:, i]) + i
        A_ext[[i,max_factor_id]] = A_ext[[max_factor_id,i]]

        A_ext[i] /= A_ext[i][i]

        for j in range(i+1,n):
            mult_fact = -A_ext[j][i] / A_ext[i][i]
            A_ext[j] += A_ext[i] * mult_fact 

    return A_ext[:, :-1], A_ext[:, -1]

def gauss_jordan_up_down(A,B):
    n = len(A)
    A_ext = np.concatenate((A,np.resize(B,(n,1))),axis=1)
    for i in range(n):
        max_factor_id = np.argmax(A_ext[i:, i]) + i
        A_ext[[i,max_factor_id]] = A_ext[[max_factor_id,i]]

        A_ext[i] /= A_ext[i][i]

        for j in range(i+1,n):
            mult_fact = -A_ext[j][i] / A_ext[i][i]
            A_ext[j] += A_ext[i] * mult_fact 

    for i in range(n-1,0,-1):
        for j in range(i-1,-1,-1):
            mult_fact = -A_ext[j][i] / A_ext[i][i]
            A_ext[j] += A_ext[i] * mult_fact


    return A_ext[:, :-1], A_ext[:, -1]

def back_sub(A,B):
    n = len(A)
    res = np.zeros(n)

    for i in range(n-1,-1,-1):
        res[i] = (B[i] - np.dot(A[i,i+1:],res[i+1:])) / A[i][i]

    return res

def solve_back_sub(A,B):
    upper, new = gauss_jordan(A,B)
    return back_sub(upper,new)

def solve_up_down(A,B):
    _, res = gauss_jordan_up_down(A,B)
    return res

def main():
    for n in N:
        A = np.random.rand(n,n)
        B = np.random.rand(n)

        gj_time1 = time()
        res1 = solve_back_sub(A,B)
        gj_time2 = time()

        np_time1 = time()
        res2 = np.linalg.solve(A,B)
        np_time2 = time()

        success = np.allclose(res1,res2)
        print(success)

        print(f"For random matrix {n}x{n}\nGauss Jordan (back sub) time: {gj_time2 - gj_time1}\nNumPy time: {np_time2 - np_time1}\n")

main()
