#include "PongPongApplication.h"
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

#include <vector>

#include "FullScreenTriangle.h"
#include <iostream>

using namespace Magnum;
PongPongApplication::PongPongApplication(const Arguments& arguments)
    : Platform::Application{arguments /*, Configuration().setSize({1024, 1024})*/}
{
    m_y_left      = windowSize().y() / 2;
    m_y_right     = windowSize().y() / 2;
    m_width_half  = windowSize().x() / 80;
    m_height_half = windowSize().y() / 10;
    m_ball        = {windowSize() / 2, {1, 5}, windowSize().x() / 50};
}

void PongPongApplication::keyPressEvent(KeyEvent& event)
{
    if (event.key() == KeyEvent::Key::Left) {
        m_right_player_state = PlayerState::Down;
    }
    if (event.key() == KeyEvent::Key::Right) {
        m_right_player_state = PlayerState::Up;
    }
    if (event.key() == KeyEvent::Key::D) {
        m_left_player_state = PlayerState::Down;
    }
    if (event.key() == KeyEvent::Key::A) {
        m_left_player_state = PlayerState::Up;
    }
    redraw();
}

void PongPongApplication::keyReleaseEvent(KeyEvent& event)
{
    if (event.key() == KeyEvent::Key::Left) {
        m_right_player_state = PlayerState::Idle;
    }
    if (event.key() == KeyEvent::Key::Right) {
        m_right_player_state = PlayerState::Idle;
    }
    if (event.key() == KeyEvent::Key::D) {
        m_left_player_state = PlayerState::Idle;
    }
    if (event.key() == KeyEvent::Key::A) {
        m_left_player_state = PlayerState::Idle;
    }
    redraw();
}

void PongPongApplication::drawRectangle(int x, int y, const MutableImageView2D& image_view, const Color3& colour)
{
    auto pixel = image_view.pixels<Color3>();
    for (auto i = -m_width_half; i < m_width_half + 1; i++) {
        for (auto j = -m_height_half; j < m_height_half + 1; j++) {
            int y_coord             = std::min(std::max(y + j, 0), image_view.size().y() - 1);
            int x_coord             = std::min(std::max(x + i, 0), image_view.size().x() - 1);
            pixel[y_coord][x_coord] = colour;
        }
    }
}

void PongPongApplication::drawBall(const MutableImageView2D& image_view, const Color3& colour)
{
    auto pixel = image_view.pixels<Color3>();
    Vector2i coord;
    for (auto i = -m_ball.radius; i < m_ball.radius + 1; i++) {
        for (auto j = -m_ball.radius; j < m_ball.radius + 1; j++) {
            coord.y() = std::min(std::max(m_ball.position.y() + j, 0), image_view.size().y() - 1);
            coord.x() = std::min(std::max(m_ball.position.x() + i, 0), image_view.size().x() - 1);
            if (Vector2d(coord - m_ball.position).length() < m_ball.radius) {
                pixel[coord.y()][coord.x()] = colour;
            }
        }
    }
}

void PongPongApplication::moveBall()
{
    m_ball.position += Vector2i(m_ball.direction);
    if (m_ball.position.y() < 0 + m_ball.radius) {
        m_ball.position.y() = 0 + m_ball.radius;
        m_ball.direction.y() *= -1;
    }
    if (m_ball.position.y() > windowSize().y() - m_ball.radius) {
        m_ball.position.y() = windowSize().y() - m_ball.radius;
        m_ball.direction.y() *= -1;
    }
    if (m_ball.position.x() == 0 + m_ball.radius || m_ball.position.x() == windowSize().x() - m_ball.radius) {
        m_ball.position = windowSize()/2;
    }
    touchPaddle();
}

void PongPongApplication::touchPaddle(){
    //TODO
}

void PongPongApplication::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    // get of the window
    const auto size_x = windowSize().x();
    const auto size_y = windowSize().y();
    int window_size   = windowSize().product();

    // Allocate storage for pixel colours
    std::vector<Color3> colours(window_size);

    //Give colours to image and get matrix
    MutableImageView2D image_view(PixelFormat::RGB32F, windowSize(), {colours.data(), colours.size()});
    auto pixel = image_view.pixels<Color3>();
    for (auto x = 0; x < image_view.size().x(); x++) {
        for (auto y = 0; y < image_view.size().y(); y++) {
            pixel[y][x] = {0, 0.3, 0.8};
        }
    }

    if (m_left_player_state == PlayerState::Up) {
        m_y_left += size_y / 100;
        m_y_left = std::min(m_y_left, size_y - m_height_half - 1);
    }
    if (m_left_player_state == PlayerState::Down) {
        m_y_left -= size_y / 100;
        m_y_left = std::max(m_y_left, m_height_half);
    }

    if (m_right_player_state == PlayerState::Up) {
        m_y_right += size_y / 100;
        m_y_right = std::min(m_y_right, size_y - m_height_half - 1);
    }
    if (m_right_player_state == PlayerState::Down) {
        m_y_right -= size_y / 100;
        m_y_right = std::max(m_y_right, m_height_half);
    }

    //Paint paddles and ball
    drawRectangle(image_view.size().x() / 80 + 5, m_y_left, image_view, {1, 1, 1});
    drawRectangle(image_view.size().x() - image_view.size().x() / 80 - 6, m_y_right, image_view, {1, 1, 1});
    drawBall(image_view, {0.8, 0, 0.2});

    moveBall();

    //upload pixels to GPU
    GL::Texture2D texture;
    texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
        .setStorage(1, GL::TextureFormat::RGB32F, image_view.size())
        .setSubImage(0, {}, image_view);

    //draw texture using shader
    auto triangleShader = FullScreenTriangle{};
    triangleShader.bindTexture(texture).draw(GL::Mesh{}.setCount(3));

    swapBuffers();
    redraw();
}
