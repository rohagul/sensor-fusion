#ifndef SYNTHETIC_MAG_DATA_H
#define SYNTHETIC_MAG_DATA_H

#include <Eigen/Dense>
#include <random>
#include <iostream>

struct SyntheticMagData {
    Eigen::MatrixXd rawSamples;   // Nx3 - feed straight into fitQuadricSVD()
    Eigen::MatrixXd actual;     // Nx3 - the actual unit vectors used to generate the raw samples
    Eigen::Matrix3d A_true;       // ground truth soft-iron forward transform
    Eigen::Vector3d b_true;       // ground truth hard-iron offset
};

SyntheticMagData generateFakeMagData(int numSamples, double noiseStdDev = 5.0);

#endif // SYNTHETIC_MAG_DATA_H