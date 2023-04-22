#include "trianglesurface.h"

TriangleSurface::TriangleSurface() : VisualObject()
{
    mMatrix.setToIdentity();
}

TriangleSurface::TriangleSurface(std::string filnavn) : VisualObject()
{
    if(filnavn == "frankes.txt")
        constructFrankes();
    readFile(filnavn);
    mMatrix.setToIdentity();
}

TriangleSurface::~TriangleSurface()
{

}
// Parameternavnet er byttet ut fra leksjonen
// Koden er ellers som for XYZ::init()
void TriangleSurface::init(GLint matrixUniform)
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

    mMatrix.translate(10, 12, -2);
    mMatrix.rotate(180, 0, 0, 1);
    mMatrix.scale(20,20,2);
}

void TriangleSurface::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
}

void TriangleSurface::readFile(std::string filename)
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

        //for (int i = 0;  i < mVertices.size(); i++)
            //std::cout << mVertices[i] << std::endl;
    }
    else
    {
        std::cout << "READFILE: File " << filename << " was not opened." << std::endl;
    }


}

void TriangleSurface::writeFile(std::string filename)
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

void TriangleSurface::constructFrankes()
{
    float xmin=0.0f, xmax=1.0f, ymin=0.0f, ymax=1.0f, h=0.05f;
    for (auto x=xmin; x<xmax; x+=h)
        for (auto y=ymin; y<ymax; y+=h)
        {
            float z = (0.75 * exp(-pow((9*(x)-2), 2)/4 - pow((9*(y)-2),2)/4)
                + 0.75 * exp(-pow((9*(x)+1), 2)/49 - (9*(y)+1)/10)
                + 0.5 * exp(-pow((9*(x)-7),2)/4 - pow((9*(y)-3),2)/4)
                + (-0.2 * exp(-pow((9*(x)-4),2) - pow((9*(y)-7),2))));
            mVertices.push_back(Vertex{x,y,z,x,y,z});
            // x + h
            z = (0.75 * exp(-pow((9*(x+h)-2), 2)/4 - pow((9*y-2),2)/4)
                + 0.75 * exp(-pow((9*(x+h)+1), 2)/49 - (9*y+1)/10)
                + 0.5 * exp(-pow((9*(x+h)-7),2)/4 - pow((9*y-3),2)/4)
                + (-0.2 * exp(-pow((9*(x+h)-4),2) - pow((9*y-7),2))));
            mVertices.push_back(Vertex{x+h,y,z,x,y,z});
            // y + h
            z = (0.75 * exp(-pow((9*x-2), 2)/4 - pow((9*(y+h)-2),2)/4)
                + 0.75 * exp(-pow((9*x+1), 2)/49 - (9*(y+h)+1)/10)
                + 0.5 * exp(-pow((9*x-7),2)/4 - pow((9*(y+h)-3),2)/4)
                + (-0.2 * exp(-pow((9*x-4),2) - pow((9*(y+h)-7),2))));
            mVertices.push_back(Vertex{x,y+h,z,x,y,z});
            mVertices.push_back(Vertex{x,y+h,z,x,y,z});

            // x + h
            z = (0.75 * exp(-pow((9*(x+h)-2), 2)/4 - pow((9*y-2),2)/4)
                + 0.75 * exp(-pow((9*(x+h)+1), 2)/49 - (9*y+1)/10)
                + 0.5 * exp(-pow((9*(x+h)-7),2)/4 - pow((9*y-3),2)/4)
                + (-0.2 * exp(-pow((9*(x+h)-4),2) - pow((9*y-7),2))));
            mVertices.push_back(Vertex{x+h,y,z,x,y,z});

            // x + h, y + h
            z = (0.75 * exp(-pow((9*(x+h)-2), 2)/4 - pow((9*(y+h)-2),2)/4)
                + 0.75 * exp(-pow((9*(x+h)+1), 2)/49 - (9*(y+h)+1)/10)
                + 0.5 * exp(-pow((9*(x+h)-7),2)/4 - pow((9*(y+h)-3),2)/4)
                + (-0.2 * exp(-pow((9*(x+h)-4),2) - pow((9*(y+h)-7),2))));
            mVertices.push_back(Vertex{x+h,y+h,z,x,y,z});
        }
        writeFile("frankes.txt");
}



void TriangleSurface::contructPlane()
{
    /* for eksamen 2019 oppg 1*/
    // Sette slik at det blir max 8 i x, og max 6 i y. Z skal være 0.

    float xmin=0.0f, xmax=1.0f, ymin=0.0f, ymax=1.0f, h=2.0f;
    for (auto x=xmin; x<xmax; x+=h)
        for (auto y=ymin; y<ymax; y+=h)
        {
            float z = 0;
            mVertices.push_back(Vertex{x,y,z,x,y,z});
            // x + h
            z = 0;
            mVertices.push_back(Vertex{x+h,y,z,x,y,z});
            // y + h
            z = 0;
            mVertices.push_back(Vertex{x,y+h,z,x,y,z});
            mVertices.push_back(Vertex{x,y+h,z,x,y,z});

            // x + h
            z = 0;
            mVertices.push_back(Vertex{x+h,y,z,x,y,z});

            // x + h, y + h
            z =0;
            mVertices.push_back(Vertex{x+h,y+h,z,x,y,z});
        }
}
