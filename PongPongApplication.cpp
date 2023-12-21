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
#include <assert.h>

#include "FullScreenTriangle.h"
#include <iostream>

using namespace Magnum;

using namespace Math::Literals;

PongPongApplication::PongPongApplication(const Arguments& arguments)
    : Platform::Application{arguments, Configuration()
                                           .setSize({1024, 512})
                                           .setTitle("PongPong")
                                           .setWindowFlags(Configuration::WindowFlag::Resizable)}
    , m_cache{Vector2i{2048}, Vector2i{512}, 22}
{
    m_y_left      = windowSize().y() / 2;
    m_y_right     = windowSize().y() / 2;
    m_width_half  = windowSize().x() / 80;
    m_height_half = windowSize().y() / 10;
    m_ball        = {windowSize() / 2, {1, 0}, windowSize().x() / 50, 1.};
    m_base_speed  = windowSize().x() / 140;

    m_paddle_speed = windowSize().x() / 70;

    // Allocate storage for pixel colours
    m_colours.resize(windowSize().product());
    //Give colours to image and get matrix
    m_image_view = MutableImageView2D(PixelFormat::RGB32F, windowSize(), {m_colours.data(), m_colours.size()});
    auto pixel = m_image_view.pixels<Color3>();
    for (auto x = 0; x < m_image_view.size().x(); x++) {
        for (auto y = 0; y < m_image_view.size().y(); y++) {
            pixel[y][x] = {0, 0.3, 0.8};
        }
    }

    /* Load a TrueTypeFont plugin and open the font */
    Utility::Resource rs("pp-data");
    m_font = m_manager.loadAndInstantiate("FreeTypeFont");
    if(!m_font || !m_font->openData(rs.getRaw("SourceSansPro-Regular.ttf"), 180.0f))
        Fatal{} << "Cannot open font file";

    /* Glyphs we need to render everything */
    m_font->fillGlyphCache(m_cache, "0123456789: ");

    /* Dynamically updated text that shows rotation/zoom of the other. Size in
       points that stays the same if you resize the window. Aligned so top
       right of the bounding box is at mesh origin, and then transformed so the
       origin is at the top right corner of the window. */
    m_dynamicText.reset(new Text::Renderer2D(*m_font, m_cache, 32.0f, Text::Alignment::TopRight));
    m_dynamicText->reserve(40, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);
    m_transformationProjectionDynamicText =
        Matrix3::projection(Vector2{windowSize()})*
        Matrix3::translation(Vector2{windowSize()}*0.5f);

    /* Set up premultiplied alpha blending to avoid overlapping text characters
       to cut into each other */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);

    updateText();
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

void PongPongApplication::updateText() {
    m_dynamicText->render(Utility::formatString("{}:{}", m_left_score, m_right_score));
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
    m_ball.position += Vector2i(m_ball.direction * m_base_speed * m_ball.speed);
    if (m_ball.position.y() < 0 + m_ball.radius) {
        m_ball.position.y() = 0 + m_ball.radius;
        m_ball.direction.y() *= -1;
    }
    if (m_ball.position.y() > windowSize().y() - m_ball.radius) {
        m_ball.position.y() = windowSize().y() - m_ball.radius;
        m_ball.direction.y() *= -1;
    }
    if (m_ball.position.x() < 0 + m_ball.radius || m_ball.position.x() > windowSize().x() - m_ball.radius) {
        if(m_ball.position.x() < 0 + m_ball.radius) {
            ++m_right_score;
            m_ball.direction = {-1,0};
        }
        else {
            ++m_left_score;
            m_ball.direction = {1,0};
        }
        m_ball.position = windowSize() / 2;
        m_ball.speed = 1.;
    }
    updateText();
    touchPaddle();
}

void PongPongApplication::touchPaddle()
{
    // left paddle interaction
    if (m_ball.position.x() - m_ball.radius <= windowSize().x() / 80 + 6 + m_width_half &&
        std::abs(m_ball.position.y() - m_y_left) < m_height_half) {
        const auto relative_position = m_ball.position.y() - m_y_left;
        const double position_ratio  = double(relative_position) / double(m_height_half);
        m_ball.direction.x() *= -1.;
        m_ball.direction.y() = position_ratio;
        assert(m_ball.direction.length() > 0.);
        m_ball.direction     = m_ball.direction.normalized();
        m_ball.speed         = std::max((std::abs(position_ratio) + 0.5)*m_ball.speed, 1.);
    }

    // right paddle interaction
    if (m_ball.position.x() + m_ball.radius >= windowSize().x() - windowSize().x() / 80 - 6 - m_width_half &&
        std::abs(m_ball.position.y() - m_y_right) < m_height_half) {
        const auto relative_position = m_ball.position.y() - m_y_right;
        const double position_ratio  = double(relative_position) / double(m_height_half);
        m_ball.direction.x() *= -1.;
        m_ball.direction.y() = position_ratio;
        assert(m_ball.direction.length() > 0.);
        m_ball.direction     = m_ball.direction.normalized();
        m_ball.speed         = std::max((std::abs(position_ratio) + 0.5)*m_ball.speed, 1.);
    }
    redraw();
}

void PongPongApplication::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    // get of the window
    const auto size_x = windowSize().x();
    const auto size_y = windowSize().y();
    int window_size   = windowSize().product();


    drawRectangle(m_image_view.size().x() / 80 + 5, m_y_left, m_image_view, {0, 0.3, 0.8});
    drawRectangle(m_image_view.size().x() - m_image_view.size().x() / 80 - 6, m_y_right, m_image_view, {0, 0.3, 0.8});

    if (m_left_player_state == PlayerState::Up) {
        m_y_left += m_paddle_speed;
        m_y_left = std::min(m_y_left, size_y - m_height_half - 1);
    }
    if (m_left_player_state == PlayerState::Down) {
        m_y_left -= m_paddle_speed;
        m_y_left = std::max(m_y_left, m_height_half);
    }

    if (m_right_player_state == PlayerState::Up) {
        m_y_right += m_paddle_speed;
        m_y_right = std::min(m_y_right, size_y - m_height_half - 1);
    }
    if (m_right_player_state == PlayerState::Down) {
        m_y_right -= m_paddle_speed;
        m_y_right = std::max(m_y_right, m_height_half);
    }

    //Paint paddles and ball
    drawRectangle(m_image_view.size().x() / 80 + 5, m_y_left, m_image_view, {1, 1, 1});
    drawRectangle(m_image_view.size().x() - m_image_view.size().x() / 80 - 6, m_y_right, m_image_view, {1, 1, 1});
    drawBall(m_image_view, {0, 0.3, 0.8});
    moveBall();
    drawBall(m_image_view, {0.8, 0, 0.2});

    //upload pixels to GPU
    GL::Texture2D texture;
    texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
        .setStorage(1, GL::TextureFormat::RGB32F, m_image_view.size())
        .setSubImage(0, {}, m_image_view);

    //draw texture using shader
    auto triangleShader = FullScreenTriangle{};
    triangleShader.bindTexture(texture).draw(GL::Mesh{}.setCount(3));

    m_shader.bindVectorTexture(m_cache.texture());

    m_shader
        .setTransformationProjectionMatrix(m_transformationProjectionDynamicText)
        .setColor(0xffffff_rgbf)
        .setOutlineRange(0.5f, 1.0f)
        .setSmoothness(0.075f)
        .draw(m_dynamicText->mesh());

    swapBuffers();
    redraw();
}
