#pragma once

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "core/globals/game_constants.h"

class Object {
private:
    glm::mat4 modelMatrix;
    glm::vec3 center;
    Object* parent;
    bool isLocal = true;
    bool isActive = true;
    bool isVisible = true;
    float hitboxRadius = 1;
public:
    Object(glm::vec3 _pos=ZERO, GLfloat _angle=0, glm::vec3 _axis=UP, glm::vec3 _size=glm::vec3(1), glm::vec3 _center=ZERO);
    
    /* getter */
    glm::mat4 get_modelMatrix() const { return modelMatrix; }
    glm::vec3 get_center() const { return center; }
    Object* get_parent() const { return parent; }
    bool get_isLocal() const { return isLocal; }
    bool get_isActive() const { return isActive; }
    bool get_isVisible() const { return isVisible; }
    bool get_hitboxRadius() const { return hitboxRadius; }

    /* additional information */
    glm::vec3 get_pos() const;
    glm::vec3 get_size() const;
    glm::quat get_quat() const;
    glm::mat4 get_finalMatrix() const;

    /* setter */
    void set_modelMatrix(glm::mat4 m) { modelMatrix = m; }
    void set_center(glm::vec3 v) { center = v; }
    void set_parent(Object* _parent, bool fix=false);
    void set_isLocal(bool b) { isLocal = b; }
    void set_isActive(bool b) { isActive = b; }
    void set_isVisible(bool b) { isVisible = b; }
    void set_hitboxRadius(float r) { hitboxRadius = r; }

    void init(glm::vec3 _pos=ZERO, GLfloat _angle=0, glm::vec3 _axis=UP, glm::vec3 _size=glm::vec3(1), glm::vec3 _center=ZERO);

    void translate(glm::vec3 v);
    void rotate(GLfloat angle, glm::vec3 axis);
    void scale(glm::vec3 v);

    /* defined in local coordinate system */
    void translate_local(glm::vec3 v);
    void rotate_local(GLfloat angle, glm::vec3 v);
    void scale_local(glm::vec3 v);

    /* defined in world coordinate system */
    void translate_world(glm::vec3 v);
    void rotate_world(GLfloat angle, glm::vec3 axis);
    void scale_world(glm::vec3 v);

    
    void update(float deltaTime) {};
    void draw() const;
    virtual void draw_shape() const = 0;

    bool check_collision(Object* other);
};