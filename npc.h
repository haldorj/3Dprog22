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

    // Getting path / movement of NPC
    void readFile(std::string filename);
    void fixPath();
    void moveNPC();
    void moveNPCFromFile();
private:
    int i = 0;
    float xi = -3;

    std::vector<Vertex> mPath;
    float mx, my, mz; // posisjon

    bool bForward = true;

    int shading=0;

    float mA = 0;
    float mB = 0;
    float mC = 0;
    float mD = 0;
};

#endif // NPC_H
