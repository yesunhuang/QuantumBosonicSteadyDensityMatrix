from typing import List

import attr

from bostix import core

from .tensor import TensorMatrix


@attr.s(kw_only=True)
class MatrixMapper:
    _raw_mapper = attr.ib()

    def row_rho(self) -> TensorMatrix:
        return TensorMatrix(raw_matrix=core.MatrixMapperRowRho(self._raw_mapper))

    def avg_moment(self, order: List[int]) -> complex:
        return core.MatrixMapperAvgMoment(self._raw_mapper, order)
