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
    // Normalize the normals of each vertex
    for (size_t i = 0; i < vertexCount; i++)
    {
        glm::vec3 vec(mVertices[i].getNormX(),
                      mVertices[i].getNormY(),
                      mVertices[i].getNormZ());
        vec = glm::normalize(-vec);

        mVertices[i].setNorm(vec.x, vec.y, vec.z);
    }
}

