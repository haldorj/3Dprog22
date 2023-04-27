#include "triangulation.h"

Triangulation::Triangulation() : VisualObject()
{
    readFile("../3Dprog22/UKE11/6.3.7 vertexData.txt");
    initIndeces();
    mMatrix.setToIdentity();
}

Triangulation::Triangulation(std::string filnavn) : VisualObject()
{
    readFile("../3Dprog22/UKE11/6.3.7 vertexData.txt");
    initIndeces();
    mMatrix.setToIdentity();
}

Triangulation::~Triangulation()
{

}

void Triangulation::init(GLint matrixUniform)
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

    //mMatrix.translate(0,0,10);
}

void Triangulation::draw()
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

glm::vec3 Triangulation::getVertex(int index)
{
    glm::vec3 v(mVertices[index].getX(),
                mVertices[index].getY(),
                mVertices[index].getZ());

    return v;
}

unsigned int Triangulation::getIndex(int triangleIndex, int vertexIndex)
{
    return mIndices[triangleIndex * 3 + vertexIndex];
}

void Triangulation::initIndeces()
{
    mIndices = {
        0, 4, 1,    // 1. trekant
        1, 4, 6,    // 2. trekant
        1, 6, 5,    // 3. osv...
        1, 5, 3,
        1, 3, 2,
        4, 10, 6,
        4, 9, 10,
        8, 10, 11,
        5, 6, 10,
        5, 10, 8,
        7, 8, 11,
        5, 8, 7,
        3, 5, 7,
        2, 3, 7
    };

    numTriangles = mIndices.size()/3;
}

void Triangulation::readFile(std::string filename)
{

    std::ifstream inn;
    inn.open(filename.c_str());

    if (inn.is_open())
    {
        mVertices.clear();
        //std::cout << "READFILE: File " << filename << " was opened!" << std::endl;
        int n;
        Vertex vertex;
        inn >> n;
        mVertices.reserve(n);
        for (int i = 0; i < n; i++)
        {
            inn >> vertex;
            mVertices.push_back(vertex);
        }
        inn.close();
    }
    else
    {
        std::cout << "READFILE: File " << filename << " was not opened." << std::endl;
    }
}

void Triangulation::writeFile(std::string filename)
{
    std::ofstream fil;
    fil.open(filename.c_str());
    if (fil.is_open())
    {
        //std::cout << "WRITEFILE: File " << filename << " was opened!" << std::endl;
        fil.clear();
        fil << mVertices.size() << std::endl;
        for (int i = 0; i < mVertices.size(); i++)
        {
            fil << mVertices[i] << std::endl;
        }
        fil.close();
        mVertices.clear();
    }
    else
    {
        std::cout << "WRITEFILE: File " << filename << " was not opened." << std::endl;
    }
}

float Triangulation::GetSurfaceHeight(glm::vec3 p)
{
    // Loop through each triangle in the mesh.
    for (int i = 0; i < (numTriangles); i++)
    {
        // Get the vertices of the triangle.
        unsigned int v0 = getIndex(i, 0);
        unsigned int v1 = getIndex(i, 1);
        unsigned int v2 = getIndex(i, 2);
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

    return 0.0f;
}

glm::vec3 Triangulation::barycentricCoordinates(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const glm::vec2 &pt)
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
