#include "heightmap.h"

HeightMap::HeightMap()
{
    initializeOpenGLFunctions();

    width = 0;
    height = 0;
    nrChannels = 0;
    fileLocation = nullptr;
}

HeightMap::HeightMap(char* fileLoc)
{
    initializeOpenGLFunctions();

    width = 0;
    height = 0;
    nrChannels = 0;
    fileLocation = fileLoc;
}

void HeightMap::LoadHeightMap()
{
    // load height map texture

    unsigned char *data = stbi_load("HeightMaps/iceland_heightmap.png", &width, &height, &nrChannels, 0);
    std::cout << "TexData: " << data
              << " w: " << width << "h: " << height << "\n";

    // vertex generation
    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nrChannels;
            // raw height at coordinate
            unsigned char y = texel[0];

            glm::vec3 d = glm::vec3(-height/2.0f + height*i/(float)height,// v.x// v.x
                                    (int) y * yScale - yShift,// v.y
                                    -width/2.0f + width*j/(float)width);

            // vertex
            mVertices.push_back(Vertex{d.x, d.y, d.z});
        }
    }
    int rez = 1;
    // After building up the vertex array, we can release the height map from memory.
    std::cout << "Loaded " << mVertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);
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
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
