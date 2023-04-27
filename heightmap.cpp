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
    if (mapData)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    GenerateTerrain(mapData);
}

void HeightMap::GenerateTerrain(unsigned char *data)
{
    // vertex generation
    float yScale = 8.0f / 256.0f;
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
            d = d * mapScale;
            mVertices.push_back(Vertex{d.x, d.z, d.y + 2.2f});
        }
    }
    // After building up the vertex array, we can release the height map from memory.
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
    // Calculate normals for lighting
    terrainCalcAvgNormalsSmooth();

    numStrips = (height-1)/rez;
    numTrisPerStrip = (width/rez)*2-2;

    int numTriangles = numStrips * numTrisPerStrip ;

    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numTriangles << " triangles total" << std::endl;
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
                       numTrisPerStrip+2,   // number of indices to render
                       GL_UNSIGNED_INT,     // index data type
                       (void*)(sizeof(unsigned) * (numTrisPerStrip+2) * strip)); // offset to starting index
    }
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void HeightMap::terrainCalcAvgNormalsSmooth()
{
    for (unsigned i = 0; i < mIndices.size() - 2; i += 2)
    {
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

void HeightMap::terrainCalcAvgNormalsFlat()
{
    for (unsigned i = 0; i < mIndices.size() - 2; i ++)
    {

        unsigned int in0, in1, in2;
        if (i % 2 == 0)
        {
            in0 = mIndices[i];
            in1 = mIndices[i + 1];
            in2 = mIndices[i + 2];
        }
        else
        {
            in0 = mIndices[i + 2];
            in1 = mIndices[i + 1];
            in2 = mIndices[i];
        }

        glm::vec3 v1(mVertices[in1].getX() - mVertices[in0].getX(),
                     mVertices[in1].getY() - mVertices[in0].getY(),
                     mVertices[in1].getZ() - mVertices[in0].getZ());

        glm::vec3 v2(mVertices[in2].getX() - mVertices[in0].getX(),
                     mVertices[in2].getY() - mVertices[in0].getY(),
                     mVertices[in2].getZ() - mVertices[in0].getZ());

        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        mVertices[in0].setNorm(normal.x, normal.y, normal.z);
        mVertices[in1].setNorm(normal.x, normal.y, normal.z);
        mVertices[in2].setNorm(normal.x, normal.y, normal.z);
    }
}

float HeightMap::GetSurfaceHeight(const glm::vec3 p)
{
    // Loop through each triangle in the mesh.
    for(unsigned strip = 0; strip < numStrips; strip++)
    {
        for (unsigned i = 0; i < numTrisPerStrip + 2; i ++)
        {
            unsigned int v0, v1, v2;
            if (i % 2 == 0)
            {
                v0 = mIndices[strip * (numTrisPerStrip + 2) + i];
                v1 = mIndices[strip * (numTrisPerStrip + 2) + i + 1];
                v2 = mIndices[strip * (numTrisPerStrip + 2) + i + 2];
            }
            else
            {
                v0 = mIndices[strip * (numTrisPerStrip + 2) + i + 2];
                v1 = mIndices[strip * (numTrisPerStrip + 2) + i + 1];
                v2 = mIndices[strip * (numTrisPerStrip + 2) + i];
            }

            glm::vec3 p0 = getVertex(v0);
            glm::vec3 p1 = getVertex(v1);
            glm::vec3 p2 = getVertex(v2);

            glm::vec3 baryCoords = barycentricCoordinates(p0, p1, p2, p);

            // Check if the player's position is inside the triangle.
            if (baryCoords.x >= 0.0f && baryCoords.y >= 0.0f && baryCoords.z >= 0.0f)
            {
                // The player's position is inside the triangle.
                // Calculate the height of the surface at the player's position.
                float height = baryCoords.x * p0.z + baryCoords.y * p1.z + baryCoords.z * p2.z;

                // Return the height as the height of the surface at the player's position.
                return height;
            }
        }
    }
    return 0;
}


glm::vec3 HeightMap::barycentricCoordinates(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const glm::vec2 &pt)
{
    glm::vec2 p12 = p2 - p1;
    glm::vec2 p13 = p3 - p1;
    glm::vec3 n = glm::vec3(glm::cross(glm::vec3(p12, 0.0f), glm::vec3(p13, 0.0f)));
    float areal_123 = glm::length(n); // double area
    glm::vec3 baryc;
    // u
    glm::vec2 p = p2 - pt;
    glm::vec2 q = p3 - pt;
    n = glm::vec3(glm::cross(glm::vec3(p, 0.0f), glm::vec3(q, 0.0f)));
    baryc.x = n.z / areal_123;
    // v
    p = p3 - pt;
    q = p1 - pt;
    n = glm::vec3(glm::cross(glm::vec3(p, 0.0f), glm::vec3(q, 0.0f)));
    baryc.y = n.z / areal_123;
    // w
    p = p1 - pt;
    q = p2 - pt;
    n = glm::vec3(glm::cross(glm::vec3(p, 0.0f), glm::vec3(q, 0.0f)));
    baryc.z = n.z / areal_123;

    return baryc;
}

glm::vec3 HeightMap::getVertex(int index)
{
    glm::vec3 v(mVertices[index].getX(),
                mVertices[index].getY(),
                mVertices[index].getZ());

    return v;
}

unsigned int HeightMap::getIndex(int triangleIndex, int vertexIndex)
{
    return mIndices[triangleIndex * 2 + vertexIndex];
}
