#ifndef NBODY_COMMON_OGL_RENDER_HPP
#define NBODY_COMMON_OGL_RENDER_HPP

#include "common/core/Bodies.hpp"

#include <GLFW/glfw3.h>

class Render {
public:
    /*!
     * \brief Constructor.
     */
    Render(int width, int height);

    /*!
     * \brief Destructor.
     */
    ~Render();

    /*!
     * \brief Window close flag getter.
     */
    bool should_close() const;

    /*!
     * \brief Draw one trajectory frame.
     */
    void show(const BodiesAoS& bodies);

private:
    /*!
     * \brief Draw all bodies.
     */
    void draw_bodies(const BodiesAoS& bodies);

    /*!
     * \brief Set the projection matrix.
     */
    void set_projection() const;

    /*!
     * \brief Update camera angles.
     */
    void update_camera(double x_offset, double y_offset);

    GLFWwindow* window_{nullptr};   /*!< GLFW window pointer. */
    int width_{0};                  /*!< Window width. */
    int height_{0};                 /*!< Window height. */
    double camera_distance_{30.0};  /*!< Camera distance from origin. */
    double camera_angle_{0.0};      /*!< Camera horizontal angle. */
    double camera_height_{0.0};     /*!< Camera vertical offset. */
    bool dragging_{false};          /*!< Mouse dragging flag. */
    double last_x_{0.0};            /*!< Last mouse x coordinate. */
    double last_y_{0.0};            /*!< Last mouse y coordinate. */

    /*!
     * \brief GLFW framebuffer resize callback.
     */
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    /*!
     * \brief GLFW scroll callback.
     */
    static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

    /*!
     * \brief GLFW mouse button callback.
     */
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    /*!
     * \brief GLFW cursor position callback.
     */
    static void cursor_position_callback(GLFWwindow* window, double x, double y);

    /*!
     * \brief GLFW keyboard callback.
     */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif
