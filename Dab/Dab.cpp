#include <Dab/Dab.hpp>

#include <Dab/OpenGL/GLDab.hpp>

namespace dab
{
using namespace stick;

Result<RenderDevice *> createRenderDevice(Allocator & _alloc)
{
    return _alloc.create<gl::GLRenderDevice>(_alloc);
}

void destroyRenderDevice(RenderDevice * _device)
{
    if (_device)
        static_cast<gl::GLRenderDevice *>(_device)->m_alloc->destroy(_device);
}

VertexLayout::VertexLayout(const VertexElementArray & _elements) : elements(_elements)
{
    finish();
}

VertexLayout::VertexLayout(VertexElementArray && _elements) : elements(std::move(_elements))
{
    finish();
}

void VertexLayout::finish()
{
    auto it = elements.begin();

    UInt32 byteOffset = 0;
    UInt32 stride = 0;
    UInt32 loc = 0;

    for (; it != elements.end(); ++it)
    {
        auto s = (*it).elementCount * s_dataTypeByteCount[static_cast<Size>((*it).dataType)];
        stride += s;
        (*it).offset = byteOffset;
        (*it).location = loc++;
        byteOffset += s;
    }

    it = elements.begin();
    for (; it != elements.end(); ++it)
    {
        (*it).stride = stride;
    }
}
PipelineSettings::PipelineSettings(Program * _prog) :
    program(_prog),
    viewport({ 0, 0, 0, 0 }),
    depthTest(false),
    depthWrite(false),
    depthFunction(CompareFunction::Less),
    colorWriteSettings({ true, true, true, true }),
    faceDirection(FaceDirection::CCW),
    cullFace(FaceType::None)
{
}

ClearSettings::ClearSettings(Float32 _r, Float32 _g, Float32 _b, Float32 _a) :
    color({ _r, _g, _b, _a })
{
}

ClearSettings::ClearSettings(const ClearColor & _col, Float32 _depth, Int32 _stencil) :
    color(_col),
    depth(_depth),
    stencil(_stencil)
{
}

RenderPassSettings::RenderPassSettings() : renderBuffer(nullptr)
{
}

RenderPassSettings::RenderPassSettings(RenderBuffer * _renderBuffer, Maybe<ClearSettings> _clear) :
    renderBuffer(_renderBuffer),
    clear(_clear)
{
}

RenderPassSettings::RenderPassSettings(const ClearSettings & _clear) :
    renderBuffer(nullptr),
    clear(_clear)
{
}

SamplerSettings::SamplerSettings() :
    wrapS(TextureWrap::ClampToEdge),
    wrapT(TextureWrap::ClampToEdge),
    wrapR(TextureWrap::ClampToEdge),
    filtering(TextureFiltering::Bilinear),
    mipMapping(false)
{
}

} // namespace dab
