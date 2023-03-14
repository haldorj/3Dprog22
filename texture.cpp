#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>

#include "Texture.h"
#include "logger.h" //only needed for logging to our Output Log

Texture::Texture()
{
    initializeOpenGLFunctions();
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = nullptr;
    makeDummyTexture();
}

Texture::Texture(char* fileLoc)
{
    initializeOpenGLFunctions();
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = fileLoc;
}

Texture::~Texture()
{
    ClearTexture();
}

void Texture::LoadTexture()
{
    unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
    if (!texData)
    {
        printf("failed to find %s\n", fileLocation);
        return;
    }
    glEnable(GL_TEXTURE_2D);
    // 1. Generate a texture object.
    glGenTextures(1, &textureID);
    // 2. Bind the texture object to a target.
    glBindTexture(GL_TEXTURE_2D, textureID);
    // 3. Configure texture state.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping X axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping Y axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 4. Load the texture image into the texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(texData);
}

void Texture::UseTexture()
{
    // 5. Make the texture unit active
    glActiveTexture(GL_TEXTURE0);
    // 6. Bind the texture object to the texture unit.
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture()
{
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = nullptr;
}

void Texture::makeDummyTexture()
{
    printf("Making dummy texture \n");
    for (int i=0; i<16; i++)
        pixels[i] = 0;
    pixels[0] = 255;
    pixels[4] = 255;
    pixels[8] = 255;
    pixels[9] = 255;
    pixels[10] = 255;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    std::cout << "Texture id: " << std::to_string(textureID) << "\n";
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 reinterpret_cast<const GLvoid*>(pixels));
}
