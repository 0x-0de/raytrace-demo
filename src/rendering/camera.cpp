#include "camera.h"

#include <cmath>

#include "renderer.h"

static GLFWwindow* win;

static Vec3 camera_pos;
static Vec3 camera_dir;

static double mouse_cx, mouse_cy, mouse_px, mouse_py, mouse_sensitivity;

void camera::init(GLFWwindow* window)
{
    win = window;

    camera_pos = Vec3(0, 0, 0);

    mouse_cx = 0;
    mouse_cy = 0;

    mouse_px = 0;
    mouse_py = 0;

    mouse_sensitivity = 200;
}

void camera::clean()
{

}

void camera::update(bool freemove)
{
    double mouse_x, mouse_y;
    glfwGetCursorPos(win, &mouse_x, &mouse_y);

    double mouse_dx = mouse_x - mouse_px;
    double mouse_dy = mouse_y - mouse_py;

    mouse_cx += mouse_dx / mouse_sensitivity;

    if(mouse_cy >= 1.57f)
    {
        mouse_cy = 1.57f;
        if(mouse_dy < 0)
            mouse_cy += mouse_dy / mouse_sensitivity;
    }
    else if(mouse_cy <= -1.57f)
    {
        mouse_cy = -1.57f;
        if(mouse_dy > 0)
            mouse_cy += mouse_dy / mouse_sensitivity;
    }
    else
    {
        mouse_cy += mouse_dy / mouse_sensitivity;
    }

    camera_dir.x = std::sin(mouse_cx);
    camera_dir.y = std::tan(-mouse_cy);
    camera_dir.z = std::cos(mouse_cx);

    camera_dir = normalize(camera_dir);

    mouse_px = mouse_x;
    mouse_py = mouse_y;

    //Moving around.

    Vec3 vel;
    float input_counter = 0;

    Vec3 forward = freemove ? camera_dir : normalize(Vec3(camera_dir.x, 0, camera_dir.z));
    Vec3 side = normalize(cross(camera_dir, Vec3(0, 1, 0)));

    if(glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
    {
        vel += forward;
        input_counter++;
    }
    if(glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
    {
        vel -= forward;
        input_counter++;
    }
    if(glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
    {
        vel += side;
        input_counter++;
    }
    if(glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
    {
        vel -= side;
        input_counter++;
    }

    camera_pos += vel * engine_delta;
}

Vec3 camera::get_camera_pos()
{
    return camera_pos;
}

Vec3 camera::get_camera_rot()
{
    return camera_dir;
}