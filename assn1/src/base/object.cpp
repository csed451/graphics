#include "object.h"

Object::Object(glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 _center)
: modelMatrix(glm::mat4(1.0f)), center(_center), isLocal(true), parent(nullptr) {
    translate_world(_pos);
    scale_local(_size);
    rotate_local(_angle, _axis);
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
    return parent ? parent->get_modelMatrix() * modelMatrix : modelMatrix;
}

void Object::set_parent(Object* _parent) {
    if (_parent) {
        modelMatrix = glm::inverse(_parent->get_modelMatrix()) * modelMatrix;
        isLocal = false;
    }
    else {
        modelMatrix = get_finalMatrix();
        isLocal = true;
    }

    parent = _parent;
}

void Object::init(glm::vec3 _pos, GLfloat _angle, glm::vec3 _axis, glm::vec3 _size, glm::vec3 _center) {
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
        glLoadMatrixf(glm::value_ptr(get_finalMatrix()));

        draw_shape();

        glPopMatrix();
    }
}

bool Object::check_collision(Object* other) {
    float distance = glm::distance(get_pos(), other->get_pos());
    return distance <= get_hitboxRadius() + other->get_hitboxRadius(); 
}
