#ifndef LINEAR_FIT_H
#define LINEAR_FIT_H

#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
struct Ellipsoid {
    Eigen::Vector3d B_offset;
    Eigen::Matrix3d A_scale;
    Eigen::MatrixXd A_inverse;
};


Eigen::MatrixXd convert_array(std::vector<std::vector<int16_t>> magdata, int rows, int columns);


Ellipsoid ellipsoid_fit_linear(Eigen::MatrixXd data);

Eigen::Matrix3d choleskyFactor(const Eigen::Matrix3d& S);

#endif