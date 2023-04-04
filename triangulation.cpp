#include "triangulation.h"

#include "CustomFiles/CustomVec2.h"

unsigned int mIndices[] = {
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

Triangulation::Triangulation() : VisualObject()
{
    readFile("UKE11/6.3.7 vertexData.txt");
    mMatrix.setToIdentity();
}

Triangulation::Triangulation(std::string filnavn) : VisualObject()
{
    readFile("UKE11/6.3.7 vertexData.txt");
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices), mIndices, GL_STATIC_DRAW);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Triangulation::draw()
{
    glBindVertexArray( mVAO );
    // Bindbuffer, indexed draws
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    // DrawElements, indexed draws
    glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
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




