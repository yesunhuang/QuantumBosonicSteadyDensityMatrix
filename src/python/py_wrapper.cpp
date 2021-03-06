// Copyright 2021 Ayajilin
// A wrapper between cpp and python

#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "python-utils/utils.h"
#include "expression/EpDeriver.h"
#include "solver/DPSolver.h"

/** ------------------------------------------------
 * 
 * * Python method area.
 * 
 --------------------------------------------------*/

static PyObject*
method_EpDeriver(PyObject* self, PyObject* args) {
    auto argReader = ayaji::PyArgsReader(args);
    // only accept 3 param
    if (!argReader.CheckArgsCount(3))
        Py_RETURN_NONE;
    auto argHams = argReader.GetArg(0);
    auto argCols = argReader.GetArg(1);
    auto argIdxs = argReader.GetArg(2);

    auto hams = ayaji::PyListTo<RawTerm>(argHams);
    auto cols = ayaji::PyListTo<RawTerm>(argCols);
    auto idxs = ayaji::PyListToNum<int>(argIdxs);

    EpDeriver* deriver = new EpDeriver(hams, cols, idxs);
    PyObject* cap = PyCapsule_New(deriver, "EpDeriver", [](PyObject* capsule) {
        EpDeriver* d = static_cast<EpDeriver*>(PyCapsule_GetPointer(capsule, "EpDeriver"));
        delete d;
    });

    return cap;
}

static PyObject*
method_DPSolver(PyObject* self, PyObject* args) {
    auto argReader = ayaji::PyArgsReader(args);
    // only accept 5 param
    if (!argReader.CheckArgsCount(5))
        Py_RETURN_NONE;
    auto argDim = argReader.GetArg(0);
    auto argEpDeriver = argReader.GetArg(1);
    auto argAlpha = argReader.GetArg(2);
    auto argEps = argReader.GetArg(3);
    auto argMaxRetTime = argReader.GetArg(4);

    auto Dim = ayaji::PyListToNum<int>(argDim);
    auto epDeriver = static_cast<EpDeriver*>(PyCapsule_GetPointer(argEpDeriver, "EpDeriver"));
    if (epDeriver == nullptr) {
        ayaji::Py_RaiseError(PyExc_TypeError, "Argument should be a EpDeriver type. Object: %R",
            argEpDeriver);
        Py_RETURN_NONE;
    }
    auto alpha = ayaji::PyObjectToNum<double>(argAlpha);
    auto eps = ayaji::PyObjectToNum<double>(argEps);
    auto maxRetTime = ayaji::PyObjectToNum<size_t>(argMaxRetTime);

    DPSolver* dpSolver = new DPSolver(Dim, *epDeriver, alpha, eps, maxRetTime);
    PyObject* cap = PyCapsule_New(dpSolver, "DPSolver", [](PyObject* capsule) {
        DPSolver* d = static_cast<DPSolver*>(PyCapsule_GetPointer(capsule, "DPSolver"));
        delete d;
    });

    return cap;
}

static PyObject*
method_DPSolverRun(PyObject* self, PyObject* args) {
    auto argReader = ayaji::PyArgsReader(args);
    // only accept 1 param
    if (!argReader.CheckArgsCount(1))
        Py_RETURN_NONE;
    auto argDpSolver = argReader.GetArg(0);

    auto dpSolver = static_cast<DPSolver*>(PyCapsule_GetPointer(argDpSolver, "DPSolver"));
    if (dpSolver == nullptr) {
        ayaji::Py_RaiseError(PyExc_TypeError, "Argument should be a DPSolver type. Object: %R",
            argDpSolver);
        Py_RETURN_NONE;
    }

    dpSolver->run();
    Py_RETURN_NONE;
}

static PyObject*
method_DPSolverGetResult(PyObject* self, PyObject* args) {
    auto argReader = ayaji::PyArgsReader(args);
    // only accept 1 param
    if (!argReader.CheckArgsCount(1))
        Py_RETURN_NONE;
    auto argDpSolver = argReader.GetArg(0);

    auto dpSolver = static_cast<DPSolver*>(PyCapsule_GetPointer(argDpSolver, "DPSolver"));
    if (dpSolver == nullptr) {
        ayaji::Py_RaiseError(PyExc_TypeError, "Argument should be a DPSolver type. Object: %R",
            argDpSolver);
        Py_RETURN_NONE;
    }

    MatrixMapper* mapper = dpSolver->getResult();
    PyObject* cap = PyCapsule_New(mapper, "MatrixMapper", nullptr);

    return cap;
}

/** ------------------------------------------------
 * 
 * * Python core runtime area.
 * 
 --------------------------------------------------*/

static PyMethodDef coreMethods[] = {
    {"EpSolver", method_EpDeriver, METH_VARARGS, NULL},
    {"DPSolver", method_DPSolver, METH_VARARGS, NULL},
    {"DPSolverRun", method_DPSolverRun, METH_VARARGS, NULL},
    {"DPSolverGetResult", method_DPSolverGetResult, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}       /* Sentinel */
};

static struct PyModuleDef coreModule = {
    PyModuleDef_HEAD_INIT,
    "core",                         /* name of module */
    /* module documentation, may be NULL */
    "The core module to solve steady density matrix problem for bosonic quantum system.",
    /* size of per-interpreter state of the module or -1
        if the module keeps state in global variables. */
    -1,
    coreMethods
};

PyMODINIT_FUNC
PyInit_core(void) {
    return PyModule_Create(&coreModule);
}
