#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <string>

#include "mesh.h"

extern double engine_delta, engine_time;
extern int engine_width, engine_height, cursor_x, cursor_y, input_mode;
extern bool lmb, rmb, backspace;

extern std::string str_input;

class Renderer
{
    public:
        Renderer();

        void render();
        void show();
    private:
        Mesh* screen;
        unsigned int screen_fbo, texture;
};

#endif