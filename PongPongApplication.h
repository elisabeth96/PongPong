#ifndef PONGPONG_PONGPONGAPPLICATION_H
#define PONGPONG_PONGPONGAPPLICATION_H

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Complex.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/DistanceFieldVectorGL.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>

#include <algorithm>
#include <vector>

#include <SDL.h>

#include "FullScreenTriangle.h"
#include <iostream>


using namespace Magnum;

enum class PlayerState
{
    Up,
    Down,
    Idle
};

struct Ball{
    Vector2i position;

    // unit length vector
    Vector2d direction;
    int radius;
    // relative to a pixel base speed
    double speed;
};

class PongPongApplication : public Platform::Application
{
public:
    explicit PongPongApplication(const Arguments& arguments);

private:
    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void drawRectangle(int x, int y, const MutableImageView2D& image_view, const Color3& colour);
    void drawBall(const MutableImageView2D& image_view, const Color3& colour);
    void moveBall();
    void touchPaddle();
    void updateText();

    int m_y_left;
    int m_y_right;
    int m_width_half;
    int m_height_half;
    Ball m_ball;
    PlayerState m_left_player_state = PlayerState::Idle;
    PlayerState m_right_player_state = PlayerState::Idle;

    int m_paddle_speed;
    double m_base_speed;

    int m_left_score = 0, m_right_score = 0;

    std::vector<Color3> m_colours;
    MutableImageView2D m_image_view = MutableImageView2D(PixelFormat::RGB32F, {0,0}, nullptr);

    PluginManager::Manager<Text::AbstractFont> m_manager;
    Containers::Pointer<Text::AbstractFont> m_font;

    Text::DistanceFieldGlyphCache m_cache;
    GL::Buffer m_vertices, m_indices;
    Containers::Pointer<Text::Renderer2D> m_dynamicText;
    Shaders::DistanceFieldVectorGL2D m_shader;

    Matrix3 m_projectionRotatingText, m_transformationProjectionDynamicText;
};

#endif //PONGPONG_PONGPONGAPPLICATION_H
