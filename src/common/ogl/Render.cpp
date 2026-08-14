#include "common/ogl/Render.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

Render::Render(int width, int height) : width_(width), height_(height) {
    if (!glfwInit()) {
        throw std::runtime_error("cannot initialize GLFW");
    }

    window_ = glfwCreateWindow(width_, height_, "N-Body Visualizer", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("cannot create OpenGL window");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetScrollCallback(window_, scroll_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetCursorPosCallback(window_, cursor_position_callback);
    glfwSetKeyCallback(window_, key_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glPointSize(3.0f);
    set_projection();
}

Render::~Render() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

bool Render::should_close() const {
    return glfwWindowShouldClose(window_);
}

void Render::set_projection() const {
    glViewport(0, 0, width_, height_);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const double aspect = static_cast<double>(width_) / static_cast<double>(height_);
    glOrtho(-camera_distance_ * aspect, camera_distance_ * aspect,
            -camera_distance_, camera_distance_, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void Render::show(const BodiesAoS& bodies) {
    glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotated(camera_height_, 1.0, 0.0, 0.0);
    glRotated(camera_angle_, 0.0, 1.0, 0.0);
    draw_bodies(bodies);
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

void Render::draw_bodies(const BodiesAoS& bodies) {
    double maximum_speed = 0.0;
    for (const Body& body : bodies) {
        const double speed = std::sqrt(body.vx * body.vx + body.vy * body.vy
                                       + body.vz * body.vz);
        maximum_speed = std::max(maximum_speed, speed);
    }

    for (const Body& body : bodies) {
        const float point_size = static_cast<float>(
            std::clamp(10.0 * body.r, 2.0, 8.0));
        glPointSize(point_size);
        if (body.m > 1.0) {
            glColor3f(1.0f, 0.85f, 0.2f);
        } else {
            const double speed = std::sqrt(body.vx * body.vx + body.vy * body.vy
                                           + body.vz * body.vz);
            const float normalized_speed = maximum_speed > 0.0
                ? static_cast<float>(speed / maximum_speed)
                : 0.0f;
            const float mass_color = static_cast<float>(
                std::clamp(body.m / 0.08, 0.0, 1.0));
            glColor3f(0.15f + 0.75f * mass_color,
                      0.25f + 0.55f * (1.0f - normalized_speed),
                      0.95f - 0.75f * mass_color);
        }
        glBegin(GL_POINTS);
        glVertex3d(body.qx, body.qy, body.qz);
        glEnd();
    }
}

void Render::update_camera(double x_offset, double y_offset) {
    camera_angle_ += x_offset * 0.4;
    camera_height_ = std::clamp(camera_height_ + y_offset * 0.4, -89.0, 89.0);
}

void Render::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
    if (render) {
        render->width_ = std::max(width, 1);
        render->height_ = std::max(height, 1);
        render->set_projection();
    }
}

void Render::scroll_callback(GLFWwindow* window, double, double y_offset) {
    Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
    if (render) {
        render->camera_distance_ = std::clamp(render->camera_distance_ - y_offset, 5.0, 100.0);
        render->set_projection();
    }
}

void Render::mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
    if (render && button == GLFW_MOUSE_BUTTON_LEFT) {
        render->dragging_ = action == GLFW_PRESS;
        glfwGetCursorPos(window, &render->last_x_, &render->last_y_);
    }
}

void Render::cursor_position_callback(GLFWwindow* window, double x, double y) {
    Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
    if (render && render->dragging_) {
        render->update_camera(x - render->last_x_, render->last_y_ - y);
        render->last_x_ = x;
        render->last_y_ = y;
    }
}

void Render::key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
