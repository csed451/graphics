#pragma once

#include <vector>
#include "core/base/object.h"
#include "core/base/scene_node.h"

template <typename T>
class ObjectPool : public Object{
private:
    std::vector<T*> pool;
    std::vector<T*> available;
public:
    ObjectPool(size_t size) : Object() {
        set_parent(&sceneRoot);
        for (size_t i = 0; i < size; ++i) {
            pool.push_back(new T());
            available.push_back(pool.back());
            pool.back()->set_isActive(false);
            pool.back()->set_parent(this);
        }
    }

    const std::vector<T*>& get_pool() const { return pool; }

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

    void update_logic([[maybe_unused]] float deltaTime) override {
        for (auto obj : pool)
            if (obj->get_isActive() && is_outside_window(obj->get_pos()))
                release(obj);
    }

    void draw_shape() const override {};

    ~ObjectPool() {
        for (auto obj : pool) {
            obj->set_parent(nullptr);
            delete obj;
        }
    }
};
