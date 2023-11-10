#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture
{
public:
    // UniquePtr가 아닌 이유 : 
    static TextureUPtr CreateFromImage(const Image *image);
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;

private:
    Texture() {}
    void CreateTexture();  // bool이 아니라 void -> 무조건 성공을 가정
    void SetTextureFromImage(const Image *image);

    uint32_t m_texture{0};
};

#endif // __TEXTURE_H__