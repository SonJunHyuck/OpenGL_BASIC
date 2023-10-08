#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"

CLASS_PTR(Shader);
/*
CLASS_PTR(Shader) 는 다음으로 대치
class Shader; \
using ShaderUPtr = std::unique_ptr<Shader>; \
using ShaderPtr = std::shared_ptr<Shader>; \
using ShaderWPtr = std::weak_ptr<Shader>;
*/

class Shader
{
public:
    static ShaderUPtr CreateFromFile(const std::string &filename, GLenum shaderType);

    ~Shader();
    uint32_t Get() const { return m_shader; }

private:
    // 생성자, 초기화 함수 모두 private -> 오직 CrateFromFile 만을 이용해서 인스턴스를 만들어라.
    Shader() {}  // new 불가능
    bool LoadFile(const std::string &filename, GLenum shaderType);
    uint32_t m_shader{0};
};

#endif // __SHADER_H__