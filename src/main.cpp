#include <ctime>
#include <iostream>

#include "../lib/GL/glew.h"
#include "../lib/GLFW/glfw3.h"

#include "rendering/camera.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"

double engine_delta, engine_time;
int engine_width, engine_height, cursor_x, cursor_y, input_mode;
bool lmb, rmb, backspace;

std::string str_input;

const char* vs_screen = R"VOGON(
#version 430 core
layout(location = 0) in vec2 pos;

out vec2 f_pos;

void main()
{
    f_pos = pos;
    gl_Position = vec4(pos, 0, 1);
}
)VOGON";

const char* fs_screen = R"VOGON(
#version 430 core
out vec4 color;

in vec2 f_pos;

uniform sampler2D tex;

void main()
{
    vec2 f = (f_pos + vec2(1, 1)) / 2;
    f *= -1;
    vec4 t = texture(tex, f);
    color = t;
}
)VOGON";

const char* vs_main = R"VOGON(
#version 430 core
layout(location = 0) in vec2 pos;

out vec2 screen_pos;

void main()
{
    screen_pos = pos;
    gl_Position = vec4(pos, 0, 1);
}
)VOGON";

const char* fs_main = R"VOGON(
#version 430 core
out vec4 color;

in vec2 screen_pos;

uniform int width, height;
uniform float fov;

uniform vec3 camera_pos, camera_dir;

layout(std430, binding = 0) buffer data_pos
{
    vec4 scene_color;
};

vec3 get_ray_dir(int x, int y)
{
    vec3 dir_x = normalize(cross(camera_dir, vec3(0, 1, 0)));
    vec3 dir_y = normalize(cross(camera_dir, dir_x));

    float dx = float (x) - float (width) / 2;
    float dy = float (y) - float (height) / 2;

    float to_rad = 3.1415926 / 180;
    float f = (fov / width) * to_rad;

    vec3 lookpoint = camera_dir + dir_x * dx * f + dir_y * dy * f;

    return lookpoint;
}

vec4 plane_intersection(vec3 ray_origin, vec3 ray_direction, vec3 plane_normal, float plane_constant)
{
    float const_a = plane_normal.x * ray_origin.x;
    float const_b = plane_normal.y * ray_origin.y;
    float const_c = plane_normal.z * ray_origin.z;

    float constant = const_a + const_b + const_c; 

    float param_a = plane_normal.x * ray_direction.x;
    float param_b = plane_normal.y * ray_direction.y;
    float param_c = plane_normal.z * ray_direction.z;

    float parameter = param_a + param_b + param_c;

    if(parameter == 0) return vec4(0, 0, 0, -2);

    float t = (-plane_constant - constant) / parameter;
    if(t < 0) return vec4(0, 0, 0, -1);

    vec3 result = ray_origin + ray_direction * t;

    return vec4(result, 0);
}

vec4 sphere_intersection(vec3 ray_origin, vec3 ray_direction, vec3 sphere_point, float sphere_radius)
{
    vec3 diff = sphere_point - ray_origin;
    float d = dot(diff, ray_direction);

    if(d < 0) return vec4(0, 0, 0, -1);

    vec3 proj = d * ray_direction + ray_origin;
    float dist = length(sphere_point - proj);
    if(dist > sphere_radius) return vec4(0, 0, 0, -1);

    float b = length(proj - ray_origin) - sqrt(sphere_radius * sphere_radius - dist * dist);
    vec3 point = ray_origin + ray_direction * b;

    return vec4(point, 0);
}

vec4 fire(vec3 pos, vec3 dir)
{
    vec4 plane = plane_intersection(pos, dir, vec3(0, 1, 0), 0);
    vec4 sphere = sphere_intersection(pos, dir, vec3(0, 0.5f, 10), 0.5f);

    if(plane.w == 0)
    {
        if(sphere.w == 0 && length(pos - sphere.xyz) < length(pos - plane.xyz))
        {
            return vec4(sphere.xyz, 1);
        }
        return vec4(plane.xyz, 0);
    }
    else if(sphere.w == 0)
    {
        return vec4(sphere.xyz, 1);
    }
    else return vec4(0, 0, 0, -1);
}

vec3 get_albedo(vec4 hit)
{
    if(hit.w == 1) //Sphere
    {
        return vec3(1, 0.2f, 0.2f);
    }
    else if(hit.w == 0) //Plane
    {
        int c = int (floor(hit.x) + floor(hit.z)) % 2;
        if(c == 0)
        {
            return vec3(1, 1, 1);
        }
        else
        {
            return vec3(0.3f, 0.3f, 1);
        }
    }
    else //Nothing
    {
        return vec3(0, 0, 0);
    }
}

vec3 cast_ray(int x, int y)
{
    vec3 dir = normalize(get_ray_dir(x, y));

    vec4 hit;
    vec3 col;
    vec3 p = camera_pos;
    vec3 d = dir;

    bool reflective = false;

    for(int i = 0; i < 2; i++)
    {
        hit = fire(p + d * 0.001f, d);

        if(i == 0) //Albedo
        {
            col = get_albedo(hit);
            if(hit.w == 1) reflective = true;
        }
        else
        {
            if(!reflective)
            {
                if(hit.w != -1) col *= vec3(0.5f, 0.5f, 0.5f);
            }
            else
            {               
                col = get_albedo(hit) * vec3(0.8f, 0.15f, 0.15f) + vec3(0.2f, 0.05f, 0.05f);
            }
        }

        p = hit.xyz;
        d = reflective ? reflect(normalize(hit.xyz - camera_pos), normalize(hit.xyz - vec3(0, 0.5f, 10))) : normalize(vec3(1, 1, 1));
    }

    return col;
}

void main()
{
    ivec2 pos = ivec2(screen_pos.x * width / 2 + width / 2, screen_pos.y * height / 2 + height / 2);
    color = vec4(cast_ray(pos.x, pos.y), 1);
}
)VOGON";

void callback_text(GLFWwindow* window, unsigned int text)
{
    str_input += (char) text;
}

void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if(key == GLFW_KEY_BACKSPACE)
        {
            backspace = true;
        }
    }
}

void loop(GLFWwindow* window)
{
    engine_delta = glfwGetTime() - engine_time;
    engine_time = glfwGetTime();

    glfwGetWindowSize(window, &engine_width, &engine_height);

    lmb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
    rmb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if(glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    cursor_x = mx - engine_width / 2;
    cursor_y = -(my - engine_height / 2);

    glViewport(0, 0, engine_width, engine_height);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main()
{
    engine_time = 0;
    str_input = "";

    backspace = false;

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Raycaster", NULL, NULL);

    glfwSetCharCallback(window, callback_text);
    glfwSetKeyCallback(window, callback_key);

    engine_width = 1280;
    engine_height = 720;

    glfwMakeContextCurrent(window);
    glewInit();

    Renderer renderer;
    Shader shader_screen, shader_main;

    shader_screen.add_shader(false, vs_screen, GL_VERTEX_SHADER);
    shader_screen.add_shader(false, fs_screen, GL_FRAGMENT_SHADER);

    shader_screen.compile();

    shader_main.add_shader(false, vs_main, GL_VERTEX_SHADER);
    shader_main.add_shader(false, fs_main, GL_FRAGMENT_SHADER);

    shader_main.compile();

    camera::init(window);

    glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window))
    {
        loop(window);

        camera::update(true);

        shader_main.use();

        shader_main.transfer_int("width", 1280);
        shader_main.transfer_int("height", 720);
        shader_main.transfer_float("fov", 140);

        shader_main.transfer_vec3("camera_pos", camera::get_camera_pos());
        shader_main.transfer_vec3("camera_dir", camera::get_camera_rot());

        renderer.render();

        shader_screen.use();
        renderer.show();
    }
    
    shader_screen.destroy();

    camera::clean();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}