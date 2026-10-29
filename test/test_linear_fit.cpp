#include "../calibration/mag_cal/synthetic_mag_data.h"
#include "../calibration/mag_cal/linear_fit.h"
#include "../plotting/plot_matrix.h"
#include <vector>
int main(){

    int numSamples=500;
    auto synth = generateFakeMagData(numSamples);

    auto result=ellipsoid_fit_linear(synth.rawSamples);
     // your function
    
    std::cout << "Ground truth A:\n" << synth.A_true << "\n";
    std::cout << "Ground truth b:\n" << synth.b_true.transpose() << "\n";
    std::cout << "Fit A:\n" << result.A_scale << "\n";
    std::cout << "Fit B:\n" << result.B_offset << "\n";
    
    Eigen::MatrixXd correctedSamples(numSamples,3);
    correctedSamples = (synth.rawSamples.rowwise() - result.B_offset.transpose())* result.A_scale.inverse().transpose();
    std::cout << "Actual:\n"
          << synth.actual.topRows(5) << "\n";

    std::cout << "Corrected:\n"
          << correctedSamples.topRows(5) << "\n";
          
    plot_matrices(
    {synth.actual, correctedSamples},
    {"Actual", "Corrected"}
    );
   
}