#!/usr/bin/env python3

from sfem_codegen import *
from tet4 import *
from tet10 import *
from tri3 import *
from tri6 import *
from mini import *
from fe_material import *

import pdb

simplify_expr = False
# simplify_expr = True

# Implicit Euler
# Chorin's projection method
# 1) temptative momentum step
# 	`1/dt * <u, v> + nu * <grad(u), grad(v)> = <u_old, v> - <(u_old . div) * u_old, v>`
# 2) Potential eqaution
#  	`<grad(p), grad(q)> = - 1/dt * <div(u), q>`
# 3) Projection/Correction
#  	`<u_new, v> = <u, v> - dt * <grad(p), v>`

class NavierStokesOp:
	def __init__(self, fe_vel, fe_pressure):
		self.fe_vel = fe_vel
		self.fe_pressure = fe_pressure

		if fe_vel.spatial_dim() == 2:
			qp = [qx, qy]
		else:
			assert fe_vel.spatial_dim() == 3
			qp = [qx, qy, qz]

		qp = sp.Matrix(fe_vel.spatial_dim(), 1, qp)
		self.qp = qp
		
		grad_vel = fe_vel.physical_tgrad(qp)
		fun_vel = fe_vel.tfun(qp)
		fun_pressure = fe_pressure.fun(qp)
		grad_pressure = fe_pressure.grad(qp)

		n_vel = len(grad_vel) 
		n_pressure = len(fun_pressure)

		mu, rho, dt = sp.symbols('mu rho dt')
		self.params = [mu, rho, dt]

		self.form2_diffusion = sp.zeros(n_vel, n_vel)
		self.form2_mass = sp.zeros(n_vel, n_vel)

		# self.form2_divergence = sp.zeros(n_vel, n_pressure)

		self.form2_laplacian = sp.zeros(n_pressure, n_pressure)
		

		for i in range(0,  n_vel):
			for j in range(0,  n_vel):
				integr = mu * fe_vel.integrate(qp, inner(grad_vel[i], grad_vel[j])) *  fe_vel.jacobian_determinant(qp)
				self.form2_diffusion[i, j] = integr

		for i in range(0,  n_vel):
			for j in range(0,  n_vel):
				integr = (1/dt) * fe_vel.integrate(qp, inner(fun_vel[i], fun_vel[j])) *  fe_vel.jacobian_determinant(qp)
				self.form2_mass[i, j] = integr

		# for i in range(0,  n_vel):
		# 	for j in range(0,  n_pressure):
		# 		integr = -(1/dt)*fe_pressure.integrate(qp, tr(grad_vel[i]) * fun_pressure[j]) * fe_pressure.jacobian_determinant(qp)
		# 		self.form2_divergence[i, j] = integr

		for i in range(0,  n_pressure):
			for j in range(0,  n_pressure):
				integr = fe_pressure.integrate(qp, inner(grad_pressure[i], grad_pressure[j])) * fe_pressure.jacobian_determinant(qp)
				self.form2_laplacian[i, j] = integr


		u = coeffs('u', n_vel)
		uh = u[0] * fun_vel[0]
		for i in range(1, n_vel):
			uh += u[i] * fun_vel[i]

		grad_uh =  u[0] * grad_vel[0]
		for i in range(1, n_vel):
			grad_uh += u[i] * grad_vel[i]

		div_uh = tr(grad_uh)

		p = coeffs('p', n_pressure)
		grad_ph = p[0] * grad_pressure[0]
		for i in range(1, n_pressure):
			grad_ph += p[i] * grad_pressure[i]

		#########################################################
		# CONVECTION
		conv = sp.zeros(fe_vel.spatial_dim(), 1)

		for d in range(0, fe_vel.spatial_dim()):
			val = 0
			for d1 in range(0, fe_vel.spatial_dim()):
				val += uh[d1] * grad_uh[d, d1]
			conv[d] = val
		#########################################################

		self.form1_convection = sp.zeros(n_vel, 1)
		self.form1_divergence = sp.zeros(n_pressure, 1)
		self.form1_correction = sp.zeros(n_vel, 1)
		self.form1_utm1 = sp.zeros(n_vel, 1)

		#########################################################

		for i in range(0, n_vel):
			integr = fe_vel.integrate(qp, inner(conv, fun_vel[i])) * fe_vel.jacobian_determinant(qp)
			self.form1_convection[i] = integr

		for i in range(0, n_vel):
			integr = fe_vel.integrate(qp, inner(uh, fun_vel[i])) * fe_vel.jacobian_determinant(qp)
			self.form1_utm1[i] = integr

		for i in range(0,  n_pressure):
			integr = -(1/dt)*fe_pressure.integrate(qp, div_uh * fun_pressure[i]) * fe_pressure.jacobian_determinant(qp)
			self.form1_divergence[i] = integr

		for i in range(0, n_vel):
			integr = (fe_vel.integrate(qp, inner(uh, fun_vel[i])) - dt * fe_vel.integrate(qp, inner(grad_ph, fun_vel[i]))) * fe_vel.jacobian_determinant(qp)
			self.form1_correction[i] = integr

		print('------------------------------')
		print('LHS (diffusion, temptative momentum)')
		print('------------------------------')
		c_code(self.assign_matrix(self.form2_mass + self.form2_diffusion))

		print('------------------------------')
		print('LHS (potential equation)')
		print('------------------------------')
		c_code(self.assign_matrix(self.form2_laplacian))

		print('------------------------------')
		print('RHS (temptative momentum)')
		print('------------------------------')
		c_code(self.assign_matrix(self.form1_utm1 - self.form1_convection))

		print('------------------------------')
		print('RHS (potential equation)')
		print('------------------------------')
		c_code(self.assign_matrix(self.form1_divergence))

		print('------------------------------')
		print('RHS (correction equation)')
		print('------------------------------')
		c_code(self.assign_matrix(self.form1_correction))

	# def apply(self):
	# 	H = self.hessian
	# 	rows, cols = H.shape
	# 	x = self.increment

	# 	Hx = H * x
	# 	return self.assign_vector(Hx)

	# def gradient(self):
	# 	return self.apply()

	def assign_tensor(self, name, a):
		fe = self.fe_vel
		qp = self.qp
		rows, cols = a.shape

		expr = []
		for i in range(0,  rows):
			for j in range(0,  cols):
				var = sp.symbols(f'{name}[{i*cols + j}]')
				value = a[i, j]

				value = subsmat(value, fe.symbol_jacobian_inverse(), fe.jacobian_inverse(qp))

				if simplify_expr:
					value = sp.simplify(value)
					
				expr.append(ast.Assignment(var, value))
		return expr

	def assign_matrix(self, a):
		return self.assign_tensor("element_matrix", a)

	def assign_vector(self, a):
		return self.assign_tensor("element_vector", a)

def main():
	# op = NavierStokesOp(Mini2D())
	op = NavierStokesOp(Tri6(), Tri3())

if __name__ == "__main__":
	main()
