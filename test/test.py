import unittest

import bostix
from bostix.matrix.mapper import MatrixMapper
from bostix.matrix.tensor import TensorMatrix
from bostix.solver import DPSolver
from bostix.utils.form import alpha_to_numeric


class TestSolver(unittest.TestCase):
    def test_simplest(self):
        dp = DPSolver(dim=[4], ham=[], col=[(1, "a")])
        print(dp)
        matrix_mapper = dp.solve(10, 1e-10, alpha=0.5)
        print(matrix_mapper.row_rho().as_2d_list())
        self.assertEqual(matrix_mapper.avg_moment(order=[1]), complex(0.0, 0.0))


if __name__ == "__main__":
    unittest.main()
