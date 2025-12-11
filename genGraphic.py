import matplotlib.pyplot as plt
import numpy as np

import numpy as np

import operator
import math
def y(x):
	return np.sin(15*x**3+7)+np.cos(9*x+6)**5
def taylor(x):
	return -1*(x-1)**0+-90*(x-1)**1+-1305/2*(x-1)**2+94680/6*(x-1)**3+2.76656e+06/24*(x-1)**4+-1.4919e+08/120*(x-1)**5+-8.06193e+09/720*(x-1)**6+2.03023e+11/5040*(x-1)**7
def y_prime(x):
	return np.cos(15*x**3+7)*15*1*3*x**2+np.sin(9*x+6)*-1*9*5*np.cos(9*x+6)**4
def tangent(x):
	return np.sin(15*1**3+7)+np.cos(9*1+6)**5+(np.cos(15*1**3+7)*15*1*3*1**2+np.sin(9*1+6)*-1*9*5*np.cos(9*1+6)**4)*(x-1)
x_vals = np.linspace(-0.5, 2.5, 1000)
plt.figure(figsize=(10, 6))
plt.plot(x_vals, y(x_vals), label="f(x)", linewidth=2)
plt.plot(x_vals, taylor(x_vals), '--', label="Тейлор f(x)", linewidth=2)
plt.plot(x_vals, tangent(x_vals), label="касетельная", linewidth=2)
plt.axis([-0.5, 2.5, -10, 10])
plt.plot(1, y(1),
	marker='o',
	markersize=10,
	color='red',
	label='точка разложения')
plt.xlabel("x", fontsize=12)
plt.ylabel("y", fontsize=12)
plt.grid(True, alpha=0.3)
plt.legend()
plt.savefig("graph.jpg")
