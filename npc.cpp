#include "npc.h"
#include <Windows.h>

NPC::NPC()
{
    mVertices.push_back(Vertex{ -0.25f,-0.25f,-0.25f, 0.583f,  0.771f,  0.014f});
    mVertices.push_back(Vertex{-0.25f,-0.25f, 0.25f, 0.609f,  0.115f,  0.436f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.327f,  0.483f,  0.844f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.822f,  0.569f,  0.201f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.435f,  0.602f,  0.223f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.310f,  0.747f,  0.185f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.597f,  0.770f,  0.761f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.559f,  0.436f,  0.730f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.359f,  0.583f,  0.152f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.483f,  0.596f,  0.789f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.559f,  0.861f,  0.639f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.195f,  0.548f,  0.859f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.014f,  0.184f,  0.576f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.771f,  0.328f,  0.970f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.406f,  0.615f,  0.116f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.676f,  0.977f,  0.133f});
    mVertices.push_back(Vertex{-0.25f,-0.25f, 0.25f, 0.971f,  0.572f,  0.833f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.140f,  0.616f,  0.489f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.997f,  0.513f,  0.064f});
    mVertices.push_back(Vertex{-0.25f,-0.25f, 0.25f, 0.945f,  0.719f,  0.592f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.543f,  0.021f,  0.978f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.279f,  0.317f,  0.505f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.167f,  0.620f,  0.077f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.347f,  0.857f,  0.137f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.055f,  0.953f,  0.042f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.714f,  0.505f,  0.345f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.783f,  0.290f,  0.734f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.722f,  0.645f,  0.174f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.302f,  0.455f,  0.848f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.225f,  0.587f,  0.040f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.517f,  0.713f,  0.338f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.053f,  0.959f,  0.120f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.393f,  0.621f,  0.362f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.673f,  0.211f,  0.457f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.820f,  0.883f,  0.371f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.820f,  0.883f,  0.371f});
}

void NPC::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    readFile("curve.txt");



    mMatrix.translate(mPath.begin()->getX(), mPath.begin()->getZ(), mPath.begin()->getY());
}

void NPC::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

    moveNPC();
}

void NPC::move(float x, float y, float z)
{
    mx += x;
    my += y;
    mz += z;
    //std::cout << "WorldPos: X: " << mx <<" Y:"<< my << "\n";
    mWorldPosition = {mx,my,mz};
    mMatrix.translate(mx, my, mz);
    mx =0;
    my =0;
    mz =0;
}

void NPC::move(float dt, QVector3D velocity)
{
  QVector3D ds=velocity*dt;

  // mPosition = mPosition + ds;		// hvis mPosisjon er Vector3d
  mPosition.translate(ds.x(), ds.y(), ds.z());	// hvis mPosisjon er Matrix4x4

  // normalen kan generelt være en parameter inn
  QVector3D normal = QVector3D{0.0f, 1.0f, 0.0f};


  // bruker kryssprodukt for å finne rotasjonsvektor
  QVector3D rotation = QVector3D::crossProduct(normal, mVelocity);
  rotation.normalize();


  // bruk formelen ds = r dt ==> dt = ds/r
  // for å finne ut hvor mye hjulet har rotert
  // og oppdater rotasjonsmatrisen
  // husk å starte med mRotation som identitetsmatrise


  mMatrix = mPosition*mRotation;		// hvis mPosition og mRotation er Matrix4x4
}

void NPC::readFile(std::string filename)
{
    std::ifstream inn;
    inn.open(filename.c_str());

    if (inn.is_open())
    {
        mPath.clear();
        std::cout << "READFILE: File " << filename << " was opened!" << std::endl;
        int n;
        Vertex vertex;
        inn >> n;
        mPath.reserve(n);
        for (int i = 0; i < n; i++)
        {
            inn >> vertex;
            mPath.push_back(vertex);
        }
        inn.close();

        //for (int i = 0;  i < mVertices.size(); i++)
            //std::cout << mVertices[i] << std::endl;
    }
    else
    {
        std::cout << "READFILE: File " << filename << " was not opened." << std::endl;
    }

}

void NPC::moveNPC()
{
    QVector3D Next;

    // MOVE FORWARD
    if (AtStart)
    {
        if (i < mPath.size() - 1)
        {
            Next = {mPath[i+1].getX() - mPath[i].getX(),
                              mPath[i+1].getZ() - mPath[i].getZ(),
                              mPath[i+1].getY() - mPath[i].getY()};

            move(Next.x(), Next.y(), Next.z());
            i++;
        }
        if (i == mPath.size() - 1)
        {
            AtStart = false;
        }
    }
    // MOVE BACKWARD
    if (!AtStart)
    {
        if (i > 1)
        {
            Next = {mPath[i-1].getX() - mPath[i].getX(),
                              mPath[i-1].getZ() - mPath[i].getZ(),
                              mPath[i-1].getY() - mPath[i].getY()};

            move(Next.x(), Next.y(), Next.z());
            i--;
        }
        if (i == 1)
            AtStart = true;
    }
}

void NPC::MoveToEnd()
{
    std::cout << "SPACE \n";
    mMatrix.translate(0.6,0.3,0);
}


