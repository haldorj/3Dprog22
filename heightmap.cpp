#define STB_IMAGE_IMPLEMENTATION

#include "heightmap.h"

HeightMap::HeightMap()
{
    initializeOpenGLFunctions();
    width = 0;
    height = 0;
    fileLocation = nullptr;

    LoadHeightMap();
}

HeightMap::HeightMap(char* fileLoc)
{
    initializeOpenGLFunctions();
    width = 0;
    height = 0;
    fileLocation = fileLoc;

    LoadHeightMap();
}

void HeightMap::LoadHeightMap()
{
    unsigned char *data = stbi_load("heightmaps/iceland_heightmap.png", &width, &height, &nChannels, 0);

    // vertex generation
    std::vector<float> vertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nChannels;
            // raw height at coordinate
            unsigned char y = texel[0];

            // vertex
            vertices.push_back( -height/2.0f + i );        // v.x
            vertices.push_back( (int)y * yScale - yShift); // v.y
            vertices.push_back( -width/2.0f + j);        // v.z
        }
    }

    // index generation
    std::vector<unsigned int> indices;
    for(unsigned int i = 0; i < height-1; i++)       // for each row a.k.a. each strip
    {
        for(unsigned int j = 0; j < width; j++)      // for each column
        {
            for(unsigned int k = 0; k < 2; k++)      // for each side of the strip
            {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    const unsigned int NUM_STRIPS = height-1;
    const unsigned int NUM_VERTS_PER_STRIP = width*2;


    stbi_image_free(data);
}

void HeightMap::init(GLint matrixUniform)
{
    // register VAO
    GLuint terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),       // size of vertices buffer
                 &vertices[0],                          // pointer to first element
                 GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int), // size of indices buffer
                 &indices[0],                           // pointer to first element
                 GL_STATIC_DRAW);
}

void HeightMap::draw()
{
    // draw mesh
    glBindVertexArray(terrainVAO);
    // render the mesh triangle strip by triangle strip - each row at a time
    for(unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                       NUM_VERTS_PER_STRIP, // number of indices to render
                       GL_UNSIGNED_INT,     // index data type
                       (void*)(sizeof(unsigned int)
                                 * NUM_VERTS_PER_STRIP
                                 * strip)); // offset to starting index
    }
}
