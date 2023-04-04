#include "triangulation.h"

#include "CustomFiles/CustomVec2.h"



Triangulation::Triangulation() : VisualObject()
{
    readFile("../3Dprog22/UKE11/6.3.7 vertexData.txt");
    //readFile("UKE11/6.3.7 vertexData.txt");
    mMatrix.setToIdentity();

    calcAverageNormals(mIndices, 42, vertices ,mVertices.size(), 8, 5);
}

Triangulation::Triangulation(std::string filnavn) : VisualObject()
{
    readFile("../3Dprog22/UKE11/6.3.7 vertexData.txt");
    //readFile("UKE11/6.3.7 vertexData.txt");
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

    //mMatrix.rotate(1, 10,0,0);

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



void Triangulation::calcAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount,
    unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indexCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;


        glm::vec3 v1(vertices[in1] - vertices[in0],
                     vertices[in1 + 1] - vertices[in0 + 1],
                     vertices[in1 + 2] - vertices[in0 + 2]);

        glm::vec3 v2(vertices[in2] - vertices[in0],
                    vertices[in2 + 1] - vertices[in0 + 1],
                    vertices[in2 + 2] - vertices[in0 + 2]);

        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < vertexCount / vLength; i++)
    {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}


