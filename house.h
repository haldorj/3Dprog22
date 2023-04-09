#ifndef HOUSE_H
#define HOUSE_H

#include <glm/glm.hpp>
#include <visualobject.h>

class House : public VisualObject
{
public:
    House();
    ~House() {};
    void init(GLint matrixUniform) override;
    void draw() override;
    void initCubeGeometry();
    void move(float x, float y, float z) override;

 private:
    float mx, my, mz; // posisjon
};

#endif // HOUSE_H
