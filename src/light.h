//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Light.h
//|
//| This is the interface to the Light class.  A Light is a colored source of
//| light in n-space.  The Vector (the superclass) indicates the direction of the
//| light source-- it is considered to be at infinity. 
//|___________________________________________________________________________________

#ifndef HLIGHT
#define HLIGHT


#include "vector.h"


class Light : public Vector
{
  
  double red, green, blue;
 
public:

  Light(void);
  Light(long dim, double pRed, double pGreen, double pBlue);
  //  Light(Light &light);
  Light(const Light &light);

  void Apply(Vector& normal, double& cRed, double& cGreen, double& cBlue);

 };


#endif
