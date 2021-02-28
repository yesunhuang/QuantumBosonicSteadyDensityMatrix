/* Copyright 2021 Ayajilin & YesunHuang*/
//
// Created by Yesun on 2021/2/27.
// This file is the main program.
//
#include <iostream>
#include "solver/DPSolver.h"

void testSingleMode() {
    double E = 0.2;
    double kappa = 1;
    int Na = 3;
    std::vector<int> Dim = std::vector<int>();
    Dim.push_back(Na);
    std::vector<int> rawIndex = std::vector<int>();
    rawIndex.push_back(0);
    rawIndex.push_back(0);
    // construct operators
    std::vector<int> H1O = std::vector<int>();
    H1O.push_back(1);
    std::vector<int> H2O = std::vector<int>();
    H1O.push_back(2);
    RawTerm H1 = {ayaji::Complex(E, 0), H1O};
    RawTerm H2 = {ayaji::Complex(E, 0), H2O};
    std::vector<int> CO = std::vector<int>();
    CO.push_back(2);
    RawTerm C0 = {ayaji::Complex(kappa, 0), CO};
    std::vector<RawTerm> Hamiltonian = std::vector<RawTerm>();
    Hamiltonian.push_back(H1);
    Hamiltonian.push_back(H2);
    std::vector<RawTerm> Collapse = std::vector<RawTerm>();
    Collapse.push_back(C0);

    EpDeriver dataSingle = EpDeriver(Hamiltonian, Collapse, rawIndex);
    DPSolver dpsolver = DPSolver(Dim, dataSingle, 0.5, 1E-10, 1000);

    dpsolver.run();
    MatrixMapper *rowSteadyMatrix = dpsolver.getResult();
    TensorMatrix steadyMatrix = rowSteadyMatrix->rowRho();

    for (int i = 0; i < Na; i++) {
        for (int j = 0; i < Na; i++) {
            ayaji::Complex result = steadyMatrix.get(i, j);
        }
    }
}

int main() {
    testSingleMode();
    return 0;
}
