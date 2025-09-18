#pragma once

#include <vector>

template <typename T>
class ObjectPool {
private:
    std::vector<T*> pool;
    std::vector<T*> available;
public:
    ObjectPool(size_t size) {
        for (size_t i = 0; i < size; ++i) {
            pool.push_back(new T());
            available.push_back(pool.back());
        }
    }

    T* acquire() {
        if (available.empty()) 
            return nullptr;
        T* obj = available.back();
        available.pop_back();
        obj->set_isActive(true);
        return obj;
    }

    void release(T* obj) {
        available.push_back(obj);
        obj->set_isActive(false);
    }

    void update(float deltaTime) {
        for (auto obj : pool)
            if (obj->get_isActive())
                obj->update(deltaTime);
    }

    void draw() {
        for (auto obj : pool)
            if (obj->get_isActive())
                obj->draw();
    }

    ~ObjectPool() {
        for (auto obj : pool) 
            delete obj;
    }
};