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
    double delta=0.5;
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
    H2O.push_back(2);
    RawTerm H1 = {ayaji::Complex(E, 0), H1O};
    RawTerm H2 = {ayaji::Complex(E, 0), H2O};
    std::vector<int> H3O=std::vector<int>();
    H3O.push_back(1);
    H3O.push_back(2);
    RawTerm H3 = {ayaji::Complex(delta,0),H3O};
    std::vector<RawTerm> Hamiltonian = std::vector<RawTerm>();
    Hamiltonian.push_back(H1);
    Hamiltonian.push_back(H2);
    Hamiltonian.push_back(H3);

    std::vector<int> CO = std::vector<int>();
    CO.push_back(2);
    RawTerm C0 = {ayaji::Complex(kappa, 0), CO};
    std::vector<RawTerm> Collapse = std::vector<RawTerm>();
    Collapse.push_back(C0);

    EpDeriver dataSingle = EpDeriver(Hamiltonian, Collapse, rawIndex);
    DPSolver dpsolver = DPSolver(Dim, dataSingle, 0.5, 1E-10, 1000);

    dpsolver.run();
    MatrixMapper *rowSteadyMatrix = dpsolver.getResult();
    TensorMatrix *steadyMatrix = rowSteadyMatrix->rowRho();

    std::vector<int> order=std::vector<int>();
    order.push_back(1);
    ayaji::Complex population=rowSteadyMatrix->avgMoment(order);
    
    std::cout << "Single Mode Test"<<std::endl;
    std::cout << "Population:"<<population << std::endl;
    std::cout << "DensityMatrix:"<<std::endl;
    std::cout << *steadyMatrix << std::endl;
}

void testSimplest()
{
    double kappa = 1;
    int Na = 4;
    std::vector<int> Dim = std::vector<int>();
    Dim.push_back(Na);
    std::vector<int> rawIndex = std::vector<int>();
    rawIndex.push_back(0);
    rawIndex.push_back(0);
    // construct operators
    std::vector<int> CO = std::vector<int>();
    CO.push_back(2);
    RawTerm C0 = {ayaji::Complex(kappa, 0), CO};
    std::vector<RawTerm> Hamiltonian = std::vector<RawTerm>();
    std::vector<RawTerm> Collapse = std::vector<RawTerm>();
    Collapse.push_back(C0);

    EpDeriver dataSingle = EpDeriver(Hamiltonian, Collapse, rawIndex);
    DPSolver dpsolver = DPSolver(Dim, dataSingle, 0.5, 1E-10, 1000);

    dpsolver.run();
    MatrixMapper *rowSteadyMatrix = dpsolver.getResult();
    TensorMatrix *steadyMatrix = rowSteadyMatrix->rowRho();

    std::vector<int> order=std::vector<int>();
    order.push_back(1);
    ayaji::Complex population=rowSteadyMatrix->avgMoment(order);

    std::cout << "Decay Test"<<std::endl;
    std::cout << "Population:"<<population << std::endl;
    std::cout << "DensityMatrix:"<<std::endl;
    std::cout << *steadyMatrix << std::endl;
}

void testSHG() {
    double E = 0.4;
    double kappa_a = 1;
    double kappa_b = 1;
    double g=1;
    int Na = 2;
    int Nb = 2;
    std::vector<int> Dim = std::vector<int>();
    Dim.push_back(Na);
    Dim.push_back(Nb);
    std::vector<int> rawIndex = std::vector<int>();
    rawIndex.push_back(0);
    rawIndex.push_back(0);
    rawIndex.push_back(0);
    rawIndex.push_back(0);
    // construct operators
    std::vector<int> H1O = std::vector<int>();
    H1O.push_back(1);
    std::vector<int> H2O = std::vector<int>();
    H2O.push_back(2);
    RawTerm H1 = {ayaji::Complex(E, 0), H1O};
    RawTerm H2 = {ayaji::Complex(E, 0), H2O};
    std::vector<int> H3O=std::vector<int>();
    std::vector<int> H4O=std::vector<int>();
    //H3O.push_back(1);H4O.push_back(2);
    H3O.push_back(1);H4O.push_back(2);
    H3O.push_back(4);H4O.push_back(3);
    RawTerm H3 = {ayaji::Complex(g,0),H3O};
    RawTerm H4 = {ayaji::Complex(g,0),H4O};
    std::vector<RawTerm> Hamiltonian = std::vector<RawTerm>();
    Hamiltonian.push_back(H1);
    Hamiltonian.push_back(H2);
    Hamiltonian.push_back(H3);
    Hamiltonian.push_back(H4);

    std::vector<int> CO = std::vector<int>();
    CO.push_back(2);
    RawTerm C0 = {ayaji::Complex(kappa_a, 0), CO};
    std::vector<int> CI = std::vector<int>();
    CI.push_back(4);
    RawTerm C1 = {ayaji::Complex(kappa_b, 0), CI};
    std::vector<RawTerm> Collapse = std::vector<RawTerm>();
    Collapse.push_back(C0);
    Collapse.push_back(C1);

    EpDeriver dataSingle = EpDeriver(Hamiltonian, Collapse, rawIndex);
    DPSolver dpsolver = DPSolver(Dim, dataSingle, 0.5, 1E-10, 1000);

    dpsolver.run();
    MatrixMapper *rowSteadyMatrix = dpsolver.getResult();
    TensorMatrix *steadyMatrix = rowSteadyMatrix->rowRho();

    std::vector<int> order=std::vector<int>();
    order.push_back(1);
    order.push_back(1);
    ayaji::Complex population=rowSteadyMatrix->avgMoment(order);
    
    std::cout << "Multi Mode Test"<<std::endl;
    std::cout << "Population:"<<population << std::endl;
    std::cout << "DensityMatrix:"<<std::endl;
    std::cout << *steadyMatrix << std::endl;
}

int main() {
    //testSimplest();
    //testSingleMode();
    testSHG();
    return 0;
}
