#include "objmesh.h"

#include <sstream>      // std::stringstream
#include <iostream>   // std::cout
#include <string>     // std::string, std::stoi


ObjMesh::ObjMesh(std::string filename)
{
    readFile(filename);
    mMatrix.setToIdentity();
}

void ObjMesh::readFile(std::string filename)
{
    std::ifstream fileIn;
    fileIn.open(filename, std::ifstream::in);
    if (!fileIn)
    {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(filename);
        return;
    }
    // One line at a time variable
    std::string oneLine;
    // One word at a time variable
    std::string oneWord;

    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec3> tempUVs;

    // Variable for constructing the indices vector
    unsigned int temp_index = 0;

    while(std::getline(fileIn, oneLine))
    {
        // Doing a trick to get one word at a time
        std::stringstream sStream;
        // Pushing line into stream
        sStream << oneLine;
        // Streaming one word out of line
        oneWord = ""; // resetting the value or else the last value might survive!
        sStream >> oneWord;

        if (oneWord == "#")
        {
            //qDebug() << "Line is comment";
            continue;
        }
        if (oneWord == "")
        {
            //qDebug() << "Line is blank";
            continue;
        }
        if (oneWord == "v")
        {
            //qDebug() << "Line is vertex " << QString::fromStdString(oneWord) << " ";
            glm::vec3 tempVertex;
            sStream >> oneWord;
            tempVertex.x = stof(oneWord);
            sStream >> oneWord;
            tempVertex.y = stof(oneWord);
            sStream >> oneWord;
            tempVertex.z = stof(oneWord);

            // Vertex made - pushing it into vertex-vector
            tempVertices.push_back(tempVertex);

            continue;
        }
        if (oneWord == "vt")
        {
            //qDebug() << "Line is UV-coordinate " << QString::fromStdString(oneWord) << " ";
            glm::vec3 tempUV;
            sStream >> oneWord;
            tempUV.x = stof(oneWord);
            sStream >> oneWord;
            tempUV.y = stof(oneWord);

            // UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            continue;
        }
        if (oneWord == "vn")
        {
            //qDebug() << "Line is normal " << QString::fromStdString(oneWord) << " ";
            glm::vec3 tempNormal;
            sStream >> oneWord;
            tempNormal.x = stof(oneWord);
            sStream >> oneWord;
            tempNormal.y = stof(oneWord);
            sStream >> oneWord;
            tempNormal.z = stof(oneWord);

            // Normal made - pushing it into normal-vector
            tempNormals.push_back(tempNormal);

            continue;
        }
        if (oneWord == "f")
        {
            //qDebug() << "Line is a face " << QString::fromStdString(oneWord) << " ";
            int index, normal, uv;
            for (int i = 0; i < 3; i++)
            {
                sStream >> oneWord; // one word read
                std::stringstream tempWord(oneWord);    // to use getline on this one word
                std::string segment;    // the numbers in the f-line
                std::vector<std::string> segmentArray; // temp array of the numbers
                while(std::getline(tempWord, segment, '/')) // splitting word in segments
                {
                    segmentArray.push_back(segment);
                }
                index = std::stoi(segmentArray[0]); // first is vertex
                if (segmentArray[1] != "")          // second is uv
                {
                    uv = std::stoi(segmentArray[1]);
                }
                else
                {
                    //qDebug() << "No uvs in mesh";
                    uv = 0;
                }
                normal = std::stoi(segmentArray[2]); // third is normal

                // Fixing the indices
                // because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1) // uv present!
                {
                    mVertices.push_back(Vertex(tempVertices[index].x, tempVertices[index].y, tempVertices[index].z,
                                               tempNormals[normal].x, tempNormals[normal].y, tempNormals[normal].z,
                                               tempUVs[uv].x, tempUVs[uv].y));
                }
                else
                {
                    mVertices.push_back(Vertex(tempVertices[index].x, tempVertices[index].y, tempVertices[index].z,
                                               tempNormals[normal].x, tempNormals[normal].y, tempNormals[normal].z,
                                               0.0f, 0.0f));
                }
                mIndices.push_back(temp_index++);
            }
            continue;
        }
    }
    fileIn.close();
}

void ObjMesh::init(GLint matrixUniform)
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
    glEnableVertexAttribArray(0);
    // color/normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),  (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ObjMesh::draw()
{
    glBindVertexArray( mVAO );
    // Bindbuffer, indexed draws
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    // DrawElements, indexed draws
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
