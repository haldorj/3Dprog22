#include "visualobject.h"

VisualObject::VisualObject()
{

}

VisualObject::~VisualObject() {
   glDeleteVertexArrays( 1, &mVAO );
   glDeleteBuffers( 1, &mVBO );
}

void VisualObject::calcAverageNormals()
{
    unsigned int indexCount = mIndices.size();
    unsigned int vertexCount = mVertices.size();

    // Iterates through each triangle in the mesh
    for (size_t i = 0; i < indexCount; i += 3)
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

        mVertices[i].setNorm(0, 0, 0);
        mVertices[i].setNorm(normal.x, normal.y, normal.z);
    }
    for (size_t i = 0; i < vertexCount; i++)
    {
//        glm::vec3 vec(mVertices[i].getNormX(),
//                      mVertices[i].getNormY(),
//                      mVertices[i].getNormZ());
//        vec = glm::normalize(vec);

//        //mVertices[i].setNorm(0, 0, 0);
//        mVertices[i].setNorm(vec.x, vec.y, vec.z);
    }
}

