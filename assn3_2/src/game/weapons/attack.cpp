#include "attack.h"

void Attack::draw_shape() const {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glScalef(0.3f, 0.3f, 0.3f);
    get_mesh()->draw();
}

void Attack::update_logic(float deltaTime) {
    translate(velocity * deltaTime);
}