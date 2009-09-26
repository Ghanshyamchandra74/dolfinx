"""Unit tests for the fem interface"""

__author__ = "Garth N. Wells (gnw20@cam.ac.uk)"
__date__ = "2009-07-28"
__copyright__ = "Copyright (C) 2009 Garth N. Wells"
__license__  = "GNU LGPL Version 2.1"

import unittest
from dolfin import *

class Assembly(unittest.TestCase):

    def test_facet_assembly(self):

        mesh = UnitSquare(24, 24)
        V = FunctionSpace(mesh, "DG", 1)

        # Define test and trial functions
        v = TestFunction(V)
        u = TrialFunction(V)

        # Define normal component, mesh size and right-hand side
        n = V.cell().n
        h = CellSize(mesh)
        h_avg = (h('+') + h('-'))/2
        f = Function(V, "500.0*exp(-(pow(x[0] - 0.5, 2) + pow(x[1] - 0.5, 2)) / 0.02)")

        # Define bilinear form
        a = dot(grad(v), grad(u))*dx \
           - dot(avg(grad(v)), jump(u, n))*dS \
           - dot(jump(v, n), avg(grad(u)))*dS \
           + 4.0/h_avg*dot(jump(v, n), jump(u, n))*dS \
           - dot(grad(v), u*n)*ds \
           - dot(v*n, grad(u))*ds \
           + 8.0/h*v*u*ds

        # Define linear form
        L = v*f*dx

        A_frobenius_norm = 157.867392939
        b_l2_norm = 1.48087142739
        self.assertAlmostEqual(assemble(a).norm("frobenius"), A_frobenius_norm, 8)
        self.assertAlmostEqual(assemble(L).norm("l2"), b_l2_norm, 8)

        A, b = assemble_system(a, L)
        self.assertAlmostEqual(A.norm("frobenius"), A_frobenius_norm, 8)
        self.assertAlmostEqual(b.norm("l2"), b_l2_norm, 8)

    def test_cell_assembly(self):

        mesh = UnitCube(4, 4, 4)
        V = VectorFunctionSpace(mesh, "DG", 1)

        v = TestFunction(V)
        u = TrialFunction(V)
        f = Constant(mesh, (10, 20, 30))

        def epsilon(v):
            return 0.5*(grad(v) + grad(v).T)

        a = inner(epsilon(v), epsilon(u))*dx
        L = inner(v, f)*dx

        A_frobenius_norm =  4.39696865276
        b_l2_norm = 0.954703269782
        self.assertAlmostEqual(assemble(a).norm("frobenius"), A_frobenius_norm, 8)
        self.assertAlmostEqual(assemble(L).norm("l2"), b_l2_norm, 8)

        A, b = assemble_system(a, L)
        self.assertAlmostEqual(A.norm("frobenius"), A_frobenius_norm, 8)
        self.assertAlmostEqual(b.norm("l2"), b_l2_norm, 8)


if __name__ == "__main__":
    print ""
    print "Testing basic PyDOLFIN fem operations"
    print "------------------------------------------------"
    unittest.main()
