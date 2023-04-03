#include "interactiveobject.h"

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

// Indices for drawing cube faces using triangle strips.
// Triangle strips can be connected by duplicating indices
// between the strips. If connecting strips have opposite
// vertex order then last index of the first strip and first
// index of the second strip needs to be duplicated. If
// connecting strips have same vertex order then only last
// index of the first strip needs to be duplicated.
GLushort indices[] = {
     0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
     4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
     8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
    12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
    16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
    20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
};

InteractiveObject::InteractiveObject() : mx{0.0f}, my{0.0f}, mz{0.0f}
{
    initCubeGeometry();
}

InteractiveObject::~InteractiveObject()
{

}

void InteractiveObject::init(GLint matrixUniform)
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),  (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void InteractiveObject::draw()
{
    glBindVertexArray( mVAO );
    // Bindbuffer, indexed draws
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    // DrawElements, indexed draws
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Barycentric coord
    PlayerCoords.x = mWorldPosition.x();
    PlayerCoords.y = mWorldPosition.y();
}

void InteractiveObject::move(float x, float y, float z)
{
    mx += x;
    my += y;
    mz += z;
    mMatrix.translate(mx, my, mz);
    mx =0;
    my =0;
    mz =0;
}

glm::vec3 InteractiveObject::barycentricCoordinates(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
{
    glm::vec2 p12 = p2-p1;
    glm::vec3 v12(p12.x, p12.y, 0.0f);

    glm::vec2 p13 = p3-p1;
    glm::vec3 v13(p13.x, p13.y, 0.0f);

    glm::vec3 n = glm::cross(v12, v13);

    float areal_123 = n.length(); // dobbelt areal
    glm::vec3 baryc; // til retur. Husk
    // u
    glm::vec2 p = p2 - PlayerCoords;
    glm::vec3 P(p.x, p.y, 0.0f);
    glm::vec2 q = p3 - PlayerCoords;
    glm::vec3 Q(q.x, q.y, 0.0f);

    n = glm::cross(P, Q);
    baryc.x = n.z/areal_123;
    // v
    p = p3 - PlayerCoords;
    q = p1 - PlayerCoords;
    n = glm::cross(P, Q);
    baryc.y = n.z/areal_123;
    // w
    p = p1 - PlayerCoords;
    q = p2 - PlayerCoords;
    n = glm::cross(P, Q);
    baryc.z = n.z/areal_123;
    return baryc;
}

void InteractiveObject::initCubeGeometry()
{
    float size = 0.5;
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f)*size, QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f)*size, QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f)*size, QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f)*size, QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f)*size, QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f)*size, QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f)*size, QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f)*size, QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f)*size, QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f)*size, QVector2D(1.0f, 0.5f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f)*size, QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f)*size, QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f)*size, QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f)*size, QVector2D(1.0f, 0.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f)*size, QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f)*size, QVector2D(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f)*size, QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f)*size, QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f)*size, QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f)*size, QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f)*size, QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f)*size, QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f)*size, QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f)*size, QVector2D(0.66f, 1.0f)}  // v23
    };
    for (auto v : vertices)
    {
        mVertices.push_back(Vertex{
                                v.position.x(),v.position.y(),v.position.z(),  // Position
                                0,0,0,  // Color
                                v.texCoord.x(), v.texCoord.y() // Textures
                            });
    }
}
