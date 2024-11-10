#pragma once
#include <vector>


class Boid {
public:
    float x, y;  // Позиция

    // Конструктор: задает координаты boid'а
    Boid(float init_x, float init_y) : x(init_x), y(init_y) {}
};

// Функция возвращает вектор с заданными boid-объектами
std::vector<Boid> CreateBoids() {
    std::vector<Boid> boids = {
        Boid(50.0f, 50.0f),   
        Boid(100.0f, 150.0f), 
        Boid(200.0f, 100.0f), 
        Boid(300.0f, 300.0f), 
        Boid(400.0f, 200.0f), 
    };
    return boids;
}
