#include "charge_points.h"
#include "log.h"
#include <cmath>
#include <sstream>

double distance(const ChargePoint& p1, const ChargePoint& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(std::pow(dx,2) + std::pow(dy,2));
}

double compute_force(const ChargePoint& p, const ChargePoint& p_prev, const ChargePoint& p_next) {
    double dist = std::min(distance(p, p_prev), distance(p, p_next));
    return k * std::pow(q,2) / std::pow(dist,2);
}

void compute_and_print_force(std::pair<size_t, size_t> boundary) {
    size_t start = std::max(0, static_cast<int>(boundary.first - 1));
    size_t end = std::min(linesArray.size(), boundary.second + 1);
    if(start == 0) {
        std::string cur_line = linesArray[0];
        std::string next_line = linesArray[1];
        ChargePoint p_prev = ChargePoint(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
        ChargePoint p = parseLineToChargePoint(cur_line);
        ChargePoint p_next = parseLineToChargePoint(next_line);
        double force = compute_force(p, p_prev, p_next);
        std::cout << p.x/angstromToMeter << " " << p.y/angstromToMeter << " force = "<< force << std::endl;
    }
    for (size_t i = start + 1; i < end - 1; ++i) {
        std::string prev_line = linesArray[i-1];
        std::string cur_line = linesArray[i];
        std::string next_line = linesArray[i+1];
        ChargePoint p_prev = parseLineToChargePoint(prev_line);
        ChargePoint p = parseLineToChargePoint(cur_line);
        ChargePoint p_next = parseLineToChargePoint(next_line);
        double force = compute_force(p, p_prev, p_next);
        std::cout << p.x/angstromToMeter << " " << p.y/angstromToMeter << " force = "<< force << std::endl;
    }
    if (end == linesArray.size()) {
        std::string prev_line = linesArray[end - 2];
        std::string cur_line = linesArray[end-1];
        ChargePoint p_prev = parseLineToChargePoint(prev_line);
        ChargePoint p = parseLineToChargePoint(cur_line);
        ChargePoint p_next = ChargePoint(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
        double force = compute_force(p, p_prev, p_next);
        std::cout << p.x/angstromToMeter << " " << p.y/angstromToMeter << " force = "<< force << std::endl;
    }

}

ChargePoint parseLineToChargePoint(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    /* Ensure the line has the correct format: "double,double,char" */
    M_assert(tokens.size() == 3, "Line format is incorrect.");
    M_assert(!tokens[0].empty() && !tokens[1].empty() && tokens[2].size() == 1,
        "Line content does not match expected format.");

    double x_val = std::stod(tokens[0]) * angstromToMeter;
    double y_val = std::stod(tokens[1]) * angstromToMeter;

    return ChargePoint(x_val, y_val);
}
