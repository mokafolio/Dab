#ifndef DAB_DAB_HPP
#define DAB_DAB_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/Result.hpp>

#include <functional>

namespace dab
{

using Float32 = stick::Float32;
using Float64 = stick::Float64;
using Size = stick::Size;
using UInt8 = stick::UInt8;
using UInt32 = stick::UInt32;
using Int32 = stick::Int32;

struct STICK_API Rect
{
    Float32 x, y, width, height;
};

enum class STICK_API DataType
{
    UInt8,
    Int8,
    UInt16,
    Int16,
    UInt32,
    Int32,
    Float32,
    Float64,
    Count
};

const UInt8 s_dataTypeByteCount[] = {
    // UInt8
    1,
    // Int8
    1,
    // UInt16
    2,
    // Int16
    2,
    // UInt32
    4,
    // Int32
    4,
    // Float32
    4,
    // Float64
    8
};

// @NOTE: For future hints regarding buffer usage
enum STICK_API BufferUsageFlags
{
    BufferUsageDefault = 0
};

enum class STICK_API VertexDrawMode
{
    Triangles,
    TriangleStrip,
    TriangleFan,
    Points,
    Lines,
    LineStrip,
    LineLoop,
    Count
};

enum class STICK_API ProgramVariableType
{
    None,
    Float32,
    Vec2f,
    Vec3f,
    Vec4f,
    Matrix4f
};

enum STICK_API BufferType
{
    BufferDepth = 1,
    BufferColor = 1 << 1,
    BufferStencil = 1 << 2
};

enum class STICK_API FaceDirection
{
    CW,
    CCW,
    Count
};

enum class STICK_API FaceType
{
    None,
    Front,
    Back,
    FrontAndBack,
    Count
};

enum class STICK_API CompareFunction
{
    Equal,
    LessEqual,
    GreaterEqual,
    NotEqual,
    Always,
    Never,
    Less,
    Greater,
    Count
};

enum class STICK_API BlendMode
{
    Add,
    Substract,
    ReverseSubstract,
    Min,
    Max,
    Count
};

enum class STICK_API BlendFunction
{
    Zero,
    One,
    SourceColor,
    InverseSourceColor,
    DestinationColor,
    InverseDestinationColor,
    SourceAlpha,
    InverseSourceAlpha,
    DestinationAlpha,
    InverseDestinationAlpha,
    ConstantColor,
    InverseConstantColor,
    ConstantAlpha,
    InverseConstantAlpha,
    Count
};

enum class STICK_API TextureFormat
{
    R8,
    R16,
    R32,
    R16F,
    R32F,
    RGB8,
    RGB16,
    RGB32,
    RGB16F,
    RGB32F,
    BGR8,
    BGR16,
    BGR32,
    BGR16F,
    BGR32F,
    RGBA8,
    RGBA16,
    RGBA32,
    RGBA16F,
    RGBA32F,
    BGRA8,
    BGRA16,
    BGRA32,
    BGRA16F,
    BGRA32F,
    Depth16,
    Depth24,
    Depth32,
    Depth24Stencil8,
    Depth32F,
    Depth32FStencil8,
    Count
};

enum class STICK_API TextureWrap
{
    Repeat,
    ClampToEdge,
    ClampToBorder,
    Count
};

enum class STICK_API TextureFiltering
{
    Nearest,  // works with and without mipmapping
    Bilinear, // works with and without mipmapping
    Trilinear // requires mip mapping
};

struct STICK_API TextureFormatInfo
{
    bool bIsColorFormat;
    bool bIsStencilFormat;
};

const TextureFormatInfo s_textureFormatInfos[] = {
    // R8
    { true, false },
    // R16
    { true, false },
    // R32
    { true, false },
    // R16F
    { true, false },
    // R32F
    { true, false },
    // RGB8
    { true, false },
    // RGB16
    { true, false },
    // RGB32
    { true, false },
    // RGB16F
    { true, false },
    // RGB32F
    { true, false },
    // BGR8
    { true, false },
    // BGR16
    { true, false },
    // BGR32
    { true, false },
    // BGR16F
    { true, false },
    // BGR32F
    { true, false },
    // RGBA8
    { true, false },
    // RGBA16
    { true, false },
    // RGBA32
    { true, false },
    // RGBA16F
    { true, false },
    // RGBA32F
    { true, false },
    // BGRA8
    { true, false },
    // BGRA16
    { true, false },
    // BGRA32
    { true, false },
    // BGRA16F
    { true, false },
    // BGRA32F
    { true, false },
    // Depth16
    { false, false },
    // Depth24
    { false, false },
    // Depth32
    { false, false },
    // Depth24Stencil8
    { false, true },
    // Depth32F
    { false, false },
    // Depth32FStencil8
    { false, true }
};

struct ClearSettings;
struct RenderPassSettings;
struct RenderBufferSettings;
struct PipelineSettings;
struct SamplerSettings;

struct STICK_API VertexElement
{
    DataType dataType;
    UInt32 elementCount;

    UInt32 offset; // the offset to the first element
    UInt32 stride; // the offset between the elements
    UInt32 location;
};

using VertexElementArray = stick::DynamicArray<VertexElement>;

struct STICK_API VertexLayout
{
    VertexLayout() = default;

    VertexLayout(const VertexElementArray & _elements);

    VertexLayout(VertexElementArray && _elements);

    void finish();

    VertexLayout(const VertexLayout &) = default;
    VertexLayout(VertexLayout &&) = default;
    VertexLayout & operator=(const VertexLayout &) = default;
    VertexLayout & operator=(VertexLayout &&) = default;

    VertexElementArray elements;
};

class Program;
class Pipeline;
class PipelineVariable;
class PipelineTexture;
class VertexBuffer;
class IndexBuffer;
class Mesh;
class Texture;
class Sampler;
class RenderBuffer;
class RenderPass;

struct STICK_API ClearColor
{
    Float32 r, g, b, a;
};

struct STICK_API ClearSettings
{
    ClearSettings() = default;
    ClearSettings(Float32 _r, Float32 _g, Float32 _b, Float32 _a);
    ClearSettings(const ClearColor & _col, Float32 _depth, Int32 _stencil);

    stick::Maybe<ClearColor> color;
    stick::Maybe<Float64> depth;
    stick::Maybe<Int32> stencil;
};

struct STICK_API RenderPassSettings
{
    RenderPassSettings();
    RenderPassSettings(RenderBuffer * _renderBuffer,
                       stick::Maybe<ClearSettings> _clear = stick::Maybe<ClearSettings>());
    RenderPassSettings(const ClearSettings & _clear);

    RenderBuffer * renderBuffer;
    stick::Maybe<ClearSettings> clear;
};

struct STICK_API BlendSettings
{
    BlendSettings();

    // helpers to set blend mode / function for color and alpha together
    void setBlendMode(BlendMode _mode);
    void setBlendFunction(BlendFunction _srcFunc, BlendFunction _destFunc);

    BlendMode colorBlendMode;
    BlendFunction colorSrcBlendFunction;
    BlendFunction colorDestBlendFunction;
    BlendMode alphaBlendMode;
    BlendFunction alphaSrcBlendFunction;
    BlendFunction alphaDestBlendFunction;
};

struct STICK_API ColorWriteSettings
{
    bool r, g, b, a;
};

struct STICK_API PipelineSettings
{
    PipelineSettings(Program * _prog = nullptr);

    Program * program;
    Rect viewport;
    stick::Maybe<Rect> scissor;
    bool multisample;
    bool depthTest;
    bool depthWrite;
    CompareFunction depthFunction;
    ColorWriteSettings colorWriteSettings;
    stick::Maybe<BlendSettings> blendSettings;
    FaceDirection faceDirection;
    FaceType cullFace;
};

struct STICK_API SamplerSettings
{
    SamplerSettings();

    TextureWrap wrapS;
    TextureWrap wrapT;
    TextureWrap wrapR;
    TextureFiltering filtering;
    bool mipMapping;
};

struct STICK_API RenderBufferSettings
{
    struct RenderTarget
    {
        TextureFormat format;
        stick::UInt32 mipmapLevelCount;
    };

    Size width;
    Size height;
    UInt32 sampleCount;
    stick::DynamicArray<RenderTarget> renderTargets;
};

class STICK_API RenderDevice
{
  public:
    virtual ~RenderDevice()
    {
    }

    virtual stick::Result<Program *> createProgram(const char * _vertexShader,
                                                   const char * _pixelShader) = 0;
    virtual void destroyProgram(Program * _prog) = 0;
    virtual stick::Result<Pipeline *> createPipeline(const PipelineSettings & _settings) = 0;
    virtual void destroyPipeline(Pipeline * _pipe) = 0;
    virtual stick::Result<VertexBuffer *> createVertexBuffer(
        BufferUsageFlags _usage = BufferUsageDefault) = 0;
    virtual void destroyVertexBuffer(VertexBuffer * _buff) = 0;
    virtual stick::Result<IndexBuffer *> createIndexBuffer(
        BufferUsageFlags _usage = BufferUsageDefault) = 0;
    virtual void destroyIndexBuffer(IndexBuffer * _buff) = 0;
    virtual stick::Result<Mesh *> createMesh(VertexBuffer ** _vertexBuffers,
                                             const VertexLayout * _layouts,
                                             Size _count,
                                             IndexBuffer * _indexBuffer = nullptr) = 0;
    virtual void destroyMesh(Mesh * _mesh) = 0;

    virtual stick::Result<Texture *> createTexture() = 0;
    virtual void destroyTexture(Texture * _texture) = 0;
    virtual stick::Result<Sampler *> createSampler(
        const SamplerSettings & _settings = SamplerSettings()) = 0;
    virtual void destroySampler(Sampler * _sampler) = 0;

    virtual stick::Result<RenderBuffer *> createRenderBuffer(
        const RenderBufferSettings & _settings) = 0;
    virtual void destroyRenderBuffer(RenderBuffer * _renderBuffer, bool _bDestroyRenderTargets) = 0;

    virtual RenderPass * beginPass(const RenderPassSettings & _settings = RenderPassSettings()) = 0;
    virtual stick::Error endPass(RenderPass * _pass) = 0;

    virtual void readPixels(stick::Int32 _x,
                            stick::Int32 _y,
                            stick::Int32 _w,
                            stick::Int32 _h,
                            TextureFormat _format,
                            void * _outData) = 0;

  protected:
    RenderDevice()
    {
    }
};

STICK_API stick::Result<RenderDevice *> createRenderDevice(
    stick::Allocator & _alloc = stick::defaultAllocator());
STICK_API void destroyRenderDevice(RenderDevice * _device);

class STICK_API Program
{
  public:
    virtual ~Program()
    {
    }

  protected:
    Program()
    {
    }
};

class STICK_API Pipeline
{
  public:
    virtual ~Pipeline()
    {
    }

    virtual PipelineVariable * variable(const char * _name) = 0;
    virtual PipelineTexture * texture(const char * _name) = 0;

  protected:
    Pipeline()
    {
    }
};

class STICK_API PipelineVariable
{
  public:
    virtual ~PipelineVariable()
    {
    }

    virtual void setFloat32(Float32 _value) = 0;
    virtual void setInt32(Int32 _value) = 0;
    virtual void setVec2f(Float32 _x, Float32 _y) = 0;
    virtual void setVec3f(Float32 _x, Float32 _y, Float32 _z) = 0;
    virtual void setVec4f(Float32 _x, Float32 _y, Float32 _z, Float32 _w) = 0;
    virtual void setMat4f(Float32 * _mat) = 0;

    //@TODO: more setters, array setters

  protected:
    PipelineVariable()
    {
    }
};

class STICK_API PipelineTexture
{
  public:
    virtual ~PipelineTexture()
    {
    }

    virtual void set(const Texture * _tex, const Sampler * _sampler) = 0;

  protected:
    PipelineTexture()
    {
    }
};

class STICK_API VertexBuffer
{
  public:
    virtual ~VertexBuffer()
    {
    }

    virtual void loadDataRaw(const void * _data, Size _byteCount) = 0;

  protected:
    VertexBuffer()
    {
    }
};

class STICK_API IndexBuffer
{
  public:
    virtual ~IndexBuffer()
    {
    }

    virtual void loadDataRaw(const void * _data, Size _byteCount) = 0;

  protected:
    IndexBuffer()
    {
    }
};

class STICK_API Mesh
{
  public:
    virtual ~Mesh()
    {
    }

  protected:
    Mesh()
    {
    }
};

class STICK_API Texture
{
  public:
    virtual ~Texture()
    {
    }

    virtual void loadPixels(UInt32 _width,
                            UInt32 _height,
                            UInt32 _depth,
                            const void * _data,
                            DataType _dataType,
                            TextureFormat _format,
                            UInt32 _alignment = 4,
                            UInt32 _mipmapLevelCount = 0) = 0;

  protected:
    Texture()
    {
    }
};

class STICK_API Sampler
{
  public:
    virtual ~Sampler()
    {
    }

  protected:
    Sampler()
    {
    }
};

class STICK_API RenderBuffer
{
  public:
    virtual ~RenderBuffer()
    {
    }

    virtual const stick::DynamicArray<Texture *> colorTargets() const = 0;
    virtual Texture * depthStencilTarget() const = 0;
    virtual UInt32 width() const = 0;
    virtual UInt32 height() const = 0;
    virtual UInt32 sampleCount() const = 0;

  protected:
    RenderBuffer()
    {
    }
};

using ExternalDrawFunction = std::function<stick::Error()>;

class STICK_API RenderPass
{
  public:
    virtual ~RenderPass()
    {
    }

    virtual void drawMesh(const Mesh * _mesh,
                          const Pipeline * _pipeline,
                          UInt32 _vertexOffset,
                          UInt32 _vertexCount,
                          VertexDrawMode _drawMode) = 0;

    virtual void drawMesh(const Mesh * _mesh,
                          const Pipeline * _pipeline,
                          UInt32 _vertexOffset,
                          UInt32 _vertexCount,
                          UInt32 _baseVertex,
                          VertexDrawMode _drawMode) = 0;

    virtual void drawCustom(ExternalDrawFunction _fn) = 0;
    virtual void setViewport(Int32 _x, Int32 _y, UInt32 _w, UInt32 _h) = 0;
    virtual void setScissor(Int32 _x, Int32 _y, UInt32 _w, UInt32 _h) = 0;
    virtual void clearBuffers(const ClearSettings & _settings) = 0;

  protected:
    RenderPass()
    {
    }
};

} // namespace dab

#endif // DAB_DAB_HPP
