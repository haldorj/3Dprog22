#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <QOpenGLFunctions_4_1_Core>

#include "stb_image.h"

#include "visualobject.h"
#include "texture.h"

class HeightMap : public VisualObject
{
public:
    HeightMap();
    HeightMap(char* fileLoc);

    void LoadHeightMap();

    void GenerateTerrain(unsigned char *data);

    void init(GLint matrixUniform) override;
    void draw() override;
private:
    int width, height, nrChannels;
    char* fileLocation;
};

#endif // HEIGHTMAP_H
