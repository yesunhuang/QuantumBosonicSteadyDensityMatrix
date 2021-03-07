from abc import ABCMeta, abstractmethod
from typing import Any, List, Tuple, Union

import attr
from matrix.mapper import MatrixMapper
from matrix.tensor import TensorMatrix
from utils.form import alpha_to_numeric

from bostix import core


def ham_col_to_alpha_form(
    l: List[List[Union[complex, str]]]
) -> List[List[Union[complex, List[int]]]]:
    ret = list()
    for ele in l:
        if len(ele) != 2:
            raise TypeError(
                f"Element must have form of `['Coef': complex, 'Operators': str]`: {repr(ele)}"
            )
        ret.append((ele[0], alpha_to_numeric(ele[1])))
    return ret


@attr.s(kw_only=True)
class SolverBase:
    __metaclass__ = ABCMeta

    dim: List[int] = attr.ib()
    ham: List[List[Union[complex, str]]] = attr.ib(converter=ham_col_to_alpha_form)
    col: List[List[Union[complex, str]]] = attr.ib(converter=ham_col_to_alpha_form)

    @ham.validator
    def _check_ham(self, attribute, value) -> None:
        for ele in value:
            if len(ele) != 2:
                raise TypeError(
                    f"Element in `ham` must have form of `['Coef': complex, 'Operators': str]`: {repr(ele)}"
                )

    @ham.validator
    def _check_col(self, attribute, value) -> None:
        for ele in value:
            if len(ele) != 2:
                raise TypeError(
                    f"Element in `col` must have form of `['Coef': complex, 'Operators': str]`: {repr(ele)}"
                )

    @abstractmethod
    def solve(self, TMax: int, eps: float, **hyper_params) -> Any:
        pass


@attr.s(kw_only=True)
class DPSolver(SolverBase):
    _ep_deriver: Any = attr.ib(default=None)
    _dp_solver: Any = attr.ib(default=None)

    def solve(self, TMax: int, eps: float, **hyper_params) -> MatrixMapper:
        alpha = hyper_params.get("alpha")
        if alpha is None:
            raise TypeError("hyper_params must have `alpha`")

        self._ep_deriver = core.EpSolver(self.ham, self.col, [0 for i in range(2 * len(self.dim))])
        self._dp_deriver = core.DPSolver(self.dim, self._ep_deriver, alpha, eps, TMax)
        core.DPSolverRun(self._dp_deriver)
        return MatrixMapper(raw_mapper=core.DPSolverGetResult(self._dp_deriver))


if __name__ == "__main__":
    print("Decay Test:")
    a = DPSolver(dim=[4], ham=[], col=[(1, "a")])
    print(a)
    matrix_mapper = a.solve(1000, 1e-5, alpha=0.5)
    print(matrix_mapper.row_rho().as_2d_list())
    print(matrix_mapper.avg_moment([1]))

    print("Single Mode Pump test:")
    E=0.2
    delta=0.5
    a = DPSolver(dim=[3], ham=[(E,"a"),(E,"A"),(delta,"Aa")], col=[(1, "a")])
    print(a)
    matrix_mapper = a.solve(1000, 1e-5, alpha=0.5)
    print(matrix_mapper.row_rho().as_2d_list())
    print(matrix_mapper.avg_moment([2]))

    print("Decay Rabi Pump test:")
    g=0.4
    E=0.4
    a = DPSolver(dim=[2,2], ham=[(E,"a"),(E,"A"),(g,"aB"),(g,"Ab")], col=[(1, "a"),(1,'b')])
    print(a)
    matrix_mapper = a.solve(1000, 1e-10, alpha=0.5)
    print(matrix_mapper.row_rho().as_2d_list())
    print(matrix_mapper.avg_moment([1,1]))

