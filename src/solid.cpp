//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid.cp
//|
//| This is the implementation of the Solid class.  A solid is an object in
//| n-space.  It is described as the intersection of a collection of half-spaces.
//|_____________________________________________________________________________

#include "adsoda_types.h"
#include "face.h"
#include "intersect.h"
#include "light.h"
#include "vector.h"
#include "solid.h"
#include "debug.h"

#include <stdlib.h>
#include <iostream>
#include <float.h>
#include <stdio.h>
//#include <algo.h>
#include <math.h>

#include <OpenGL/gl.h>        /* graphics library    */
#include <GLUT/glut.h>

//============================= Protypes ============================//

void GLColor(double red, double green, double blue);
int ComparePointers(const void *ptr1, const void *ptr2);
int CompareCornersCoord2(const void *corner1, const void *corner2);
int CompareCornersRoughly(const void *corner1, const void *corner2);
void GLPoint1D(const Vector &p);
void GLPoint2D(const Vector &p);
void GLPoint3D(const Vector &p, bool dump);

extern bool LeftSide(double start_h, double start_v, double end_h, double end_v, double point_h, double point_v);


//====  GLOBALS

//extern long znear,zfar;
//extern int cube,thick;




//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Solid
//|
//| Purpose: This method creates a Solid.
//|
//| Parameters: dim: dimension of the solid
//|_________________________________________________________________________________

Solid::Solid(long dim)
{
  
  dimension = dim;

  color.red = 0;
  color.green = 0;
  color.blue = 0;

  //  No silhouette computed yet
  silhouette = NULL;

  //  Adjacencies not yet computed
  adjacencies_valid = false;

}  //==== Solid::Solid() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Solid
//|
//| Purpose: This method creates a copy of this solid which is a copy of another
//|          Solid
//|
//| Parameters: solid: the Solid to duplicate.
//|_________________________________________________________________________________

Solid::Solid(Solid& solid)
  // : Faces(solid.faces),        //  Copy the Faces list
  // : Corners(solid.corners)       //  Copy the Corners list
{
 
  //  Set dimension to same as dimension of solid
  dimension = solid.dimension;

  //  Copy the color of the Solid
  color = solid.color;

  // Copy the faces
  for (std::vector<Face *>::iterator face = solid.faces.begin(); face != solid.faces.end(); face++)    
    AddFace(new Face(**face));

  //  No silhouette computed yet
  silhouette = NULL;

  //  Adjacencies not yet computed
  adjacencies_valid = false;

} //==== Solid::Solid() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::~Solid
//|
//| Purpose: This method disposes of a Solid.
//|
//| Parameters: none
//|_________________________________________________________________________________

Solid::~Solid(void)
{

  // delete all faces from the faces list
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++)
    delete(*face);

  // delete all corners in the corners list
  for (std::vector<Vector *>::iterator corner = corners.begin(); corner != corners.end(); corner++)
    delete(*corner);

  //  Get rid of silhouette, if any
  if (silhouette)
    delete(silhouette);

} //==== Solid::~Solid() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Corners
//|
//| Purpose: This method gets the corners of this solid
//|
//| Parameters: returns the corners vector
//|_________________________________________________________________________________

const std::vector<Vector *> &Solid::Corners(void) const {
  
  return corners;
  
}  //==== Corners() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::SetColor
//|
//| Purpose: This method sets the color of this Solid.
//|
//| Parameters: red, green, blue: the components of the color
//|_________________________________________________________________________________

void Solid::SetColor(double red, double green, double blue)
{

  color.red = red;
  color.green = green;
  color.blue = blue;

} //==== Solid::SetColor() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::SetColor
//|
//| Purpose: This method sets the color of this Solid.
//|
//| Parameters: new_color: the color
//|_________________________________________________________________________________

void Solid::SetColor(Color& new_color)
{

  color = new_color;
 
} //==== Solid::SetColor() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::GetColor
//|
//| Purpose: This method gets the color of this Solid.
//|
//| Parameters: red, green, blue: receive the components of the color
//|_________________________________________________________________________________

void Solid::GetColor(double& red, double& green, double& blue) const
{

  red = color.red;
  green = color.green;
  blue = color.blue;

} //==== Solid::GetColor() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::GetColor
//|
//| Purpose: This method gets the color of this Solid.
//|
//| Parameters: the_color: receives the color
//|_________________________________________________________________________________

void Solid::GetColor(Color& the_color)
{

  the_color = color;

} //==== Solid::GetColor() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DumpAdjacencies
//|
//| Purpose: This method dumps all the adjacency relationships of a solid to std::cout.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Solid::DumpAdjacencies(void)
{
  
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++) {

    // Get this face
    Face *this_face = *face;
    
    std::cout << "Face: ";
    this_face->DumpEquation();
    std::cout << "\n\n";
    
    std::cout << "  Adjacent faces:  ";
    
    //    long j;
    //    for (j = 1; j <= this_face->adjacent_faces.NumElements(); j++) {
    //      ((Face *) this_face->adjacent_faces.Element(j))->DumpEquation();
    //      std::cout << "\n                   ";
    //    }
  
    std::cout << "\n\n  Touching corners:  ";
    
    for (std::vector<Vector *>::iterator corner = this_face->touching_corners.begin();
	 corner != this_face->touching_corners.end();
	 corner++) {
      
      std::cout << '(';
      
      long k;
      for (k = 0; k < (*corner)->Dimension() - 1; k++)
	std::cout << (*corner)->coordinates[k] << ", ";

      std::cout << (*corner)->coordinates[k] << ")\n                     ";
    }
   
    std::cout << "\n\n";
    
  }
   
} //==== Solid::DumpAdjacencies() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DumpFaces
//|
//| Purpose: This method dumps the faces of this Solid to std::cout.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Solid::DumpFaces(void)
{
  
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++) {
    (*face)->DumpEquation();
    std::cout << '\n';
  }
  
  std::cout << '\n';
   
} //==== Solid::DumpFaces() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::EnsureAdjacencies
//|
//| Purpose: This method ensures that adjacencies are valid for this Solid.
//|          if they are not, it recomputes them.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Solid::EnsureAdjacencies(void) {

  if (!adjacencies_valid)
    FindAdjacencies();

}  //==== Ensureadjacencies() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::FindAdjacencies
//|
//| Purpose: This method finds all the corners of the solid, and sets up the
//|          touching_corners and adjacent_faces lists of each Face to indicate
//|          which corners touch a Face, and which Faces are adjacent to which
//|          other Faces.  If there are redundant Faces in this Solid, this method
//|          removes them.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Solid::FindAdjacencies(void)
{

  register long i;
  
  //  delete all corners in this list
  for (std::vector<Vector *>::iterator corner = corners.begin(); corner != corners.end(); corner++)
    delete(*corner);
  corners.erase(corners.begin(), corners.end());

  //  Loop through all faces of this Solid
  std::vector<Face *>::iterator face;
  for (face = faces.begin(); face != faces.end(); face++) {
    
    // Clear touching corners and adjacent faces list for this face
    std::vector<Vector *> &tcorners = (*face)->touching_corners;
    //    for (std::vector<Vector *>::iterator tcorner = tcorners.begin(); tcorner != tcorners.end(); tcorner++)
    //      delete(*tcorner);
    tcorners.erase(tcorners.begin(), tcorners.end());

    std::vector<Face *> &afaces = (*face)->adjacent_faces;
//    for (std::vector<Face *>::iterator aface = afaces.begin(); aface != afaces.end(); aface++)
    //      delete(*aface);
    afaces.erase(afaces.begin(), afaces.end());

  } 

  //
  //  The following algorithm finds all combinations of Faces of this Solid which can intersect
  //  in a point, and finds their intersection.  For each intersection it calls ProcessCorner to
  //  check if the intersection is a corner, and to update the appropriate adjacencies if it is.
  //
  //  The most complex code below is the code which determines which faces to intersect.  It
  //  functions like a dimension-digit counter, counting faces in the indices[] array.  It counts
  //  through all decreasing combinations of the faces; this ensures that no combinations will be
  //  missed, but that no combination will be duplicated.
  //

  //  This is the list of Faces to intersect.
  std::vector<Face *> intersected_faces;

  //  Allocate space for the indices array
  register unsigned long *indices = (unsigned long *) malloc(dimension * sizeof(unsigned long));
  ASSERT(indices != NULL);
 
  //  Initialize indices to minimum values
  unsigned long j;
  for (j = 0; j < dimension; j++)
    indices[j] = dimension - j;
  
  //  This holds the digit to be incremented next.  It is used to generate carries when a digit
  //  passes its maximum value.
  register unsigned long carry_digit = 0;
              
  //  Start one under so first set of indices will be the minimum values.
  indices[carry_digit]--;
 
  //  Loop until we have carry out of last digit
  while (carry_digit != dimension) {

    //  Get the carry digit  
    register unsigned long index = indices[carry_digit];
  
    //  If this index is at its max, generate carry to next digit, and start loop over
    if (indices[carry_digit] == (faces.size() - carry_digit)) {
      carry_digit++;        //  Generate carry into next digit
      continue;         //  Start loop over
    }

    //  Loop through all indices to right of carry digit, initializing them
    for (i = carry_digit; i >= 0; i--)
      indices[i] = index + 1 + (carry_digit-i);

    //  Start carry at digit 0 again (for next time through loop)  
    carry_digit = 0;

    //
    //  At this point indices contains the sort of permutation we are looking for:
    //  it is ordered strictly descending.  We now make a list of the corresponding
    //  faces and find their intersection.
    //

    // Loop through all indices, adding corresponding Face to the list
    for (j = 0; j < dimension; j++)
      intersected_faces.push_back(faces[indices[j] - 1]);

    //  Create a new point for the intersection  
    Vector *point = new Vector(dimension);

    //  Intersect the hyperplanes.
    bool valid = point->IntersectHyperplanes(intersected_faces);

    // If it's valid, process this intersection
    if (valid)
      ProcessCorner(point, intersected_faces);

    //  No intersection found-- discard point    
    else 
      delete(point);

    //  Clear faces list for next time through the loop
    intersected_faces.erase(intersected_faces.begin(), intersected_faces.end());
    
  }  // while no carry past end
  
  free(indices);

  //  Loop through all faces
  for (face = faces.begin(); face != faces.end(); ) {

    //  If this face touches more than dimension corners, it's a real face.    
    if ((*face)->touching_corners.size() >= dimension)
      face++;
    
    //  It doesn't touch dimension corners, so it's a redundant face; remove it   
    else {
      //      delete(*face);  //  DEBUG
      faces.erase(face);
      face = faces.begin();
    }
   
  } //  loop through faces

  //  Adjacencies are now valid
  adjacencies_valid = true;

}  //==== Solid::FindAdjacencies() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::ProcessCorner
//|
//| Purpose: This method is called when FindAllIntersections finds the intersection
//|          of a set of Faces.  This checks to see if the intersection is really a
//|          corner of this Solid, and if it is it adds it to the Solid's corners
//|          list and updates the touching_corners and adjacent_faces Lists of the
//|          contributing Faces.
//|
//| Parameters: corner:             the intersection point
//|             contributing_faces: the Faces whose intersection is corner
//|_________________________________________________________________________________

void Solid::ProcessCorner(Vector *corner, std::vector<Face *>& contributing_faces)
{

  //  If this point is not in the intersection of all the faces of this Solid, it is
  //    not a corner, and we should delete it and ignore it.
  if (!corner->InsideOrOnHalfspaces(*((std::vector<Halfspace *> *) &faces))) {
    delete(corner);
    return;
  } 

  // if (corners.Contains(corner))
  // corners.AppendUnique(corner, CompareCornersRoughly); //  Add this corner to this Solid's corners list

  //  Add this corner to this Solid's corners list
  corners.push_back(corner);

  //  Loop through all contributing Faces
  for (std::vector<Face *>::iterator face = contributing_faces.begin(); face != contributing_faces.end(); face++) {
    
    //    Face *this_face = (Face *)contributing_faces.Element(i);
  
    //  Add this corner to the touching_corner list
    //  if (!this_face->touching_corners->Contains(corner))
    //  this_face->touching_corners.
    //    AppendUnique(corner, CompareCornersRoughly);
    //               //  for this Face, if it's not already there.

    //  Add this corner to the touching_corner list for this face
    (*face)->touching_corners.push_back(corner);

    // Loop through all contributing faces
    for (std::vector<Face *>::iterator aface = contributing_faces.begin(); aface != contributing_faces.end(); aface++) {
      
      //  Don't add a Face to its own adjacent_faces list
      if (face != aface) {
 
	//  Add this Face to the adjacent_faces list for this Face, if it's not already there.
	std::vector<Face *>::iterator findi = (*face)->adjacent_faces.begin();
	for ( ; findi != (*face)->adjacent_faces.end(); findi++)
	  if (*findi == *aface)
	    break;
	if (findi == (*face)->adjacent_faces.end())
	  (*face)->adjacent_faces.push_back(*aface);
 
      }  // if i != j
  
    }  //  end for j
    
  }  //  end for i


}  //==== Solid::ProcessCorner ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Procedure ComparePointers
//|
//| Purpose: This procedure compares two pointers to see if they are equal.  If
//|          they are, it returns 0.  This is used as a callback for PtrList::Compare,
//|          as used in Solid::ProcessCorner.
//|
//| Parameters: pointer1: a pointer
//|             pointer2: another pointer
//|_________________________________________________________________________________

int ComparePointers(const void *pointer1, const void *pointer2)
{

  return ((long) pointer2) - ((long) pointer1);

} //==== ComparePointers() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::CompareCornersCoord2
//|
//| Purpose: This procedure compares the second coordinate of two points.  It returns
//|          0 if they are the same, a negative number if the first point is greater
//|          than the second, or a positive number if the first point is less than
//|          the second.  This is used by DrawPolygon to QSort the corner points into
//|          vertical order.
//|
//| Parameters: 
//|___________________________________________________________________________________

int CompareCornersCoord2(const void *corner1, const void *corner2)
{

  return (int) ((*((Vector **) corner2))->coordinates[1] - (*((Vector **) corner1))->coordinates[1]);

} //==== CompareCornersCoord2() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::CompareCornersRoughly
//|
//| Purpose: This procedure compares two points.  It compares all coordinates, and
//|          returns 0 if they are roughly equal and 1 if they are not.
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|___________________________________________________________________________________

int CompareCornersRoughly(const void *corner1, const void *corner2)
{

  //  Quick access to coordinates
  register double *coords1 = ((Vector *) corner1)->coordinates;
  register double *coords2 = ((Vector *) corner2)->coordinates;

  //  Loop through all coordinates
  register long num_coords = ((Vector *) corner1)->Dimension();
  register long i;
  for (i = 0; i < num_coords; i++) {
    
    register double difference = coords1[i] - coords2[i];

    //  Found different coordinates; not equal    
    if ((difference > VERY_SMALL_NUM) || (difference < -VERY_SMALL_NUM))
      return 1;
   
  }
 
  //  No different coordinates; equal
  return 0;
  
}  //==== CompareCornersRoughly() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Project
//|
//| Purpose: This procedure projects an object onto the x1x2...x(n-1)
//|          hyperplane (the hyperplane where the xn coordinate is 0).  It creates
//|          a set of (n-1)-dimensional object which are the projection of this
//|          object's faces onto that hyperplane.  This assumes that the object
//|          is entirely above the hyperplane (that all xn coordinates are greater
//|          than 0).
//|
//| Parameters: projected_solids: Receives a list of (n-1)-dimensional Solids which
//|                               are the projections of this Solid's faces.
//|             lights:           a list of lights in this space (determines the
//|                               color of projected Faces.
//|_________________________________________________________________________________

void Solid::Project(std::vector<Solid *>& projected_solids, std::vector<Light>& lights, const Color &ambient) {

  //
  //
  //  When an n-dimensional Solid is projected onto (n-1)-space, each face of that
  //  object becomes a separate (n-1)-dimensional Solid.  For instance, a cube
  //  is projected onto 2-space an a set of six parallelograms, each corresponding to
  //  a single face of the cube.  This procedure generates the projections of the
  //  faces of a Solid.
  //
  //  Each face projects to a new object, so we need to figure out the boundaries of
  //  that object.  The boundaries are found by intersecting the face with each
  //  adjacent face-- each such intersection yields a boundary, and all together they
  //  yield a complete description of the projection face Solid.
  //
  //  The projection of the intersection of two halfspaces
  //
  //    a1*x1 + a2*x2 + ... + an*xn + k > 0
  //    b1*x1 + b2*x2 + ... + bn*xn + j > 0
  //
  //  is the hyperplane
  //
  //    (bn*a1 - an*b1)x1 + (bn*a2 - an*b2)x2 + ... + (bn*a(n-1) - (an*b(n-1))x(n-1)
  //                                                                   + (bn*k - an*j) > 0
  //
  //  In n-space this is bounded by a hyperplane extendeding out of the x1x2...x(n-1)
  //  hyperplane, since the xn coefficient is 0.  We can project this onto
  //  (n-1)-space as desired by simply considering this to be an equation in (n-1)-space.
  //  This yields the correct boundary equation of the (n-1)-Solid, up to the sign.
  //  We check the sign by finding a corner of the n-Solid which bounds the face we're
  //  projecting, and which does not bound the adjacent face.  We then check whether the
  //  projection of that point is inside the projected halfspace (as it should
  //  be, since the entire projected face should be inside all its boundary halfspaces).
  //  If it is not, we reverse the projected halfspace.
  //
  //  Note that we also use backface culling here to eliminate from consideration all
  //  faces pointing away from the x1x2...x(n-1) hyperplane.
  //
  

  //  Loop through all faces of this Solid
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++) {
    
    //  Construct the projection of this face
    Solid *projection = new Solid(dimension-1);

    //  Get this face
    Face *this_face = *face;
  
    //  If the normal vector points downward, this face is pointing away from
    //    the projection hyperplane and can be ignore (backface culling).
    if (this_face->coordinates[dimension-1] <= 0) {
      delete(projection);
      continue;
    }

    //  Create Vector to hold normalized normal
    Vector normalized_normal(dimension);
    
    //  Copy all coordinates of the normal
    register unsigned long i;
    for(i = 0; i < dimension; i++)
      normalized_normal.coordinates[i] = this_face->coordinates[i];
 
    //  Normalize face normal
    normalized_normal.Normalize();

    //  Add all lights' contributions to intensity
    double lights_red = ambient.red;
    double lights_green = ambient.green;
    double lights_blue = ambient.blue;
    register unsigned long k;
    for (std::vector<Light>::iterator light = lights.begin(); light != lights.end(); light++)
      (*light).Apply(normalized_normal, lights_red, lights_green, lights_blue);
    
    //  Adjust face color according to light intensity
    projection->SetColor(color.red * lights_red, color.green * lights_green, color.blue * lights_blue);

    //  Loop through all adjacent faces
    for (std::vector<Face *>::iterator aface = this_face->adjacent_faces.begin();
	 aface != this_face->adjacent_faces.end();
	 aface++) {
      
      //  Create a face for the projection of the intersection of this_face with adjacent_face.
      Face *projection_face = new Face(dimension-1);
      
      //  Compute repeated terms
      //      std::cout << "dimension: " << dimension << endl;
      //      std::cout << "this_face: " << (unsigned long) this_face << endl;
      //      std::cout << "(*aface): " << (unsigned long) (*aface) << endl;
      double an = this_face->coordinates[dimension-1];
      double bn = (*aface)->coordinates[dimension-1];

      //  Compute projection of intersection
      for (k = 0; k <= dimension-2; k++)
	projection_face->coordinates[k] = bn*this_face->coordinates[k] - an*(*aface)->coordinates[k];
   
      //  Compute the constant term
      projection_face->coordinates[k] = bn*this_face->coordinates[dimension] -
	an*(*aface)->coordinates[dimension];

      //
      //  We have the equation of this face of the projection, up to a sign.
      //  Now we find a corner which is on this_face but not on the adjacent face
      //
   
      //  Loop through all corners which touch this_face
      bool corner_found = false;
      std::vector<Vector *>::iterator corner = this_face->touching_corners.begin();
      for (; corner != this_face->touching_corners.end(); corner++) {

	if (corner_found)
	  break;
 
	//  Check if this corner also touches the adjacent face    
	std::vector<Vector *> &tcorners = (*aface)->touching_corners;
	std::vector<Vector *>::iterator tc;
	for (tc = tcorners.begin(); tc != tcorners.end(); tc++)
	  if (*tc == *corner)
	    break;
	if (tc == tcorners.end()) {

	  // We found an acceptable corner    
	  corner_found = true;
	  //	  break;

	  //  Temporarily knock this corner down a dimension.  This is the same as
	  //    discarding the last coordinate, which just projects it onto the
	  //    projection hyperplane.
	  (*corner)->dimension--;
   
	  //  Check if the projected corner is inside the projection face
	  //  If to, negate this equation (flip the normal)
	  if (!(*corner)->InsideHalfspace(*((Halfspace *) projection_face)))
	    projection_face->Negate();     

	  //  Restore the corner to its proper dimension
	  (*corner)->dimension++;
   
	}  // if also touches adjacent face
     
	//  This corner didn't work; look at next
	//	else {
	//	  k++;
	//	}
    
      } //  end while (!corner_found)

      //      if (corner != this_face->touching_corners.end())
	
      //  Add a face to the projection of this_face
      if (corner_found)
	projection->AddFace(projection_face);
      else
	delete(projection_face);

    } //  end for aface
    
    //  Add this face's projection to the list
    projected_solids.push_back(projection);
  
  } //  end for face

}  //==== Solid::Project() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::EnsureSilhouette
//|
//| Purpose: This method ensures that silhouette is valid for this Solid.
//|          if it is not, it recomputes it.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Solid::EnsureSilhouette(void) {

  if (!silhouette)
    FindSilhouette();

}  //==== EnsureSilhouette() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::FindSilhouette
//|
//| Purpose: This method finds the silhouette of the projection of this Solid
//|          onto the x1x2...x(n-1) hyperplane (the hyperplane where the xn
//|          coordinate is 0).  The silhouette is an (n-1)-dimensional object on
//|          the x1x2...x(n-1) hyperplane.  This method creates creates an
//|          n-dimensional object which is the extrusion of the silhouette in
//|          the xn direction.  For instance, if the silhouette were a square,
//|          this would create an infinitly long square tunnel perpendicular to
//|          the x1x2...x(n-1) hyperplane.
//|
//| Parameters: none.
//|_________________________________________________________________________________

void Solid::FindSilhouette(void) {

  //  VERIFY(this);

  Debug("FindSilhouette: this=" << this << "; dimension=" << dimension << std::endl);

  //  Get rid of any silhouette which exists
  if (silhouette) {
    Debug("deleting Solid: " << silhouette << std::endl);
    delete(silhouette);
  }

  //  Create the silhouette Solid
  silhouette = new(Solid)(dimension);
 
  //
  //  This method is similar to Solid::Project, but it ignores all projected
  //  edges except those that are between a front face and a back face.  All such
  //  edges are faces of the silhouette, and are added to the silhouette Solid.
  //

  EnsureAdjacencies();

  //  Loop through all faces of this Solid
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++) {

    Face *this_face = *face;

    //  Only consider backfaces
    if (this_face->coordinates[dimension-1] > 0)
      continue;
    
    //  Loop through all adjacent faces
    for (std::vector<Face *>::iterator aface = this_face->adjacent_faces.begin();
	 aface != this_face->adjacent_faces.end();
	 aface++) {
   
      //  Only consider frontfaces
      if ((*aface)->coordinates[dimension-1] <= 0)
	continue;
      
      //  Create a face for the projection of the intersection of this_face with adjacent_face.
      Face *projection_face = new Face(dimension);

      //  Compute repeated terms
      double an = this_face->coordinates[dimension-1];
      double bn = (*aface)->coordinates[dimension-1];
   
      //  Compute projection of intersection
      register unsigned long k;
      for (k = 0; k <= dimension-2; k++)
	projection_face->coordinates[k] = bn*this_face->coordinates[k] - an*(*aface)->coordinates[k];
   
      //  Make the last term 0.  This means that any xn will
      //   satisfy the equation, so it extrudes the 
      //   silhouette in the xn direction.
      projection_face->coordinates[k++] = 0.0;

      //  Compute the constant      
      projection_face->coordinates[k] =
	bn*this_face->coordinates[dimension] - an*(*aface)->coordinates[dimension];

      //
      //  We have the equation of this face of the projection, up to a sign.
      //  Now we find a corner which is on this_face but not on the adjacent face
      //
   
      //  Loop through all corners which touch this_face
      bool corner_found = false;
      k = 1;
      while (!corner_found) {
 
	Vector *corner = (Vector *) this_face->touching_corners[k-1];
    
	//  Check if this corner also touches the adjacent face
	std::vector<Vector *> &tcorners = (*aface)->touching_corners;
	std::vector<Vector *>::iterator tc;
	for (tc = tcorners.begin(); tc != tcorners.end(); tc++)
	  if (*tc == corner)
	    break;
	if (tc == tcorners.end()) {
	  //	if (!(*aface)->touching_corners.Contains(corner)) {

	  // We found an acceptable corner    
	  corner_found = true;

	  //  Check if the projected corner is inside the projection face;
	  // if so, negate this equation (flip the normal)
	  if (!corner->InsideOrOnHalfspace(*((Halfspace *) projection_face)))
	    projection_face->Negate();
     
	}  // if corner touches adjacent face
  
	//  This corner didn't work; look at next   
	else
	  k++;
 
      } //  end while (!corner_found)
      
      //  Add a face to the silhouette
      silhouette->AddFace(projection_face);
      
    } //  end for j
    
  } //  end for i
  
}  //==== Solid::FindSilhouette() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::GetSilhouette
//|
//| Purpose: This method returns the silhouette of this Solid.  You must call
//|          FindSilhouette before calling this method.
//|
//| Parameters: returns a pointer to the silhouette.
//|_________________________________________________________________________________

Solid *Solid::GetSilhouette(void)
{

  //  ASSERT(silhouette != NULL);
  return silhouette;

} //==== Solid::GetSilhouette() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Translate
//|
//| Purpose: This method translates a solid through n-space.
//|
//| Parameters: offset: the vector indicating the direction and distance to 
//|                     translate this halfspace.
//|_________________________________________________________________________________

void Solid::Translate(Vector& offset)
{

  // Translate all faces
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++)
    (*face)->Translate(offset);

  adjacencies_valid = false;

} //==== Solid::Translate() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Transform
//|
//| Purpose: This method applies a matrix transformation to this Solid.
//|
//| Parameters: m: the matrix of the transformation to apply.
//|_________________________________________________________________________________

void Solid::Transform(const AMatrix& m)
{

  // Transform all faces
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++)
    (*face)->Transform(m);

  adjacencies_valid = false;

} //==== Solid::Transform() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::AddFace
//|
//| Purpose: This method adds another face to the definition of this solid.
//|
//| Parameters: face: the Face to add
//|_________________________________________________________________________________

void Solid::AddFace(Face *face) {

  faces.push_back(face);

  adjacencies_valid = false;

} //==== Solid::AddHalfspace() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::IsEmpty
//|
//| Purpose: This method returns true if this is an empty solid (if the intersection
//|          of the half-spaces is empty).
//|
//| Parameters: none
//|_________________________________________________________________________________

bool Solid::IsEmpty(void)
{

  return false;

} //==== Solid::IsEmpty() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::OrderSolids
//|
//| Purpose: This method returns BEHIND if this Solid is behind solid, INFRONT if
//|          it is in front of solid, or NEITHER otherwise.  You must call
//|          FindSilhouette and FindAdjacencies for both this Solid and solid
//|          before calling this. 
//|
//| Parameters: solid: the Solid to compare with this Solid
//|_________________________________________________________________________________

int Solid::OrderSolids(Solid& solid)
{

  EnsureSilhouette();
  solid.EnsureSilhouette();
  EnsureAdjacencies();
  solid.EnsureAdjacencies();
 
  //
  //  Here we check all the corners of this Solid.  If any corner is inside the silhouette of solid,
  //  we check whether it's in front of a frontface or behind a backface-- this determines whether
  //  this Solid is in front of or behind solid.
  //
  
  //  Loop through all corners in this Solid
  std::vector<Vector *>::iterator corner;
  for (corner = corners.begin(); corner != corners.end(); corner++) {
  
    //  Check if this corner is in the silhouette of solid; if not, check next corner
    if (!(*corner)->InsideHalfspaces(*((std::vector<Halfspace *> *) &(solid.silhouette->faces))))
      continue;
  
    //  Loop through all faces of solid
    for (std::vector<Face *>::iterator face = solid.faces.begin(); face != solid.faces.end(); face++) {
  
      //  If this is a backface, and if corner is behind it,
      //   then this Solid must be behind solid      
      if ((*face)->coordinates[dimension-1] <= 0) {
	if (!(*corner)->InsideHalfspace(**face))
	  return BEHIND;      
      }

      //  This is a frontface; if corner is in front of it,    
      //   then this Solid must be in front of solid
      else {
	if (!(*corner)->InsideHalfspace(**face))
	  return INFRONT;
      }
   
    }  // faces
    
  }  // corners
  
  //
  //  We didn't find any corners in the silhouette, but this doesn't necessarily mean the
  //  Solids are disjoint-- it could be that all the corners of one lie outside the silhouette
  //  of the other, but the silhouette of the one entirely encloses the silhouette of the other.
  //  here we test for this by doing the same check as above, but in reverse.
  //
  
  //  Loop through all corners in solid
  for (corner = solid.corners.begin(); corner != solid.corners.end(); corner++) {
    
    //  If this corner is not in silhouette of this Solid, check next corner
    if (!(*corner)->InsideHalfspaces(*((std::vector<Halfspace *> *) &(silhouette->faces))))
	continue;
    
    //  Loop through all faces of this Solid
    for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++) {
  
      //  If this is a backface, and if corner is behind it, then solid must be behind this Solid
      if ((*face)->coordinates[dimension-1] <= 0)
	{
	  if (!(*corner)->InsideHalfspace(**face))
	    return INFRONT;
	}

      //  If this is a frontface, and if corner is in front of it, then solid must be in front of this Solid
      else
	{
	  if (!(*corner)->InsideHalfspace(**face))
	    return BEHIND;
	}
      
    }  // faces
    
  }  // corners
 
  return NEITHER;          //  Silhouettes are disjoint

} //==== Solid::OrderSolids() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Duplicate
//|
//| Purpose: This method creates a copy of this solid.
//|
//| Parameters: copy: receives a copy of this solid.
//|_________________________________________________________________________________

void Solid::Duplicate(Solid& copy)
{
 
  //  Set dimension of copy same as dimension of this
  copy.dimension = dimension;

  //  Copy the faces and corners
  for (std::vector<Face *>::iterator face = copy.faces.begin(); face != copy.faces.end(); face++)
    AddFace(new Face(**face));
  for (std::vector<Vector *>::iterator corner = copy.corners.begin(); corner != copy.corners.end(); corner++)
    corners.push_back(new Vector(**corner));

  adjacencies_valid = false;

} //==== Solid::Duplicate() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::Subtract
//|
//| Purpose: This method creates a list of Solids which together form the difference
//|          of solid from this Solid.  The difference contains everything that
//|          is in this Solid, but not in solid.
//|
//| Parameters: solid:      the Solid to subtract from the Solid
//|             difference: receives difference (a collection of Solids)
//|_________________________________________________________________________________

void Solid::Subtract(Solid& solid, std::vector<Solid *>& difference)
{

  //  Make a copy of this Solid
  register Solid *remainingSolid = new Solid(*this);

  //  difference.Append(remainingSolid);
  //  return;

  //  Loop through all faces of solid
  for (std::vector<Face *>::iterator face = solid.faces.begin(); face != solid.faces.end(); face++) {
  
    Solid *insideSolid;
    Solid *outsideSolid;
  
    //  Slice what remains of this Solid along this face
    (*face)->SliceSolid(*remainingSolid, insideSolid, outsideSolid);
  
    //  The section of the solid which is outside this face is part of the difference (since it is
    //   outside solid).
    difference.push_back(outsideSolid);
  
    //  We don't need the solid we just sliced anymore
    delete(remainingSolid);
  
    //  Save what was inside this face to be sliced by other faces.      
    remainingSolid = insideSolid;

  }  // faces

  delete(remainingSolid);

} //==== Solid::Subtract() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::ScanConvert
//|
//| Purpose: This method scan converts this Solid into a voxel array.  The array
//|          must be of the same dimension as this Solid, and minima and maxima
//|          must contain the minumum and maximum index into the array for each
//|          dimension.
//|
//| Parameters: voxel_array: the array of Voxels to scan convert into.
//|             minima:      an array of doubles; each element represents the minimum
//|                          index into voxel_array for the corresponding dimension.
//|             maxima:      an array of doubles; each element represents the maximum
//|                          index into voxel_array for the corresponding dimension.
//|_________________________________________________________________________________

void Solid::ScanConvert(Voxel *voxel_array, long *minimum, long *maximum)
{

  //  Create the point used to scan
  Vector *point = new(Vector)(dimension);

  //  Get quick access to the coordinates
  double *point_coords = point->coordinates;
 
  //  Set point to minimum in all dimensions
  register unsigned long i;
  for (i = 0; i < dimension; i++)
    point_coords[i] = minimum[i];
  
  //
  //  The following code runs through the entire voxel array.  It runs through all possible
  //  combinations of the coordinates in order.  The operation of this code can be likened to
  //  a multi-digit counter; the first coordinate is incremented repeatedly until it passes its end
  //  value, at which point it is set to its start value and a carry is generated which causes
  //  the next coordinate to be incremented, and possible to generate a carry into the next digit.
  //
  //

  //  Start at beginning of voxel array  
  Voxel *voxel_array_element = voxel_array;
  
  bool carry;
  i = 0;
  do {
    //  i indicates which coordinate we are
    //  currently incrementing.  The loop
    //  terminates when i is greater than
    //  dimension-1 (there is a carry out of
    //  last coordinate).
    
    //  If we're inside the solid, mark it the color of
    //    this solid.
    if (point->InsideOrOnHalfspaces(*((std::vector<Halfspace *> *) &faces)))
      *voxel_array_element = color;
  
    //  Go to next voxel array element
    voxel_array_element++;
  
    carry = false;
    do {

      //  Increment a coordinate
      point_coords[i] += 1.;
  
      //  Check if we've passed end
      if (point_coords[i] > maximum[i]) {

	//  Passed end; start over again at start
         //  and increment the next coordinate
	point_coords[i] = minimum[i];
	i++;
	
	//  Carry out of last coordinate; exit loop 
	if (i == dimension) break;
	
	//  We have a "carry" into the next coordinate
	carry = true;

      }  // if past end

      //  No carry; start again with coordinate 0  
      else {
	i = 0;
	carry = false;
      }
      
    } while (carry);
    
  } while (i != dimension);

} //==== Solid::ScanConvert() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DrawUsingOpenGL3D
//|
//| Purpose: This method draws this as a 3D object using OpenGL.
//|          The dimension of this solid must be 3
//|
//| Parameters: lights:  the lights in the space where we are drawing this
//|             ambient: the ambient color of the space where we are drawing this
//|             outline: true iff the faces should be outlined with wireframe
//|             fill:    true iff the faces should be filled with solid color
//|_________________________________________________________________________________


Vector *p1;
Vector *p2;
Vector *p1p2;
Vector *polygonNormal3D;

void DumpVector3D(const Vector &v) {

  std::cout << "(" << v.coordinates[0] << ", " << v.coordinates[1] << ", " << v.coordinates[2] << ")";
}

bool debugTheta = false;
double Theta(const Vector &pi) {

  Vector p1pi = pi - *p1;
  Vector p1p2Crossp1pi = p1p2->CrossProduct3D(p1pi);
  double mag_p1p2Crossp1pi = p1p2Crossp1pi.Magnitude();
  double p1p2Dotp1pi = *p1p2 * p1pi;
  double theta = atan2(mag_p1p2Crossp1pi, p1p2Dotp1pi);

  if ((p1p2Crossp1pi * (*polygonNormal3D)) < 0)
    theta = -theta;

  return theta;

}  //==== Theta() ====//



typedef Vector *VectorPtr;

class OrderVertices3D {
public:

  bool operator() (const VectorPtr &pi, const VectorPtr &pj) const {

    //
    // This comparison operator is used to order the vertices of a polygon in 3-space.
    // polygonNormal3D (global variable) is the normal of the polygon.
    // p1 and p2 (globals) are two (arbitrary) vertices of the polygon.
    // p1p2 is the vector from p2 to p1.
    //
    // This computes the angle from p1p2 to p1pi and from p1p2 to p1pj,
    // and uses it to order the vertices in right-handed order.
    // 

    return Theta(*pi) > Theta(*pj);
    //    return Theta(*pi) < Theta(*pj);

  }


};


void Solid::DrawUsingOpenGL3D(std::vector<Light>& lights, const Color &ambient, bool outline, bool fill)
{

  //  EnsureAdjacencies();
  ASSERT(adjacencies_valid);
  ASSERT(dimension == 3);

  //  Create Vector to hold normalized normal
  Vector normalized_normal(3);

  // Draw all faces
  std::vector<Vector *> vertices;
  int i = 0;
  for (std::vector<Face *>::iterator face = faces.begin(); face != faces.end(); face++) {

    i++;

    //  If the normal vector points downward, this face is pointing away from
    //    the projection hyperplane and can be ignore (backface culling).
    //    if ((*face)->coordinates[dimension-1] <= 0) {
    //      delete(projection);
    //      continue;
    //    }

    //  Copy all coordinates of the normal
    normalized_normal.coordinates[0] = (*face)->coordinates[0];
    normalized_normal.coordinates[1] = (*face)->coordinates[1];
    normalized_normal.coordinates[2] = (*face)->coordinates[2];
 
    //  Normalize face normal
    normalized_normal.Normalize();

    //  Add all lights' contributions to intensity
    double lights_red = ambient.red;
    double lights_green = ambient.green;
    double lights_blue = ambient.blue;
    //    register unsigned long k;
    for (std::vector<Light>::iterator light = lights.begin(); light != lights.end(); light++)
      (*light).Apply(normalized_normal, lights_red, lights_green, lights_blue);
    
    //  Adjust face color according to light intensity
    //    projection->SetColor(color.red * lights_red, color.green * lights_green, color.blue * lights_blue);
    double face_red = color.red * lights_red;
    double face_green = color.green * lights_green;
    double face_blue = color.blue * lights_blue;

    GLColor(face_red, face_green, face_blue);

    // Get all corners touching this face in a vector;
    // get the first corner separately; it will be the beginning and end of the polygon
    // we draw
    vertices.erase(vertices.begin(), vertices.end());
    p1 = (*face)->touching_corners[0];
    p2 = (*face)->touching_corners[1];
    polygonNormal3D = *face;
    p1p2 = new Vector(*p2 - *p1);
    std::vector<Vector *>::iterator corner = (*face)->touching_corners.begin();
    corner++;
    for (; corner != (*face)->touching_corners.end(); corner++)
      vertices.push_back(*corner);

    // Do I need to sort here, into "clockwise" order, say?
    sort(vertices.begin(), vertices.end(), OrderVertices3D());
    
    //    delete(p1p2);

    // Fill this face, if fill is on
    if (fill) {

      glBegin(GL_POLYGON);
      //      bgnpolygon();
      GLPoint3D(*p1, false);
      for (std::vector<Vector *>::iterator vertex = vertices.begin(); vertex != vertices.end(); vertex++)
	GLPoint3D(**vertex, false);
      GLPoint3D(*p1, false);
      //      endpolygon();
      glEnd();

    } // if fill

    // Outline this face, if outlining is on
    if (outline) {

      glLineWidth(2);
      glBegin(GL_LINE_LOOP);
      GLColor(1, 1, 1);
      GLPoint3D(*p1, false);
      for (std::vector<Vector *>::iterator vertex = vertices.begin(); vertex != vertices.end(); vertex++)
	GLPoint3D(**vertex, false);
      GLPoint3D(*p1, false);
      glEnd();

    }  // if outline

  }  // for face
    
} //==== Solid::DrawUsingOpenGL3D() ====//



void GLPoint3D(const Vector &p, bool dump) {

  const double *coords = p.coordinates;

  float pf[3];
  pf[0] = (float) (coords[0]/100);
  pf[1] = (float) (coords[1]/100);
  pf[2] = (float) (coords[2]/100);

  if (dump) {
    double thetap = Theta(p);
    std::cout << " (" << pf[0] << ", " << pf[1] << ", " << pf[2] << ") [" << thetap << "]  " << std::endl;
  }

  //  v3f(pf);
  glVertex3f(pf[0], pf[1], pf[2]);
  
}  //==== GLPoint3D() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DrawUsingOpenGL2D
//|
//| Purpose: This method draws this as a 2D polygon using OpenGL.
//|          Dimension MUST be 2 (this must be a polygon).  FindAdjacencies must
//|          be called before calling this.
//|
//| Parameters: 
//|_________________________________________________________________________________


typedef Vector *VectorPtr;
class YGreater {
public:
  bool operator() (const VectorPtr &x, const VectorPtr &y) const
  { return ((x)->coordinates[1] > (y)->coordinates[1]); }
};



void Solid::DrawUsingOpenGL2D(bool outline, bool fill)
{

//  EnsureAdjacencies();
  ASSERT(adjacencies_valid);
  ASSERT(dimension == 2);

  if (Corners().size() < 2)
    return;

  // Sort the corners into ascending order
  //  corners.Sort(CompareCornersCoord2);
  sort(corners.begin(), corners.end(), YGreater());

  // Create lists of points for left and right sides of the polygon
  std::vector<Vector *> left_side;
  std::vector<Vector *> right_side;
 
  // Get top and bottom points of polygon
  std::vector<Vector *>::iterator topCorner = (std::vector<Vector *>::iterator) corners.begin();
  std::vector<Vector *>::iterator bottomCorner = (std::vector<Vector *>::iterator) corners.end();
  bottomCorner--;

  // Both sides start with the top point
  left_side.push_back(*topCorner);
  right_side.push_back(*topCorner);
 
  // Get pointer to top point coordinates
  double *top_point_coords = (*topCorner)->coordinates;

  // Get pointer to bottom point coordinates
  double *bottom_point_coords = (*bottomCorner)->coordinates;
 
  // Loop through all points except top and bottom, in top-to-bottom order
  std::vector<Vector *>::iterator corner = topCorner;
  corner++;
  for (; corner != bottomCorner; corner++) {
  
    if (LeftSide(bottom_point_coords[0],
		 bottom_point_coords[1],
		 top_point_coords[0],
		 top_point_coords[1],
		 (*corner)->coordinates[0],
		 (*corner)->coordinates[1]))
      
      // Add this to left side list
      left_side.push_back(*corner);
    
    else
      
      // Add this to right side list
      right_side.push_back(*corner);
    
  }
  
  // Both sides end with the top point
  left_side.push_back(*bottomCorner);
  right_side.push_back(*bottomCorner);

  double red, green, blue;
  GetColor(red, green, blue);

  if (fill) {

    GLColor(red, green, blue);
    
    // Draw path down left side
    glBegin(GL_POLYGON);
    
    std::vector<Vector *>::iterator p;
    for (p = left_side.begin(); p != left_side.end(); p++)
      GLPoint2D(**p);
    
    // Draw path down right side
    for (p = right_side.end(); ; ) {
      p--;
      GLPoint2D(**p);
      if (p == right_side.begin())
	break;
    }

    glEnd();
    
  }  // if polygon

  if (outline) {

    GLColor(1, 1, 1);
    
    // Draw path down left side
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    
    std::vector<Vector *>::iterator p;
    for (p = left_side.begin(); p != left_side.end(); p++)
      GLPoint2D(**p);
    
    // Draw path down right side
    for (p = right_side.end(); ; ) {
      p--;
      GLPoint2D(**p);
      if (p == right_side.begin())
	break;
    }

    glEnd();

  } // if outline

} //==== Solid::DrawUsingOpenGL2D() ====//


void GLPoint2D(const Vector &p) {

  const double *coords = p.coordinates;

  float pf[3];
  pf[0] = (float) (coords[0]/100);
  pf[1] = (float) (coords[1]/100);
  pf[2] = (float) 0;
  
  //  v3f(pf);
  glVertex3f(pf[0], pf[1], pf[2]);
  
}  //==== DrawLineTo() ====//
  


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Procedure LeftSide
//|
//| Purpose: Find which side of a ray a point is on
//|
//| Parameters: point: point to classify
//|             ray_start: start point of ray
//|             ray_end:   endpoint of ray
//|             returns TRUE if point is on the left, looking from ray_start to ray_end
//|_____________________________________________________________________________________

bool LeftSide(double start_h, double start_v, double end_h, double end_v, double point_h, double point_v)
{
  
  register double dot_product = (end_v - start_v) * (point_h - start_h) + (start_h - end_h) * (point_v - start_v);
  
  return (dot_product < 0);

} //==== LeftSide() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DrawUsingOpenGL1D
//|
//| Purpose: This method draws this as a 1D line segment using OpenGL.
//|          Dimension MUST be 1 (this must be a line segment).  FindAdjacencies must
//|          be called before calling this.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Solid::DrawUsingOpenGL1D(bool outline, bool fill)
{

  //  EnsureAdjacencies();
  ASSERT(adjacencies_valid);
  ASSERT(dimension == 1);

  double red, green, blue;
  GetColor(red, green, blue);
  GLColor(red, green, blue);

  // Draw the line segment
  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
  GLPoint1D(*(corners[0]));
  GLPoint1D(*(corners[1]));
  glEnd();

} //==== Solid::DrawUsingOpenGL2D() ====//



void GLPoint1D(const Vector &p) {

  const double *coords = p.coordinates;

  float pf[3];
  pf[0] = (float) (coords[0]/100);
  pf[1] = (float) -2;
  pf[2] = (float) 0;
  
  glVertex3f(pf[0], pf[1], pf[2]);
  //  v3f(pf);
  
}  //==== GLPoint1D() ====//



void GLColor(double red, double green, double blue) {
  
  //  unsigned long ired = (unsigned long) (red*255);
  //  unsigned long igreen = (unsigned long) (green*255);
  //  unsigned long iblue = (unsigned long) (blue*255);
//  unsigned long packcolor = ired;
  //  packcolor += igreen*256;
  //  packcolor += iblue*256*256;
  //  cpack(packcolor);
  glColor3d(red, green, blue);

}  //==== GLColor() ====//



#if 0

//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DrawPolygonWireframe
//|
//| Purpose: This method draws this onto a Bitmap as a wireframe polygon.
//|          Dimension MUST be 2 (this must be a polygon).  FindAdjacencies must
//|          be called before calling this.
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|_________________________________________________________________________________

void Solid::DrawPolygonWireframe(Bitmap& bitmap)
{

  EnsureAdjacencies();
  //  ASSERT(adjacencies_valid);
  ASSERT(dimension == 2);

  corners.Sort(CompareCornersCoord2);     //  Sort the corners into ascending order
 
  PtrList left_side; 
  PtrList right_side;         //  Create lists of points for left and
  //    right sides of the polygon
 
  Vector *top_point = (Vector *) corners.Element(1); //  Get top and bottom points of polygon
  Vector *bottom_point = (Vector *)
    corners.Element(corners.NumElements());
 
  left_side.Append(top_point);      //  Both sides start with the top point
  right_side.Append(top_point);
 
  double *top_point_coords = top_point->coordinates; //  Get pointer to top point coordinates
  double *bottom_point_coords =
    bottom_point->coordinates; //  Get pointer to bottom point coordinates
 
  long i;
  for (i = 2; i < corners.NumElements(); i++)  //  Loop through all points except top and bottom,
    //    in top-to-bottom order
    {
  
      Vector *this_point =
	(Vector *) corners.Element(i);  //  Get this point
  
      if (LeftSide(bottom_point_coords[0],
		   bottom_point_coords[1],
		   top_point_coords[0],
		   top_point_coords[1],
		   this_point->coordinates[0],
		   this_point->coordinates[1]))
   
	left_side.Append(this_point);    //  Add this to left side list
  
      else
  
	right_side.Append(this_point);    //  Add this to right side list
   
    }

  left_side.Append(bottom_point);      //  Both sides end with the top point
  right_side.Append(bottom_point);
 
  for (i = 1; i < left_side.NumElements(); i++)  //  Draw path down left side
    {
      double *this_point_coords = ((Vector *) left_side.Element(i))->coordinates;
      double *next_point_coords = ((Vector *) left_side.Element(i+1))->coordinates;
  
      bitmap.DrawLine(this_point_coords[0], this_point_coords[1],
		      next_point_coords[0], next_point_coords[1],
		      color);
    }

  for (i = 1; i < right_side.NumElements(); i++)  //  Draw path down right side
    {
      double *this_point_coords = ((Vector *) right_side.Element(i))->coordinates;
      double *next_point_coords = ((Vector *) right_side.Element(i+1))->coordinates;
  
      bitmap.DrawLine(this_point_coords[0], this_point_coords[1],
		      next_point_coords[0], next_point_coords[1],
		      color);
    }



} //==== Solid::DrawPolygonWireframe() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DrawPolygonFilled
//|
//| Purpose: This method draws this onto a Bitmap as a filled (solid color) polygon.
//|          Dimension MUST be 2 (this must be a polygon).  FindAdjacencies must
//|          be called before calling this.
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|_________________________________________________________________________________

void Solid::DrawPolygonFilled(Bitmap& bitmap)
{

  EnsureAdjacencies();
  //  ASSERT(adjacencies_valid);       //  Make sure the adjacencies are valid
  ASSERT(dimension == 2);        //  Make sure the dimension is 2

  if (corners.NumElements() == 0)
    return;            //  Don't draw empty polygons
  
  corners.Sort(CompareCornersCoord2);      //  Sort the corners into ascending order
 
  PtrList leftSide; 
  PtrList rightSide;          //  Create lists of points for left and
  //    right sides of the polygon
 
  Vector *topPoint = (Vector *) corners.Element(1);  //  Get top and bottom points of polygon
  Vector *bottomPoint = (Vector *)
    corners.Element(corners.NumElements());
 
  leftSide.Append(topPoint);        //  Both sides start with the top point
  rightSide.Append(topPoint);
 
  double *topPointCoords = topPoint->coordinates;   //  Get pointer to top point coordinates
  double *bottomPointCoords = bottomPoint->coordinates; //  Get pointer to bottom point coordinates
 
  register long i;
  for (i = 2; i < corners.NumElements(); i++)    //  Loop through all points except top and bottom,
    //    in top-to-bottom order
    {
  
      Vector *thisPoint =
	(Vector *) corners.Element(i);   //  Get this point
  
      if (LeftSide(bottomPointCoords[0],
		   bottomPointCoords[1],
		   topPointCoords[0],
		   topPointCoords[1],
		   thisPoint->coordinates[0],
		   thisPoint->coordinates[1]))
   
	leftSide.Append(thisPoint);      //  Add this to left side list
  
      else
  
	rightSide.Append(thisPoint);     //  Add this to right side list
   
    }

  leftSide.Append(bottomPoint);       //  Both sides end with the top point
  rightSide.Append(bottomPoint);
 

  register long currentScanline = topPointCoords[1];  //  Start scanning at the top
  register char *currentScanlineBase = (char *)
    (bitmap.rows[bitmap.bounds.bottom - currentScanline]
     - bitmap.bounds.left);  //  Find base address in Bitmap of top line

  long bottomScanline = bottomPointCoords[1];    //  Scan until the bottom

  long nextLeftVertexScanline = topPointCoords[1];
  long nextRightVertexScanline = topPointCoords[1];  //  Here we fool the loop into doing our
  //   initialization for us.  It will stop twice,
  //   once on the left and once on the right,
  //   thinking it has reached another vertex.  The
  //   vertices it reaches are in fact the top
  //   vertex, once in the left and once in the right
  //   list.
 
  long leftIndex = 0;          //  This will be incremented to 1 on the first pass
  //   through the while loop, making it point to
  //   the top point (in the left side list)

  long rightIndex = 0;         //  This will be incremented to 1 on the second pass
  //   through the while loop, making it point to
  //   the top point (in the right side list)
 

  double *leftStartCoords;
  double *rightStartCoords;        //  These contain the coordinates of the starting
  //   points of the left and right segments we are
  //   currently scanning in.
               
  double *leftEndCoords = topPointCoords;
  double *rightEndCoords = topPointCoords;    //  These contain the coordinates of the endind
  //   points of the left and right segments we are
  //   currently scanning in.  Again we are getting
  //   free initialization, this time by setting the
  //   "end" of the initial segments to the top point,
  //   which is actually the point we want to start
  //   scanning at.  The loops immediately thinks it
  //   has reached a new point, and will set the
  //   "end points" defined here (the top point's
  //   coordinates) as the coordinates of the start of
  //   the new segments, as desired.

  register double currentLeft = topPointCoords[0];  //  Start the left scanner at the top point's x
  register double currentRight = topPointCoords[0];  //  Start the right scanner at the top point's x
 
  register double leftIncrement;
  register double rightIncrement;       //  Amount of change for currentLeft and
  //   currentRight per scanline step.  These will
  //   be initialized inside the loop

  long nextVertexScanline;        //  This always contains the scanline of the next
  //   vertex we will encounter
 
  while(1)            //  Loop until we get to the bottom
    {
  
      nextVertexScanline =
	(nextLeftVertexScanline >
	 nextRightVertexScanline) ?
	nextLeftVertexScanline :
	nextRightVertexScanline;    //  Find the scanline of the next vertex
  
      for(; currentScanline > nextVertexScanline;
	  currentScanline--)      //  Loop through all scanlines until next vertex
	{
	  register Pixel *currentPixel =
	    (Pixel *) currentScanlineBase;
	  currentPixel += (int) currentLeft;    //  Point to leftmost pixel of this scanline

	  for (i = currentLeft; i <= currentRight; i++) //  Loop through all pixels in this scanline
	    {
	      *currentPixel = color;      //  Set pixel
	      currentPixel++;        //  Point to next pixel
	    }
    
   
	  currentScanlineBase += bitmap.row_bytes;  //  Move to next line
   
	  currentLeft -= leftIncrement;     //  Adjust left side for new scanline
	  currentRight -= rightIncrement;     //  Adjust right side for new scanline
	} 
 
      if (currentScanline == bottomScanline)
	break;           //  *** Loop exit condition
 
      if (nextVertexScanline == nextLeftVertexScanline) //  Check for a vertex on the left
	{
   
	  leftIndex++;         //  Advance one vertex on the left

	  leftStartCoords = leftEndCoords;    //  The end of the previous segment is the
	  //   start of this one

	  leftEndCoords = ((Vector *) leftSide.
			   Element(leftIndex + 1))->coordinates; //  Get the coordinates of end of this segment

	  currentLeft = leftStartCoords[0];    //  Get the x coordinate to start at
   
	  nextLeftVertexScanline = leftEndCoords[1];  //  Find the scanline of the next left vertex

	  leftIncrement = 
	    ((float) (leftEndCoords[0] -
		      leftStartCoords[0])) /
	    ((float) (leftEndCoords[1] -
		      leftStartCoords[1])); // Compute the new left x inc per scanline step
   
	}
   
      else            //  We have encountered a vertex on the right
	{
   
	  rightIndex++;         //  Advance one vertex on the right

	  rightStartCoords = rightEndCoords;    //  The end of the previous segment is the
	  //   start of this one

	  rightEndCoords = ((Vector *) rightSide.
			    Element(rightIndex + 1))->coordinates; //  Get the coordinates of end of this segment
   
	  currentRight = rightStartCoords[0];    //  Get the x coordinate to start at

	  nextRightVertexScanline = rightEndCoords[1]; //  Find the scanline of the next right vertex

	  rightIncrement = 
	    ((float) (rightEndCoords[0] -
		      rightStartCoords[0])) /
	    ((float) (rightEndCoords[1] -
		      rightStartCoords[1])); // Compute the new right x inc per scanline step
   
	}
 
    }

} //==== Solid::DrawPolygonFilled() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Solid::DrawLineSegment
//|
//| Purpose: This method draws this onto a Bitmap.  The dimension MUST be 1 (this
//|          must be a line segment).
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|_________________________________________________________________________________

void Solid::DrawLineSegment(Bitmap& bitmap)
{

  //
  //  Things get a bit strange here because things are one-dimensional.  Vectors are
  //  just numbers and Faces are equations of the form
  //
  //    ax + b > 0
  //
  //  The boundaries of these faces are at x > -b/a when a is positive, and x < -b/a
  //  when a is negative.  The following code finds the maximum value of -b/a for all
  //  faces where a is positive, and the minimum value of -b/a for all faces where a
  //  is negative.  These values are the endpoints of the Solid1D.
  //
  //  It might seem that since a line segment has only two endpoints, this could have
  //  only two faces, and we could just take them and use them to draw the line.
  //  Unfortunately, there is nothing in the definition of Solid which prevents there
  //  from being redundant faces.  We could have, for instance:
  //
  //     x > 1,  x > 5,   x <= 7
  //
  //  Which has three faces but only two endpoints, 5 and 7.  x > 1 is redundant.
  //  FindAdjacencies eliminates redundant faces in 2D+ objects, but doesn't work
  //  for 1D objects because it uses corners to determine redundancy, and no
  //  non-coincident faces can intersect in 1-space, so there are no corners.
  //  Below we use a different technique to eliminate redundancy.
  //

  double max = DBL_MAX;        //  Start with whole number line
  double min = -DBL_MAX;
  
  long i;
  for (i = 1; i <= faces.NumElements(); i++)   //  Loop through all faces
    {
  
      Face *this_face = (Face *) faces.Element(i); //  Get this face
  
      double boundary = (-this_face->coordinates[1]/
			 this_face->coordinates[0]); //  Compute the boundary for this face
  
      if (this_face->coordinates[0] > 0) 
	{
	  if (boundary > min)
	    min = boundary; 
	}
   
      else
	{
	  if (boundary < max)
	    max = boundary; 
	}
  
    }

  bitmap.DrawLine(min, 0, max, 0, color);   //  Draw the line segment
 
} //==== Solid::DrawLineSegment() ====//
#endif
