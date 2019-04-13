//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid.h
//|
//| This is the interface to the Solid class.  A solid is an object in n-space.
//| It is described as the intersection of a collection of half-spaces.
//|_____________________________________________________________________________

#ifndef HSOLID
#define HSOLID

#include "color.h"
#include "light.h"
#include <vector.h>


class AMatrix;
class Face;
class Vector;

class Solid
{
  
private:
  
  unsigned long dimension;

  Solid *silhouette;

  vector<Face *> faces;
  vector<Vector *> corners;
  bool adjacencies_valid;
   
protected:
  
  Color color;
  
public:
  
  Solid(long dimension);
  Solid(Solid &solid);
  ~Solid(void);

  const vector<Vector *> &Corners(void) const;

  void SetColor(double red, double green, double blue);
  void SetColor(Color& new_color);
  void GetColor(double &red, double &green, double &blue) const;
  void GetColor(Color& new_color);

  void DumpAdjacencies(void);
  void DumpFaces(void);
  void FindAdjacencies(void);
  void EnsureAdjacencies(void);

  //  void FindAllIntersections(vector<Face *>& intersected_faces, short first_available, short num_needed, long dimension);

  void ProcessCorner(Vector *corner, vector<Face *>& contributing_faces);
  
  void Project(vector<Solid *>& projected_solids, vector<Light>& lights, const Color &ambient);
  void Translate(Vector& offset);
  void Transform(const AMatrix& m);
  void AddFace(Face *face);
  bool IsEmpty(void);
  int OrderSolids(Solid& solid);
  void Duplicate(Solid& copy);
  void Subtract(Solid& solid, vector<Solid *>& difference);
  
  void FindSilhouette(void);
  Solid *GetSilhouette(void);
  void EnsureSilhouette(void);

  void ScanConvert(Color *voxel_array, long *minima, long *maxima);

  void DrawUsingOpenGL1D(bool outline, bool fill);
  void DrawUsingOpenGL2D(bool outline, bool fill);
  void DrawUsingOpenGL3D(vector<Light>& lights, const Color &ambient, bool outline, bool fill);


#if 0
  void DrawPolygonWireframe(Bitmap& bitmap);
  void DrawPolygonFilled(Bitmap& bitmap);
  void DrawLineSegment(Bitmap& bitmap);
#endif

  void DrawUsingOpenGL(void);
 
 };

#endif
