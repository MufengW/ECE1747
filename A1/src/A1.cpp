#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>  /* for std::atoi */
#include "charge_points.h"
#include "data_loader.h"

std::vector<ChargePoint> load_data(const std::string& filename);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mode>" << std::endl;
        return 1;
    }

    std::vector<ChargePoint> points = load_data("data/particles-student-1.csv");
    int numRows = std::min(static_cast<int>(points.size()), 100);
    for (int i = 0; i < numRows; ++i) {
        const auto& point = points[i];
        std::cout << "x_pos: " << point.getX() << ", y_pos: " << point.getY() << ", dist: " << point.getDist()
             << std::endl;
    }
    int mode = std::atoi(argv[1]);

    switch (mode) {
        case 1:
            std::cout << "Running in mode 1" << std::endl;
            break;

        case 2:
            std::cout << "Running in mode 2" << std::endl;
            break;

        case 3:
            std::cout << "Running in mode 3" << std::endl;
            break;
        default:
            std::cerr << "Invalid mode specified: " << mode << std::endl;
            return 1;
    }

    return 0;
}
