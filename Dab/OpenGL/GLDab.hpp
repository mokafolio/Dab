#ifndef DAB_OPENGL_GLDAB_HPP
#define DAB_OPENGL_GLDAB_HPP

#include <Dab/Dab.hpp>
#include <GL/gl3w.h>
#include <Stick/FixedArray.hpp>
#include <Stick/StaticArray.hpp>
#include <Stick/String.hpp>
#include <Stick/UniquePtr.hpp>
#include <Stick/Variant.hpp>

#include <functional>

namespace dab
{
namespace gl
{

using namespace stick;

//@TODO: Remove everything we don't need
enum STICK_API RenderFlag : UInt64
{
    RF_DepthTest = 1 << 0,
    RF_Multisampling = 1 << 1,
    RF_Blending = 1 << 2,
    RF_DepthWrite = 1 << 3,
    RF_ColorWriteRed = 1 << 4,
    RF_ColorWriteGreen = 1 << 5,
    RF_ColorWriteBlue = 1 << 6,
    RF_ColorWriteAlpha = 1 << 7,
    RF_FrontFaceClockwise = 1 << 8,
    RF_ScissorTest = 1 << 9,
    RF_Viewport = 1 << 10,
    RF_CullFaceShift = 11,
    RF_CullFaceMask = (1 << RF_CullFaceShift) | (1 << (RF_CullFaceShift + 1)),
    RF_ColorBlendModeShift = RF_CullFaceShift + 2,
    RF_ColorBlendModeMask = (1 << RF_ColorBlendModeShift) | (1 << (RF_ColorBlendModeShift + 1)) |
                            (1 << (RF_ColorBlendModeShift + 2)),
    RF_AlphaBlendModeShift = RF_CullFaceShift + 5,
    RF_AlphaBlendModeMask = (1 << RF_AlphaBlendModeShift) | (1 << (RF_AlphaBlendModeShift + 1)) |
                            (1 << (RF_AlphaBlendModeShift + 2)),
    RF_ColorSourceBlendFuncShift = RF_CullFaceShift + 8,
    RF_ColorSourceBlendFuncMask =
        (1 << RF_ColorSourceBlendFuncShift) | (1 << (RF_ColorSourceBlendFuncShift + 1)) |
        (1 << (RF_ColorSourceBlendFuncShift + 2)) | (1 << (RF_ColorSourceBlendFuncShift + 3)),
    RF_ColorDestBlendFuncShift = RF_CullFaceShift + 12,
    RF_ColorDestBlendFuncMask =
        (1 << RF_ColorDestBlendFuncShift) | (1 << (RF_ColorDestBlendFuncShift + 1)) |
        (1 << (RF_ColorDestBlendFuncShift + 2)) | (1 << (RF_ColorDestBlendFuncShift + 3)),
    RF_AlphaSourceBlendFuncShift = RF_CullFaceShift + 16,
    RF_AlphaSourceBlendFuncMask =
        (1 << RF_AlphaSourceBlendFuncShift) | (1 << (RF_AlphaSourceBlendFuncShift + 1)) |
        (1 << (RF_AlphaSourceBlendFuncShift + 2)) | (1 << (RF_AlphaSourceBlendFuncShift + 3)),
    RF_AlphaDestBlendFuncShift = RF_CullFaceShift + 20,
    RF_AlphaDestBlendFuncMask = ((UInt64)1 << RF_AlphaDestBlendFuncShift) |
                                ((UInt64)1 << (RF_AlphaDestBlendFuncShift + 1)) |
                                ((UInt64)1 << (RF_AlphaDestBlendFuncShift + 2)) |
                                ((UInt64)1 << (RF_AlphaDestBlendFuncShift + 3)),
    RF_DepthFuncShift = RF_CullFaceShift + 24,
    RF_DepthFuncMask = ((UInt64)1 << RF_DepthFuncShift) | ((UInt64)1 << (RF_DepthFuncShift + 1)) |
                       ((UInt64)1 << (RF_DepthFuncShift + 2)) |
                       ((UInt64)1 << (RF_DepthFuncShift + 3))
};

//@TODO: Complete this list with array types etc.
enum class STICK_API GLUniformType
{
    None,
    Float32,
    Int32,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4
};

struct STICK_API GLBlockedUniform
{
    String name;
    GLuint byteOffset;
    GLUniformType type;
};

struct STICK_API GLUniformBlock
{
    String name;
    StaticArray<GLBlockedUniform, 16> uniforms;
    UInt32 bindingPoint;
    GLuint byteCount;
};
using GLUniformBlockArray = stick::DynamicArray<GLUniformBlock>;

struct STICK_API GLUniformBlockStorage
{
    // points to the region in global UBO memory
    // that stored the most recent version of it.
    // this is updated by each draw call.
    UInt32 lastByteOffset;
    DynamicArray<char> data;
};
using GLUniformBlockStorageArray = stick::DynamicArray<GLUniformBlockStorage>;

struct STICK_LOCAL GLUBOBinding
{
    UInt32 bindingPoint;
    UInt32 byteOffset;
    UInt32 byteCount;
};
using GLUBOBindingArray = stick::DynamicArray<GLUBOBinding>;

struct STICK_API GLTextureBinding
{
    String name;
    GLuint location; // uniform location of the sampler
};
using GLTextureBindingArray = stick::DynamicArray<GLTextureBinding>;

class STICK_API GLProgram : public Program
{
    friend class GLRenderDevice;

  public:
    GLProgram(GLuint _prog);
    ~GLProgram() override;

    GLuint m_glProgram;
    GLUniformBlockArray m_uniformBlocks;
    // the textures that the program requires/uses
    GLTextureBindingArray m_textures;
};

class GLPipeline;

class STICK_API GLPipelineVariable : public PipelineVariable
{
    friend class GLRenderDevice;

  public:
    GLPipelineVariable(GLPipeline * _pipe, UInt32 _blockIndex, UInt32 _uniformIndex);

    ~GLPipelineVariable() override;

    void setFloat32(Float32 _value) override;
    void setInt32(Int32 _value) override;
    void setVec2f(Float32 _x, Float32 _y) override;
    void setVec3f(Float32 _x, Float32 _y, Float32 _z) override;
    void setVec4f(Float32 _x, Float32 _y, Float32 _z, Float32 _w) override;
    void setMat4f(Float32 * _mat) override;

    void setHelper(const void * _data, Size _byteCount, GLUniformType _type);

    GLPipeline * m_pipeline;
    UInt32 m_blockIndex;
    UInt32 m_uniformIndex;
};

class GLTexture;
class GLSampler;
class STICK_API GLPipelineTexture : public PipelineTexture
{
    friend class GLRenderDevice;

  public:
    GLPipelineTexture(GLPipeline * _pipe);
    void set(const Texture * _tex, const Sampler * _sampler) override;

    GLPipeline * m_pipeline;
    const GLTexture * m_texture;
    const GLSampler * m_sampler;
};

//@NOTE: For implementation simplicity we heap allocate each pipeline variable/texture for now...
using GLPipelineVariableArray = stick::DynamicArray<UniquePtr<GLPipelineVariable>>;
using GLPipelineTextureArray = stick::DynamicArray<UniquePtr<GLPipelineTexture>>;

class STICK_API GLPipeline : public Pipeline
{
    friend class GLRenderDevice;

  public:
    GLPipeline(Allocator & _alloc);
    ~GLPipeline() override;
    PipelineVariable * variable(const char * _name) override;
    PipelineTexture * texture(const char * _name) override;

    GLProgram * m_program;
    // bitmask of opengl render state flags for this pipeline
    UInt64 m_renderState;
    Rect m_scissorRect;
    Rect m_viewportRect;
    bool m_bChangedSinceLastDrawCall;
    GLPipelineVariableArray m_variables;
    GLPipelineTextureArray m_textures;
    GLUniformBlockStorageArray m_uniformBlockStorage;
};

class STICK_API GLVertexBuffer : public VertexBuffer
{
    friend class GLRenderDevice;

  public:
    GLVertexBuffer(GLuint _vb, BufferUsageFlags _flags);

    ~GLVertexBuffer() override;

    void loadDataRaw(const void * _data, Size _byteCount) override;

    GLuint m_glVertexBuffer;
    BufferUsageFlags m_usageFlags;
};

class STICK_API GLIndexBuffer : public IndexBuffer
{
    friend class GLRenderDevice;

  public:
    GLIndexBuffer(GLuint _indexBuffer);

    ~GLIndexBuffer() override;

    void loadDataRaw(const void * _data, Size _byteCount) override;

    GLuint m_glIndexBuffer;
};

class STICK_API GLMesh : public Mesh
{
  public:
    friend class GLRenderDevice;

    GLMesh(Allocator & _alloc, GLuint _vao);

    GLuint m_glVao;
    DynamicArray<GLVertexBuffer *> m_vertexBuffers;
    GLIndexBuffer * m_indexBuffer;
};

class STICK_API GLTexture : public Texture
{
  public:
    GLTexture(GLuint _glTex);

    void loadPixels(UInt32 _width,
                    UInt32 _height,
                    UInt32 _depth,
                    const void * _data,
                    DataType _dataType,
                    TextureFormat _format,
                    UInt32 _alignment,
                    UInt32 _mipmapLevelCount) override;

    GLuint m_glTexture;
    GLenum m_glTarget;
    TextureFormat m_format;
};

class STICK_API GLSampler : public Sampler
{
  public:
    GLSampler(GLuint _glSampler);

    GLuint m_glSampler;
};

class STICK_API GLRenderBuffer : public RenderBuffer
{
  public:
    GLRenderBuffer(Allocator & _alloc,
                   UInt32 _width,
                   UInt32 _height,
                   UInt32 _sampleCount,
                   GLuint _fbo,
                   GLuint _msaaFbo);

    const DynamicArray<Texture *> colorTargets() const override;
    Texture * depthStencilTarget() const override;
    UInt32 width() const override;
    UInt32 height() const override;
    UInt32 sampleCount() const override;
    void finalizeForReading();

    struct RenderTarget
    {
        GLTexture * texture;
        GLuint msaaRenderBuffer;
        GLenum attachmentPoint;
        bool bIsDepthTarget;
    };

    UInt32 m_width;
    UInt32 m_height;
    UInt32 m_sampleCount;
    GLuint m_glFBO;
    GLuint m_glMSAAFBO;
    DynamicArray<RenderTarget> m_renderTargets;
    DynamicArray<GLuint> m_colorAttachmentPoints;
    DynamicArray<Texture *> m_colorTargets;
    Texture * m_depthStencilTarget;
    bool m_bDirty;
};

struct STICK_API GLDrawCmd
{
    const GLMesh * mesh;
    const GLPipeline * pipeline;
    UInt32 vertexOffset;
    UInt32 vertexCount;
    VertexDrawMode drawMode;
    GLUBOBindingArray uboBindings;
};

struct STICK_API GLExternalDrawCmd
{
    std::function<Error()> fn;
};

struct STICK_API GLViewportCmd
{
    Rect rect;
};

struct STICK_API GLClearCmd
{
    ClearSettings settings;
};

class GLRenderPass;

class STICK_API GLRenderDevice : public RenderDevice
{
  public:
    GLRenderDevice(Allocator & _alloc);

    ~GLRenderDevice() override;

    Result<Program *> createProgram(const char * _vertexShader, const char * _pixelShader) override;
    void destroyProgram(Program * _prog) override;
    Result<Pipeline *> createPipeline(const PipelineSettings & s) override;
    void destroyPipeline(Pipeline * _pipe) override;
    Result<VertexBuffer *> createVertexBuffer(BufferUsageFlags _usage) override;
    void destroyVertexBuffer(VertexBuffer * _buff) override;
    Result<IndexBuffer *> createIndexBuffer(BufferUsageFlags _usage) override;
    void destroyIndexBuffer(IndexBuffer * _buff) override;
    Result<Mesh *> createMesh(VertexBuffer ** _vertexBuffers,
                              const VertexLayout * _layouts,
                              Size _count,
                              IndexBuffer * _indexBuffer = nullptr) override;
    void destroyMesh(Mesh * _mesh) override;

    Result<Texture *> createTexture() override;
    void destroyTexture(Texture * _texture) override;
    Result<Sampler *> createSampler(const SamplerSettings & _settings) override;
    void destroySampler(Sampler * _sampler) override;

    Result<RenderBuffer *> createRenderBuffer(const RenderBufferSettings & _settings) override;
    void destroyRenderBuffer(RenderBuffer * _renderBuffer) override;

    RenderPass * beginPass(const RenderPassSettings & _settings) override;
    void endPass(RenderPass * _pass) override;

    void beginFrame() override;
    stick::Error endFrame() override;

    UInt32 copyToUBO(Size _byteCount, const void * _data);

    // helper to remove from the storage arrays (i.e. m_programs, m_pipelines etc., see below)
    template <class T, class B>
    void removeItem(T & _array, B * _item)
    {
        if (!_item)
            return;

        auto it = std::find_if(_array.begin(), _array.end(), [_item](const UniquePtr<B> & _a) {
            return _a.get() == _item;
        });
        STICK_ASSERT(it != _array.end());
        _array.remove(it);
    }

    Allocator * m_alloc;
    DynamicArray<UniquePtr<GLProgram>> m_programs;
    DynamicArray<UniquePtr<GLPipeline>> m_pipelines;
    DynamicArray<UniquePtr<GLVertexBuffer>> m_vertexBuffers;
    DynamicArray<UniquePtr<GLIndexBuffer>> m_indexBuffers;
    DynamicArray<UniquePtr<GLMesh>> m_meshes;
    DynamicArray<UniquePtr<GLTexture>> m_textures;
    DynamicArray<UniquePtr<GLSampler>> m_samplers;
    DynamicArray<UniquePtr<GLRenderBuffer>> m_renderBuffers;
    DynamicArray<UniquePtr<GLRenderPass>> m_renderPasses; // all allocated render passes
    DynamicArray<GLRenderPass *> m_renderPassFreeList;    // unused render passes
    DynamicArray<GLRenderPass *> m_currentRenderPasses; // renderpasses queued for the current frame
    bool m_bInFrame;
    Maybe<GLDrawCmd> m_lastDrawCall;
    GLuint m_ubo;
    UInt8 * m_mappedUBO;
    UInt32 m_mappedUBOOffset;
    UInt32 m_uboOffsetAlignment;
};

using GLCmd = stick::Variant<GLDrawCmd, GLExternalDrawCmd, GLViewportCmd, GLClearCmd>;
using GLCmdBuffer = stick::DynamicArray<GLCmd>;

class STICK_API GLRenderPass : public RenderPass
{
  public:
    GLRenderPass(GLRenderDevice * _device, Allocator & _alloc);
    void drawMesh(const Mesh * _mesh,
                  const Pipeline * _pipeline,
                  UInt32 _vertexOffset,
                  UInt32 _vertexCount,
                  VertexDrawMode _drawMode) override;
    void setViewport(Float32 _x, Float32 _y, Float32 _w, Float32 _h) override;
    void clearBuffers(const ClearSettings & _settings) override;
    void reset();

    GLRenderDevice * m_device;
    RenderBuffer * m_renderBuffer;
    GLCmdBuffer m_commands;
};

} // namespace gl
} // namespace dab

#endif // DAB_OPENGL_GLDAB_HPP
