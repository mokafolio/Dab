#include <Dab/Dab.hpp>
#include <cstdlib>

#include <Luke/Window.hpp>

#define RETURN_ON_ERR(_err)                                                                        \
    if (_err)                                                                                      \
    {                                                                                              \
        printf("Error: %s\n", _err.message().cString());                                           \
        return EXIT_FAILURE;                                                                       \
    }

using namespace dab;
using namespace luke;
using namespace stick;

int main(int _argc, const char * _args[])
{   
    WindowSettings set;
    set.setSampleCount(8);

    Window window;
    Error err = window.open(set);
    RETURN_ON_ERR(err);

    RenderDevice * device = createRenderDevice().ensure();

    // a very simple vertex shader for the triangle
    const char * vertexShader =
        "#version 410 core \n"
        "layout(std140) uniform Stuff\n"
        "{\n"
            "vec4 moreCol;\n"
        "};\n"
        "layout(location = 0) in vec2 vertex; \n"
        "layout(location = 1) in vec4 color; \n"
        "out vec4 ocol; \n"
        "void main() \n"
        "{ \n"
        "   ocol = color * moreCol; \n"
        "   gl_Position = vec4(vertex, 0.0, 1.0); \n"
        "} \n";

    // a simple fragment shader for the triangle
    const char * fragmentShader =
        "#version 410 core \n"
        "in vec4 ocol; \n"
        "out vec4 fragColor; \n"
        "void main() \n"
        "{ \n"
        "   fragColor = ocol; \n"
        "} \n";

    Program * prog = device->createProgram(vertexShader, fragmentShader).ensure();
    PipelineSettings ps(prog);

    Pipeline * pipe = device->createPipeline(ps).ensure();

    VertexBuffer * vb = device->createVertexBuffer().ensure();
    // load the vertex data for the triangle into the vertex buffer
    Float32 triangleData[18] = { -0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0, -0.5, 0.0,
                                 1.0,  0.0, 1.0, 0.5, 0.5, 0.0, 0.0, 1.0,  1.0 };
    vb->loadDataRaw(triangleData, 18 * sizeof(Float32));

    VertexLayout layout({
        {DataType::Float32, 2}, //vertex
        {DataType::Float32, 4} //color
    });

    Mesh * mesh = device->createMesh(&vb, &layout, 1).ensure();
    PipelineVariable * moreCol = pipe->variable("moreCol");
    moreCol->setVec4f(1.0, 0.0, 0.9, 1.0);

    while (!window.shouldClose())
    {
        luke::pollEvents();

        RenderPass * pass = device->beginPass(ClearSettings(1.0, 1.0, 0.0, 1.0));
        pass->setViewport(0, 0, window.widthInPixels(), window.heightInPixels());
        pass->drawMesh(mesh, pipe, 0, 3, VertexDrawMode::Triangles);
        
        auto err = device->endPass(pass);
        RETURN_ON_ERR(err);

        window.swapBuffers();
    }

    destroyRenderDevice(device);

    return EXIT_SUCCESS;
}