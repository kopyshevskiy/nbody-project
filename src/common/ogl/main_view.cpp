#include "common/core/TrajectoryReader.hpp"
#include "common/ogl/Render.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void print_usage(const char* executable) {
    std::cout << "Usage: " << executable << " TRAJECTORY.csv\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    try {
        TrajectoryReader reader(argv[1]);
        Render render(1000, 800);
        BodiesAoS bodies;
        std::size_t step = 0;
        double time = 0.0;

        while (!render.should_close() && reader.read_next_frame(bodies, step, time)) {
            render.show(bodies);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        while (!render.should_close()) {
            render.show(bodies);
        }
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
