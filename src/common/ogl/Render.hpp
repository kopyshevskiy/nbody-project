#ifndef NBODY_COMMON_OGL_RENDER_HPP
#define NBODY_COMMON_OGL_RENDER_HPP

#include "common/core/Bodies.hpp"

#include <GLFW/glfw3.h>

class Render {
public:
    Render(int width, int height);
    ~Render();

    bool should_close() const;
    void show(const BodiesAoS& bodies);

private:
    void draw_bodies(const BodiesAoS& bodies);
    void set_projection() const;
    void update_camera(double x_offset, double y_offset);

    GLFWwindow* window_{nullptr};
    int width_{0};
    int height_{0};
    double camera_distance_{30.0};
    double camera_angle_{0.0};
    double camera_height_{0.0};
    bool dragging_{false};
    double last_x_{0.0};
    double last_y_{0.0};

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double x, double y);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif
