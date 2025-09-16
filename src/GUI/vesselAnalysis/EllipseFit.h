#pragma once
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <iostream>

using namespace std;

Eigen::VectorXd fitEllipse(const std::vector<double>& x, const std::vector<double>& y);
Eigen::VectorXd ellipseParameters(const Eigen::VectorXd& coeffs);