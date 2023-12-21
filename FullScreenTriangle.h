#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/GL/GL.h>

// Shader to draw a full screen triangle
class FullScreenTriangle : public Magnum::GL::AbstractShaderProgram
{
public:
    using Position = Magnum::Shaders::Generic3D::Position;

    explicit FullScreenTriangle(Magnum::NoCreateT)
        : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    explicit FullScreenTriangle();

    FullScreenTriangle& bindTexture(Magnum::GL::Texture2D& texture);
};
