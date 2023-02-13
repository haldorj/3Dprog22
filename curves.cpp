#include "curves.h"

Curves::Curves()
{
    constructCurve();
    mMatrix.setToIdentity();
}


Curves::Curves(std::string filnavn)
{
    constructCurve();
    readFile(filnavn);
    mMatrix.setToIdentity();
}

Curves::~Curves(){}

void Curves::constructCurve()
{
    calculateCurve();
    float y = 0;
    float xmin = -4.0f, xmax = 4.0f, h = 0.25f;
    for (auto x = xmin; x < xmax; x += h)
    {
        float z = (0.54651*x) + 0.15912;                // x , y
        mVertices.push_back(Vertex{ x,y,z,1,1,0 });

        z = (0.54651*(x+h)) + 0.15912;                //x+h, y
        mVertices.push_back(Vertex{ x + h,y,z,1,1,0 });
    }
    writeFile("curve.txt");
}

void Curves::calculateCurve()
{

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

    mMatrix.rotate(-90, 1, 0, 0);
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
