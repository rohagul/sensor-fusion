#ifndef PLOT_DATA_H
#define PLOT_DATA_H

#include <matplot/matplot.h>
#include <sstream>
#include <string>
#include <chrono>
using namespace matplot;


void plot_data_realtime(int16_t x, int16_t y, int16_t z, int numSamples);


#endif