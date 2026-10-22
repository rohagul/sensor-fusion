#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>  //treats a string as a stream of characters
#include "allan_variance.h"

int main(){
    std::vector<double> t,ax,ay,az,gx,gy,gz;
    std::ifstream dataFile("data/imu_log.csv"); 
    std::string line; // line variable to hold each line of the CSV file
    double fs=14;
    if(!dataFile.is_open()){
        std::cerr << "Error opening file"<<std::endl;
        return 1;
    }
    
    std::getline(dataFile,line); //input stream, string_variable, delimiter 
    //skipping the header

    while(std::getline(dataFile,line)){ //when file runs out, getline fails and exits loop

        std::stringstream ss(line); //wraps the line in a stringstream object
        std::vector<std::string> values; //vector to hold all the values in the line
        std::string value; 
        while(std::getline(ss,value,',')) values.push_back(value); //saves all 8 values in a string vector 
        
        if (values.size()<8) continue; //skip lines with insufficient data

        try{
            //code that might throw an exception
            t.push_back(std::stod(values[1]));
            ax.push_back(std::stod(values[2])); //convert string to double and push
            ay.push_back(std::stod(values[3]));
            az.push_back(std::stod(values[4]));
            gx.push_back(std::stod(values[5]));
            gy.push_back(std::stod(values[6]));
            gz.push_back(std::stod(values[7]));
        } catch (...){
            continue;
        }
    }

    allan_variance(ax,fs);

}