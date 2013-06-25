#include "Planet.h"

Planet::Planet() {}

Planet::Planet(float rad, float dist, const char * tex) {
    sphere = new VBOMesh("mesh/sphere.obj");
    texture.LoadFromFile(tex);
    radius = rad;
    distance = dist;
}

Planet::Planet(float rad, float dist, VBOMesh obj, const char * tex) {
    sphere = &obj;
    texture.LoadFromFile(tex);
}

Planet::~Planet() { }

mat4 Planet::Update(float angle) {
    model = mat4(1.0f);
  	model *= glm::rotate(model, angle*20*radius,0.0f,1.0f,0.0f);
  	model *=  glm::translate(distance, 0.0f, 0.0f);
  	model *= glm::scale(radius, radius, radius);
    return model;
}

void Planet::Render() {
  	glBindTexture(GL_TEXTURE_2D, texture.GetTextureHandle());
    sphere->render();
}