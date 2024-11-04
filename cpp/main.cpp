#include "UseImGui.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace std;

int W = 0;
int H = 0;

struct Boid {
    float x, y; // позиция
    float vx, vy; // скорость
    float size; // размер

    Boid(float x_, float y_, float size_) : x(x_), y(y_), size(size_) {
        vx = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        vy = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
    }

    void update() {
        x += vx * 0.01f;
        y += vy * 0.01f;

        // избегаем столкновения с краями экрана и разворачиваемся
        if (x > 1.0f) {
            x = 1.0f;
            vx = -vx;
        }
        if (x < -1.0f) {
            x = -1.0f;
            vx = -vx;
        }
        if (y > 1.0f) {
            y = 1.0f;
            vy = -vy;
        }
        if (y < -1.0f) {
            y = -1.0f;
            vy = -vy;
        }
    }

    void draw() const {
        float angle = atan2(vy, vx);
        float cosA = cos(angle);
        float sinA = sin(angle);

        glBegin(GL_TRIANGLES);
        glVertex2f(x + cosA * size, y + sinA * size); // вершина треугольника
        glVertex2f(x + cosA * (-size/2) - sinA * (size/2), y + sinA * (-size/2) + cosA * (size/2));
        glVertex2f(x + cosA * (-size/2) + sinA * (size/2), y + sinA * (-size/2) - cosA * (size/2));
        glEnd();
    }
};

class CustomImGui : public UseImGui {
public:
    vector<Boid> boids;

    CustomImGui() {
        for (int i = 0; i < 30; ++i) { // Создаем 30 "птичек"
            boids.emplace_back(
                static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
                static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
                0.05f // размер "птичек"
            );
        }
    }

    virtual void Update() override {
        for (auto& boid : boids) {
            boid.update();
            boid.draw();
        }
    }
};

int main() {
    // Setup window
    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Boid Simulation", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw("Unable to context to OpenGL");

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    auto* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    W = mode->width;
    H = mode->height;

    CustomImGui myimgui;
    myimgui.Init(window, glsl_version);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        myimgui.NewFrame();
        myimgui.Update();
        myimgui.Render();
        glfwSwapBuffers(window);
    }
    myimgui.Shutdown();

    return 0;
}
