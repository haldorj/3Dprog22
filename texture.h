#ifndef TEXTURE_H
#define TEXTURE_H

#include "CustomFiles/CommonValues.h"

#include <QOpenGLFunctions_4_1_Core>

class Texture : protected QOpenGLFunctions_4_1_Core
{
    GLubyte pixels[16];     // For the standard texture from the no-parameter constructor
    ///Utility class Logger to write to Output Log in the application.
    class Logger *mLogger{nullptr};
public:
    Texture();
    Texture(char* fileLoc);
    ~Texture();

    bool LoadTexture();
    bool LoadTextureA();
    void UseTexture();
    void ClearTexture();
    void makeDummyTexture();

    GLuint id() const { return textureID; }
private:
    GLuint textureID;
    int width, height, bitDepth;
    char* fileLocation;
};

#endif // TEXTURE_H
