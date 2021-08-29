

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

class PongPongApplication : public Platform::Application
{
public:
    explicit PongPongApplication(const Arguments& arguments);

private:
    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void drawRectangle(int x, int y, const MutableImageView2D& image_view, const Color3& colour);
    int m_y_left;
    int m_y_right;
    int m_width_half;
    int m_height_half;

    enum class PlayerState
    {
        Up,
        Down,
        Idle
    };

    PlayerState m_left_player_state = PlayerState::Idle;
    PlayerState m_right_player_state = PlayerState::Idle;
};

PongPongApplication::PongPongApplication(const Arguments& arguments)
    : Platform::Application{arguments}
{
    m_y_left  = windowSize().y() / 2;
    m_y_right = windowSize().y() / 2;
    m_width_half = windowSize().x() / 80;
    m_height_half = windowSize().y() / 10;
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
        m_y_left = std::min(m_y_left, size_y-m_height_half-1);
    }
    if (m_left_player_state == PlayerState::Down) {
        m_y_left -= size_y / 100;
        m_y_left = std::max(m_y_left, m_height_half);
    }

    if (m_right_player_state == PlayerState::Up) {
        m_y_right += size_y / 100;
        m_y_right = std::min(m_y_right, size_y-m_height_half-1);
    }
    if (m_right_player_state == PlayerState::Down) {
        m_y_right -= size_y / 100;
        m_y_right = std::max(m_y_right, m_height_half);
    }


    //Paint squares

    drawRectangle(image_view.size().x() / 80 + 5, m_y_left, image_view, {1, 1, 1});
    drawRectangle(image_view.size().x() - image_view.size().x() / 80 - 6, m_y_right, image_view, {1, 1, 1});

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

MAGNUM_APPLICATION_MAIN(PongPongApplication)
