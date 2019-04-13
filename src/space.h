//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space.h
//|
//| This is the interface to the Space class.  A Space is euclidian
//| n-space, perhaps containing some Solids
//|_____________________________________________________________________________

#ifndef HSPACE
#define HSPACE


#include "color.h"
#include "vector.h"
#include "light.h"
#include <vector>
#include "halfspace.h"

class Vector;
class AMatrix;
//class Hyperplane;
class Solid;
class Halfspace;

class Space
{

  Color ambient;
  std::vector<Light> lights;
  long	dimension;
  

public:

  std::vector<Solid *> solids;
	
  //  The (n-1)-space associate with this Space.
  //  Space *lowerSpace;

  Space(long dimension, const Color &pambient);
  Space(Space& space);
  ~Space(void);

  long Dimension(void) { return dimension; } 

  void AddSolid(Solid *solid);
  void AddLight(Light *light);
  
  void Subtract(Solid& solid);
  void Add(Space& space);

  void Clear(void);
  void ClearAndDelete(void);

  void Project(Space *projection);

  void Transform(const AMatrix& m);
	
  //  void FindAdjacencies(void);
  void EliminateEmptySolids(void);
  void EnsureAdjacencies(void);

  void FindSilhouettes(void);
  
  void RemoveHiddenSolids(void);	

  void DrawIntoVoxelArray(Voxel *voxel_array, long *minimum, long *maximum);

//  void DrawOntoBitmapFilled(Bitmap& bitmap);
//  void DrawOntoBitmapWireframe(Bitmap& bitmap);
//  void DrawOntoBitmapX(Bitmap& bitmap);

  void DrawUsingOpenGL1D(bool outline, bool fill);
  void DrawUsingOpenGL2D(bool outline, bool fill);
  void DrawUsingOpenGL3D(bool outline, bool fill);

  bool IntersectHyperplanes(std::vector<Hyperplane *>& hyperplanes, Vector& intersection);
  
  bool PointInsideHalfspace(Halfspace& halfspace, Vector& point);
  bool PointInsideHalfspaces(std::vector<Halfspace *>& halfspaces, Vector& point);
  bool PointInsideOrOnHalfspace(Halfspace& halfspace, Vector& point);
  bool PointInsideOrOnHalfspaces(std::vector<Halfspace *>& halfspaces, Vector& point);
  
};

#endif
