#include "visualobject.h"

VisualObject::VisualObject()
{

}

VisualObject::~VisualObject() {
   glDeleteVertexArrays( 1, &mVAO );
   glDeleteBuffers( 1, &mVBO );
}

void VisualObject::calcAverageNormalsFlat()
{
    unsigned int indexCount = mIndices.size();

    // Iterates through each triangle in the mesh
    for (size_t i = 0; i < indexCount; i += 3)
    {
        // Store each vertex of the current triangle
        unsigned int in0 = mIndices[i];
        unsigned int in1 = mIndices[i + 1];
        unsigned int in2 = mIndices[i + 2];

        // Calculate the normal vector by with the cross product of the two vectors forming the triangle.
        glm::vec3 v1(mVertices[in1].getX() - mVertices[in0].getX(),
                     mVertices[in1].getY() - mVertices[in0].getY(),
                     mVertices[in1].getZ() - mVertices[in0].getZ());

        glm::vec3 v2(mVertices[in2].getX() - mVertices[in0].getX(),
                     mVertices[in2].getY() - mVertices[in0].getY(),
                     mVertices[in2].getZ() - mVertices[in0].getZ());

        glm::vec3 normal = glm::cross(v1, v2);

        // normalize the normal (set length to 1)
        normal = glm::normalize(normal);

        // Assign the same normal to all three vertices of the triangle
        mVertices[in0].setNorm(normal.x, normal.y, normal.z);
        mVertices[in1].setNorm(normal.x, normal.y, normal.z);
        mVertices[in2].setNorm(normal.x, normal.y, normal.z);
    }
}

void VisualObject::calcAverageNormalsSmooth()
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

        // Calculate the normal vector by with the cross product of the two vectors forming the triangle.
        glm::vec3 v1(mVertices[in1].getX() - mVertices[in0].getX(),
                     mVertices[in1].getY() - mVertices[in0].getY(),
                     mVertices[in1].getZ() - mVertices[in0].getZ());

        glm::vec3 v2(mVertices[in2].getX() - mVertices[in0].getX(),
                     mVertices[in2].getY() - mVertices[in0].getY(),
                     mVertices[in2].getZ() - mVertices[in0].getZ());

        glm::vec3 normal = glm::cross(v1, v2);
        // Normalize the normal (set length to 1)
        normal = glm::normalize(normal);

        // Update the accumulated normal of each vertex in the triangle with the normal of the current face.
        // Some vertices are present in different triangles,
        // we do this in order to calculate the AVERAGE normal for EACH vertex in the mesh.
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
    // Iterate through all vertices in the mesh and normalize their accumulated normal vector.
    // This gives us the average normal for each vertex in the mesh, based on the normals of its adjacent faces.
    for (size_t i = 0; i < vertexCount; i++)
    {
        glm::vec3 vec(mVertices[i].getNormX(),
                      mVertices[i].getNormY(),
                      mVertices[i].getNormZ());
        vec = glm::normalize(vec);

        mVertices[i].setNorm(vec.x, vec.y, vec.z);
    }
}

