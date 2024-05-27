#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <fstream>

class Vertex {

public:

   Vertex();
   Vertex(float x, float y, float z, float r, float g, float b);
   Vertex(float x, float y, float z, float r, float g, float b, float u, float v);
   Vertex(float x, float y, float z);

   void setXYZ(float x, float y, float z);
   void setCol(float r, float g, float b);
   void setNorm(float nx, float ny, float nz);

   float getX() const { return m_xyz[0]; }
   float getY() const { return m_xyz[1]; }
   float getZ() const { return m_xyz[2]; }

   float getNormX() const { return m_normal[0]; }
   float getNormY() const { return m_normal[1]; }
   float getNormZ() const { return m_normal[2]; }

   Vertex operator-(const Vertex& other) const
   {
       return {m_xyz[0] - other.m_xyz[0], m_xyz[1] - other.m_xyz[1], m_xyz[2] - other.m_xyz[2]};
   };

   // Overloads
   //! Overloaded ostream operator which writes all vertex data on an open textfile stream
   friend std::ostream& operator<< (std::ostream&, const Vertex&);
   //! Overloaded ostream operator which reads all vertex data from an open textfile stream
   friend std::istream& operator>> (std::istream&, Vertex&);

private:
   float m_xyz[3];       	// position i 3D
   float m_normal[3];       // normal in 3D or rgb colors
   float m_st[2];        	// texture coordinates if used

};
#endif // VERTEX_H
