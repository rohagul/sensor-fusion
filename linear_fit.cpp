#include "linear_fit.h"
#include <iostream>


Ellipsoid ellipsoid_fit_linear(Eigen::MatrixXd data){

    //Calculate A matrix 
    Eigen::MatrixXd A(data.rows(),10);
    double x,y,z;
    for (int i=0;i<data.rows();i++){

        x=data(i,0);
        y=data(i,1);
        z=data(i,2);

        A(i,0)=x*x;
        A(i,1)=y*y;
        A(i,2)=z*z;
        A(i,3)=2*x*y;
        A(i,4)=2*x*z;
        A(i,5)=2*y*z;
        A(i,6)=2*x;
        A(i,7)=2*y;
        A(i,8)=2*z;
        A(i,9)=1;
    }
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV);

    Eigen::VectorXd p(10);

    p=svd.matrixV().col(9); // singular vector corresp to smallest singular value.

    Eigen::Matrix3d Q;
    Q << p(0), p(3)/2.0, p(4)/2.0, 
    p(3)/2.0, p(1), p(5)/2.0,
    p(4)/2.0, p(5)/2.0, p(2);
    
    //std::cout << "Q:\n" << Q << "\n";

    Eigen::Vector3d q(p(6), p(7), p(8));
    
    double J = p(9);

    Eigen::Vector3d center = -Q.ldlt().solve(q);
    
    double k = center.transpose() * Q * center - J;
    Eigen::Matrix3d M = Q / k;
    Eigen::Matrix3d W = M.inverse();

    
    Ellipsoid ellipsoid; 
    ellipsoid.B_offset = center;
    ellipsoid.A_scale = choleskyFactor(W);
    ellipsoid.A_inverse = ellipsoid.A_scale.inverse();

    return ellipsoid; 
}


Eigen::Matrix3d choleskyFactor(const Eigen::Matrix3d& S) {
    return S.llt().matrixL();
}