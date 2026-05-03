import numpy as np

s = [2,3.6667,5,7.2,10]
n = [50,100,200,500,1000]

def zeta_f32_forward(s,n):
    summ = np.float32(0)
    for k in range(1,n):
        summ += np.float32(1/(k**s))
    return summ

def zeta_f32_backward(s,n):
    summ = np.float32(0)
    for k in range(n,0,-1):
        summ += np.float32(1/(k**s))
    return summ
    
def eta_f32_forward(s,n):
    summ = np.float32(0)
    for k in range(1,n):
        summ += np.float32((-1)**(k-1)) * np.float32((1/(k**s)))
    return summ

def eta_f32_backward(s,n):
    summ = np.float32(0)
    for k in range(n,0,-1):
        summ += np.float32((-1)**(k-1)) * np.float32((1/(k**s)))
    return summ

def zeta_f64_forward(s,n):
    summ = np.float64(0)
    for k in range(1,n):
        summ += np.float64(1/(k**s))
    return summ

def zeta_f64_backward(s,n):
    summ = np.float64(0)
    for k in range(n,0,-1):
        summ += np.float64(1/(k**s))
    return summ
    
def eta_f64_forward(s,n):
    summ = np.float64(0)
    for k in range(1,n):
        summ += np.float64((-1)**(k-1)) * np.float64((1/(k**s)))
    return summ

def eta_f64_backward(s,n):
    summ = np.float64(0)
    for k in range(n,0,-1):
        summ += np.float64((-1)**(k-1)) * np.float64((1/(k**s)))
    return summ

def main():
    for i in range(len(s)):
        print(f"S: {s[i]}, N: {n[i]}")
        print("")
        print("Zeta")
        print("Float32: ")
        print(f"Forward: {zeta_f32_forward(s[i],n[i])}")
        print(f"Backward: {zeta_f32_backward(s[i],n[i])}")
        print("Float64: ")
        print(f"Forward: {zeta_f64_forward(s[i],n[i])}")
        print(f"Backward: {zeta_f64_backward(s[i],n[i])}")
        print("")
        print("Eta")
        print("Float32: ")
        print(f"Forward: {eta_f32_forward(s[i],n[i])}")
        print(f"Backward: {eta_f32_backward(s[i],n[i])}")
        print("Float64: ")
        print(f"Forward: {eta_f64_forward(s[i],n[i])}")
        print(f"Backward: {eta_f64_backward(s[i],n[i])}")
        print("")

main()
