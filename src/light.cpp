//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Light.cp
//|
//| This is the implementation to the Light class.  A Light is a colored source of
//| light in n-space.  The Vector (the superclass) indicates the direction of the
//| light source-- it is considered to be at infinity. 
//|_______________________________________________________________________________________


#include "light.h"
#include "face.h"



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Light::Light
//|
//| Purpose: This method creates a Light.
//|
//| Parameters: dim:    dimension of the Light
//|             pRed:   red intensity of the Light
//|             pGreen: green intensity of the Light
//|             pBlue:  blue intensity of the Light
//|_________________________________________________________________________________

Light::Light(long dim, double pRed, double pGreen, double pBlue) : Vector(dim)
{

  red = pRed;
  green = pGreen;
  blue = pBlue;

} //==== Light::Light() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Light::Light
//|
//| Purpose: This method creates a Light which is a copy of light.
//|
//| Parameters: light: the light to copy
//|_________________________________________________________________________________

Light::Light(const Light& light) :

  // Make a copy of this as a vector
  Vector(light)

{

  // Copy the intensity of this light
  red = light.red;
  green = light.green;
  blue = light.blue;

} //==== Light::Light() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Light::Apply
//|
//| Vector:  This method finds the intensity of this Light on a face.
//|
//| Parameters: normal: the normal of the Face.  MUST BE NORMALIZED.
//|             cRed:   the cumulative red intensity so far
//|             cGreen: the cumulative green intensity so far
//|             cBlue:  the cumulative blue intensity so far
//|_________________________________________________________________________________

void Light::Apply(Vector& normal, double &cRed, double &cGreen, double &cBlue)
{

  // Dot the light direction with the normalized normal of Face.
  register double intensity = -(normal * (*this));

  // Face is a backface, from light's perspective
  if (intensity < 0)
    return;
  
  // Add the intensity componentwise
  cRed += red * intensity;
  cGreen += green * intensity;
  cBlue += blue * intensity;
 
  //  cout << "(cRed, cGreen, cBlue): (" << cRed << ", " << cGreen << ", " << cBlue << ")" << endl;

  // Clip to unit range
  if (cRed > 1.0) cRed = 1.0;
  if (cGreen > 1.0) cGreen = 1.0;
  if (cBlue > 1.0) cBlue = 1.0;

} //==== Light::Apply() ====//

