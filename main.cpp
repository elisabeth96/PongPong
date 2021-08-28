//
// Created by elisabeth on 28.08.21.
//
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
#include <algorithm>

#include "FullScreenTriangle.h"

using namespace Magnum;

class PongPongApplication : public Platform::Application
{
public:
    explicit PongPongApplication(const Arguments& arguments);

private:
    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void drawRectangle(int x, int y, const MutableImageView2D& image_view, const Color3& colour);
    int m_y1;
    int m_y2;
};

PongPongApplication::PongPongApplication(const Arguments& arguments)
    : Platform::Application{arguments}
{
    m_y1 = windowSize().y() / 2;
    m_y2 = windowSize().y() / 2;
}

void PongPongApplication::keyPressEvent(KeyEvent& event)
{
    if (event.key() == KeyEvent::Key::Left) {
        m_y2 -= windowSize().y() / 200;
    }
    else if (event.key() == KeyEvent::Key::Right) {
        m_y2 += windowSize().y() / 200;
    }
    else if (event.key() == KeyEvent::Key::A) {
        m_y1 -= windowSize().y() / 200;
    }
    else if (event.key() == KeyEvent::Key::D) {
        m_y1 += windowSize().y() / 200;
    }
    drawEvent();
}

void PongPongApplication::drawRectangle(int x, int y, const MutableImageView2D& image_view, const Color3& colour)
{
    auto pixel = image_view.pixels<Color3>();
    int size_x = image_view.size().x() / 80;
    int size_y = image_view.size().y() / 10;
    for (auto i = -size_x; i < size_x + 1; i++) {
        for (auto j = -size_y; j < size_y + 1; j++) {
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
    int window_size = windowSize().product();

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
    //Paint squares
    drawRectange(image_view.size().x() / 80 + 5, m_y1, image_view, {1, 1, 1});
    drawRectange(image_view.size().x() - image_view.size().x() / 80 - 6, m_y2, image_view, {1, 1, 1});

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
}

MAGNUM_APPLICATION_MAIN(PongPongApplication)
