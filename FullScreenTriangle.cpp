#include "FullScreenTriangle.h"

#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/Reference.h>

#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>

using namespace Magnum;
using namespace Corrade;

FullScreenTriangle::FullScreenTriangle()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    const Utility::Resource rs{"pp-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("FullScreenTriangle.vert"));

    frag.addSource(rs.get("FullScreenTriangle.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

FullScreenTriangle& FullScreenTriangle::bindTexture(GL::Texture2D& texture)
{
    texture.bind(0);
    return *this;
}
