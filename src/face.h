//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Face.h
//|
//| This is the definition for the Face class.  An Face is one face of an n-dimensional
//| solid.  It is defined by a Halfspace.
//|_______________________________________________________________________________________

#ifndef HFACE
#define HFACE


#include "halfspace.h"


class Face : public Halfspace
{

  //  Solid needs access to these Lists:  
  friend class Solid;
  
  //  List of pointers to the corners which touch this face
  vector<Vector *> touching_corners;

  //  List of pointers to the Faces which touch this face
  vector<Face *> adjacent_faces;
  
public:
  
  Face(long dim);
  Face(Halfspace& halfspace);
  Face(Face& face);
  virtual ~Face(void);
  
};

#endif
