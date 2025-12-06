import matplotlib.pyplot as plt
import numpy as np

import numpy as np

import operator
import math
def y(x):
	return np.cos(x)*np.sin(x)
def taylor(x):
	return -1*(x-5)**1+0/2*(x-5)**2+4/6*(x-5)**3+0/24*(x-5)**4+-16/120*(x-5)**5
def y_prime(x):
	return np.sin(x)*-1*np.sin(x)+np.cos(x)*np.cos(x)
def tangent(x):
	return np.cos(5)*np.sin(5)+(np.sin(5)*-1*np.sin(5)+np.cos(5)*np.cos(5))*(x-5)
x_vals = np.linspace(-5, 10, 1000)
plt.figure(figsize=(10, 6))
plt.plot(x_vals, y(x_vals), label="f(x)", linewidth=2)
plt.plot(x_vals, taylor(x_vals), '--', label="Тейлор f(x)", linewidth=2)
plt.plot(x_vals, tangent(x_vals), label="касетельная", linewidth=2)
plt.axis([-5, 10, -5, 5])
plt.plot(5, y(5),
	marker='o',
	markersize=10,
	color='red',
	label='точка разложения')
plt.xlabel("x", fontsize=12)
plt.ylabel("y", fontsize=12)
plt.grid(True, alpha=0.3)
plt.savefig("graph.jpg")
