#include "UseImGui.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace std;

#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif

int W = 0;
int H = 0;

struct Boid {
    float x, y; // позиция
    float vx, vy; // скорость
    float size; // размер
    float warning_distance; // дистанция предупреждения
    float ax, ay; // ускорение

    Boid(float x_, float y_, float size_, float warning_distance_) : x(x_), y(y_), size(size_), warning_distance(warning_distance_) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
        vx = cos(angle) * 0.01f;
        vy = sin(angle) * 0.01f;
        ax = 0.0f;
        ay = 0.0f;
    }

    void update() {
        vx += ax;
        vy += ay;

        // нормируем скорость
        float speed = sqrt(vx * vx + vy * vy);
        vx = (vx / speed) * 0.01f;
        vy = (vy / speed) * 0.01f;

        x += vx;
        y += vy;

        // избегаем столкновения с краями экрана и разворачиваемся до столкновения
        if (x > 1.0f - warning_distance) {
            ax = -0.005f; // замедляем и изменяем направление
        } else if (x < -1.0f + warning_distance) {
            ax = 0.005f; // замедляем и изменяем направление
        } else {
            ax = 0.0f; // сбрасываем ускорение если не вблизи границы
        }
		
        if (y > 1.0f - warning_distance) {
            ay = -0.005f; // замедляем и изменяем направление
        } else if (y < -1.0f + warning_distance) {
            ay = 0.005f; // замедляем и изменяем направление
        } else {
            ay = 0.0f; // сбрасываем ускорение если не вблизи границы
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
                0.05f, // размер "птичек"
                0.1f // дистанция предупреждения
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
