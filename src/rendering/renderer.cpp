#include "renderer.h"

#include <iostream>

#include "../../lib/GL/glew.h"

#define RENDER_WIDTH 1280
#define RENDER_HEIGHT 720

Renderer::Renderer()
{
    screen = new Mesh(1);

    screen->add_data(0, Vec2(-1, -1));
    screen->add_data(0, Vec2(1, -1));
    screen->add_data(0, Vec2(1, 1));
    screen->add_data(0, Vec2(-1, 1));

    screen->add_index(0);
    screen->add_index(1);
    screen->add_index(2);
    screen->add_index(0);
    screen->add_index(3);
    screen->add_index(2);

    screen->compile();

    glGenFramebuffers(1, &screen_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is incomplete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
    screen->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::show()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, engine_width, engine_height);
    glClear(GL_COLOR_BUFFER_BIT);
    screen->bind();
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}