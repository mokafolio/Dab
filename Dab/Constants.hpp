#ifndef DAB_CONSTANTS_HPP
#define DAB_CONSTANTS_HPP

#include <Stick/Platform.hpp>
#include <type_traits> //for underlying_type

namespace dab
{
// STICK_ENUM_WITH_BASE(STICK_API, RenderFlag, stick::UInt64){
//     RF_DepthTest = 1 << 0,
//     RF_Multisampling = 1 << 1,
//     RF_Blending = 1 << 2,
//     RF_DepthWrite = 1 << 3,
//     RF_ColorWriteRed = 1 << 4,
//     RF_ColorWriteGreen = 1 << 5,
//     RF_ColorWriteBlue = 1 << 6,
//     RF_ColorWriteAlpha = 1 << 7,
//     RF_FrontFaceClockwise = 1 << 8,
//     RF_StencilTest = 1 << 9,
//     RF_CullFaceShift = 10,
//     RF_CullFaceMask = (1 << RF_CullFaceShift) | (1 << (RF_CullFaceShift + 1)),
//     RF_ColorBlendModeShift = 12,
//     RF_ColorBlendModeMask = (1 << RF_ColorBlendModeShift) | (1 << (RF_ColorBlendModeShift + 1)) |
//                             (1 << (RF_ColorBlendModeShift + 2)),
//     RF_AlphaBlendModeShift = 15,
//     RF_AlphaBlendModeMask = (1 << RF_AlphaBlendModeShift) | (1 << (RF_AlphaBlendModeShift + 1)) |
//                             (1 << (RF_AlphaBlendModeShift + 2)),
//     RF_ColorSourceBlendFuncShift = 18,
//     RF_ColorSourceBlendFuncMask = (1 << RF_ColorSourceBlendFuncShift) |
//                                   (1 << (RF_ColorSourceBlendFuncShift + 1)) |
//                                   (1 << (RF_ColorSourceBlendFuncShift + 2)) |
//                                   (1 << (RF_ColorSourceBlendFuncShift + 3)),
//     RF_ColorDestBlendFuncShift = 22,
//     RF_ColorDestBlendFuncMask = (1 << RF_ColorDestBlendFuncShift) |
//                                 (1 << (RF_ColorDestBlendFuncShift + 1)) |
//                                 (1 << (RF_ColorDestBlendFuncShift + 2)) |
//                                 (1 << (RF_ColorDestBlendFuncShift + 3)),
//     RF_AlphaSourceBlendFuncShift = 26,
//     RF_AlphaSourceBlendFuncMask = (1 << RF_AlphaSourceBlendFuncShift) |
//                                   (1 << (RF_AlphaSourceBlendFuncShift + 1)) |
//                                   (1 << (RF_AlphaSourceBlendFuncShift + 2)) |
//                                   (1 << (RF_AlphaSourceBlendFuncShift + 3)),
//     RF_AlphaDestBlendFuncShift = 30,
//     RF_AlphaDestBlendFuncMask = ((stick::UInt64)1 << RF_AlphaDestBlendFuncShift) |
//                                 ((stick::UInt64)1 << (RF_AlphaDestBlendFuncShift + 1)) |
//                                 ((stick::UInt64)1 << (RF_AlphaDestBlendFuncShift + 2)) |
//                                 ((stick::UInt64)1 << (RF_AlphaDestBlendFuncShift + 3)),
//     RF_DepthFuncShift = 34,
//     RF_DepthFuncMask = ((stick::UInt64)1 << RF_DepthFuncShift) |
//                        ((stick::UInt64)1 << (RF_DepthFuncShift + 1)) |
//                        ((stick::UInt64)1 << (RF_DepthFuncShift + 2)) |
//                        ((stick::UInt64)1 << (RF_DepthFuncShift + 3))
// };

// STICK_ENUM_WITH_BASE(STICK_API, StencilFlag, stick::UInt64){
//     SF_StencilFuncFrontFaceShift = 0,
//     SF_StencilFuncFrontFaceMask = ((stick::UInt64)1 << SF_StencilFuncFrontFaceShift) |
//                                   ((stick::UInt64)1 << (SF_StencilFuncFrontFaceShift + 1)) |
//                                   ((stick::UInt64)1 << (SF_StencilFuncFrontFaceShift + 2)) |
//                                   ((stick::UInt64)1 << (SF_StencilFuncFrontFaceShift + 3)),
//     SF_StencilFuncBackFaceShift = 4,
//     SF_StencilFuncBackFaceMask = ((stick::UInt64)1 << SF_StencilFuncBackFaceShift) |
//                                  ((stick::UInt64)1 << (SF_StencilFuncBackFaceShift + 1)) |
//                                  ((stick::UInt64)1 << (SF_StencilFuncBackFaceShift + 2)) |
//                                  ((stick::UInt64)1 << (SF_StencilFuncBackFaceShift + 3)),
//     SF_StencilOpSFailFrontFaceShift = 8,
//     SF_StencilOpSFailFrontFaceMask = ((stick::UInt64)1 << SF_StencilOpSFailFrontFaceShift) |
//                                      ((stick::UInt64)1 << (SF_StencilOpSFailFrontFaceShift + 1)) |
//                                      ((stick::UInt64)1 << (SF_StencilOpSFailFrontFaceShift + 2)),
//     SF_StencilOpDPFailFrontFaceShift = 11,
//     SF_StencilOpDPFailFrontFaceMask = ((stick::UInt64)1 << SF_StencilOpDPFailFrontFaceShift) |
//                                       ((stick::UInt64)1 << (SF_StencilOpDPFailFrontFaceShift + 1)) |
//                                       ((stick::UInt64)1 << (SF_StencilOpDPFailFrontFaceShift + 2)),
//     SF_StencilOpDPPassFrontFaceShift = 14,
//     SF_StencilOpDPPassFrontFaceMask = ((stick::UInt64)1 << SF_StencilOpDPPassFrontFaceShift) |
//                                       ((stick::UInt64)1 << (SF_StencilOpDPPassFrontFaceShift + 1)) |
//                                       ((stick::UInt64)1 << (SF_StencilOpDPPassFrontFaceShift + 2)),
//     SF_StencilOpSFailBackFaceShift = 17,
//     SF_StencilOpSFailBackFaceMask = ((stick::UInt64)1 << SF_StencilOpSFailBackFaceShift) |
//                                     ((stick::UInt64)1 << (SF_StencilOpSFailBackFaceShift + 1)) |
//                                     ((stick::UInt64)1 << (SF_StencilOpSFailBackFaceShift + 2)),
//     SF_StencilOpDPFailBackFaceShift = 20,
//     SF_StencilOpDPFailBackFaceMask = ((stick::UInt64)1 << SF_StencilOpDPFailBackFaceShift) |
//                                      ((stick::UInt64)1 << (SF_StencilOpDPFailBackFaceShift + 1)) |
//                                      ((stick::UInt64)1 << (SF_StencilOpDPFailBackFaceShift + 2)),
//     SF_StencilOpDPPassBackFaceShift = 23,
//     SF_StencilOpDPPassBackFaceMask = ((stick::UInt64)1 << SF_StencilOpDPPassBackFaceShift) |
//                                      ((stick::UInt64)1 << (SF_StencilOpDPPassBackFaceShift + 1)) |
//                                      ((stick::UInt64)1 << (SF_StencilOpDPPassBackFaceShift + 2))
// };

STICK_API_ENUM_CLASS(FaceDirection){ Clockwise, CounterClockwise, Count };

STICK_API_ENUM_CLASS(FaceType){ None, Front, Back, FrontAndBack, Count };

STICK_API_ENUM_CLASS(CompareFunction){ Equal, LessEqual, GreaterEqual, NotEqual, Always,
                                       Never, Less,      Greater,      Count };

STICK_API_ENUM_CLASS(StencilOperation){ Keep,          Zero,      Replace,       Increment,
                                        IncrementWrap, Decrement, DecrementWrap, Invert };

STICK_API_ENUM_CLASS(BlendMode){ Add, Substract, ReverseSubstract, Min, Max, Count };

STICK_API_ENUM_CLASS(BlendFunction){ Zero,
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
                                     Count };

STICK_API_ENUM_CLASS(BufferDataType){ UInt8, Int8,    UInt16,  Int16, UInt32,
                                      Int32, Float32, Float64, Count };

STICK_API_ENUM_CLASS(BufferUsage){ DynamicDraw, StaticDraw,  StreamDraw, DynamicRead, StaticRead,
                                   StreamRead,  DynamicCopy, StaticCopy, StreamCopy,  Count };

STICK_API_ENUM_CLASS(VertexDrawMode){ Triangles,
                                      TrianglesAdjacency, // only for use with geometry shaders
                                      TriangleStrip,
                                      TriangleStripAdjacency, // only for use with geometry shaders
                                      TriangleFan,
                                      Points,
                                      Lines,
                                      LinesAdjacency, // only for use with geometry shaders
                                      LineStrip,
                                      LineStripAdjacency, // only for use with geometry shaders
                                      LineLoop,
                                      Count };

STICK_API_ENUM_CLASS(TextureFormat){ R8,
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
                                     Count };

STICK_API_ENUM_CLASS(ReadFormat){ Red, Green, Blue, Alpha, RGB, RGBA, BGR, BGRA, Count };

STICK_API_ENUM_CLASS(TextureWrap){ Repeat, ClampToEdge, ClampToBorder, Count };

STICK_API_ENUM_CLASS(TextureFiltering){
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

// TODO: Static assert this

STICK_API_ENUM_CLASS(PixelDataType){ UInt8, Int8, UInt16, Int16, UInt32, Int32, Float32, Count };

STICK_API_ENUM_CLASS(ProgramVariableType){ None, Float32, Vec2f, Vec3f, Vec4f, Matrix3f, Matrix4f };

STICK_API_ENUM_CLASS(BufferType){ Depth = 1, Color = 1 << 1, Stencil = 1 << 2 };

template <class T>
inline BufferType operator|(BufferType lhs, T rhs)
{
    using underlying = typename std::underlying_type<BufferType>::type;
    return static_cast<BufferType>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <class T>
inline BufferType & operator|=(BufferType & lhs, T rhs)
{
    using underlying = typename std::underlying_type<BufferType>::type;
    lhs = static_cast<BufferType>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}

struct STICK_API BufferDataTypeInfo
{
    stick::Size byteCount;
};

const BufferDataTypeInfo s_bufferDataTypeInfos[] = {
    // UInt8
    { 1 },
    // Int8
    { 1 },
    // UInt16
    { 2 },
    // Int16
    { 2 },
    // UInt32
    { 4 },
    // Int32
    { 4 },
    // Float32
    { 4 },
    // Float64
    { 8 },
};

static_assert((stick::Size)BufferDataType::Count ==
                  sizeof(s_bufferDataTypeInfos) / sizeof(s_bufferDataTypeInfos[0]),
              "BufferDataType mapping is not complete!");

} // namespace dab

#endif // DAB_CONSTANTS_HPP
