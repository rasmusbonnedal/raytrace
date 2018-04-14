#pragma once
#include <GL/gl3w.h>

class GLTextureBlitter
{
public:
    GLTextureBlitter();
    ~GLTextureBlitter();
    void init(int width, int height);
    void update(unsigned char* data);
    void blit();

private:
    void deinit();
    GLuint m_texture, m_fbo;
    int m_width, m_height;
};
