#include "data_processor.h"

std::vector<ChargePoint> load_data(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        exit(1);
    }

    std::vector<ChargePoint> points;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ChargePoint point;

        std::string value;
        std::getline(ss, value, ',');
        point.setX(std::stod(value) * angstromToMeter);

        std::getline(ss, value, ',');
        point.setY(std::stod(value) * angstromToMeter);

        // point.setDist(std::numeric_limits<double>::max());
        points.push_back(point);
    }

    file.close();
    return points;
}

void compute_and_print_force(std::vector<ChargePoint>& points) {
    double dist = 0;
    double force = 0;
    for (unsigned long i = 0; i < points.size(); ++i) {
        double dist_to_prev = (i > 0) ?
            ChargePoint::distance(points[i], points[i - 1]) : std::numeric_limits<double>::max();
        double dist_to_next = (i < points.size() - 1) ?
            ChargePoint::distance(points[i], points[i + 1]) : std::numeric_limits<double>::max();
        dist = std::min(dist_to_prev, dist_to_next);
        force = points[i].compute_force(dist);
        std::cout << "force = " << force << std::endl;
    }
}
