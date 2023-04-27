#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <QOpenGLFunctions_4_1_Core>

#include "visualobject.h"
#include "CustomFiles/CommonValues.h"

class HeightMap : public VisualObject
{
public:
    HeightMap();
    HeightMap(char* fileLoc);

    void LoadHeightMap();
    void GenerateTerrain(unsigned char *data);
    void terrainCalcAvgNormalsSmooth();
    void terrainCalcAvgNormalsFlat();

    float GetSurfaceHeight(glm::vec3 p);
    glm::vec3 barycentricCoordinates(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const glm::vec2 &pt);

    glm::vec3 getVertex(int index);
    unsigned int getIndex(int triangleIndex, int vertexIndex);

    void init(GLint matrixUniform) override;
    void draw() override;
private:
    float mapScale = 0.2;

    int width, height, nrChannels;
    char* fileLocation;

    int numStrips;
    int numTrisPerStrip;
    int numTriangles;
};

#endif // HEIGHTMAP_H
