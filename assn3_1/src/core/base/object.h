#pragma once

#include <GL/freeglut.h>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "core/globals/game_constants.h"
#include "core/render/mesh.h"

class Object {
private:
    glm::mat4 modelMatrix;
    glm::vec3 center;
    Object* parent;
    bool isLocal = true;
    bool isActive = true;
    bool isVisible = true;
    float hitboxRadius = 1;
    std::vector<Object*> children;
    std::shared_ptr<Mesh> mesh;
    glm::mat4 meshLocalTransform = glm::mat4(1.0f);
    glm::vec4 meshBaseTint = glm::vec4(1.0f);

    void detach_from_parent();
    void add_child_reference(Object* child);
    void remove_child_reference(Object* child);
    void draw_mesh_internal() const;
protected:
    virtual glm::vec4 resolve_mesh_tint() const;
public:
    Object(glm::vec3 _pos=ZERO, GLfloat _angle=0, glm::vec3 _axis=UP, glm::vec3 _size=glm::vec3(1), glm::vec3 _center=ZERO);
    virtual ~Object();
    
    /* getter */
    glm::mat4 get_modelMatrix() const { return modelMatrix; }
    glm::vec3 get_center() const { return center; }
    Object* get_parent() const { return parent; }
    bool get_isLocal() const { return isLocal; }
    bool get_isActive() const { return isActive; }
    bool get_isVisible() const { return isVisible; }
    float get_hitboxRadius() const { return hitboxRadius; }
    
    const std::shared_ptr<Mesh>& get_mesh() const { return mesh; }
    const glm::mat4& get_mesh_transform() const { return meshLocalTransform; }
    const glm::vec4& get_mesh_color() const { return meshBaseTint; }

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

    void set_mesh(const std::shared_ptr<Mesh>& m) { mesh = m; }
    bool load_mesh(const std::string& path);
    void set_mesh_transform(const glm::mat4& transform) { meshLocalTransform = transform; }
    void set_mesh_color(const glm::vec4& color) { meshBaseTint = color; }

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

    
    virtual void update(float /*deltaTime*/) {};
    void draw() const;
    virtual void draw_shape() const {};

    bool check_collision(Object* other);

    void clear_children();
    const std::vector<Object*>& get_children() const { return children; }
};
