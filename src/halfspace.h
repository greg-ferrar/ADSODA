//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace.h
//|
//| This is the interface to the Halfspace class.  An Halfspace is half of an n-space.
//| it is described by the equation of the bounding hyperplane.  A point is considered
//| to be inside the halfspace if the left side of the equation is greater than the
//| right side when the coordinates of the point (vector) are plugged in.  The first
//| n coefficients can also be viewed as the normal vector, and the last as a constant
//| which determines which of the possible parallel hyperplanes in n-space this one is.
//|____________________________________________________________________________________

#ifndef HHALFSPACE
#define HHALFSPACE


#include "adsoda_types.h"
#include "solid.h"
#include <vector>

class AMatrix;

class Halfspace : public Vector
{
  
public:
  
  Halfspace(long dim);
  Halfspace(Halfspace &halfspace);
  virtual ~Halfspace(void);
  
  void	DumpEquation(void);
  void	Negate(void);
  void	Translate(Vector& offset);
  
  void	Transform(const AMatrix& m);
	    
  void	SliceSolid(Solid& solid, Solid*& inside_solid, Solid*& outside_solid);

};

//  A halfspace is represented by the equation
//  of the bounding hyperplane, so a Hyperplane
//  is really the same as a Halfspace.
typedef Halfspace Hyperplane;

#endif
