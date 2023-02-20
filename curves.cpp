#include "curves.h"

using namespace Eigen;

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
    calculateCurveTask1Parabel();
    float y = 0;
    float xmin = -4.0f, xmax = 4.0f, h = 0.25f;
    for (auto x = xmin; x < xmax; x += h)
    {
        //float z = (mX*x) + mY;                // x , y

        float z = (mA * pow(x,3)) + (mB * pow(x,2)) + (mC * x) + mD;

        mVertices.push_back(Vertex{ x,y,z,1,1,0 });

        //z = (mX*(x+h)) + mY;                  //x+h, y

        z = (mA * pow((x+h),3)) + (mB * pow((x+h),2)) + (mC * (x+h)) + mD;

        mVertices.push_back(Vertex{ x + h,y,z,1,1,0 });
    }
    writeFile("curve.txt");
}

void Curves::calculateCurveTask1()
{
    MatrixXd y(8,1);
    y(0,0) = -1;
    y(1,0) = -1;
    y(2,0) = -2;
    y(3,0) =  1;
    y(4,0) =  1;
    y(5,0) = -1;
    y(6,0) =  2;
    y(7,0) =  2;

    MatrixXd A(8,2);

    A(0,0) = -3;    A(0,1) = 1;
    A(1,0) = -2;    A(1,1) = 1;
    A(2,0) = -1.5;  A(2,1) = 1;
    A(3,0) = -1;    A(3,1) = 1;
    A(4,0) =  1;    A(4,1) = 1;
    A(5,0) =  1;    A(5,1) = 1;
    A(6,0) =  2;    A(6,1) = 1;
    A(7,0) =  3;    A(7,1) = 1;

    MatrixXd B = A.transpose()*A;
    MatrixXd c = A.transpose()*y;
    MatrixXd x = B.inverse()*c;

    mX = x(0,0);
    mY = x(1,0);

    std::cout << "Function for the curve: " << std::endl;
    std::cout << "y = " << mX << "x + " << mY << std::endl;
}

void Curves::calculateCurveTask1Parabel()
{
    MatrixXd y(8,1);
    y(0,0) = -1;
    y(1,0) = -1;
    y(2,0) = -2;
    y(3,0) =  1;
    y(4,0) =  1;
    y(5,0) = -1;
    y(6,0) =  2;
    y(7,0) =  2;

    MatrixXd A(8,3);

    A(0,0) = pow(-3,2);    A(0,1) = -3;    A(0,2) = 1;
    A(1,0) = pow(-2,2);    A(1,1) = -2;    A(1,2) = 1;
    A(2,0) = pow(-1.5,2);  A(2,1) = -1.5;  A(2,2) = 1;
    A(3,0) = pow(-1,2);    A(3,1) = -1;    A(3,2) = 1;
    A(4,0) = pow( 1,2);    A(4,1) =  1;    A(4,2) = 1;
    A(5,0) = pow( 1,2);    A(5,1) =  1;    A(5,2) = 1;
    A(6,0) = pow( 2,2);    A(6,1) =  2;    A(6,2) = 1;
    A(7,0) = pow( 3,2);    A(7,1) =  3;    A(7,2) = 1;

    MatrixXd B = A.transpose()*A;
    MatrixXd c = A.transpose()*y;
    MatrixXd x = B.inverse()*c;

    if (x(0,0))
        mB = x(0,0);
    if (x(1,0))
        mC = x(1,0);
    if (x(2,0))
        mD = x(2,0);

    std::cout << "Function for the curve: " << std::endl;
    std::cout << "y = " << mX << "x + " << mY << std::endl;
}

void Curves::calculateCurve2()
{
    MatrixXd A(4,4);

    A(0,0) = pow(-3,3); A(0,1) = pow(-3,2); A(0,2) = -3; A(0,3) = 1;
    A(1,0) = pow(-2,3); A(1,1) = pow(-2,2); A(1,2) = -2; A(1,3) = 1;
    A(2,0) = pow(2,3);  A(2,1) = pow(2,2);  A(2,2) =  2; A(2,3) = 1;
    A(3,0) = pow(3,3);  A(3,1) = pow(3,2);  A(3,2) =  3; A(3,3) = 1;

    MatrixXd b(4,1);

    b(0,0) = -3;
    b(1,0) = 2;
    b(2,0) = -2;
    b(3,0) = 3;

    MatrixXd x = A.inverse()*b;

    if (x(0,0))
        mA = x(0,0);
    if (x(1,0))
        mB = x(1,0);
    if (x(2,0))
        mC = x(2,0);
    if (x(3,0))
        mD = x(3,0);

    std::cout << x;
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
