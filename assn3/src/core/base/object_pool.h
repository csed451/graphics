#pragma once

#include <vector>
#include "core/base/object.h"
#include "core/base/scene_context.h"

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
            pool.back()->set_isActive(false);
        }
    }

    const std::vector<T*>& get_pool() const { return pool; }

    T* acquire() {
        if (available.empty()) 
            return nullptr;
        T* obj = available.back();
        available.pop_back();
        obj->set_isActive(true);
        if (auto* root = get_scene_root())
            root->add_child(obj);
        return obj;
    }

    void release(T* obj) {
        if (auto* root = get_scene_root())
            obj->set_parent(nullptr);
        available.push_back(obj);
        obj->set_isActive(false);
    }

    void update(float deltaTime) {
        for (auto obj : pool) {
            if (obj->get_isActive()) {
                if (is_outside_window(obj->get_pos())) {
                    release(obj);
                } else {
                    obj->update(deltaTime);
                }
            }
        }
    }

    void draw() const {
        for (auto obj : pool)
            obj->draw();
    }

    ~ObjectPool() {
        for (auto obj : pool) {
            if (auto* root = get_scene_root())
                obj->set_parent(nullptr);
            delete obj;
        }
    }
};
