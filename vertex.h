#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <fstream>

class Vertex {
   //! Overloaded ostream operator which writes all vertex data on an open textfile stream
   friend std::ostream& operator<< (std::ostream&, const Vertex&);

   //! Overloaded ostream operator which reads all vertex data from an open textfile stream
   friend std::istream& operator>> (std::istream&, Vertex&);



private:
   float m_xyz[3];       	// position i 3D
   float m_normal[3];       // normal in 3D or rgb colors
   float m_st[2];        	// texture coordinates if used

public:
   Vertex();
   Vertex(float x, float y, float z, float r, float g, float b);
   Vertex(float x, float y, float z, float r, float g, float b, float u, float v);
   Vertex(float x, float y, float z);

   Vertex operator-(const Vertex& other) const {
       return {m_xyz[0] - other.m_xyz[0], m_xyz[1] - other.m_xyz[1], m_xyz[2] - other.m_xyz[2]};
   };


   float getX() {return m_xyz[0];}
   float getY() {return m_xyz[1];}
   float getZ() {return m_xyz[2];}
};
#endif // VERTEX_H
