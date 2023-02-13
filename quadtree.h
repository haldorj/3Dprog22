#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <list>
#include <vector>

namespace gsml
{
typedef std::pair<double, double> Point2D; // Erstatter Vector2D klasse


// Template klasse, laget for ID og T
// ID er ikke implementert ennå
// Bruker Point2D - se typedef ovenfor - i stedet for Vector2D
template <class ID, class T>
class QuadTree
{
private:
   // Punkter som definerer hjørnene til objektet
   // i samme rekkefølge som pekerne til subtrær
   Point2D m_a;
   Point2D m_b;
   Point2D m_c;
   Point2D m_d;
   // Pekere til subtrær er nødvendig i en dynamisk
   // quadtre implementering. Vi kunne ha lagt det
   // i array også
   QuadTree* m_sv;
   QuadTree* m_so;
   QuadTree* m_no;
   QuadTree* m_nv;


   // Hvert quad lagrer sine egne objekter
   // (T kan være objekter, pekere eller referanser)
   // Legger opp til mulig søk på id
   std::list<std::pair<ID,T>> m_objects;


   bool isLeaf() const;


   // Hjelpevariabel for å lagre alle T-ene
   // for senere iterering
   std::vector<T> m_all_objects; // skulle vært static


   // Hjelpefunksjon - preorder traversering av subtre
   void traverse_all(std::vector<T>& all_objects);


public:
   QuadTree();
   QuadTree(const Point2D &v1, const Point2D &v2, const Point2D &v3, const Point2D & v4);
   void init(const Point2D &v1, const Point2D &v2, const Point2D &v3, const Point2D & v4);
   void subDivide(int n);
   void print() const;
   void print_all() const;
   QuadTree* insert(const Point2D& point, const ID& id, const T& object);
   QuadTree* find(const Point2D& p);
   const QuadTree* find(const Point2D& p) const;
   T& find(const ID& id);
   T& operator[] (ID id);


   // Jukser litt med disse iteratorene. Det er ikke ekte iteratorfunksjoner
   // for QuadTre klassen, men for en container av T-er
   typename std::vector<T>::iterator begin()
   {
       traverse_all(m_all_objects);
       return m_all_objects.begin();
   }
   typename std::vector<T>::iterator end()
   {
       return m_all_objects.end();
   }
};
}


#endif // QUADTREE_H
