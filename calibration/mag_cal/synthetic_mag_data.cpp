#include "synthetic_mag_data.h"


    SyntheticMagData generateFakeMagData(int numSamples, double noiseStdDev) {
    std::mt19937 rng(42);   // fixed seed - reproducible runs while you're debugging
    std::normal_distribution<double> gauss(0.0, 1.0);
    std::normal_distribution<double> noise(0.0, noiseStdDev);

    // Realistic, deliberately NOT axis-aligned - diagonal terms near the
    // QMC5883P's nominal 3750 LSB/Gauss (adjusted, since a real chip deviates
    // from datasheet spec), small off-diagonal terms simulating mild misalignment.
    Eigen::Matrix3d A_true;
    A_true << 3000,  120,  -60,
               110, 3200,   90,
               -70,   80, 2900;

    Eigen::Vector3d b_true(180.0, -220.0, 340.0);   // hard-iron offset, raw LSB units

    Eigen::MatrixXd rawSamples(numSamples, 3);
    Eigen::MatrixXd actual(numSamples, 3);  
    for (int i = 0; i < numSamples; i++) {
        Eigen::Vector3d u(gauss(rng), gauss(rng), gauss(rng));
        u.normalize();   // uniformly random point on the TRUE unit sphere
        Eigen::Vector3d raw = A_true * u + b_true; 
                     
        raw += Eigen::Vector3d(noise(rng), noise(rng), noise(rng)); // sensor noise
 
        rawSamples.row(i) = raw.transpose();
        actual.row(i) = u.transpose();
    }
    
    return {rawSamples,actual, A_true, b_true};
}