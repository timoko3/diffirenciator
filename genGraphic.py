import matplotlib.pyplot as plt
import numpy as np

import numpy as np

import operator
import math
addH = operator.add    
subH = operator.sub    
mulH = operator.mul    
divH = operator.truediv
powH = operator.pow    

sinH = math.sin
cosH = math.cos
tanH = math.tan
cotH = lambda x: 1 / math.tan(x)
lnH = math.log
shH = math.sinh
chH = math.cosh
arcsinH = math.asin
arccosH = math.acos
arctanH = math.atan
sqrtH = math.sqrt
def y(x):
	return np.sin(x)
def taylor(x):
	return (x-1)**0+(x-1)**1+-1/2*(x-1)**2
def y_prime(x):
	return np.cos(x)
x_vals = np.linspace(-5, 5, 400)
plt.figure(figsize=(10, 6))
plt.plot(x_vals, y(x_vals), label="f(x)", linewidth=2)
plt.plot(x_vals, taylor(x_vals), '--', label="Тейлор f(x)", linewidth=2)
plt.plot(x_vals, y_prime(x_vals), label="f'(x)", linewidth=2)
plt.xlabel("x", fontsize=12)
plt.ylabel("y", fontsize=12)
plt.grid(True, alpha=0.3)
plt.savefig("graph.jpg")
