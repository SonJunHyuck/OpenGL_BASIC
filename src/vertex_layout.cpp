#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create()
{
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();  // return void (무조건 성공할거니까)
    return std::move(vertexLayout);
}

VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}

void VertexLayout::Bind() const
{
    glBindVertexArray(m_vertexArrayObject);
}

void VertexLayout::SetAttrib( uint32_t attribIndex, int count, uint32_t type, bool normalized,
    size_t stride, uint64_t offset) const
{
    glEnableVertexAttribArray(attribIndex);

    // stride : vertex 간의 간격 (vertex 안에는 pos, color 등 다양한 attr이 있음)
    glVertexAttribPointer(attribIndex, count, type, normalized, stride, (const void *)offset);
}

void VertexLayout::Init()
{
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}