#ifndef NPC_H
#define NPC_H

#include "visualobject.h"

class NPC : public VisualObject
{
public:
    NPC() {};
    NPC(std::string path);
    ~NPC() {};
    void init(GLint matrixUniform) override;
    void draw() override;
    void move(float x, float y, float z) override;
    void move(float dt, QVector3D velocity) override;

    // Getting path / movement of NPC
    void readFile(std::string filename);
    void moveNPC();
private:
    int i = 1;

    std::vector<Vertex> mPath;
    float mx, my, mz; // posisjon

    bool AtStart = true;
};

#endif // NPC_H
