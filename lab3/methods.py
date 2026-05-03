from functions import *
import numpy as np 
import math

def bisection(a,b,eps,max_iter,f):
    # a + (b-a)/2
    # |f(c) < eps|
    # |b - a| < 
    
    if np.sign(f(a)) == np.sign(f(b)):
        print("Błędne krańce")
        return
    
    for i in range(max_iter):
        c = a + (b - a) / 2

        if abs(f(c)) < eps:
            return c, i + 1
        elif abs((b - a) / 2) < eps:
            return c, i + 1

        if np.sign(f(a)) != np.sign(f(c)):
            b = c
        else:
            a = c

    return a + (b - a) / 2, max_iter

def newton(a,b,eps,max_iter,f,fp):

    if np.sign(f(a)) == np.sign(f(b)):
        print("Błędne krańce")
        return

    x0 = (a + b) / 2 

    for i in range(max_iter):
        xn = x0 - (f(x0)/fp(x0))
        
        if abs(f(xn)) < eps:
            return xn, i + 1
        elif abs(xn - x0) < eps:
            return xn, i + 1

        x0 = xn

    return x0 - f(x0)/fp(x0) , max_iter

def secant(a,b,eps,max_iter,f):
    x0 = a 
    x1 = b

    if np.sign(f(x0)) == np.sign(f(x1)):
        print("Błędne krańce")
        return

    for i in range(max_iter):
        x2 = (f(x1)*x0 - f(x0)*x1) / (f(x1)-f(x0))

        if abs(f(x2)) < eps:
            return x2, i + 1

        x0, x1 = x1, x2
    
    return (f(x1)*x0 - f(x0)*x1) / (f(x1)-f(x0)), max_iter

def hybrid(a,b,eps,max_iter,f,to_bisect):

    if np.sign(f(a)) == np.sign(f(b)):
        print("Błędne krańce")
        return

    counter = 0

    for i in range(to_bisect):
        counter += 1
        c = a + (b - a) / 2

        if abs(f(c)) < eps:
            return c, i + 1
        elif abs((b - a) / 2) < eps:
            return c, i + 1

        if np.sign(f(a)) != np.sign(f(c)):
            b = c
        else:
            a = c

    x0 = a 
    x1 = b

    if np.sign(f(x0)) == np.sign(f(x1)):
        print("Błędne krańce")
        return

    for i in range(max_iter):
        counter += 1
        x2 = (f(x1)*x0 - f(x0)*x1) / (f(x1)-f(x0))

        if abs(f(x2)) < eps:
            return x2, i + 1 + counter

        x0, x1 = x1, x2

    return (f(x1)*x0 - f(x0)*x1) / (f(x1)-f(x0)), counter + max_iter




eps = 10e-10
print("BISEKCJA")
print(bisection((3/2) * math.pi,2*math.pi,eps,10000,f1))
print(bisection(0.0001,math.pi/2,eps,10000,f2))
print(bisection(1,3,eps,10000,f3))

print("NEWTON")
print(newton((3/2) * math.pi,2*math.pi,eps,10000,f1,f1_p))
print(newton(0.0001,math.pi/2,eps,10000,f2,f2_p))
print(newton(1,3,eps,10000,f3,f3_p))

print("SECANT")
print(secant((3/2) * math.pi,2*math.pi,eps,10000,f1))
print(secant(0.0001,math.pi/2,eps,10000,f2))
print(secant(1,3,eps,10000,f3))

print("HYBRID")
print(hybrid((3/2) * math.pi,2*math.pi,eps,10000,f1,5))
print(hybrid(0.0001,math.pi/2,eps,10000,f2,5))
print(hybrid(1,3,eps,10000,f3,5))
