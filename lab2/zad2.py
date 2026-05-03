import numpy as np

N = [5,10,50,100,200,500]

def factorize_LU(A):
    n = len(A)
    U = A.copy()
    L = np.eye(n)

    for i in range(n):
        for j in range(i+1,n):
            L[j][i] = U[j][i] / U[i][i]
            U[j] -= U[i] * L[j][i]

    return L, U

def main():
    for n in N:
        A = np.random.rand(n,n)
        A_org = A.copy()

        L,U = factorize_LU(A)

        err = np.linalg.norm(A_org - L @ U)
        print(f"For random matrix {n}x{n}\nAboulute erorr: {err}")

main()

