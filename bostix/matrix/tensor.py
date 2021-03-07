from typing import List

import attr

from bostix import core


@attr.s(kw_only=True)
class TensorMatrix:
    _raw_matrix = attr.ib()

    def as_2d_list(self) -> List[List[complex]]:
        return core.TensorMatrixTo2DList(self._raw_matrix)
