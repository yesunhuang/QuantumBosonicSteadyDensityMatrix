/* Copyright 2021 YesunHuang & AyajiLin */
/**
 * @ FileName: Complex.h
 * @ Function: declare the function of Complex
 * @ Author: AyajiLin & YesunHuang
 * @ Mail: 493987054@qq.com & 944812203@qq.com
 * @ Github: https://github.com/leonardodalinky & https://github.com/yesunhuang
 * @ Date: 04/01/2020
 *
 */

#pragma once
#ifndef _COMPLEX_H_
#define _COMPLEX_H_

// #define HIGH_PRECISION

#include <cstdlib>
#include <exception>
#include <string>
#include <ostream>

#ifdef HIGH_PRECISION
// load the high precision gnu library
#include "lib/gmp.h"
#pragma comment(lib, "lib/libgmp.a")
#endif  // HIGH_PRECISION

namespace ayaji {
class Complex {
public:
    Complex();
    explicit Complex(double real);
    Complex(double real, double image);
    Complex(const Complex& o);
    Complex(Complex&& o) noexcept;
    ~Complex();

    Complex& operator=(const Complex& o);
    Complex operator+(const Complex& o) const;
    Complex operator-(const Complex& o) const;
    Complex operator*(const Complex& o) const;
    Complex operator/(const Complex& o) const;

    Complex operator+(const double o) const;
    Complex operator-(const double o) const;
    Complex operator*(const double o) const;
    Complex operator/(const double o) const;

    Complex& operator+=(const Complex& o);
    Complex& operator-=(const Complex& o);
    Complex& operator*=(const Complex& o);
    Complex& operator/=(const Complex& o);

    Complex& operator+=(const double o);
    Complex& operator-=(const double o);
    Complex& operator*=(const double o);
    Complex& operator/=(const double o);

    Complex& operator++();
    Complex& operator--();
    Complex operator++(int);
    Complex operator--(int);

    Complex conj() const;
    
    bool isZero() const;

    double getReal() const;
    double getImage() const;
    std::string getString() const;

    void setReal(double _real);
    void setImage(double _image);
    void set(double _real, double _image);

    friend inline std::ostream& operator<< (std::ostream &out, const Complex& c) {
        out << c.real << "+" << c.image << "i";
        return out;
    }

private:
    static constexpr double episilon = 10e-7;
#ifdef HIGH_PRECISION
    // high precision
    const int FLOAT_PRECISION = 256;  // precision of floating point numbers
    mpf_t real;
    mpf_t image;
#else
    // normal precision
    double real;
    double image;
#endif  // HIGH_PRECISION
};
#ifndef HIGH_PRECISION
    Complex operator+(double lhs, const Complex &rhs);
    Complex operator-(double lhs, const Complex &rhs);
    Complex operator*(double lhs, const Complex &rhs);
    Complex operator/(double lhs, const Complex &rhs);
#endif
}  // namespace ayaji

#endif
