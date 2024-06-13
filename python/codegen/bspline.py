#!/usr/bin/env python3

from sfem_codegen import *

import sympy as sp
import numpy as np
import matplotlib.pyplot as plt

def bspline(i, order, t):
	if order == 1:
		return sp.Piecewise((1, (t >= i)), (0, True)) * sp.Piecewise((1, (t < i+1)), (0, True))
	else:
		return (t - i)/(i+order-1 - i) * bspline(i, order-1, t) + (i+order - t)/(i+order - (i+1)) * bspline(i+1, order-1, t) 

order = 5
extras = 0
nctrl = (order + 1) + extras
# if False:
if True:
	
	x = np.linspace(0, nctrl-1, nctrl)
	# y = np.sin(x * (2*3.14/nctrl))
	y = (x-nctrl/2)**2 + np.sin(x * (2*3.14/nctrl))

	n = 100
	t = np.linspace(0, nctrl-1, n)
	yt = np.zeros(t.shape)

	for i in range(0, n):
		ti = t[i]

		f = 0
		for j in range(0, nctrl):
			f += y[j] * bspline(j-(order+1)/2, order+1, ti)
		yt[i] = f

	plt.plot(x, y, marker='8')
	plt.plot(t, yt)
	plt.savefig("bspline.pdf")

expr = []
t = sp.symbols('t', positive=True)
for j in range(0, nctrl):
	s = bspline(j-(order+1)/2,order+1, t)
	var = sp.symbols(f'w[{j}]') 
	expr.append(ast.Assignment(var, s))

c_code(expr)


