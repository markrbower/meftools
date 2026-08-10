#include "matplotlibcpp.h"
#include <vector>

namespace plt = matplotlibcpp;

int main() {
    std::vector<double> x, y;
    for (double i = -5; i <= 5; i += 0.1) {
        x.push_back(i);
        y.push_back(sin(i));
    }

    matplotlibcpp::plot(x, y);
    matplotlibcpp::show();
}
