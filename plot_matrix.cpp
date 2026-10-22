#include "plot_matrix.h"

#include <matplot/matplot.h>
#include <iostream>
#include <vector>

void plot_matrices(
    const std::vector<Eigen::MatrixXd>& matrices,
    const std::vector<std::string>& labels)
{
    if (matrices.size() != labels.size()) {
        std::cerr << "Error: number of matrices must match number of labels.\n";
        return;
    }

    auto fig = matplot::figure(true);

    fig->name("3D Data");
    fig->size(1000, 800);

    auto ax = fig->current_axes();

    ax->xlabel("X");
    ax->ylabel("Y");
    ax->zlabel("Z");
    ax->title("3D Data");

    for (size_t k = 0; k < matrices.size(); ++k) {

        const auto& matrix = matrices[k];

        // Data must be N x 3
        if (matrix.cols() != 3) {
            std::cerr << "Error: matrix " << k
                      << " must have 3 columns.\n";
            continue;
        }

        std::vector<double> X(matrix.rows());
        std::vector<double> Y(matrix.rows());
        std::vector<double> Z(matrix.rows());

        for (int i = 0; i < matrix.rows(); ++i) {
            X[i] = matrix(i, 0);
            Y[i] = matrix(i, 1);
            Z[i] = matrix(i, 2);
        }

        auto points = matplot::scatter3(ax, X, Y, Z);

        points->display_name(labels[k]);
       
        if (k == 0) {
        points->color("blue");
        }
        else if (k == 1) {
            points->color("red");
        }
    }

    ax->legend();

    fig->draw();
    matplot::show();
}