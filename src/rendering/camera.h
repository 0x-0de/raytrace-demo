#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../utils/utilmath.h"

#include "../../lib/GLFW/glfw3.h"

namespace camera
{
    void init(GLFWwindow* window);
    void clean();

    void update(bool freemove);

    Vec3 get_camera_pos();
    Vec3 get_camera_rot();
};

#endif