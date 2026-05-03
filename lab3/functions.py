import math

def f1(x):
    return math.cos(x) * math.cosh(x) - 1

def f2(x):
    return 1/x - math.tan(x)

def f3(x):
    return math.pow(2,-x) + math.pow(math.e,x) + 2 * math.cos(x) - 6 

def f1_p(x):
    return math.cos(x) * math.sinh(x) - math.sin(x) * math.cosh(x)

def f2_p(x):
    return -(1/math.pow(x,2)) - (1/math.pow(math.cos(x),2))

def f3_p(x):
    return math.pow(math.e,x) - math.pow(2,-x) * math.log(2) - 2 * math.sin(x)
