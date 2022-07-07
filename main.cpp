#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "parser\parser.h"
#include "render\Scene.h"
#include "lattice/Lattice.h"

const int defWidth = 1920;
const int defHeight = 1080;

Rendering::Scene scene;

void mouseCallback(GLFWwindow* wnd, double xpos, double ypos)
{
    scene.mouseCallback(wnd, xpos, ypos);
}

GLFWwindow* windowInit(int wndWidth = defWidth, int wndHeight = defHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(wndWidth, wndHeight, "OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Window could not be opened." << std::endl;
        glfwTerminate();
        return window;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetWindowSizeLimits(window, wndWidth, wndHeight, wndWidth, wndHeight);

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    //glfwSetScrollCallback(window, mouse_scroll_callback);

    glewInit();
    glViewport(0, 0, wndWidth, wndHeight);

    //glfwMaximizeWindow(window);

    return window;
}

void singleObjRender(const std::vector<std::string>::const_iterator& first, const std::vector<std::string>::const_iterator& last)
{
    const std::string& argument = *first;

    GLFWwindow *const window = windowInit();
    if(window == nullptr) {
        std::cerr << "Rendering failure" << std::endl;
        return;
    }

    const Shading::Shader vertex("../shaders/shader.vert");
    const Shading::Shader fragment("../shaders/shader.frag");
    const Shading::Program objShader(vertex, fragment);

    PolygonMesh::Mesh object(argument);
    object.allocateBufferData();
    object.load();

    Rendering::RenderObject::Material material;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.camera.zoom), (float)defWidth / defHeight, 0.1f, 100.0f);

    Rendering::RenderObject obj_fin(object, material, objShader);
    objShader.use();
    objShader.set("material.opacity", 1.0f);

    obj_fin.modelMatrix = glm::scale(obj_fin.modelMatrix, glm::vec3(0.05f));

    scene.addObject(obj_fin);

    float deltaTime;
    float lastFrame = 0.0f;

    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.keyboardCallback(window, deltaTime);

        scene.frame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

#include <chrono>

void latticeDemo(const std::vector<std::string>::const_iterator& first, const std::vector<std::string>::const_iterator& last)
{
    GLFWwindow* window = windowInit();
    if(window == nullptr)
    {
        std::cerr << "Rendering failure" << std::endl;
        return;
    }

    Shading::Shader vertex("../shaders/shader.vert");
    Shading::Shader fragment("../shaders/shader.frag");
    Shading::Program objShader(vertex, fragment);

    objShader.use();
    objShader.set("material.opacity", 1.0f);

    std::vector<std::string>::const_iterator iter = first;

    auto ts = std::chrono::system_clock::now();
    PolygonMesh::Mesh rest(*iter);
    std::cout << "Time to read base object ["<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - ts).count() << " ms]" << std::endl;
    iter = iter + 1;
    ts = std::chrono::system_clock::now();
    PolygonMesh::Mesh deformed(*iter);
    std::cout << "Time to read deformed object ["<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - ts).count() << " ms]" << std::endl;
    iter = iter + 1;
    ts = std::chrono::system_clock::now();
    PolygonMesh::Mesh toDeform(*iter);
    std::cout << "Time to read object to deform ["<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - ts).count() << " ms]" << std::endl;

    LatticeOperation::Lattice lattice(rest, deformed, toDeform);

    ts = std::chrono::system_clock::now();
    PolygonMesh::Mesh result = lattice.calculate(8.0f);
    std::cout << "Time to perform lattice deformation ["<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - ts).count() << " ms]" << std::endl;

    rest.allocateBufferData();
    rest.load();
    deformed.allocateBufferData();
    deformed.load();
    toDeform.allocateBufferData();
    toDeform.load();
    result.allocateBufferData();
    result.load();

    Rendering::RenderObject::Material material;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.camera.zoom), (float)defWidth / defHeight, 0.1f, 100.0f);
    Rendering::RenderObject restRender(rest, material, objShader);

    restRender.modelMatrix = glm::scale(restRender.modelMatrix, glm::vec3(0.05f));


    scene.addObject(restRender);

    Rendering::RenderObject deformedRender(deformed, material, objShader);

    deformedRender.modelMatrix = glm::scale(deformedRender.modelMatrix, glm::vec3(0.05f));
    scene.addObject(deformedRender);

    material.diffuse = glm::vec3(0.4f);
    material.specular = glm::vec3(0.0f);

    Rendering::RenderObject toDeformRender(toDeform, material, objShader);

    toDeformRender.modelMatrix = glm::scale(toDeformRender.modelMatrix, glm::vec3(0.05f));
    scene.addObject(toDeformRender);

    Rendering::RenderObject resultRender(result, material, objShader);

    resultRender.modelMatrix = glm::scale(resultRender.modelMatrix, glm::vec3(0.05f));
    scene.addObject(resultRender);

    float deltaTime;
    float lastFrame = 0.0f;

    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.keyboardCallback(window, deltaTime);

        scene.frame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

int main(int argc, char* argv[])
{
    try {
        parser parser;

        parser::Command renderCommand('r', singleObjRender, 1);
        parser::Command latticeCommand('l', latticeDemo, 3);

        parser.addCommand(renderCommand);
        parser.addCommand(latticeCommand);

        parser.parse(argc - 1, argv + 1);
    } catch (std::exception& e) {
        std::cerr << e.what();
    }

    return 0;
}