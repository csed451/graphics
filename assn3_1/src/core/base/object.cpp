#include "core/base/object.h"
#include "core/globals/camera.h"
#include <algorithm>

Object::Object(glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 _center)
: modelMatrix(glm::mat4(1.0f)),
  center(_center),
  parent(nullptr),
  isLocal(true) {
    init(_pos, _angle, _axis, _size, _center);
}

Object::~Object() {
    clear_children();
    detach_from_parent();
}

glm::vec3 Object::get_pos() const {
    return glm::vec3(get_finalMatrix()[3]);
}

glm::vec3 Object::get_size() const {
    glm::mat3 rotationMatrix = glm::mat3(get_finalMatrix());
    GLfloat scaleX = glm::length(rotationMatrix[0]);
    GLfloat scaleY = glm::length(rotationMatrix[1]);
    GLfloat scaleZ = glm::length(rotationMatrix[2]);
    return glm::vec3(scaleX, scaleY, scaleZ);
}

glm::quat Object::get_quat() const {
    // extract only a direction of modelMatrix
    glm::mat3 rotationMatrix = glm::mat3(get_finalMatrix());
    rotationMatrix[0] = glm::normalize(rotationMatrix[0]);
    rotationMatrix[1] = glm::normalize(rotationMatrix[1]);
    rotationMatrix[2] = glm::normalize(rotationMatrix[2]);

    return glm::quat_cast(rotationMatrix);
}

glm::mat4 Object::get_finalMatrix() const {
    return parent ? parent->get_finalMatrix() * modelMatrix : modelMatrix;
}

void Object::set_parent(Object* _parent, bool fix) {
    if (parent == _parent)
        return;

    if (parent)
        parent->remove_child_reference(this);

    parent = _parent;

    if (parent) {
        if (fix)
            modelMatrix = glm::inverse(parent->get_finalMatrix()) * modelMatrix;
        isLocal = false;
        parent->add_child_reference(this);
    }
    else {
        modelMatrix = get_finalMatrix();
        isLocal = true;
    }
}

void Object::init(glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 /*_center*/) {
    modelMatrix = glm::mat4(1.0f);
    translate_world(_pos);
    rotate_local(_angle, _axis);
    scale_local(_size);
}

void Object::translate(glm::vec3 v) {
    if (isLocal) 
        translate_local(v);
    else
        translate_world(v);
}

void Object::rotate(GLfloat angle, glm::vec3 axis) {
    if (isLocal) 
        rotate_local(angle, axis);
    else
        rotate_world(angle, axis);
}

void Object::scale(glm::vec3 v) {
    if (isLocal) 
        scale_local(v);
    else
        scale_world(v);
}

void Object::translate_local(glm::vec3 v) {
    modelMatrix = glm::translate(modelMatrix, v);
}

void Object::rotate_local(GLfloat angle, glm::vec3 axis) {
    translate_local(center);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
    translate_local(-center);
}

void Object::scale_local(glm::vec3 v) {
    translate(center);
    modelMatrix = glm::scale(modelMatrix, v);
    translate(-center);
}

void Object::translate_world(glm::vec3 v) {
    glm::mat4 mT = glm::translate(glm::mat4(1.0f), v);
    modelMatrix = mT * modelMatrix;
}

void Object::rotate_world(GLfloat angle, glm::vec3 axis) {
    glm::mat4 mR = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    modelMatrix = mR * modelMatrix;
}

void Object::scale_world(glm::vec3 v) {
    glm::mat4 mS = glm::scale(glm::mat4(1.0f), v);
    modelMatrix = mS * modelMatrix;
}

void Object::draw() const {
    if (isActive && isVisible) {
        glPushMatrix();
        glm::mat4 finalMatrix = get_finalMatrix();
        glm::mat4 mvp = cameraMatrix * finalMatrix;
        glLoadMatrixf(glm::value_ptr(mvp));

        draw_shape();

        for (auto child : children)
            if (child)
                child->draw();

        glPopMatrix();
    }
}

bool Object::check_collision(Object* other) {
    float distance = glm::distance(get_pos(), other->get_pos());
    return distance <= get_hitboxRadius() + other->get_hitboxRadius(); 
}

void Object::clear_children() {
    std::vector<Object*> temp = children;
    children.clear();
    for (auto* child : temp)
        if (child)
            child->set_parent(nullptr);
}

void Object::detach_from_parent() {
    if (!parent)
        return;
    parent->remove_child_reference(this);
    parent = nullptr;
    isLocal = true;
}

void Object::add_child_reference(Object* child) {
    if (!child)
        return;
    if (std::find(children.begin(), children.end(), child) == children.end())
        children.push_back(child);
}

void Object::remove_child_reference(Object* child) {
    if (!child)
        return;
    auto it = std::remove(children.begin(), children.end(), child);
    if (it != children.end())
        children.erase(it, children.end());
}
