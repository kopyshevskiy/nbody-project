#include "common/core/TrajectoryReader.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

bool parse_row(const std::string& line, std::size_t& step, double& time, std::size_t& id, Body& body) {
    char comma = '\0';
    std::istringstream row(line);
    row >> step >> comma >> time >> comma >> id >> comma >> body.m >> comma
        >> body.qx >> comma >> body.qy >> comma >> body.qz >> comma
        >> body.vx >> comma >> body.vy >> comma >> body.vz;
    body.r = 1.0;
    row >> std::ws;
    if (!row.eof()) {
        row >> comma >> body.r;
    }
    return !row.fail();
}

TrajectoryReader::TrajectoryReader(const std::string& path) : input_(path) {
    if (!input_) {
        throw std::runtime_error("cannot open trajectory input: " + path);
    }

    std::string header;
    std::getline(input_, header);
}

bool TrajectoryReader::read_next_frame(BodiesAoS& bodies, std::size_t& step, double& time) {
    std::vector<Body> frame;
    std::string line;

    if (has_pending_line_) {
        line = pending_line_;
        has_pending_line_ = false;
    } else if (!std::getline(input_, line)) {
        return false;
    }

    std::size_t current_step = 0;
    double current_time = 0.0;
    std::size_t id = 0;
    Body body;
    if (!parse_row(line, current_step, current_time, id, body)) {
        return false;
    }

    step = current_step;
    time = current_time;
    frame.push_back(body);

    while (std::getline(input_, line)) {
        std::size_t next_step = 0;
        double next_time = 0.0;
        std::size_t next_id = 0;
        Body next_body;
        if (!parse_row(line, next_step, next_time, next_id, next_body)) {
            continue;
        }
        if (next_step != current_step) {
            pending_line_ = line;
            has_pending_line_ = true;
            break;
        }
        frame.push_back(next_body);
    }

    bodies = BodiesAoS{std::move(frame)};
    return true;
}
