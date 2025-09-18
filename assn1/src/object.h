#pragma once

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object {
private:
    glm::mat4 modelMatrix;
    glm::vec3 center;
    bool isLocal;       // default: true
    Object* parent;

    bool isActive=false;
public:
    Object(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3());
    /* getter */
    glm::mat4 get_modelMatrix() const { return modelMatrix; }
    glm::vec3 get_center() const { return center; }
    bool get_isLocal() const { return isLocal; }
    Object* get_parent() const { return parent; }
    bool get_isActive() const { return isActive; }
    /* additional information */
    glm::vec3 get_pos() const;
    glm::vec3 get_size() const;
    glm::quat get_quat() const;
    glm::mat4 get_finalMatrix() const;
    /* setter */
    void set_modelMatrix(glm::mat4 m);
    void set_center(glm::vec3 v);
    void set_local(bool b);
    void set_parent(Object* _parent);
    void set_isActive(bool b) { isActive = b; }

    void init(glm::vec3 _pos=glm::vec3(), GLfloat _angle=0, glm::vec3 _axis=glm::vec3(1,0,0), glm::vec3 _size=glm::vec3(1), glm::vec3 _center=glm::vec3());

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
};