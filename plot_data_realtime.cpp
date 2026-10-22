#include "plot_data_realtime.h"

void plot_data_realtime(int16_t x, int16_t y, int16_t z, int numSamples) {

    // Function-local statics: constructed exactly once, on the first call,
    // and persist across all subsequent calls -- this replaces the illegal
    // bare statements that were sitting at global scope.
    static std::vector<double> X, Y, Z;
    static auto fig = figure(true);
    static auto ax = [&]() {
     
        fig->size(1000, 800);
        auto a = fig->current_axes();
        a->xlabel("X");
        a->ylabel("Y");
        a->zlabel("Z");
    
        return a;
    }();
    static auto last_plot_update = std::chrono::steady_clock::now();

    X.push_back(x);
    Y.push_back(y);
    Z.push_back(z);

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_plot_update);

    if (elapsed.count() >= 100) {
        ax->clear();
        scatter3(ax, X, Y, Z);
        fig->draw();
        last_plot_update = now;   // only reset the clock when we actually redraw
    }
}