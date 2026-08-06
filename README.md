# Quantum Bosonic Steady-State Density Matrix

`bostix` is a research-oriented Python/C++ solver for steady-state density matrices of dissipative bosonic systems in a truncated Fock basis. It converts Hamiltonian and Lindblad terms into coupled equations for density-matrix elements and solves them with a damped fixed-point (dynamic-programming) iteration.

## Evolution

The target is the stationary solution of the Lindblad master equation

$$
0 = -i[H, \rho]
    + \sum_j \frac{\gamma_j}{2}
      \left(2O_j\rho O_j^\dagger
      - O_j^\dagger O_j\rho
      - \rho O_j^\dagger O_j\right).
$$

Each bosonic mode is truncated to a finite dimension. For mode dimensions `[d0, d1, ...]`, the combined Hilbert-space dimension is `D = d0 * d1 * ...`, and the returned steady-state density matrix is a dense `D x D` complex matrix.

Current capabilities include:

- one or more truncated bosonic modes;
- Hamiltonians expressed as sums of ladder-operator monomials;
- single-boson loss channels;
- a damped Jacobi/fixed-point steady-state solver;
- trace-normalized dense density-matrix output; and
- occupation-number moments such as `mean(n_a)` and `mean(n_a * n_b)`.

The native code is written in C++11. Python calls it through the compiled `bostix.core` extension.

## Installation

### Prerequisites

You need:

- Python 3 with development headers;
- CMake 3.16 or newer;
- a C++11 compiler;
- OpenMP; and
- the repository requirements in `requirements.txt`.

On Windows, use a Visual Studio x64 Native Tools command prompt, or initialize MSVC manually with `vcvarsall.bat amd64`, before installing. On Linux, install your distribution's compiler, CMake, Python development, and OpenMP packages.

On macOS, OpenMP is not supplied by Apple Clang. In addition, the current source uses an absolute include for `/usr/local/include/omp.h`. That file must exist at exactly that path, or the source/include layout must be adjusted before the project will build; CMake include flags alone cannot redirect it.

### Build and install the Python package

Clone the repository and create an isolated environment:

```bash
git clone https://github.com/yesunhuang/QuantumBosonicSteadyDensityMatrix.git
cd QuantumBosonicSteadyDensityMatrix

python -m venv .venv
```

Activate the environment on Linux or macOS:

```bash
source .venv/bin/activate
```

On Windows PowerShell:

```powershell
.venv\Scripts\Activate.ps1
```

On Windows Command Prompt:

```bat
.venv\Scripts\activate.bat
```

Then install the build dependencies and package:

```bash
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
python -m pip install --no-build-isolation .
```

Installing `requirements.txt` first is important because this repository uses the classic scikit-build `setup.py` workflow and does not declare build-system requirements in a `pyproject.toml` file. The final command disables pip's isolated build environment so that the already-installed scikit-build tooling is available. The requirements file combines build tools and runtime packages with Matplotlib for the example notebooks.

## Quick start

This example contains one four-level mode with pure single-boson loss. Its steady state is the vacuum, so the mean occupation should be close to zero.

```python
from bostix.solver import DPSolver

solver = DPSolver(
    dim=[4],
    ham=[],
    col=[(1.0, "a")],
)

result = solver.solve(TMax=1000, eps=1e-5, alpha=0.5)

rho_ss = result.row_rho().as_2d_list()
mean_occupation = result.avg_moment(order=[1])

print(rho_ss)
print(mean_occupation)
```

Keep `solver` alive while using `result`: the result object refers to native memory owned by the solver. Copy the outputs you need before calling `solve()` again on the same solver, because a new solve invalidates its previous result.

## Defining a model

### Fock-space truncation

`dim` contains one positive truncation dimension per mode. For example, `dim=[3, 2]` keeps states `0, 1, 2` for the first mode and `0, 1` for the second mode. The code assumes these values are valid but does not currently check them.

### Operator strings

Hamiltonian and collapse terms are `(coefficient, operator_string)` pairs. The letter identifies both the mode and whether the operator creates or annihilates a boson:

| Mode | Creation | Annihilation |
| --- | --- | --- |
| First | `A` | `a` |
| Second | `B` | `b` |
| Third | `C` | `c` |
| ... | subsequent uppercase letters through `Z` | subsequent lowercase letters through `z` |

Examples:

- `"A"` represents the first-mode creation operator;
- `"a"` represents the first-mode annihilation operator;
- `"Aa"` represents the first-mode number operator;
- `"aB"` represents first-mode annihilation followed by second-mode
  creation; and
- `"Ab"` represents first-mode creation followed by second-mode
  annihilation.

Only ASCII letters are accepted, and every referenced mode must have a matching entry in `dim` (`B` or `b`, for example, requires at least two dimensions). Within a Hamiltonian monomial, group operators by mode and write each same-mode block in normal order, with creation operators before annihilation operators. Include Hermitian-conjugate Hamiltonian terms explicitly; the solver does not add them automatically.

For example, a driven mode coupled to a second lossy mode can be written as:

```python
from bostix.solver import DPSolver

E = 0.1
g = 0.4
kappa_a = 1.0
kappa_b = 1.0

hamiltonian = [
    (E, "a"),
    (E, "A"),
    (g, "aB"),
    (g, "Ab"),
]

collapse_terms = [
    (kappa_a, "a"),
    (kappa_b, "b"),
]

solver = DPSolver(dim=[3, 3], ham=hamiltonian, col=collapse_terms)
result = solver.solve(TMax=1000, eps=1e-5, alpha=0.5)
```

### Collapse-rate convention

The coefficient in a collapse term is the Lindblad rate `gamma`, not the amplitude `sqrt(gamma)`. For a channel with jump operator `sqrt(kappa) * a` in the common convention, pass `(kappa, "a")` to `col`.

The validated path is a lowercase, single-annihilation-operator loss term. The native parser accepts exactly one operator letter in each collapse term, so composite jump operators such as `"aa"` are not supported. Although uppercase creation operators pass the parser, the gain-channel branch is not a correct, validated implementation of the Lindblad equation and should not be used.

## Python API

### `DPSolver`

```text
DPSolver(*, dim, ham, col)
```

The constructor is keyword-only.

| Argument | Meaning |
| --- | --- |
| `dim` | List of Fock-space truncation dimensions, one per mode. |
| `ham` | List of `(complex_coefficient, operator_string)` Hamiltonian terms. |
| `col` | List of `(rate, one_operator_string)` collapse terms. |

### `DPSolver.solve`

```python
result = solver.solve(TMax, eps, alpha=value)
```

| Argument | Meaning |
| --- | --- |
| `TMax` | Positive integer giving the maximum number of fixed-point sweeps. |
| `eps` | Positive residual tolerance used by the native convergence check. |
| `alpha` | Mixing weight between the recurrence update and previous value. It is required and should normally satisfy `0 < alpha <= 1`; repository examples use `0.5`. |

The native solver knows whether it stopped by tolerance or by the iteration
limit, but the current Python wrapper does not expose that status. It always
returns the last iterate.

### Results

```python
# Dense D x D list of Python complex values
rho = result.row_rho().as_2d_list()

# Occupation-number moment; one exponent per mode
moment = result.avg_moment(order=[1, 1])
```

For multiple modes, the first entry in `dim` is the most-significant mode in the flattened matrix basis. For example, with `dim=[d0, d1]`, the state `[n0, n1]` has matrix index `n0 * d1 + n1`.

`avg_moment([p0, p1, ...])` computes

$$
\left\langle \prod_i n_i^{p_i} \right\rangle
= \sum_{n_0,n_1,\ldots}
  \left(\prod_i n_i^{p_i}\right)
  \rho_{n_0n_0,n_1n_1,\ldots}.
$$

It is an occupation-number moment, not a normally ordered factorial moment. Use exactly one non-negative integer exponent per mode.

Both result operations restore the solver's internal factorial-scaled representation and trace-normalize the state before returning values.

## Principles

The expression builder derives, for every density-matrix multi-index `x`, a sparse recurrence of the form

$$
P_0(x)\rho_x = \sum_r P_r(x+r)\rho_{x+r}.
$$

For non-vacuum entries, `DPSolver` applies the damped update

$$
\rho_x^{(t+1)} =
\alpha\frac{\sum_r P_r(x+r)\rho_{x+r}^{(t)}}{P_0(x)}
+ (1-\alpha)\rho_x^{(t)}.
$$

The vacuum coefficient receives a separate, implementation-specific normalization update. Each sweep reads from one dense buffer and writes to another, and the implementation writes conjugate-transposed elements to maintain Hermiticity. Physical factorial restoration and trace normalization occur when the result is first read.

If `D = product(dim)`, the density matrix contains `D^2` complex values. Memory therefore scales as `O(D^2)`, and a sweep scales approximately as `O(D^2 * K)`, where `K` is the number of distinct neighbor couplings. Small Fock cutoffs are strongly recommended while developing a model.

## Standalone C++ build

The native example target can be built without installing the Python package:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
```

This produces `build/main` on Unix-like systems, `build/main.exe` with a single-config Windows generator, or `build/Release/main.exe` with the Visual Studio generator. The executable is a hard-coded demonstration, not a command-line interface. In the current source it prints a full `1000 x 1000` matrix, so expect very large output if you run it.

## Tests and notebooks

The small automated test is `test/test.py`. After installing the package, run it from the parent directory of the checkout so that the unbuilt source package does not shadow the installed native extension:

```bash
cd ..
python QuantumBosonicSteadyDensityMatrix/test/test.py
```

The notebooks under `test/` contain decay, driven-mode, coupled-mode, and nonlinear examples, including comparisons with QuTiP. They are exploratory notebooks and may require optional packages such as NumPy, SciPy, and QuTiP that are not declared as core dependencies.

## Repository layout

| Path | Contents |
| --- | --- |
| `bostix/` | Python-facing solver and result wrappers. |
| `include/` | C++ matrix, expression, and solver headers. |
| `src/cpp/` | Dynamic-programming solver, expression derivation, and complex arithmetic. |
| `src/python/py_wrapper.cpp` | CPython extension bindings. |
| `src/main.cpp` | Standalone hard-coded C++ demonstrations. |
| `test/` | A Python smoke test and exploratory notebooks. |
| `ImplementInstruction/` | Original Chinese mathematical and design notes, with Markdown and PDF versions. |
| `QOsystemMonteCarlo/` | A separate Monte Carlo prototype, data files, and report notebook; it is not part of the main CMake target. |

The documents in `ImplementInstruction/` describe the original design and include proposed interfaces that are not present in the current Python API. Use the API documented in this README and in `bostix/solver.py` for the implemented behavior.

## Known limitations

- The Python package exposes only `DPSolver`; the experimental C++ `TDSolver` is not wrapped for Python.
- The main dynamic-programming sweep is currently serial even though OpenMP is required and linked by the build.
- Partial-trace output is declared in C++ but not implemented.
- Composite collapse operators are unsupported, and the parsed creation/gain path is not a validated implementation.
- Convergence status is discarded by the Python wrapper.
- Parameter ranges, operator-mode bounds, and moment-order lengths receive
  little or no validation; invalid input may fail in native code.
- A result becomes invalid if its solver is destroyed or used to solve again.
- Every `row_rho()` call currently leaks its temporary dense native matrix;
  call it once per result and retain the list returned by `as_2d_list()`.
- The letter-based input syntax can name at most 26 modes.
- Dense `D x D` storage makes the method expensive as the mode count or Fock cutoffs increase.
- The Monte Carlo and neural-network methods discussed in the original design notes are experimental or unimplemented, not supported Python features.
