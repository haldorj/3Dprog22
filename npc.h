#ifndef NPC_H
#define NPC_H

#include "visualobject.h"

class NPC : public VisualObject
{
public:
    NPC();
    ~NPC() {};
    void init(GLint matrixUniform) override;
    void draw() override;
    void move(float x, float y, float z) override;
private:
   float mx, my, mz; // posisjon
};

#endif // NPC_H
