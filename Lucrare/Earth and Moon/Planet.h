#include "vbomesh.h"
#include "cTexture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <GL\glew.h>
#include <GL\freeglut.h>

using glm::mat4;
using glm::vec3;

class Planet
{
private:
  float radius;
  float distance;
  float velocity;
  
  VBOMesh *sphere;
  cTexture texture;
  mat4 model;
public:
  Planet();
  Planet(float rad, float dist, const char* tex);
  Planet(float rad, float dist, VBOMesh obj, const char * tex);
  ~Planet();
  mat4 Update(float angle);
  void Render();
};
