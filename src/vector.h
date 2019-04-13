//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector.h
//|
//| This is the interface to the Vector class.  An Vector is an n-dimensional vector.
//| Note that a vector can be viewed as a point simply by placing its tail at the
//| origin and considering the point at its head.  Practically speaking, this amounts
//| to using the vector's coordinates as though they were the coordinates of a point.
//|___________________________________________________________________________________

#ifndef HVECTOR
#define HVECTOR


#include "adsoda_types.h"
#include <vector>

class Halfspace;
class PtrList;
class Face;

class Vector
{

  // Solid::Project needs to be able to monkey around with dimension
  friend class Solid;
  
public:
  
  // Pointer to array of coordinates
  double *coordinates;
  
  Vector(long dim);
  Vector(const Vector& vector);
  ~Vector();
  
  long Dimension(void) const;
  
  double Magnitude(void) const;
  void Normalize(void);

  Vector CrossProduct3D(const Vector &v) const;
  
  bool IntersectHyperplanes(std::vector<Face *>& hyperplanes);
  
  bool InsideHalfspace(Halfspace& halfspace);
  bool InsideHalfspaces(std::vector<Halfspace *>& halfspaces);
  bool InsideOrOnHalfspace(Halfspace& halfspace);
  bool InsideOrOnHalfspaces(std::vector<Halfspace *>& halfspaces);
  
  void operator= (const Vector &v);

  friend Vector operator+(const Vector &v1, const Vector &v2);
  friend Vector operator-(const Vector &v1, const Vector &v2);
  friend double operator*(const Vector &v1, const Vector &v2);
  friend Vector operator-(const Vector &v);
	
//  Vector Dot(const Vector &v1, const Vector &v2) { return v1*v2; }

protected:
  
  long dimension;

};

#endif
