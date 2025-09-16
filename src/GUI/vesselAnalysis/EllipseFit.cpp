#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include "stdafx.h"
#include "EllipseFit.h"

Eigen::VectorXd fitEllipse(const std::vector<double>& x, const std::vector<double>& y)
{
    size_t size = x.size();
	int n = static_cast<int>(size);

    Eigen::MatrixXd D1(n, 3);
    Eigen::MatrixXd D2(n, 3);
    Eigen::MatrixXd S1(3, 3);
    Eigen::MatrixXd S2(3, 3);
    Eigen::MatrixXd S3(3, 3);
    Eigen::MatrixXd T(3, 3);
    Eigen::MatrixXd M(3, 3);
    Eigen::MatrixXd C(3, 3);

    // Populate D1 and D2 matrices
    for (int i = 0; i < n; ++i) {
        D1(i, 0) = x[i] * x[i];
        D1(i, 1) = x[i] * y[i];
        D1(i, 2) = y[i] * y[i];

        D2(i, 0) = x[i];
        D2(i, 1) = y[i];
        D2(i, 2) = 1.0;
    }

    // Compute S1, S2, and S3 matrices
    S1 = D1.transpose() * D1;
    S2 = D1.transpose() * D2;
    S3 = D2.transpose() * D2;

    // Compute T matrix
    T = -S3.inverse() * S2.transpose();

    // Compute M matrix
    M = S1 + S2 * T;

    // Define C matrix
    C << 0, 0, 2,
        0, -1, 0,
        2, 0, 0;

    // Invert C matrix and update M
    M = C.inverse() * M;

    // Compute eigenvalues and eigenvectors of M
    Eigen::EigenSolver<Eigen::MatrixXd> eigensolver(M);
    Eigen::VectorXd eigval = eigensolver.eigenvalues().real();
    Eigen::MatrixXd eigvec = eigensolver.eigenvectors().real();

    // Compute con
    double con = 4 * eigvec(0, 0) * eigvec(2, 0) - eigvec(1, 0) * eigvec(1, 0);

    // Extract ak based on con
    Eigen::MatrixXd ak(3, 1);
    if (con > 0) {
        ak = eigvec.col(0);
    }

    Eigen::VectorXd result(6);
    result << ak, T* ak;

    return result;
}

Eigen::VectorXd ellipseParameters(const Eigen::VectorXd& coeffs) {
    /*double a = coeffs[0];
    double b = coeffs[1] / 2;
    double c = coeffs[2];
    double d = coeffs[3] / 2;
    double f = coeffs[4] / 2;
    double g = coeffs[5];*/

    double a = coeffs(0);
    double b = coeffs(1) / 2;
    double c = coeffs(2);
    double d = coeffs(3) / 2;
    double f = coeffs(4) / 2;
    double g = coeffs(5);


    double den = b * b - a * c;
    if (den > 0) {
        throw std::invalid_argument("coeffs do not represent an ellipse: b^2 - 4ac must be negative!");
    }

    // The location of the ellipse centre.
    double x0 = (c * d - b * f) / den;
    double y0 = (a * f - b * d) / den;

    double num = 2 * (a * f * f + c * d * d + g * b * b - 2 * b * d * f - a * c * g);
    double fac = std::sqrt((a - c) * (a - c) + 4 * b * b);

    // The semi-major and semi-minor axis lengths (these are not sorted).
    double ap = std::sqrt(num / den / (fac - a - c));
    double bp = std::sqrt(num / den / (-fac - a - c));

    // Sort the semi-major and semi-minor axis lengths but keep track of
    // the original relative magnitudes of width and height.
    bool width_gt_height = true;
    if (ap < bp) {
        width_gt_height = false;
        std::swap(ap, bp);

    }

    // The eccentricity.
    double r = (bp / ap) * (bp / ap);
    if (r > 1) {
        r = 1 / r;
    }
    double e = std::sqrt(1 - r);

    // The angle of anticlockwise rotation of the major-axis from x-axis.
    double phi;
    if (b == 0) {
        phi = (a < c) ? 0 : M_PI / 2;
    }
    else {
		phi = std::atan(2. * b / (a - c)) / 2;
        if (a > c) {
            phi += M_PI / 2;
        }
    }
	
    if (!width_gt_height) {
        // Ensure that phi is the angle to rotate to the semi-major axis.
        phi  += (M_PI / 2);
    }
    phi = std::fmod(phi, M_PI);

    Eigen::VectorXd result(5);
	result << x0, y0, ap, bp, (phi * -180) / M_PI;

    return result;
}