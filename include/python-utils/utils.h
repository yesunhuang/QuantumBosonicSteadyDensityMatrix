// Copyright 2021 Ayajilin
// Some utils for c-python interface.
#ifndef INCLUDE_PYTHON_UTILS_UTILS_H_
#define INCLUDE_PYTHON_UTILS_UTILS_H_

#include "./complex.h"
#include "Python.h"
#include "expression/EpDeriver.h"
#include <assert.h>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ayaji {

template <typename... Args>
inline void Py_DEBUG(const char *fmt, Args &&...args) {
#ifdef _DEBUG
    printf(fmt, std::forward<Args>(args)...);
#endif
}

/**
 * @brief Raise a python error with message.
 *
 * @tparam Args
 * @param errType Python error type, e.g. PyExc_RuntimeError.
 * @param errFmt
 * @param args
 */
template <typename... Args>
inline void Py_RaiseError(PyObject *errType, const char *errFmt,
                          Args &&...args) {
    if (!PyErr_Occurred()) {
        PyErr_Format(errType, errFmt, std::forward<Args>(args)...);
    }
    PyErr_Print();
}

/**
 * @brief Reader for python args.
 *
 */
class PyArgsReader {
  public:
    explicit PyArgsReader(PyObject *args) {
        assert(PyTuple_Check(args));
        this->args = args;
    }
    /**
     * @brief Get the count of args.
     *
     * @return unsigned count of args
     */
    unsigned GetArgsCount() {
        auto cnt = PyTuple_Size(this->args);
        assert(cnt >= 0);
        return static_cast<unsigned>(cnt);
    }
    /**
     * @brief Check count of args.
     *
     * @param n
     * @return true
     * @return false
     */
    bool CheckArgsCount(unsigned n) {
        unsigned now = GetArgsCount();
        if (now < n) {
            Py_RaiseError(PyExc_TypeError,
                          "Too few arguments required, given %u, but require "
                          "%u. Object: %R",
                          now, n, this->args);
            return false;
        } else if (now > n) {
            Py_RaiseError(PyExc_TypeError,
                          "Too many arguments required, given %u, but require "
                          "%u. Object: %R",
                          now, n, this->args);
            return false;
        }
        return true;
    }
    /**
     * @brief Get the arg object at specified position.
     *
     * @param pos position of argument
     * @return PyObject* argument at pos
     */
    PyObject *GetArg(unsigned pos) {
        assert(pos < GetArgsCount());
        return PyTuple_GetItem(this->args, static_cast<Py_ssize_t>(pos));
    }
    PyObject *args;
};

/**
 * @brief Reader for python list or tuple.
 *
 */
class PyListReader {
  public:
    explicit PyListReader(PyObject *o) : obj(o) {
        if (PyList_Check(o)) {
            this->type = ListType::List;
            this->size = PyList_Size(o);
        } else if (PyTuple_Check(o)) {
            this->type = ListType::Tuple;
            this->size = PyTuple_Size(o);
        } else {
            Py_RaiseError(PyExc_TypeError,
                          "Argument not a tuple or list. Object: %R", o);
        }
    }
    /**
     * @brief Get the item at specific index.
     *
     * @param index
     * @return PyObject* return nullptr if error occurs
     */
    PyObject *GetItem(unsigned index) {
        if (index >= static_cast<unsigned>(this->size)) {
            Py_RaiseError(PyExc_IndexError,
                          "Out of index(%u of %u). Object: %R", index,
                          GetSize(), this->obj);
            return nullptr;
        }
        switch (this->type) {
        case ListType::List:
            return PyList_GetItem(this->obj, static_cast<Py_ssize_t>(index));
        case ListType::Tuple:
            return PyTuple_GetItem(this->obj, static_cast<Py_ssize_t>(index));
        default:
            Py_RaiseError(PyExc_RuntimeError, "List GetItem fatal error.");
            return nullptr;
        }
    }
    unsigned GetSize() { return static_cast<unsigned>(this->size); }

    PyObject *obj;

    enum ListType { List, Tuple };

  private:
    Py_ssize_t size;
    ListType type;
};

/**
 * @brief Convert python object to numeric type.
 *
 * @tparam T target numberic type
 * @param o python object
 * @return T target number
 */
template <typename T> inline T PyObjectToNum(PyObject *o) {
    if (PyComplex_CheckExact(o)) {
        return static_cast<T>(PyComplex_RealAsDouble(o));
    } else if (PyFloat_CheckExact(o)) {
        return static_cast<T>(PyFloat_AsDouble(o));
    } else if (PyLong_CheckExact(o)) {
        return static_cast<T>(PyLong_AsLong(o));
    } else {
        Py_RaiseError(PyExc_TypeError,
                      "Could not convert to number. Object: %R", o);
        // Not to reach
        return T();
    }
}
template <> inline ayaji::Complex PyObjectToNum(PyObject *o) {
    if (PyComplex_CheckExact(o)) {
        return ayaji::Complex{PyComplex_RealAsDouble(o),
                              PyComplex_ImagAsDouble(o)};
    } else if (PyFloat_CheckExact(o)) {
        return ayaji::Complex{PyFloat_AsDouble(o)};
    } else if (PyLong_CheckExact(o)) {
        return ayaji::Complex{PyLong_AsDouble(o)};
    } else {
        Py_RaiseError(PyExc_TypeError,
                      "Could not convert to ayaji::Complex. Object: %R", o);
        return ayaji::Complex();
    }
}

template <typename T> inline std::vector<T> PyListToNum(PyObject *o) {
    PyListReader reader(o);
    std::vector<int> ret;
    for (unsigned i = 0; i < reader.GetSize(); ++i) {
        PyObject *tmp = reader.GetItem(i);
        // PyLong
        ret.push_back(PyObjectToNum<T>(tmp));
    }
    return std::move(ret);
}

template <typename T> inline T PyObjectTo(PyObject *o) {
    static_assert("Not implemented yet!");
}
template <> inline RawTerm PyObjectTo(PyObject *o) {
    PyListReader reader(o);
    if (reader.GetSize() != 2) {
        Py_RaiseError(PyExc_TypeError, "Not a valid RawTerm. Object: %R", o);
        return RawTerm();
    }
    PyObject *t1 = reader.GetItem(0);
    ayaji::Complex coef = PyObjectToNum<ayaji::Complex>(t1);
    PyObject *t2 = reader.GetItem(1);
    std::vector<int> termOp = PyListToNum<int>(t2);

    RawTerm ret;
    ret.coef = coef;
    ret.termOp = std::move(termOp);
    return std::move(ret);
}

/**
 * @brief Convert python list/tuple to std::vector<T>.
 *
 * @tparam T
 * @param o
 * @return std::vector<T>
 */
template <typename T> inline std::vector<T> PyListTo(PyObject *o) {
    static_assert("Not implemented yet!");
}
template <> inline std::vector<RawTerm> PyListTo(PyObject *o) {
    PyListReader reader(o);
    std::vector<RawTerm> ret;
    auto size = reader.GetSize();
    for (unsigned i = 0; i < size; ++i) {
        PyObject *tmp = reader.GetItem(i);
        ret.push_back(PyObjectTo<RawTerm>(tmp));
    }
    return std::move(ret);
}

} // namespace ayaji

#endif // INCLUDE_PYTHON_UTILS_UTILS_H_
