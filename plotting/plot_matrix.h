#ifndef PLOT_MATRIX_H
#define PLOT_MATRIX_H

#include <Eigen/Dense>
#include <vector>
#include <string>

void plot_matrices(
    const std::vector<Eigen::MatrixXd>& matrices,
    const std::vector<std::string>& labels
);

#endif