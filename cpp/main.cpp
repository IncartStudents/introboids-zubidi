#include "UseImGui.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace std;

#ifndef PI 
#define PI 3.14159265358979323846 
#endif

int W = 0;
int H = 0;

struct Boid {
    float x, y; // позиция
    float vx, vy; // скорость
    float ax, ay; // ускорение
    float size; // размер боид
    float visualRange = 0.2f; // диапазон видимости других боид
    float minDistance = 0.05f; // минимальное расстояние до других боид

    Boid(float x_, float y_, float size_) : x(x_), y(y_), size(size_) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * PI;
        vx = cos(angle) * 0.01f;
        vy = sin(angle) * 0.01f;
        ax = 0.0f;
        ay = 0.0f;
    }

    // Полет к центру 
    void Cohesion(const vector<Boid>& boids) {
        float centerX = 0.0f, centerY = 0.0f;
        int numNeighbors = 0;
        float centeringFactor = 0.002f;

        for (const auto& other : boids) {
            if (&other != this && distanceTo(other) < visualRange) {
                centerX += other.x;
                centerY += other.y;
                numNeighbors++;
            }
        }

        if (numNeighbors > 0) {
            centerX /= numNeighbors;
            centerY /= numNeighbors;
            ax += (centerX - x) * centeringFactor;
            ay += (centerY - y) * centeringFactor;
        }
    }

    // Избегание столкновений 
    void Separation(const vector<Boid>& boids) {
        float moveX = 0.0f, moveY = 0.0f;
        float avoidFactor = 0.05f;

        for (const auto& other : boids) {
            if (&other != this && distanceTo(other) < minDistance) {
                moveX += x - other.x;
                moveY += y - other.y;
            }
        }

        ax += moveX * avoidFactor;
        ay += moveY * avoidFactor;
    }

    // Выравнивание скорости 
    void Alignment(const vector<Boid>& boids) {
        float avgVX = 0.0f, avgVY = 0.0f;
        int numNeighbors = 0;
        float matchingFactor = 0.005f;

        for (const auto& other : boids) {
            if (&other != this && distanceTo(other) < visualRange) {
                avgVX += other.vx;
                avgVY += other.vy;
                numNeighbors++;
            }
        }

        if (numNeighbors > 0) {
            avgVX /= numNeighbors;
            avgVY /= numNeighbors;
            ax += (avgVX - vx) * matchingFactor;
            ay += (avgVY - vy) * matchingFactor;
        }
    }

    // Разворот перед границами экрана
    void keepWithinBounds() {
        float margin = 0.1f;
        float turnFactor = 0.01f;

        if (x < -1.0f + margin) ax += turnFactor;
        if (x > 1.0f - margin) ax -= turnFactor;
        if (y < -1.0f + margin) ay += turnFactor;
        if (y > 1.0f - margin) ay -= turnFactor;
    }

    // Ограничение максимальной скорости
    void limitSpeed() {
        float speedLimit = 0.05f;
        float speed = sqrt(vx * vx + vy * vy);

        if (speed > speedLimit) {
            vx = (vx / speed) * speedLimit;
            vy = (vy / speed) * speedLimit;
        }
    }

    // Обновление положения боид
    void update(const vector<Boid>& boids) {
        Cohesion(boids);
        Separation(boids);
        Alignment(boids);
        keepWithinBounds();
        limitSpeed();

        // Обновляем скорость и позицию
        vx += ax;
        vy += ay;
        x += vx;
        y += vy;

        // Сбрасываем ускорение
        ax = 0.0f;
        ay = 0.0f;
    }

    // Отрисовка боид
    void draw() const {
        float angle = atan2(vy, vx);
        float cosA = cos(angle);
        float sinA = sin(angle);

        glBegin(GL_TRIANGLES);
        glVertex2f(x + cosA * size, y + sinA * size);
        glVertex2f(x + cosA * (-size / 2) - sinA * (size / 2), y + sinA * (-size / 2) + cosA * (size / 2));
        glVertex2f(x + cosA * (-size / 2) + sinA * (size / 2), y + sinA * (-size / 2) - cosA * (size / 2));
        glEnd();
    }

    // Вычисление расстояния между боид
    float distanceTo(const Boid& other) const {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

class CustomImGui : public UseImGui {
public:
    vector<Boid> boids;

    CustomImGui() {
        for (int i = 0; i < 50; ++i) {
            boids.emplace_back(
                static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
                static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
                0.05f
            );
        }
    }

    virtual void Update() override {
        for (auto& boid : boids) {
            boid.update(boids);
            boid.draw();
        }
    }
};


int main() {
    if (!glfwInit()) return 1;

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Boid Simulation", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw("Unable to context to OpenGL");

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
