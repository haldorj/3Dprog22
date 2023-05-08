#include "npc.h"
#include <Windows.h>

NPC::NPC(std::string path)
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

    // Get path from file.
    readFile(path);

    // Get path from function.
    if(path == "curve.txt")
    {
        // The function for the curve.
        mA = 0; mB = 0.0728164f; mC = 0.547531f; mD = -0.125218f;
        // Set initial position.
        mMatrix.translate(-3.0,-1.112463,0.0);
    }
    if(path == "curve2.txt")
    {
        // The function for the curve.
        mA = 0.4f; mB = 0.0f; mC = -2.6f; mD = 0.0f;
        // Set initial position.
        mMatrix.translate(-3.5,-3.5,0.0);
    }
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
}

void NPC::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());

    if (bShouldRender)
        glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    else
        glDrawArrays(GL_NONE, 0, 0);

    //moveNPCFromFile();
    moveNPC();
}

void NPC::move(float x, float y, float z)
{
    mx += x;
    my += y;
    mz += z;
    //std::cout << "WorldPos: X: " << mx <<" Y:"<< my << "\n";
    mWorldPosition += {mx, my, mz};
    mMatrix.translate(mx, my, mz);
    mx =0;
    my =0;
    mz =0;
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

//        for (int i = 0;  i < mPath.size(); i++)
//            std::cout << mPath[i] << std::endl;
    }
    else
    {
        std::cout << "READFILE: File " << filename << " was not opened." << std::endl;
    }

}

void NPC::moveNPC()
{
    QVector3D Current;
    QVector3D Next;
    QVector3D Trajectory;

    float z = 0;
    float h = 0.03f;
    if (bForward)
    {

        if (xi < 3)
        {
            float y = (mA * pow(xi,3)) + (mB * pow(xi,2)) + (mC * xi) + mD;
            Current = {xi,y,z};

            xi += h;

            y = (mA * pow(xi,3)) + (mB * pow(xi,2)) + (mC * xi) + mD;
            Next = {xi,y,z};

            Trajectory = Next - Current;
            move(Trajectory.x(),Trajectory.y(),Trajectory.z());
        }
        if (xi >= 3)
        {
            xi = 3;
            bForward = !bForward;
        }
    }

    if (bForward == false)
    {
        if (xi > -3)
        {
            float y = (mA * pow(xi,3)) + (mB * pow(xi,2)) + (mC * xi) + mD;
            Current = {xi,y,z};

            xi -= h;

            y = (mA * pow(xi,3)) + (mB * pow(xi,2)) + (mC * xi) + mD;
            Next = {xi,y,z};

            Trajectory = Next - Current;
            move(Trajectory.x(),Trajectory.y(),Trajectory.z());
        }
        if (xi <= -3)
        {
            xi = -3;
            bForward = !bForward;
        }
    }
}

void NPC::moveNPCFromFile()
{
    float var = 1;
    QVector3D Next;

    //std::cout << i << "\n";

    // MOVE FORWARD
    if (bForward)
    {
        if (i < mPath.size() - var)
        {
            Next = {mPath[i+1].getX() - mPath[i].getX(),
                    mPath[i+1].getZ() - mPath[i].getZ(),
                    mPath[i+1].getY() - mPath[i].getY()};

            move(Next.x(), Next.y(), Next.z());
            i++;
        }
        if (i == mPath.size() - var)
        {
            bForward = false;
        }
    }
    // MOVE BACKWARD
    if (!bForward)
    {
        if (i > var)
        {
            Next = {mPath[i-1].getX() - mPath[i].getX(),
                    mPath[i-1].getZ() - mPath[i].getZ(),
                    mPath[i-1].getY() - mPath[i].getY()};

            move(Next.x(), Next.y(), Next.z());
            i--;
        }
        if (i == var)
            bForward = true;
    }
}


