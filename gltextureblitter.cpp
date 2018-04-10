#include "gltextureblitter.h"
#include "glerror.h"

GLTextureBlitter::GLTextureBlitter() : m_texture(0), m_fbo(0) {}

GLTextureBlitter::~GLTextureBlitter()
{
	deinit();
}

void GLTextureBlitter::init(int width, int height)
{
    deinit();
    m_width = width;
    m_height = height;

    CHECK_GL(glGenFramebuffers(1, &m_fbo));
    CHECK_GL(glGenTextures(1, &m_texture));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_texture));
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
}

void GLTextureBlitter::update(unsigned char* data)
{
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_texture));
    CHECK_GL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, data));
}

void GLTextureBlitter::blit()
{
    CHECK_GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_texture));
    CHECK_GL(glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0));
    CHECK_GL(glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
}

void GLTextureBlitter::deinit()
{
    CHECK_GL(glDeleteFramebuffers(1, &m_fbo));
    CHECK_GL(glDeleteTextures(1, &m_texture));
    m_fbo = 0;
    m_texture = 0;
}
