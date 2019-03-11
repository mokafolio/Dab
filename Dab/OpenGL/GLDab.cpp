#include <Dab/OpenGL/GLDab.hpp>

#ifdef STICK_DEBUG
#define ASSERT_NO_GL_ERROR(_func)                                                                  \
    do                                                                                             \
    {                                                                                              \
        _func;                                                                                     \
        GLenum err = glGetError();                                                                 \
        if (err != GL_NO_ERROR)                                                                    \
        {                                                                                          \
            switch (err)                                                                           \
            {                                                                                      \
            case GL_NO_ERROR:                                                                      \
                printf(                                                                            \
                    "%s line %i GL_NO_ERROR: No error has been recorded.\n", __FILE__, __LINE__);  \
                break;                                                                             \
            case GL_INVALID_ENUM:                                                                  \
                printf(                                                                            \
                    "%s line %i GL_INVALID_ENUM: An unacceptable value is specified for an "       \
                    "enumerated argument. The offending command is ignored and has no other side " \
                    "effect than to set the error flag.\n",                                        \
                    __FILE__,                                                                      \
                    __LINE__);                                                                     \
                break;                                                                             \
            case GL_INVALID_VALUE:                                                                 \
                printf(                                                                            \
                    "%s line %i GL_INVALID_VALUE: A numeric argument is out of range. The "        \
                    "offending command is ignored and has no other side effect than to set the "   \
                    "error flag.\n",                                                               \
                    __FILE__,                                                                      \
                    __LINE__);                                                                     \
                break;                                                                             \
            case GL_INVALID_OPERATION:                                                             \
                printf(                                                                            \
                    "%s line %i GL_INVALID_OPERATION: The specified operation is not allowed in "  \
                    "the current state. The offending command is ignored and has no other side "   \
                    "effect than to set the error flag.\n",                                        \
                    __FILE__,                                                                      \
                    __LINE__);                                                                     \
                break;                                                                             \
            case GL_INVALID_FRAMEBUFFER_OPERATION:                                                 \
                printf(                                                                            \
                    "%s line %i GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not "  \
                    "complete. The offending command is ignored and has no other side effect "     \
                    "than to set the error flag.\n",                                               \
                    __FILE__,                                                                      \
                    __LINE__);                                                                     \
                break;                                                                             \
            case GL_OUT_OF_MEMORY:                                                                 \
                printf(                                                                            \
                    "%s line %i GL_OUT_OF_MEMORY: There is not enough memory left to executeLua "  \
                    "the command. The state of the GL is undefined, except for the state of the "  \
                    "error flags, after this error is recorded.\n",                                \
                    __FILE__,                                                                      \
                    __LINE__);                                                                     \
                break;                                                                             \
            }                                                                                      \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (false)
#else
#define ASSERT_NO_GL_ERROR(_func) _func
#endif

#define UNIFORM_BUFFER_SIZE 64 * 1024
#define BUFFER_OFFSET(_off) (char *)(0 + _off)

namespace dab
{
namespace gl
{
using namespace stick;

struct GLTextureFormat
{
    GLenum glFormat;
    GLenum glInternalFormat;
    GLenum glDataType; // the corresponding, native gl data type
};

static GLTextureFormat s_glTextureFormats[] = {
    // R8
    { GL_RED, GL_R8, GL_UNSIGNED_BYTE },
    // R16
    { GL_RED, GL_R16, GL_UNSIGNED_SHORT },
    // R32
    { GL_RED, GL_R32UI, GL_UNSIGNED_INT },
    // R16F
    { GL_RED, GL_R16F, GL_FLOAT },
    // R32F
    { GL_RED, GL_R32F, GL_FLOAT },
    // RGB8
    { GL_RGB, GL_RGB8, GL_UNSIGNED_BYTE },
    // RGB16
    { GL_RGB, GL_RGB16, GL_UNSIGNED_SHORT },
    // RGB32
    { GL_RGB, GL_RGB32UI, GL_UNSIGNED_INT },
    // RGB16F
    { GL_RGB, GL_RGB16F, GL_FLOAT },
    // RGB32F
    { GL_RGB, GL_RGB32F, GL_FLOAT },
    // BGR8
    { GL_BGR, GL_RGB8, GL_UNSIGNED_BYTE },
    // BGR16
    { GL_BGR, GL_RGB16, GL_UNSIGNED_SHORT },
    // BGR32
    { GL_BGR, GL_RGB32UI, GL_UNSIGNED_INT },
    // BGR16F
    { GL_BGR, GL_RGB16F, GL_FLOAT },
    // BGR32F
    { GL_BGR, GL_RGB32F, GL_FLOAT },
    // RGBA8
    { GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE },
    // RGBA16
    { GL_RGBA, GL_RGBA16, GL_UNSIGNED_SHORT },
    // RGBA32
    { GL_RGBA, GL_RGBA32UI, GL_UNSIGNED_INT },
    // RGBA16F
    { GL_RGBA, GL_RGBA16F, GL_FLOAT },
    // RGBA32F
    { GL_RGBA, GL_RGBA32F, GL_FLOAT },
    // BGRA8
    { GL_BGRA, GL_RGBA8, GL_UNSIGNED_BYTE },
    // BGRA16
    { GL_BGRA, GL_RGBA16, GL_UNSIGNED_SHORT },
    // BGRA32
    { GL_BGRA, GL_RGBA32UI, GL_UNSIGNED_INT },
    // BGRA16F
    { GL_BGRA, GL_RGBA16F, GL_FLOAT },
    // BGRA32F
    { GL_BGRA, GL_RGBA32F, GL_FLOAT },
    // Depth16
    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_SHORT },
    // Depth24
    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT },
    // Depth32
    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_UNSIGNED_INT },
    // Depth24Stencil8
    { GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8 },
    // Depth32F
    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT },
    // Depth32FStencil8
    { GL_DEPTH_STENCIL, GL_DEPTH32F_STENCIL8, GL_FLOAT_32_UNSIGNED_INT_24_8_REV }
};

static_assert((Size)TextureFormat::Count ==
                  sizeof(s_glTextureFormats) / sizeof(s_glTextureFormats[0]),
              "TextureFormat mapping is not complete!");

static GLenum s_glDataTypes[] = {
    // UInt8
    GL_UNSIGNED_BYTE,
    // Int8
    GL_BYTE,
    // UInt16
    GL_UNSIGNED_SHORT,
    // Int16
    GL_SHORT,
    // UInt32
    GL_UNSIGNED_INT,
    // Int32
    GL_INT,
    // Float32
    GL_FLOAT,
    // Float64
    GL_DOUBLE
};
static_assert((Size)DataType::Count == sizeof(s_glDataTypes) / sizeof(s_glDataTypes[0]),
              "BufferDataType mapping is not complete!");

static GLenum s_glVertexDrawModes[] = {
    // Triangles
    GL_TRIANGLES,
    // TriangleStrip
    GL_TRIANGLE_STRIP,
    // TriangleFan
    GL_TRIANGLE_FAN,
    // Points
    GL_POINTS,
    // Lines
    GL_LINES,
    // LineStrip
    GL_LINE_STRIP,
    // LineLoop
    GL_LINE_LOOP
};
static_assert((Size)VertexDrawMode::Count ==
                  sizeof(s_glVertexDrawModes) / sizeof(s_glVertexDrawModes[0]),
              "VertexDrawMode mapping is not complete!");

static GLenum s_glCompareFuncs[] = {
    // Equal
    GL_EQUAL,
    // LessEqual
    GL_LEQUAL,
    // GreaterEqual
    GL_GEQUAL,
    // NotEqual
    GL_NOTEQUAL,
    // Always
    GL_ALWAYS,
    // Never
    GL_NEVER,
    // Less
    GL_LESS,
    // Greater
    GL_GREATER
};

static_assert((Size)CompareFunction::Count ==
                  sizeof(s_glCompareFuncs) / sizeof(s_glCompareFuncs[0]),
              "CompareFunction mapping is not complete!");

static GLenum s_glBlendModes[] = {
    // Add
    GL_FUNC_ADD,
    // Substract
    GL_FUNC_SUBTRACT,
    // ReverseSubstract
    GL_FUNC_REVERSE_SUBTRACT,
    // Min
    GL_MIN,
    // Max
    GL_MAX
};

static_assert((Size)BlendMode::Count == sizeof(s_glBlendModes) / sizeof(s_glBlendModes[0]),
              "BlendMode mapping is not complete!");

static GLenum s_glBlendFuncs[] = {
    // Zero
    GL_ZERO,
    // One
    GL_ONE,
    // SourceColor
    GL_SRC_COLOR,
    // InverseSourceColor
    GL_ONE_MINUS_SRC_COLOR,
    // DestinationColor
    GL_DST_COLOR,
    // InverseDestinationColor
    GL_ONE_MINUS_DST_COLOR,
    // SourceAlpha
    GL_SRC_ALPHA,
    // InverseSourceAlpha
    GL_ONE_MINUS_SRC_ALPHA,
    // DestinationAlpha
    GL_DST_ALPHA,
    // InverseDestinationAlpha
    GL_ONE_MINUS_DST_ALPHA,
    // ConstantColor
    GL_CONSTANT_COLOR,
    // InverseConstantColor
    GL_ONE_MINUS_CONSTANT_COLOR,
    // ConstantAlpha
    GL_CONSTANT_ALPHA,
    // InverseConstantAlpha
    GL_ONE_MINUS_CONSTANT_ALPHA
};

static_assert((Size)BlendFunction::Count == sizeof(s_glBlendFuncs) / sizeof(s_glBlendFuncs[0]),
              "BlendFunction mapping is not complete!");

static GLenum s_glFaceType[] = {
    // None
    GL_NONE, // this should never happen
             // Front
    GL_FRONT,
    // Back
    GL_BACK,
    // FrontAndBack
    GL_FRONT_AND_BACK
};

static_assert((Size)FaceType::Count == sizeof(s_glFaceType) / sizeof(s_glFaceType[0]),
              "FaceType mapping is not complete!");

static GLenum s_glWrap[] = {
    // Repeat
    GL_REPEAT,
    // ClampToEdge
    GL_CLAMP_TO_EDGE,
    // ClampToBorder
    GL_CLAMP_TO_BORDER
};

static_assert((Size)TextureWrap::Count == sizeof(s_glWrap) / sizeof(s_glWrap[0]),
              "TextureWrap mapping is not complete!");

GLRenderDevice::GLRenderDevice(Allocator & _alloc) :
    m_alloc(&_alloc),
    m_programs(_alloc),
    m_pipelines(_alloc),
    m_vertexBuffers(_alloc),
    m_indexBuffers(_alloc),
    m_meshes(_alloc),
    m_textures(_alloc),
    m_samplers(_alloc),
    m_renderBuffers(_alloc),
    m_renderPasses(_alloc),
    m_renderPassFreeList(_alloc),
    m_currentRenderPasses(_alloc),
    m_bInFrame(false),
    m_mappedUBO(nullptr)
{
    STICK_ASSERT(!gl3wInit());
    ASSERT_NO_GL_ERROR(glGenBuffers(1, &m_ubo));
    ASSERT_NO_GL_ERROR(
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint *)&m_uboOffsetAlignment));
}

GLRenderDevice::~GLRenderDevice()
{
    glDeleteBuffers(1, &m_ubo);
    // all the other resources clean up after themselves in their respective destructors.
}

static Error compileShader(const char * _shaderCode, GLenum _shaderType, GLuint & _outHandle)
{
    Error ret;
    GLenum glHandle = glCreateShader(_shaderType);
    GLint len = strlen(_shaderCode);
    ASSERT_NO_GL_ERROR(glShaderSource(glHandle, 1, &_shaderCode, &len));
    ASSERT_NO_GL_ERROR(glCompileShader(glHandle));

    // check if the shader compiled
    GLint state;
    ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_COMPILE_STATUS, &state));
    if (state == GL_FALSE)
    {
        GLint infologLength;
        ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_INFO_LOG_LENGTH, &infologLength));

        char * str = (char *)malloc(infologLength);
        ASSERT_NO_GL_ERROR(glGetShaderInfoLog(glHandle, infologLength, &infologLength, str));

        ret = Error(ec::InvalidOperation,
                    String::concat("Could not compile GLSL shader: ", str),
                    STICK_FILE,
                    STICK_LINE);
        glDeleteShader(glHandle);
        free(str);
    }
    else
    {
        _outHandle = glHandle;
    }
    return ret;
}

Result<Program *> GLRenderDevice::createProgram(const char * _vertexShader,
                                                const char * _pixelShader)
{
    auto ret = stick::makeUnique<GLProgram>(*m_alloc);
    auto err = ret->init(*m_alloc, _vertexShader, _pixelShader);
    if (err)
        return err;
    m_programs.append(std::move(ret));
    return m_programs.last().get();
}

void GLRenderDevice::destroyProgram(Program * _prog)
{
    removeItem(m_programs, static_cast<GLProgram *>(_prog));
}

// helpers to create the pipeline bitmask
static void setFlag(UInt64 & _bitMask, RenderFlag _flag, bool _b)
{
    if (_b)
        _bitMask |= _flag;
    else
        _bitMask &= ~_flag;
}

static bool isFlagSet(UInt64 _bitMask, RenderFlag _flag)
{
    return (_bitMask & _flag) == _flag;
}

template <class T>
static void setField(UInt64 & _bitMask, RenderFlag _fieldShift, RenderFlag _fieldMask, T _value)
{
    _bitMask &= ~_fieldMask;
    _bitMask |= (static_cast<UInt64>(_value) << _fieldShift) & _fieldMask;
}

template <class T>
static T field(UInt64 _bitMask, RenderFlag _fieldShift, RenderFlag _fieldMask)
{
    return static_cast<T>((_bitMask & _fieldMask) >> _fieldShift);
}

static UInt64 differenceMask(UInt64 _a, UInt64 _b)
{
    return _a ^ _b;
}

static bool isFlagDifferent(UInt64 _diffMask, RenderFlag _flag)
{
    return (_diffMask & _flag) != 0;
}

static bool isFieldDifferent(UInt64 _diffMask, UInt64 _fieldMask)
{
    return (_diffMask & _fieldMask) != 0;
}

Result<Pipeline *> GLRenderDevice::createPipeline(const PipelineSettings & _settings)
{
    m_pipelines.append(stick::makeUnique<GLPipeline>(*m_alloc, *m_alloc, _settings));
    return m_pipelines.last().get();
}

void GLRenderDevice::destroyPipeline(Pipeline * _pipe)
{
    removeItem(m_pipelines, static_cast<GLPipeline *>(_pipe));
}

Result<VertexBuffer *> GLRenderDevice::createVertexBuffer(BufferUsageFlags _usage)
{
    m_vertexBuffers.append(stick::makeUnique<GLVertexBuffer>(*m_alloc, _usage));
    return m_vertexBuffers.last().get();
}

void GLRenderDevice::destroyVertexBuffer(VertexBuffer * _buff)
{
    removeItem(m_vertexBuffers, static_cast<GLVertexBuffer *>(_buff));
}

Result<IndexBuffer *> GLRenderDevice::createIndexBuffer(BufferUsageFlags _usage)
{
    m_indexBuffers.append(stick::makeUnique<GLIndexBuffer>(*m_alloc, _usage));
    return m_indexBuffers.last().get();
}

void GLRenderDevice::destroyIndexBuffer(IndexBuffer * _buff)
{
    removeItem(m_indexBuffers, static_cast<GLIndexBuffer *>(_buff));
}

Result<Mesh *> GLRenderDevice::createMesh(VertexBuffer ** _vertexBuffers,
                                          const VertexLayout * _layouts,
                                          Size _count,
                                          IndexBuffer * _indexBuffer)
{
    m_meshes.append(stick::makeUnique<GLMesh>(
        *m_alloc, *m_alloc, _vertexBuffers, _layouts, _count, _indexBuffer));
    return m_meshes.last().get();
}

void GLRenderDevice::destroyMesh(Mesh * _mesh)
{
    removeItem(m_meshes, static_cast<GLMesh *>(_mesh));
}

Result<Texture *> GLRenderDevice::createTexture()
{
    m_textures.append(makeUnique<GLTexture>(*m_alloc));
    return m_textures.last().get();
}

void GLRenderDevice::destroyTexture(Texture * _texture)
{
    /* @TODO: this is not a very scaleable solution if there are many textures and
     * pipelines...Possibly come up with something better. The goal is to make sure that a texture
     * that get's destroyed can't be referenced by any pipeline. A solution could be that every
     * texture keeps track where it's bound.*/
    for (auto & pipe : m_pipelines)
    {
        for (auto & tex : pipe->m_textures)
        {
            if (tex->m_texture == _texture)
                tex->m_texture = nullptr;
        }
    }

    //@TODO: Should we remove the texture from its renderbuffer or simply say that's undefined
    // behavior for now?
    GLTexture * gltex = static_cast<GLTexture *>(_texture);
    STICK_ASSERT(gltex->m_renderBuffer == nullptr);
    removeItem(m_textures, gltex);
}

Result<Sampler *> GLRenderDevice::createSampler(const SamplerSettings & _settings)
{
    m_samplers.append(makeUnique<GLSampler>(*m_alloc, _settings));
    return m_samplers.last().get();
}

void GLRenderDevice::destroySampler(Sampler * _sampler)
{
    //@TODO: see comment in destroyTexture
    for (auto & pipe : m_pipelines)
    {
        for (auto & tex : pipe->m_textures)
        {
            if (tex->m_sampler == _sampler)
                tex->m_sampler = nullptr;
        }
    }
    removeItem(m_samplers, static_cast<GLSampler *>(_sampler));
}

static Error validateFrameBuffer()
{
    GLenum err;
    err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //@TODO: Better error codes
    switch (err)
    {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return Error(ec::InvalidOperation,
                     "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: Invalid OpenGL FBO attachment!",
                     STICK_FILE,
                     STICK_LINE);
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return Error(ec::InvalidOperation,
                     "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: OpenGL FBO has no attachments!",
                     STICK_FILE,
                     STICK_LINE);
    case GL_FRAMEBUFFER_UNSUPPORTED:
        return Error(ec::InvalidOperation,
                     "GL_FRAMEBUFFER_UNSUPPORTED: The OpenGL format combination is not supported "
                     "by FBOs on your platform!",
                     STICK_FILE,
                     STICK_LINE);
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return Error(
            ec::InvalidOperation, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER!", STICK_FILE, STICK_LINE);
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return Error(
            ec::InvalidOperation, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER!", STICK_FILE, STICK_LINE);
    }

    return Error();
}

Result<RenderBuffer *> GLRenderDevice::createRenderBuffer(const RenderBufferSettings & _settings)
{
    auto rb = makeUnique<GLRenderBuffer>(*m_alloc, this);
    auto err = rb->init(_settings);
    if (err)
        return err;

    m_renderBuffers.append(std::move(rb));
    return m_renderBuffers.last().get();
}

void GLRenderDevice::destroyRenderBuffer(RenderBuffer * _rb, bool _bDestroyRenderTargets)
{
    GLRenderBuffer * glrb = static_cast<GLRenderBuffer *>(_rb);
    glrb->deallocate(_bDestroyRenderTargets);
    removeItem(m_renderBuffers, glrb);
}

RenderPass * GLRenderDevice::beginPass(const RenderPassSettings & _settings)
{
    STICK_ASSERT(m_bInFrame);
    GLRenderPass * ret;

    if (m_renderPassFreeList.count())
    {
        ret = m_renderPassFreeList.last();
        ret->reset();
        m_renderPassFreeList.removeLast();
    }
    else
    {
        m_renderPasses.append(makeUnique<GLRenderPass>(*m_alloc, this, *m_alloc));
        ret = m_renderPasses.last().get();
    }
    ret->m_renderBuffer = static_cast<GLRenderBuffer *>(_settings.renderBuffer);
    if (_settings.clear)
        ret->clearBuffers(*_settings.clear);
    return ret;
}

void GLRenderDevice::endPass(RenderPass * _pass)
{
    m_currentRenderPasses.append(static_cast<GLRenderPass *>(_pass));
}

void GLRenderDevice::beginFrame()
{
    STICK_ASSERT(m_currentRenderPasses.count() == 0 && !m_bInFrame);
    m_bInFrame = true;

    ASSERT_NO_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, m_ubo));
    ASSERT_NO_GL_ERROR(glBufferData(GL_UNIFORM_BUFFER, UNIFORM_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW));
    m_mappedUBO = (UInt8 *)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    m_mappedUBOOffset = 0;
    STICK_ASSERT(m_mappedUBO);
}

static void clearBuffers(const ClearSettings & _clear)
{
    GLuint clearMask = 0;
    if (_clear.color)
    {
        auto & cc = *_clear.color;
        ASSERT_NO_GL_ERROR(glClearColor(cc.r, cc.g, cc.b, cc.a));
        clearMask |= GL_COLOR_BUFFER_BIT;
    }
    if (_clear.depth)
    {
        ASSERT_NO_GL_ERROR(glClearDepth(*_clear.depth));
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }
    if (_clear.stencil)
    {
        ASSERT_NO_GL_ERROR(glClearStencil(*_clear.stencil));
        clearMask |= GL_STENCIL_BUFFER_BIT;
    }
    if (clearMask != 0)
        ASSERT_NO_GL_ERROR(glClear(clearMask));
}

static void bindRenderBufferImpl(GLRenderBuffer * _rb, bool _bMarkDirty)
{
    if (_rb)
    {
        if (_rb->m_glMSAAFBO)
            ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, _rb->m_glMSAAFBO));
        else
            ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, _rb->m_glFBO));

        _rb->m_bDirty = _bMarkDirty;
        ASSERT_NO_GL_ERROR(glDrawBuffers((GLuint)_rb->m_colorAttachmentPoints.count(),
                                         &_rb->m_colorAttachmentPoints[0]));
    }
    else
        ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Error GLRenderDevice::endFrame()
{
    Error err;
    STICK_ASSERT(m_bInFrame);

    //@TODO: can we remove this extra binding by making we always return to the dab expected gl
    // state i.e. after custom draw hooks?
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, m_ubo));
    ASSERT_NO_GL_ERROR(glUnmapBuffer(GL_UNIFORM_BUFFER));
    m_mappedUBO = nullptr;
    m_lastDrawCall.reset();

    for (auto * pass : m_currentRenderPasses)
    {
        bindRenderBufferImpl(pass->m_renderBuffer, true);
        for (auto & cmd : pass->m_commands)
        {
            if (auto mcc = cmd.maybe<GLClearCmd>())
            {
                clearBuffers((*mcc).settings);
            }
            else if (auto mvc = cmd.maybe<GLViewportCmd>())
            {
                ASSERT_NO_GL_ERROR(glViewport(
                    (*mvc).rect.x, (*mvc).rect.y, (*mvc).rect.width, (*mvc).rect.height));
            }
            else if (auto mdc = cmd.maybe<GLDrawCmd>())
            {
                const GLPipeline * pipeline = (*mdc).pipeline;
                const GLProgram * program = pipeline->m_program;
                const GLMesh * mesh = (*mdc).mesh;

                if (!m_lastDrawCall || (*m_lastDrawCall).pipeline->m_program != program)
                    ASSERT_NO_GL_ERROR(glUseProgram(program->m_glProgram));

                UInt64 diffMask = m_lastDrawCall
                                      ? differenceMask((*m_lastDrawCall).pipeline->m_renderState,
                                                       pipeline->m_renderState)
                                      : (UInt64)-1;
                if (diffMask != 0)
                {
                    if (isFlagSet(pipeline->m_renderState, RF_Viewport))
                    {
                        ASSERT_NO_GL_ERROR(glViewport(pipeline->m_viewportRect.x,
                                                      pipeline->m_viewportRect.y,
                                                      pipeline->m_viewportRect.width,
                                                      pipeline->m_viewportRect.height));
                    }

                    // Scissor
                    if (isFlagDifferent(diffMask, RF_ScissorTest))
                    {
                        if (isFlagSet(pipeline->m_renderState, RF_ScissorTest))
                        {
                            ASSERT_NO_GL_ERROR(glEnable(GL_SCISSOR_TEST));
                        }
                        else
                        {
                            ASSERT_NO_GL_ERROR(glDisable(GL_SCISSOR_TEST));
                        }
                    }

                    if (isFlagDifferent(diffMask, RF_Blending))
                    {
                        if (isFlagSet(pipeline->m_renderState, RF_Blending))
                        {
                            ASSERT_NO_GL_ERROR(glEnable(GL_BLEND));
                        }
                        else
                        {
                            ASSERT_NO_GL_ERROR(glDisable(GL_BLEND));
                        }
                    }

                    if (isFieldDifferent(diffMask, RF_ColorBlendModeMask) ||
                        isFieldDifferent(diffMask, RF_AlphaBlendModeMask))
                    {
                        ASSERT_NO_GL_ERROR(glBlendEquationSeparate(
                            s_glBlendModes[field<UInt64>(pipeline->m_renderState,
                                                         RF_ColorBlendModeShift,
                                                         RF_ColorBlendModeMask)],
                            s_glBlendModes[field<UInt64>(pipeline->m_renderState,
                                                         RF_AlphaBlendModeShift,
                                                         RF_AlphaBlendModeMask)]));
                    }

                    if (isFieldDifferent(diffMask, RF_ColorSourceBlendFuncMask) ||
                        isFieldDifferent(diffMask, RF_ColorDestBlendFuncMask) ||
                        isFieldDifferent(diffMask, RF_AlphaSourceBlendFuncMask) ||
                        isFieldDifferent(diffMask, RF_AlphaDestBlendFuncMask))
                    {
                        ASSERT_NO_GL_ERROR(glBlendFuncSeparate(
                            s_glBlendFuncs[field<UInt64>(pipeline->m_renderState,
                                                         RF_ColorSourceBlendFuncShift,
                                                         RF_ColorSourceBlendFuncMask)],
                            s_glBlendFuncs[field<UInt64>(pipeline->m_renderState,
                                                         RF_ColorDestBlendFuncShift,
                                                         RF_ColorDestBlendFuncMask)],
                            s_glBlendFuncs[field<UInt64>(pipeline->m_renderState,
                                                         RF_AlphaSourceBlendFuncShift,
                                                         RF_AlphaSourceBlendFuncMask)],
                            s_glBlendFuncs[field<UInt64>(pipeline->m_renderState,
                                                         RF_AlphaDestBlendFuncShift,
                                                         RF_AlphaDestBlendFuncMask)]));
                    }

                    if (isFlagDifferent(diffMask, RF_DepthTest))
                    {
                        if (isFlagSet(pipeline->m_renderState, RF_DepthTest))
                        {
                            ASSERT_NO_GL_ERROR(glEnable(GL_DEPTH_TEST));
                        }
                        else
                        {
                            ASSERT_NO_GL_ERROR(glDisable(GL_DEPTH_TEST));
                        }
                    }

                    if (isFlagDifferent(diffMask, RF_Multisampling))
                    {
                        if (isFlagSet(pipeline->m_renderState, RF_Multisampling))
                        {
                            ASSERT_NO_GL_ERROR(glEnable(GL_MULTISAMPLE));
                        }
                        else
                        {
                            ASSERT_NO_GL_ERROR(glDisable(GL_MULTISAMPLE));
                        }
                    }

                    if (isFlagDifferent(diffMask, RF_DepthWrite))
                    {
                        ASSERT_NO_GL_ERROR(
                            glDepthMask(isFlagSet(pipeline->m_renderState, RF_DepthWrite)));
                    }

                    if (isFieldDifferent(diffMask, RF_DepthFuncMask))
                    {
                        ASSERT_NO_GL_ERROR(glDepthFunc(s_glCompareFuncs[field<UInt64>(
                            pipeline->m_renderState, RF_DepthFuncShift, RF_DepthFuncMask)]));
                    }

                    if (isFlagDifferent(diffMask, RF_ColorWriteRed) ||
                        isFlagDifferent(diffMask, RF_ColorWriteGreen) ||
                        isFlagDifferent(diffMask, RF_ColorWriteBlue) ||
                        isFlagDifferent(diffMask, RF_ColorWriteAlpha))
                    {
                        ASSERT_NO_GL_ERROR(
                            glColorMask(isFlagSet(pipeline->m_renderState, RF_ColorWriteRed),
                                        isFlagSet(pipeline->m_renderState, RF_ColorWriteGreen),
                                        isFlagSet(pipeline->m_renderState, RF_ColorWriteBlue),
                                        isFlagSet(pipeline->m_renderState, RF_ColorWriteAlpha)));
                    }

                    if (isFlagDifferent(diffMask, RF_FrontFaceClockwise))
                    {
                        ASSERT_NO_GL_ERROR(glFrontFace(
                            isFlagSet(pipeline->m_renderState, RF_FrontFaceClockwise) ? GL_CW
                                                                                      : GL_CCW));
                    }

                    if (isFieldDifferent(diffMask, RF_CullFaceMask))
                    {
                        UInt64 cff = field<UInt64>(
                            pipeline->m_renderState, RF_CullFaceShift, RF_CullFaceMask);
                        if (cff != (UInt64)FaceType::None)
                        {
                            ASSERT_NO_GL_ERROR(glEnable(GL_CULL_FACE));
                            ASSERT_NO_GL_ERROR(glCullFace(s_glFaceType[cff]));
                        }
                        else
                        {
                            ASSERT_NO_GL_ERROR(glDisable(GL_CULL_FACE));
                        }
                    }
                }

                // point towards the correct locations in the uniform buffer
                for (UInt32 j = 0; j < (*mdc).uboBindings.count(); ++j)
                {
                    if (!m_lastDrawCall || j >= (*m_lastDrawCall).uboBindings.count() ||
                        (*mdc).uboBindings[j].byteOffset !=
                            (*m_lastDrawCall).uboBindings[j].byteOffset)
                    {
                        ASSERT_NO_GL_ERROR(glBindBufferRange(GL_UNIFORM_BUFFER,
                                                             (*mdc).uboBindings[j].bindingPoint,
                                                             m_ubo,
                                                             (*mdc).uboBindings[j].byteOffset,
                                                             (*mdc).uboBindings[j].byteCount));
                    }
                }

                // bind all necessary textures
                for (Size i = 0; i < pipeline->m_textures.count(); ++i)
                {
                    GLPipelineTexture * tex = pipeline->m_textures[i].get();
                    if (tex->m_texture)
                    {
                        // if this is a render target, make sure its blit in case its attached to a
                        // MSAA fbo
                        if (tex->m_texture->m_renderBuffer)
                            tex->m_texture->m_renderBuffer->finalizeForReading(
                                pass->m_renderBuffer);

                        STICK_ASSERT(tex->m_sampler);
                        ASSERT_NO_GL_ERROR(glBindSampler((GLuint)i, tex->m_sampler->m_glSampler));
                        if (!m_lastDrawCall ||
                            (*m_lastDrawCall).pipeline->m_textures[i].get() != tex)
                        {
                            ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0 + (GLuint)i));
                            ASSERT_NO_GL_ERROR(glBindTexture(tex->m_texture->m_glTarget,
                                                             tex->m_texture->m_glTexture));
                        }
                    }
                }

                // draw the mesh
                ASSERT_NO_GL_ERROR(glBindVertexArray(mesh->m_glVao));
                GLenum glVertexMode = s_glVertexDrawModes[static_cast<Size>((*mdc).drawMode)];

                if (mesh->m_indexBuffer)
                {
                    ASSERT_NO_GL_ERROR(
                        glDrawElements(glVertexMode,
                                       (*mdc).vertexCount,
                                       GL_UNSIGNED_INT,
                                       BUFFER_OFFSET(sizeof(GLuint) * (*mdc).vertexOffset)));
                }
                else
                {
                    ASSERT_NO_GL_ERROR(
                        glDrawArrays(glVertexMode, (*mdc).vertexOffset, (*mdc).vertexCount));
                }

                m_lastDrawCall = *mdc;
            }
            else if (auto mdc = cmd.maybe<GLExternalDrawCmd>())
            {
                err = (*mdc).fn();

                //@TODO: Should we clear the render passes etc. before returning any errors so that
                // there is the possibility of recovery??
                if (err)
                    return err;

                // we reset the last draw call to make sure the render state is fully being enabled
                // for the following draw call as there is no way for us to know hat the external
                // draw command changed regarding the opengl state.
                m_lastDrawCall.reset();
            }
        }

        pass->reset();
        m_renderPassFreeList.append(pass);
    }
    m_currentRenderPasses.clear();
    m_bInFrame = false;

    return err;
}

void GLRenderDevice::readPixels(
    Int32 _x, Int32 _y, Int32 _w, Int32 _h, TextureFormat _format, void * _outData)
{
    GLTextureFormat fmt = s_glTextureFormats[(Size)_format];
    //@TODO: should propably set glReadBuffers here
    ASSERT_NO_GL_ERROR(glReadPixels(_x, _y, _w, _h, fmt.glFormat, fmt.glDataType, _outData));
}

UInt32 GLRenderDevice::copyToUBO(Size _byteCount, const void * _data)
{
    UInt32 ret = m_mappedUBOOffset;
    STICK_ASSERT(m_mappedUBOOffset + _byteCount < UNIFORM_BUFFER_SIZE);
    std::memcpy(m_mappedUBO + m_mappedUBOOffset, _data, _byteCount);
    // advance the offset 16 byte aligned
    m_mappedUBOOffset += _byteCount;
    m_mappedUBOOffset += stick::alignOffset(m_mappedUBO + m_mappedUBOOffset, m_uboOffsetAlignment);
    return ret;
}

GLProgram::GLProgram()
{
}

Error GLProgram::init(Allocator & _alloc, const char * _vertexShader, const char * _pixelShader)
{
    GLuint vertexShader, pixelShader;
    Error err = compileShader(_vertexShader, GL_VERTEX_SHADER, vertexShader);
    if (!err)
        err = compileShader(_pixelShader, GL_FRAGMENT_SHADER, pixelShader);
    if (err)
        return err;

    GLuint program = glCreateProgram();
    ASSERT_NO_GL_ERROR(glAttachShader(program, vertexShader));
    ASSERT_NO_GL_ERROR(glAttachShader(program, pixelShader));
    ASSERT_NO_GL_ERROR(glLinkProgram(program));

    // check if we had success
    GLint state;
    ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_LINK_STATUS, &state));

    if (state == GL_FALSE)
    {
        char str[2048] = { 0 };
        GLint infologLength = 1024;
        ASSERT_NO_GL_ERROR(glGetProgramInfoLog(program, infologLength, &infologLength, str));

        err = Error(ec::InvalidOperation,
                    String::concat("Error linking GLSL program: ", str),
                    STICK_FILE,
                    STICK_LINE);
    }

    ASSERT_NO_GL_ERROR(glDeleteShader(vertexShader));
    ASSERT_NO_GL_ERROR(glDeleteShader(pixelShader));

    if (err)
    {
        glDeleteProgram(program);
        return err;
    }

    GLint numBlocks;
    ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks));

    GLint alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    // GLuint index = glGetUniformBlockIndex(program, "Constants");
    char nameBuffer[128] = { 0 };
    for (int blockIdx = 0; blockIdx < numBlocks; ++blockIdx)
    {
        GLUniformBlock block;
        std::memset(nameBuffer, 0, 128);
        ASSERT_NO_GL_ERROR(glGetActiveUniformBlockName(program, blockIdx, 128, NULL, nameBuffer));
        //@TODO String allocator
        block.name = String(nameBuffer);
        // block.tmpStorage = { 0 };
        block.byteCount = 0;
        // block.bindingPoint = nextUBOBindingPoint();
        block.bindingPoint = blockIdx;
        // block.lastFrameID = 0;

        ASSERT_NO_GL_ERROR(glUniformBlockBinding(program, blockIdx, block.bindingPoint));

        int activeUniformsInBlock;
        ASSERT_NO_GL_ERROR(glGetActiveUniformBlockiv(
            program, blockIdx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniformsInBlock));

        GLint indices[32];
        ASSERT_NO_GL_ERROR(glGetActiveUniformBlockiv(
            program, blockIdx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices));
        std::memset(nameBuffer, 0, 128);
        GLint size;
        GLint type;
        GLint offset;
        GLuint byteCount;
        GLUniformType mt;
        for (UInt32 i = 0; i < activeUniformsInBlock; i++)
        {
            const UInt32 & idx = (UInt32)indices[i];

            ASSERT_NO_GL_ERROR(glGetActiveUniformName(program, idx, 128, 0, nameBuffer));
            ASSERT_NO_GL_ERROR(glGetActiveUniformsiv(program, 1, &idx, GL_UNIFORM_TYPE, &type));
            ASSERT_NO_GL_ERROR(glGetActiveUniformsiv(program, 1, &idx, GL_UNIFORM_OFFSET, &offset));
            ASSERT_NO_GL_ERROR(glGetActiveUniformsiv(program, 1, &idx, GL_UNIFORM_SIZE, &size));

            // we don't support arrays for now.
            STICK_ASSERT(size == 1);

            // determine the type
            if (type == GL_FLOAT)
            {
                mt = GLUniformType::Float32;
                byteCount = 4;
            }
            else if (type == GL_INT)
            {
                mt = GLUniformType::Int32;
                byteCount = 4;
            }
            else if (type == GL_FLOAT_VEC2)
            {
                mt = GLUniformType::Vec2;
                byteCount = 8;
            }
            else if (type == GL_FLOAT_VEC3)
            {
                mt = GLUniformType::Vec3;
                byteCount = 12;
            }
            else if (type == GL_FLOAT_VEC4)
            {
                mt = GLUniformType::Vec4;
                byteCount = 16;
            }
            else if (type == GL_FLOAT_MAT3)
            {
                mt = GLUniformType::Mat3;
                byteCount = 36;
            }
            else if (type == GL_FLOAT_MAT4)
            {
                mt = GLUniformType::Mat4;
                byteCount = 64;
            }
            else
            {
                //@TODO: Error;
            }
            // ASSERT_NO_GL_ERROR(glGetActiveUniform(program, index, 512, &len, &size, &type,
            // nameBuffer));

            block.uniforms.append({ String(nameBuffer), (GLuint)offset, mt });
            if (i > 0)
                block.byteCount += offset;
            if (i == activeUniformsInBlock - 1)
                block.byteCount += byteCount;
        }
        m_uniformBlocks.append(std::move(block));
    }

    // grag the texture uniforms
    // check what uniforms are active
    GLint uniformCount;
    ASSERT_NO_GL_ERROR(glUseProgram(program));
    ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount));
    for (GLint i = 0; i < uniformCount; ++i)
    {
        char nameBuffer[512] = { 0 };
        GLsizei len, size;
        GLenum type;
        ASSERT_NO_GL_ERROR(glGetActiveUniform(program, i, 512, &len, &size, &type, nameBuffer));
        GLuint loc = glGetUniformLocation(program, nameBuffer);

        // ignore everything but samples
        if (type == GL_SAMPLER_1D || type == GL_SAMPLER_2D || type == GL_SAMPLER_3D)
        {
            m_textures.append({ String(nameBuffer, _alloc), loc });
            ASSERT_NO_GL_ERROR(glUniform1i(loc, m_textures.count() - 1));
        }
    }

    m_glProgram = program;
    return Error();
}

GLProgram::~GLProgram()
{
    glDeleteProgram(m_glProgram);
}

GLPipeline::GLPipeline(Allocator & _alloc, const PipelineSettings & _settings) :
    m_program(nullptr),
    m_renderState(0),
    m_scissorRect({ 0, 0, 0, 0 }),
    m_viewportRect({ 0, 0, 0, 0 }),
    m_bChangedSinceLastDrawCall(true),
    m_variables(_alloc),
    m_textures(_alloc),
    m_uniformBlockStorage(_alloc)
{
    UInt64 renderState = 0;

    setFlag(renderState, RF_Multisampling, _settings.multisample);
    setFlag(renderState, RF_DepthTest, _settings.depthTest);
    setFlag(renderState, RF_DepthWrite, _settings.depthWrite);
    setField(renderState, RF_DepthFuncShift, RF_DepthFuncMask, _settings.depthFunction);
    setFlag(renderState, RF_ColorWriteRed, _settings.colorWriteSettings.r);
    setFlag(renderState, RF_ColorWriteGreen, _settings.colorWriteSettings.g);
    setFlag(renderState, RF_ColorWriteBlue, _settings.colorWriteSettings.b);
    setFlag(renderState, RF_ColorWriteAlpha, _settings.colorWriteSettings.a);
    setFlag(renderState, RF_FrontFaceClockwise, _settings.faceDirection == FaceDirection::CW);
    setField(renderState, RF_CullFaceShift, RF_CullFaceMask, _settings.cullFace);

    if (_settings.blendSettings)
    {
        setFlag(renderState, RF_Blending, true);
        setField(renderState,
                 RF_ColorBlendModeShift,
                 RF_ColorBlendModeMask,
                 s_glBlendModes[(Size)_settings.blendSettings->colorBlendMode]);
        setField(renderState,
                 RF_AlphaBlendModeShift,
                 RF_AlphaBlendModeMask,
                 s_glBlendModes[(Size)_settings.blendSettings->alphaBlendMode]);
        setField(renderState,
                 RF_ColorSourceBlendFuncShift,
                 RF_ColorSourceBlendFuncMask,
                 s_glBlendFuncs[(Size)_settings.blendSettings->colorSrcBlendFunction]);
        setField(renderState,
                 RF_ColorDestBlendFuncShift,
                 RF_ColorDestBlendFuncMask,
                 s_glBlendFuncs[(Size)_settings.blendSettings->colorDestBlendFunction]);
        setField(renderState,
                 RF_AlphaSourceBlendFuncShift,
                 RF_AlphaSourceBlendFuncMask,
                 s_glBlendFuncs[(Size)_settings.blendSettings->alphaSrcBlendFunction]);
        setField(renderState,
                 RF_AlphaDestBlendFuncShift,
                 RF_AlphaDestBlendFuncMask,
                 s_glBlendFuncs[(Size)_settings.blendSettings->alphaDestBlendFunction]);
    }

    m_program = static_cast<GLProgram *>(_settings.program);
    m_renderState = renderState;
    m_variables.reserve(4);
    m_uniformBlockStorage.reserve(m_program->m_uniformBlocks.count());
    for (Size i = 0; i < m_program->m_uniformBlocks.count(); ++i)
    {
        auto & blk = m_program->m_uniformBlocks[i];
        for (Size j = 0; j < blk.uniforms.count(); ++j)
        {
            m_variables.append(stick::makeUnique<GLPipelineVariable>(_alloc, this, i, j));
        }
        GLUniformBlockStorage storage;
        storage.data = DynamicArray<char>(_alloc);
        storage.data.resize(blk.byteCount);
        m_uniformBlockStorage.append(std::move(storage));
    }

    m_textures.reserve(2);
    for (Size i = 0; i < m_program->m_textures.count(); ++i)
    {
        m_textures.append(stick::makeUnique<GLPipelineTexture>(_alloc, this));
    }
}

GLPipeline::~GLPipeline()
{
    // nothing to do here
}

PipelineVariable * GLPipeline::variable(const char * _name)
{
    for (auto & var : m_variables)
    {
        GLBlockedUniform & uniform =
            m_program->m_uniformBlocks[var->m_blockIndex].uniforms[var->m_uniformIndex];
        if (uniform.name == _name)
            return var.get();
    }
    return nullptr;
}

PipelineTexture * GLPipeline::texture(const char * _name)
{
    for (Size idx = 0; idx < m_program->m_textures.count(); ++idx)
    {
        if (m_program->m_textures[idx].name == _name)
            return m_textures[idx].get();
    }
    return nullptr;
}

// // pipeline variable helpers
// static inline Error setBlockedVariable(GLUniformBlock & _block,
//                                        UInt32 _uniformIndex,
//                                        const void * _data,
//                                        Size _byteCount,
//                                        GLUniformType _type)
// {
//     STICK_ASSERT(_uniformIndex < _block.uniforms.count());
//     GLBlockedUniform & uniform = _block.uniforms[_uniformIndex];

//     // check if the variable changed
//     if (std::memcmp(_block.tmpStorage.ptr() + uniform.byteOffset, _data, _byteCount) == 0)
//         return Error();

//     if (uniform.type == _type)
//         std::memcpy(_block.tmpStorage.ptr() + uniform.byteOffset, _data, _byteCount);
//     else
//         return Error(ec::InvalidOperation,
//                      "Unsupported uniform type or type mismatch",
//                      STICK_FILE,
//                      STICK_LINE);
//     return Error();
// }

GLPipelineVariable::GLPipelineVariable(GLPipeline * _pipe,
                                       UInt32 _blockIndex,
                                       UInt32 _uniformIndex) :
    m_pipeline(_pipe),
    m_blockIndex(_blockIndex),
    m_uniformIndex(_uniformIndex)
{
}

GLPipelineVariable::~GLPipelineVariable()
{
}

void GLPipelineVariable::setFloat32(Float32 _value)
{
    setHelper(&_value, sizeof(Float32), GLUniformType::Float32);
}

void GLPipelineVariable::setInt32(Int32 _value)
{
    setHelper(&_value, sizeof(Int32), GLUniformType::Int32);
}

void GLPipelineVariable::setVec2f(Float32 _x, Float32 _y)
{
    Float32 tmp[] = { _x, _y };
    setHelper(tmp, sizeof(tmp), GLUniformType::Vec2);
}

void GLPipelineVariable::setVec3f(Float32 _x, Float32 _y, Float32 _z)
{
    Float32 tmp[] = { _x, _y, _z };
    setHelper(tmp, sizeof(tmp), GLUniformType::Vec3);
}

void GLPipelineVariable::setVec4f(Float32 _x, Float32 _y, Float32 _z, Float32 _w)
{
    Float32 tmp[] = { _x, _y, _z, _y };
    setHelper(tmp, sizeof(tmp), GLUniformType::Vec4);
}

void GLPipelineVariable::setMat4f(Float32 * _mat)
{
    setHelper(_mat, sizeof(Float32) * 16, GLUniformType::Mat4);
}

void GLPipelineVariable::setHelper(const void * _data, Size _byteCount, GLUniformType _type)
{
    STICK_ASSERT(m_blockIndex < m_pipeline->m_program->m_uniformBlocks.count());
    GLUniformBlock & blk = m_pipeline->m_program->m_uniformBlocks[m_blockIndex];
    GLUniformBlockStorage & storage = m_pipeline->m_uniformBlockStorage[m_blockIndex];
    STICK_ASSERT(m_uniformIndex < blk.uniforms.count());

    GLBlockedUniform & uniform = blk.uniforms[m_uniformIndex];

    // check if the variable changed
    if (std::memcmp(storage.data.ptr() + uniform.byteOffset, _data, _byteCount) == 0)
        return;

    // otherwise ensure that the type is correct in debug build and copy the data to the tmp
    // storage
    STICK_ASSERT(uniform.type == _type);
    std::memcpy(storage.data.ptr() + uniform.byteOffset, _data, _byteCount);
}

GLPipelineTexture::GLPipelineTexture(GLPipeline * _pipe) :
    m_pipeline(_pipe),
    m_texture(nullptr),
    m_sampler(nullptr)
{
}

void GLPipelineTexture::set(const Texture * _tex, const Sampler * _sampler)
{
    m_texture = static_cast<const GLTexture *>(_tex);
    m_sampler = static_cast<const GLSampler *>(_sampler);
}

GLVertexBuffer::GLVertexBuffer(BufferUsageFlags _flags) : m_usageFlags(_flags)
{
    ASSERT_NO_GL_ERROR(glGenBuffers(1, &m_glVertexBuffer));
}

GLVertexBuffer::~GLVertexBuffer()
{
    glDeleteBuffers(1, &m_glVertexBuffer);
}

void GLVertexBuffer::loadDataRaw(const void * _data, Size _byteCount)
{
    //@TODO: Take usage into account
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer));
    ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _byteCount, _data, GL_STATIC_DRAW));
}

GLIndexBuffer::GLIndexBuffer(BufferUsageFlags _flags) : m_usageFlags(_flags)
{
    ASSERT_NO_GL_ERROR(glGenBuffers(1, &m_glIndexBuffer));
}

GLIndexBuffer::~GLIndexBuffer()
{
    glDeleteBuffers(1, &m_glIndexBuffer);
}

void GLIndexBuffer::loadDataRaw(const void * _data, Size _byteCount)
{
    //@TODO: Take usage into account
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_glIndexBuffer));
    ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _byteCount, _data, GL_STATIC_DRAW));
}

GLMesh::GLMesh(Allocator & _alloc,
               VertexBuffer ** _vertexBuffers,
               const VertexLayout * _layouts,
               Size _count,
               IndexBuffer * _indexBuffer) :
    m_vertexBuffers(_alloc),
    m_indexBuffer(static_cast<GLIndexBuffer *>(_indexBuffer))
{
    ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &m_glVao));
    ASSERT_NO_GL_ERROR(glBindVertexArray(m_glVao));

    for (Size i = 0; i < _count; ++i)
    {
        GLVertexBuffer * vb = static_cast<GLVertexBuffer *>(_vertexBuffers[i]);
        const VertexLayout & layout = _layouts[i];
        ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vb->m_glVertexBuffer));
        for (const auto & el : layout.elements)
        {
            STICK_ASSERT(el.elementCount <= 4);
            ASSERT_NO_GL_ERROR(glVertexAttribPointer(el.location,
                                                     static_cast<UInt32>(el.elementCount),
                                                     s_glDataTypes[static_cast<Size>(el.dataType)],
                                                     GL_FALSE,
                                                     static_cast<UInt32>(el.stride),
                                                     ((char *)0 + el.offset)));
            ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(el.location));
            //@TODO: Do we need to support per instance attributes or matrix attributes?
        }

        m_vertexBuffers.append(vb);
    }
}

GLMesh::~GLMesh()
{
    glDeleteVertexArrays(1, &m_glVao);
}

GLRenderPass::GLRenderPass(GLRenderDevice * _device, Allocator & _alloc) :
    m_device(_device),
    m_commands(_alloc)
{
}

void GLRenderPass::drawMesh(const Mesh * _mesh,
                            const Pipeline * _pipeline,
                            UInt32 _vertexOffset,
                            UInt32 _vertexCount,
                            VertexDrawMode _drawMode)
{
    // copy all the uniforms of the pipeline to the uniform buffer
    GLUBOBindingArray bindings(*m_device->m_alloc);
    const GLPipeline * pipe = static_cast<const GLPipeline *>(_pipeline);
    bindings.reserve(pipe->m_uniformBlockStorage.count());

    for (Size i = 0; i < pipe->m_uniformBlockStorage.count(); ++i)
    {
        GLUniformBlockStorage & storage =
            const_cast<GLUniformBlockStorage &>(pipe->m_uniformBlockStorage[i]);
        auto & block = pipe->m_program->m_uniformBlocks[i];
        storage.lastByteOffset = m_device->copyToUBO(storage.data.count(), storage.data.ptr());
        bindings.append(
            { block.bindingPoint, storage.lastByteOffset, (UInt32)storage.data.count() });
    }

    m_commands.append((GLDrawCmd){ static_cast<const GLMesh *>(_mesh),
                                   static_cast<const GLPipeline *>(_pipeline),
                                   _vertexOffset,
                                   _vertexCount,
                                   _drawMode,
                                   std::move(bindings) });
}

void GLRenderPass::drawCustom(ExternalDrawFunction _fn)
{
    m_commands.append((GLExternalDrawCmd){ _fn });
}

void GLRenderPass::setViewport(Float32 _x, Float32 _y, Float32 _w, Float32 _h)
{
    m_commands.append((GLViewportCmd){ _x, _y, _w, _h });
}

void GLRenderPass::clearBuffers(const ClearSettings & _settings)
{
    m_commands.append((GLClearCmd){ _settings });
}

void GLRenderPass::reset()
{
    m_renderBuffer = nullptr;
    m_commands.clear();
}

GLTexture::GLTexture() :
    m_glTarget(GL_TEXTURE_2D),
    m_format(TextureFormat::RGBA8),
    m_renderBuffer(nullptr)
{
    ASSERT_NO_GL_ERROR(glGenTextures(1, &m_glTexture));
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_glTexture);
}

void GLTexture::loadPixels(UInt32 _width,
                           UInt32 _height,
                           UInt32 _depth,
                           const void * _data,
                           DataType _dataType,
                           TextureFormat _format,
                           UInt32 _alignment,
                           UInt32 _mipmapLevelCount)
{
    m_glTarget = GL_TEXTURE_1D;
    if (_height > 1)
        m_glTarget = _depth > 1 ? GL_TEXTURE_3D : GL_TEXTURE_2D;

    ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
    ASSERT_NO_GL_ERROR(glBindTexture(m_glTarget, m_glTexture));

    // tex.format = cmd.command.loadPixelsCommand.format;
    ASSERT_NO_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, _alignment));

    GLenum glDataType = s_glDataTypes[static_cast<Size>(_dataType)];
    const GLTextureFormat & format = s_glTextureFormats[static_cast<Size>(_format)];

    if (m_glTarget == GL_TEXTURE_1D)
    {
        ASSERT_NO_GL_ERROR(glTexImage1D(
            m_glTarget, 0, format.glInternalFormat, _width, 0, format.glFormat, glDataType, _data));
    }
    else if (m_glTarget == GL_TEXTURE_2D)
    {
        ASSERT_NO_GL_ERROR(glTexImage2D(m_glTarget,
                                        0,
                                        format.glInternalFormat,
                                        _width,
                                        _height,
                                        0,
                                        format.glFormat,
                                        glDataType,
                                        _data));
    }
    else if (m_glTarget == GL_TEXTURE_3D)
    {
        ASSERT_NO_GL_ERROR(glTexImage3D(m_glTarget,
                                        0,
                                        format.glInternalFormat,
                                        _width,
                                        _height,
                                        _depth,
                                        0,
                                        format.glFormat,
                                        glDataType,
                                        _data));
    }
}

GLSampler::GLSampler(const SamplerSettings & _settings)
{
    GLenum minFilter, magFilter;
    minFilter = magFilter = GL_NEAREST;
    bool bMipMapping = _settings.mipMapping;
    if (_settings.filtering == TextureFiltering::Nearest)
    {
        if (bMipMapping)
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        else
            minFilter = GL_NEAREST;

        magFilter = GL_NEAREST;
    }
    else if (_settings.filtering == TextureFiltering::Bilinear)
    {
        if (bMipMapping)
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        else
            minFilter = GL_LINEAR;

        magFilter = GL_LINEAR;
    }
    else if (_settings.filtering == TextureFiltering::Trilinear)
    {
        if (bMipMapping)
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        else
            minFilter = GL_LINEAR;

        magFilter = GL_LINEAR;
    }

    ASSERT_NO_GL_ERROR(glGenSamplers(1, &m_glSampler));
    ASSERT_NO_GL_ERROR(glSamplerParameteri(
        m_glSampler, GL_TEXTURE_WRAP_S, s_glWrap[static_cast<Size>(_settings.wrapS)]));
    ASSERT_NO_GL_ERROR(glSamplerParameteri(
        m_glSampler, GL_TEXTURE_WRAP_T, s_glWrap[static_cast<Size>(_settings.wrapT)]));
    ASSERT_NO_GL_ERROR(glSamplerParameteri(
        m_glSampler, GL_TEXTURE_WRAP_R, s_glWrap[static_cast<Size>(_settings.wrapR)]));
    ASSERT_NO_GL_ERROR(glSamplerParameteri(m_glSampler, GL_TEXTURE_MAG_FILTER, magFilter));
    ASSERT_NO_GL_ERROR(glSamplerParameteri(m_glSampler, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLSampler::~GLSampler()
{
    glDeleteSamplers(1, &m_glSampler);
}

GLRenderBuffer::GLRenderBuffer(GLRenderDevice * _device) :
    m_device(_device),
    m_glMSAAFBO(0),
    m_renderTargets(*_device->m_alloc),
    m_colorAttachmentPoints(*_device->m_alloc),
    m_colorTargets(*_device->m_alloc),
    m_depthStencilTarget(nullptr),
    m_bDirty(true)
{
}

Error GLRenderBuffer::init(const RenderBufferSettings & _settings)
{
    ASSERT_NO_GL_ERROR(glGenFramebuffers(1, &m_glFBO));
    if (_settings.sampleCount > 1)
    {
        ASSERT_NO_GL_ERROR(glGenFramebuffers(1, &m_glMSAAFBO));
    }

    m_width = _settings.width;
    m_height = _settings.height;
    m_sampleCount = _settings.sampleCount;

    GLuint nextColorTargetID = 0;
    for (const auto & rt : _settings.renderTargets)
    {
        const TextureFormatInfo & info = s_textureFormatInfos[static_cast<Size>(rt.format)];
        const GLTextureFormat & format = s_glTextureFormats[static_cast<Size>(rt.format)];
        bool bIsColorAttachment = info.bIsColorFormat;

        auto tex = makeUnique<GLTexture>(*m_device->m_alloc);
        tex->m_glTarget = GL_TEXTURE_2D;
        tex->m_format = rt.format;
        tex->m_renderBuffer = this;

        auto texHandle = tex->m_glTexture;
        ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
        ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_2D, texHandle));

        ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
        ASSERT_NO_GL_ERROR(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, rt.mipmapLevelCount));
        // ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        // ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

        GLenum dt = format.glFormat == GL_DEPTH_STENCIL ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE;

        ASSERT_NO_GL_ERROR(glTexImage2D(GL_TEXTURE_2D,
                                        0,
                                        format.glInternalFormat,
                                        (GLuint)_settings.width,
                                        (GLuint)_settings.height,
                                        0,
                                        format.glFormat,
                                        dt,
                                        0));

        ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, m_glFBO));

        GLRenderBuffer::RenderTarget target = { 0 };
        target.texture = tex.get();
        if (bIsColorAttachment)
        {
            ASSERT_NO_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                      GL_COLOR_ATTACHMENT0 + nextColorTargetID,
                                                      GL_TEXTURE_2D,
                                                      texHandle,
                                                      0));
            m_colorAttachmentPoints.append(GL_COLOR_ATTACHMENT0 + nextColorTargetID);
            target.attachmentPoint = GL_COLOR_ATTACHMENT0 + nextColorTargetID;
            target.bIsDepthTarget = false;
            m_colorTargets.append(tex.get());
        }
        else
        {
            // if we don't set this, the framebuffer validation fails if we only have a depth tex
            // attached
            ASSERT_NO_GL_ERROR(glDrawBuffer(GL_NONE));
            ASSERT_NO_GL_ERROR(glReadBuffer(GL_NONE));
            ASSERT_NO_GL_ERROR(glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                !info.bIsStencilFormat ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT,
                GL_TEXTURE_2D,
                texHandle,
                0));

            target.attachmentPoint =
                info.bIsStencilFormat ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
            target.bIsDepthTarget = true;
            m_depthStencilTarget = tex.get();
        }

        Error err = validateFrameBuffer();
        if (err)
            return err;

        if (_settings.sampleCount > 1)
        {
            Int32 maxSamples;

            ASSERT_NO_GL_ERROR(glGetIntegerv(GL_MAX_SAMPLES, &maxSamples));

            //@TODO: Should this silently limit the sample count or fail?
            if (m_sampleCount > maxSamples)
                m_sampleCount = maxSamples;

            ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, m_glMSAAFBO));

            GLuint glrb;
            ASSERT_NO_GL_ERROR(glGenRenderbuffers(1, &glrb));
            ASSERT_NO_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, glrb));
            ASSERT_NO_GL_ERROR(glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                                                                m_sampleCount,
                                                                format.glInternalFormat,
                                                                (GLuint)m_width,
                                                                (GLuint)m_height));

            if (bIsColorAttachment)
            {
                ASSERT_NO_GL_ERROR(
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                              GL_COLOR_ATTACHMENT0 + nextColorTargetID,
                                              GL_RENDERBUFFER,
                                              glrb));
            }
            else
            {
                ASSERT_NO_GL_ERROR(glDrawBuffer(GL_NONE));
                ASSERT_NO_GL_ERROR(glReadBuffer(GL_NONE));
                ASSERT_NO_GL_ERROR(glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER,
                    !info.bIsStencilFormat ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT,
                    GL_RENDERBUFFER,
                    glrb));
            }
            target.msaaRenderBuffer = glrb;

            err = validateFrameBuffer();
            if (err)
                return err;

            if (bIsColorAttachment)
                nextColorTargetID++;
        }

        m_device->m_textures.append(std::move(tex));
        m_renderTargets.append(target);
    }

    ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    return Error();
}

GLRenderBuffer::~GLRenderBuffer()
{
    deallocate(false);
}

void GLRenderBuffer::deallocate(bool _bDestroyRenderTargets)
{
    if (!m_device)
        return;

    ASSERT_NO_GL_ERROR(glDeleteFramebuffers(1, &m_glFBO));
    if (m_glMSAAFBO)
    {
        for (auto & rt : m_renderTargets)
        {
            ASSERT_NO_GL_ERROR(glDeleteRenderbuffers(1, &(rt.msaaRenderBuffer)));
        }
        ASSERT_NO_GL_ERROR(glDeleteFramebuffers(1, &m_glMSAAFBO));
    }

    for (auto & rt : m_renderTargets)
    {
        rt.texture->m_renderBuffer = nullptr;
        if (_bDestroyRenderTargets)
        {
            m_device->destroyTexture(rt.texture);
        }
    }

    /* we use this is a flag to signal that the renderbuffer has been deallocated already */
    m_device = nullptr;
}

const DynamicArray<Texture *> GLRenderBuffer::colorTargets() const
{
    return m_colorTargets;
}

Texture * GLRenderBuffer::depthStencilTarget() const
{
    return m_depthStencilTarget;
}

UInt32 GLRenderBuffer::width() const
{
    return m_width;
}

UInt32 GLRenderBuffer::height() const
{
    return m_height;
}

UInt32 GLRenderBuffer::sampleCount() const
{
    return m_sampleCount;
}

void GLRenderBuffer::finalizeForReading(GLRenderBuffer * _currentBuffer)
{
    if (m_bDirty)
    {
        if (m_sampleCount > 1)
        {
            for (auto & target : m_renderTargets)
            {
                ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_glMSAAFBO));
                if (target.attachmentPoint != GL_DEPTH_ATTACHMENT &&
                    target.attachmentPoint != GL_DEPTH_STENCIL_ATTACHMENT)
                {
                    ASSERT_NO_GL_ERROR(glReadBuffer(target.attachmentPoint));
                }

                ASSERT_NO_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_glFBO));
                if (target.attachmentPoint != GL_DEPTH_ATTACHMENT &&
                    target.attachmentPoint != GL_DEPTH_STENCIL_ATTACHMENT)
                {
                    ASSERT_NO_GL_ERROR(glDrawBuffer(target.attachmentPoint));
                }
                if (target.attachmentPoint == GL_DEPTH_ATTACHMENT)
                {
                    ASSERT_NO_GL_ERROR(glBlitFramebuffer(0,
                                                         0,
                                                         (GLuint)m_width,
                                                         (GLuint)m_height,
                                                         0,
                                                         0,
                                                         (GLuint)m_width,
                                                         (GLuint)m_height,
                                                         GL_DEPTH_BUFFER_BIT,
                                                         GL_NEAREST));
                }
                else if (target.attachmentPoint == GL_DEPTH_STENCIL_ATTACHMENT)
                {
                    ASSERT_NO_GL_ERROR(
                        glBlitFramebuffer(0,
                                          0,
                                          (GLuint)m_width,
                                          (GLuint)m_height,
                                          0,
                                          0,
                                          (GLuint)m_width,
                                          (GLuint)m_height,
                                          GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
                                          GL_NEAREST));
                }
                else
                {
                    ASSERT_NO_GL_ERROR(glBlitFramebuffer(0,
                                                         0,
                                                         (GLuint)m_width,
                                                         (GLuint)m_height,
                                                         0,
                                                         0,
                                                         (GLuint)m_width,
                                                         (GLuint)m_height,
                                                         GL_COLOR_BUFFER_BIT,
                                                         GL_NEAREST));
                }
            }
        }

        m_bDirty = false;

        // rebind the previous buffer
        bindRenderBufferImpl(_currentBuffer, false);
    }
}

} // namespace gl
} // namespace dab
