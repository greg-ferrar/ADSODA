//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Face.cp
//|
//| This is the implementation of the Face class.  An Face is one face of an n-dimensional
//| solid.
//|_______________________________________________________________________________________


#include "face.h"

#include "string.h"


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Face::Face
//|
//| Purpose: This method creates a Face.
//|
//| Parameters: dim: dimension of the Face
//|_________________________________________________________________________________

Face::Face(long dim) :

  // Construct as a Halfspace
  Halfspace(dim)

{

}	//==== Face::Face() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Face::Face
//|
//| Purpose: This method creates a Face which is a copy of the passed Face.
//|
//| Parameters: face: the Face of which to make a copy
//|_________________________________________________________________________________

Face::Face(Face &face) :

  // Create halfspace of correct dimension
  Halfspace(face.Dimension())

{

  // Copy the coefficients
  memmove(coordinates, face.coordinates, (dimension + 1) * sizeof(double));

} //==== Face::Face() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Face::Face
//|
//| Purpose: This method creates a Face.
//|
//| Parameters: halfspace: halfspace to use for this Face
//|_________________________________________________________________________________

Face::Face(Halfspace& halfspace) :

  // Construct as a Halfspace
  Halfspace(halfspace.Dimension())

{

  // Copy the coefficients
  memmove(coordinates, halfspace.coordinates, (dimension + 1) * sizeof(double));

} //==== Face::Face() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Face::Face
//|
//| Purpose: This method disposes of a Face.
//|
//| Parameters: none
//|_________________________________________________________________________________

Face::~Face(void) {

} //==== Face::Face() ====//



