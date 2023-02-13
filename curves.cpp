#include "curves.h"

Curves::Curves()
{
    DoCurve();
    mMatrix.setToIdentity();
}


Curves::~Curves(){}

void Curves::DoCurve()
{
    // f(x) = cosx
    float y = 0;

    float xmin = 0.0f, xmax = 6.0f, h = 0.25f;
    for (auto x = xmin; x < xmax; x += h)
    {
        float z = cos(x);                // x , y
        mVertices.push_back(Vertex{ x,y,z,1,1,0 });

        z = cos(x + h);                //x+h, y
        mVertices.push_back(Vertex{ x + h,y,z,1,1,0 });
    }
    writeFile("oblig1oppg1.txt");
}

void Curves::readFile(std::string filename)
{

    std::ifstream inn;
    inn.open(filename.c_str());

    if (inn.is_open())
    {
        mVertices.clear();
        std::cout << "READFILE: File " << filename << " was opened!" << std::endl;
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

        //for (int i = 0;  i < mVertices.size(); i++)
            //std::cout << mVertices[i] << std::endl;
    }
    else
    {
        std::cout << "READFILE: File " << filename << " was not opened." << std::endl;
    }


}

void Curves::writeFile(std::string filename)
{
    std::ofstream fil;
    fil.open(filename.c_str());
    if (fil.is_open())
    {
        std::cout << "WRITEFILE: File " << filename << " was opened!" << std::endl;
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


void Curves::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;

    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof( Vertex ),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mMatrix.translate(2.5, -1, 0);
    mMatrix.rotate(-90, 1, 0);
    mMatrix.scale(0.5, 0.5, 0.5);
}

void Curves::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_LINES, 0, mVertices.size());
}

void Curves::move(float dx, float dy, float dz)
{

}
