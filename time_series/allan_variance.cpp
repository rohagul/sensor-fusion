#include "allan_variance.h"
#include <armadillo>
#include <cmath>
#include <vector>
#include <algorithm>
#include <matplot/matplot.h>

int max_clusters=1000; //no of cluster size options. 
const int min_clusters = 10;

void allan_variance(std::vector<double> &data, double fs) {
    //Non overlapping AVAR 
    int N=data.size(); // number of samples
    double tau_0=1.0/fs; // sample time
    double Mmax=std::floor(N/double(min_clusters)); //max cluster size 
    arma::rowvec arma_v;
    arma_v=arma::logspace<arma::rowvec>(std::log10(1),std::log10(Mmax),max_clusters); 
    std::vector<double> Mraw = arma::conv_to<std::vector<double>>::from(arma_v); // cluster sizes
     // cluster sizes must be whole numbers of samples: round once, drop duplicates
    std::vector<int> M;
    for (double v : Mraw) M.push_back(static_cast<int>(std::lround(v)));
    M.erase(std::unique(M.begin(), M.end()), M.end());

    std::vector<double> tau;
    std::vector<double> yi;
    std::vector<double> adev;
    
    for (int i=0; i<M.size(); i++){
        tau.push_back(M[i]*tau_0);
    }

    for (int i=0; i<M.size();i++){
        int cluster_size=M[i];
        int num_clusters= std::floor(N/cluster_size);
        if (num_clusters <2) break; 
        yi.clear();
        for (int j=0;j<num_clusters;j++){
            double sum1=0;
            for(int k=0; k<cluster_size;k++){
                sum1+=data[j*cluster_size + k];
            }
            yi.push_back(sum1/cluster_size);
        }
        double diff=0.0;
        double sum=0.0;
        for (int j=0; j<num_clusters-1;j++){
            diff=yi[j+1] - yi[j];
            sum+=diff*diff;
               }
        
        adev.push_back(std::sqrt(0.5/(num_clusters-1) * sum));
    }

    matplot::loglog(tau, adev);
    matplot::gca()->x_axis().scale(matplot::axis_type::axis_scale::log);
    matplot::gca()->y_axis().scale(matplot::axis_type::axis_scale::log);
    matplot::xlabel("tau (s)");
    matplot::ylabel("sigma(deg/s)");
    matplot::grid(true);
    matplot::show();
    
}