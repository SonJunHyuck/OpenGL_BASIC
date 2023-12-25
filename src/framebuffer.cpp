#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer()
{
    if (m_depthStencilBuffer)
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault()
{
    // 프레임버퍼에 그리던 것을, 다시 화면에 그리는 것으로 변경
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment)
{
    // Attach Color Texture
    m_colorAttachment = colorAttachment;

    // Gen & Bind Framebuffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);  // 이 프레임버퍼에 그림을 그릴 것이다.

    // Buffer에 Texture를 쓸 것이다. (이 Framebuffer에 Colorbuffer를 붙일 것이고, Colorbuffer는 Texture2D 형태고, colorAttachment에 붙일 것이다.)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->Get(), 0);  // 0은 mipmap Level

    // Gen & Bind RenderBuffer (Texture화 하지않고, 바로 Buffer로 만들어서 쓸 것임)
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, colorAttachment->GetWidth(), colorAttachment->GetHeight());
    
    // Default Buffer로 전환 -> 화면에 그려주는 버퍼로 전환
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // 이 Framebuffer에 Renderbuffer(Depth_Stencil)를 Attachment (현재 Framebuffer에는 Color만 붙어 있으니까)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);

    // 지금까지의 세팅을 사용 가능한지 점검 (하드웨어마다 지원 가능 범위가 달라서 꼭 점검필요)
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}