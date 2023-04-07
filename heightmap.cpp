#include "heightmap.h"

HeightMap::HeightMap()
{
    initializeOpenGLFunctions();

    width = 0;
    height = 0;
    nrChannels = 0;
    fileLocation = nullptr;

    numStrips = 0;
    numTrisPerStrip = 0;
}

HeightMap::HeightMap(char* fileLoc)
{
    initializeOpenGLFunctions();

    width = 0;
    height = 0;
    nrChannels = 0;
    fileLocation = fileLoc;

    numStrips = 0;
    numTrisPerStrip = 0;
}

void HeightMap::LoadHeightMap()
{
    // load height map texture

    unsigned char *mapData = stbi_load(fileLocation, &width, &height, &nrChannels, 0);
    if (!mapData)
    {
        printf("failed to find %s\n", fileLocation);
        return;
    }
    GenerateTerrain(mapData);
}

void HeightMap::GenerateTerrain(unsigned char *data)
{
    // vertex generation
    float yScale = 64.0f / 256.0f;
    float yShift = 16.0f;  // apply a scale+shift to the height data

    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nrChannels;
            // raw height at coordinate
            unsigned char y = texel[0];

            glm::vec3 d = glm::vec3(-height/2.0f + height*i/(float)height,// v.x
                                    (int) y * yScale - yShift,
                                    -width/2.0f + width*j/(float)width);

            // vertex
            mVertices.push_back(Vertex{d.x, d.z, d.y});
        }
    }

    // After building up the vertex array, we can release the height map from memory.
    std::cout << "Loaded " << mVertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);

    int rez = 1;
    for(unsigned i = 0; i < height-1; i += rez) // for each row a.k.a. each strip
    {
        for(unsigned j = 0; j < width; j += rez) // for each row a.k.a. each strip
        {
            for(unsigned k = 0; k < 2; k++)  // for each side of the strip
            {
                mIndices.push_back(j + width * (i + k * rez));
            }
        }
    }

    calcNormalsHmap();

    numStrips = (height-1)/rez;
    numTrisPerStrip = (width/rez)*2-2;

    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

}

void HeightMap::calcNormalsHmap()
{
    for (unsigned i = 0; i < mIndices.size() - 2; i += 2)
    {
        // Store each vertex of the current triangle
        unsigned int in0 = mIndices[i];
        unsigned int in1 = mIndices[i + 1];
        unsigned int in2 = mIndices[i + 2];

        glm::vec3 v1(mVertices[in1].getX() - mVertices[in0].getX(),
                     mVertices[in1].getY() - mVertices[in0].getY(),
                     mVertices[in1].getZ() - mVertices[in0].getZ());

        glm::vec3 v2(mVertices[in2].getX() - mVertices[in0].getX(),
                     mVertices[in2].getY() - mVertices[in0].getY(),
                     mVertices[in2].getZ() - mVertices[in0].getZ());

        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        mVertices[in0].setNorm(mVertices[in0].getNormX() + normal.x,
                               mVertices[in0].getNormY() + normal.y,
                               mVertices[in0].getNormZ() + normal.z);

        mVertices[in1].setNorm(mVertices[in1].getNormX() + normal.x,
                               mVertices[in1].getNormY() + normal.y,
                               mVertices[in1].getNormZ() + normal.z);

        mVertices[in2].setNorm(mVertices[in2].getNormX() + normal.x,
                               mVertices[in2].getNormY() + normal.y,
                               mVertices[in2].getNormZ() + normal.z);
    }
    for (unsigned i = 0; i < mVertices.size(); i++)
    {
        glm::vec3 vec(mVertices[i].getNormX(),
                      mVertices[i].getNormY(),
                      mVertices[i].getNormZ());
        vec = glm::normalize(vec);

        mVertices[i].setNorm(vec.x, vec.y, vec.z);
    }
}

void HeightMap::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    // Setup indexed draws
    glGenBuffers(1, &mIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
    glEnableVertexAttribArray(0);
    // color/normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),  (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mMatrix.scale(0.03);
}

void HeightMap::draw()
{
    glBindVertexArray( mVAO );
    // Bindbuffer, indexed draws
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    // DrawElements, indexed draws
    for(unsigned strip = 0; strip < numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                       numTrisPerStrip + 2,   // number of indices to render
                       GL_UNSIGNED_INT,     // index data type
                       (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
    }
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
