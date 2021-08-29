#ifndef PONGPONG_PONGPONGAPPLICATION_H
#define PONGPONG_PONGPONGAPPLICATION_H

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Mesh.h>

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
    Vector2d direction;
    int radius;
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
    int m_y_left;
    int m_y_right;
    int m_width_half;
    int m_height_half;
    Ball m_ball;
    PlayerState m_left_player_state = PlayerState::Idle;
    PlayerState m_right_player_state = PlayerState::Idle;
};

#endif //PONGPONG_PONGPONGAPPLICATION_H
