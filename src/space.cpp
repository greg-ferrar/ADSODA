//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space.cp
//|
//| This is the implementation of the Space class.  A Space is euclidian
//| n-space.
//|_____________________________________________________________________________

#include "halfspace.h"
#include "face.h"
#include "solid.h"
#include "space.h"
#include "light.h"
#include "debug.h"

#include "solve.h"

#include <stdlib.h>
#include <string.h>


//==== PROTOTYPES

void clearBackground(void);



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Space
//|
//| Purpose: This method creates a Space.
//|
//| Parameters: dim: dimension of the Space
//|             pambient: ambient light for this space
//|_________________________________________________________________________________

Space::Space(long dim, const Color &pambient)
{

  dimension = dim;
  
  ambient = pambient;

} //==== Space::Space() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Space
//|
//| Purpose: This method creates a Space which is a copy of another Space
//|
//| Parameters: space: Space to make a copy of
//|_________________________________________________________________________________

Space::Space(Space& space)
{

  // Set the dimension of this Space
  dimension = space.dimension;

  // Duplicate all Solids, and add them to the copy
  for (vector<Solid *>::iterator solid = space.solids.begin(); solid != space.solids.end(); solid++)
    solids.push_back(new Solid(**solid));

  // Duplicate all Lights, and add them to the copy
  for (vector<Light>::iterator light = space.lights.begin(); light != space.lights.end(); light++)
    lights.push_back(*light);

  ambient = space.ambient;
  
} //==== Space::Space() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::~Space
//|
//| Purpose: This method disposes of a Space.
//|
//| Parameters: none
//|_________________________________________________________________________________

Space::~Space(void)
{
  //  Get rid of lower-dimensional space
  // delete(lowerSpace);

  // delete all Lights in the lights list
  //  for (i = 1; i <= lights.NumElements(); i++)
  //    delete((Light *) lights.Element(i));

  // delete all Solids in the solids list
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++) {
    //    delete(*solid);
    delete(*solid);
  }

  //  register long i;
  //  for (i = 1; i <= solids.NumElements(); i++) {
    //    Debug("Deleting solid as member of deleted space: " << solids.Element(i) << endl);
  //    delete((Solid *) solids.Element(i));
  //  }

} //==== Space::~Space() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::AddSolid
//|
//| Purpose: This method adds a solid to this Space.
//|
//| Parameters: solid: the Solid to add
//|_________________________________________________________________________________

void Space::AddSolid(Solid *solid)
{

  // Add this Solid to the solids list
  solids.push_back(solid);

} //==== Space::AddSolid() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::AddLight
//|
//| Purpose: This method adds a solid to this Space.
//|
//| Parameters: solid: the Solid to add
//|_________________________________________________________________________________

void Space::AddLight(Light *light)
{

  // Normalize the light vector
  light->Normalize();

  // Add this Light to the lights list
  lights.push_back(*light);

} //==== Space::AddLight() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Clear
//|
//| Purpose: This method removes all Solids from this space.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Space::Clear(void)
{

  // Remove all Solids from the list
  //  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
  //    delete(*solid);
  solids.erase(solids.begin(), solids.end());

} //==== Space::Clear() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::ClearAnddelete
//|
//| Purpose: This method removes all Solids from this space, and deletes them
//|
//| Parameters: none
//|_________________________________________________________________________________

void Space::ClearAndDelete(void) {

  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++) {
    delete(*solid);
  }
  solids.erase(solids.begin(), solids.end());

} //==== Space::ClearAnddelete() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::EliminateEmptySolids
//|
//| Purpose: This method eliminates empty Solids.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Space::EliminateEmptySolids(void) {

  // Loop through all Solids in this Space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++) {
  
    // Call EnsureAdjacencies for this Solid
    (*solid)->EnsureAdjacencies();
 
    // Check if this is an empty Solid (no corners); if so, remove it from the list
    // and delete it from memory
    if ((*solid)->Corners().size() <= dimension) {

      //      cout << "REMOVING EMPTY SOLID" << endl;
      delete(*solid);
      solids.erase(solid);
   
      // Start over
      //      solid = solids.begin();
      solid--;

    }

  }  // loop through all solids
       
} //==== Space::EliminateEmptySolids() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Subtract
//|
//| Purpose: This method removes anything intersecting solid from Space.  If a
//|          Solid in this Space partially intersects solid, the intersecting part
//|          will be removed and the non-intersecting part will remain (perhaps
//|          broken into several Solids).
//|
//| Parameters: solid: the solid to subtract from this Space
//|_________________________________________________________________________________

void Space::Subtract(Solid& solid)
{

  // Create a list of Solids to receive difference of solid from Solids in this Space
  vector<Solid *> difference;

  // Create a list of Solids to receive difference of solid from a single Solid
  vector<Solid *> solid_difference;
 
  //  Loop through all Solids in this Space
  vector<Solid *>::iterator this_solid;
  for (this_solid = solids.begin(); this_solid != solids.end(); this_solid++) {
    
    // Subtract solid from this_solid, and put resulting Solid(s) in difference
    (*this_solid)->Subtract(solid, solid_difference);
  
    // Move all solids in the difference of solid from this_solid to the difference list
    for (vector<Solid *>::iterator s = solid_difference.begin(); s != solid_difference.end(); s++)
      difference.push_back(*s);

    solid_difference.erase(solid_difference.begin(), solid_difference.end());

  }  // loop through solids

  // Remove all Solids in this Space
  for (this_solid = solids.begin(); this_solid != solids.end(); this_solid++)
    delete(*this_solid);
  Clear();
  //  solids.erase(solids.begin(), solids.end());
 
  // Copy all solids in difference to this Space
  for (vector<Solid *>::iterator diffi = difference.begin(); diffi != difference.end(); diffi++)
    solids.push_back(*diffi);

} //==== Space::Subtract() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Add
//|
//| Purpose: This method adds all the solids in space to this Space.  It does not
//|          check for overlap of Solids.
//|
//| Parameters: space: the space containing the solids to add
//|_________________________________________________________________________________

void Space::Add(Space& space)
{

  // Add all solids in space to this space
  for (vector<Solid *>::iterator solid = space.solids.begin(); solid != space.solids.end(); solid++)
    AddSolid(*solid);

} //==== Space::Add() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Project
//|
//| Purpose: This method projects all the Solids onto projection.
//|
//| Parameters: projection: receives the projection (all Solids are removed,
//|                         and replaced by the projection solids).
//|_________________________________________________________________________________

void Space::Project(Space *projection)
{

  // Eliminate any empty solids in the the source space
  EliminateEmptySolids();

  // Remove all objects from the destination space
  projection->ClearAndDelete();

  // Loop through all Solids in this Space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++) {

    // Create a new list for projected faces
    vector<Solid *> projected_faces;
  
    // Project this solid
    (*solid)->Project(projected_faces, lights, ambient);
  
    // Add all faces of projection as a Solid
    for (vector<Solid *>::iterator pface = projected_faces.begin(); pface != projected_faces.end(); pface++) {
      projection->AddSolid(*pface);
    }

  }  // project all solids

} //==== Space::Project() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::RemoveHiddenSolids
//|
//| Purpose: This method removes all Solids from this Space which are not visible.
//|          If a Solid is partially visible, the visible part remains and the
//|          hidden part is removed.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Space::RemoveHiddenSolids(void)
{

  // Make a copy of this space
  Space *sourceSpace = new Space(*this);

  // Find adjacencies for each Solid in sourceSpace
  //  sourceSpace->EnsureAdjacencies();
 
  // Find the silhouettes for each Solid in sourceSpace
  //  sourceSpace->FindSilhouettes();

  // delete all Solids in this Space
  for (vector<Solid *>::iterator deleteSolid = solids.begin(); deleteSolid != solids.end(); deleteSolid++)
    delete(*deleteSolid);
  Clear();

  // Loop through all Solids in the original Space
  for (vector<Solid *>::iterator sourceSpaceSolid = sourceSpace->solids.begin();
       sourceSpaceSolid != sourceSpace->solids.end();
       sourceSpaceSolid++) {
  
    // Create a space to hold what remains of this Solid after all hidden parts are clipped away.
    Space *solidResult = new Space(dimension, ambient);
  
    // Get a copy of this solid
    Solid *clippedSolid = new Solid(**sourceSpaceSolid);
  
    // Find adjacencies and silhouettes for this Solid
    clippedSolid->EnsureAdjacencies();
    clippedSolid->EnsureAdjacencies();
    clippedSolid->EnsureSilhouette();
  
    // Add this solid to the clipped solid space
    Solid *clippedSolidCopy = new Solid(*clippedSolid);
    solidResult->AddSolid(clippedSolidCopy);

    // Loop though all Solids in this Space
    for (vector<Solid *>::iterator clipSolid = sourceSpace->solids.begin();
	 clipSolid != sourceSpace->solids.end();
	 clipSolid++) {
   
      // Don't clip a Solid with itself
      if (clipSolid == sourceSpaceSolid)
	continue;
   
      // Check whether the solid we're clipping is behind the solid we're clipping it to;
      // if it's not behind, go to the next clip Solid
      clippedSolid->EnsureSilhouette();
      //      clippedSolid->GetSilhouette()->FindAdjacencies();
      //      clippedSolid->GetSilhouette()->DrawUsingOpenGL2D();
      if (clippedSolid->OrderSolids(**clipSolid) != BEHIND)
	continue;

      // The Solid we're clipping is begin the Solid we're clipping with; clip the Solid in back
      // against the one in front.
      solidResult->Subtract(*((*clipSolid)->GetSilhouette()));

    }  // clip all solids
    
    // We have finished clipping this solid; add what remains of it to this Space.
    for (vector<Solid *>::iterator solidResultSolid = solidResult->solids.begin();
	 solidResultSolid != solidResult->solids.end();
	 solidResultSolid++) {
      solids.push_back(new Solid(**solidResultSolid));
    }

    // delete the solidResult space (but not the solids which were in it, and are now in this Space).
    delete(solidResult);
    delete(clippedSolid);
  }
  
  // delete the original copy of this Space
  delete(sourceSpace);

  //  EnsureAdjacencies();

} //==== Space::RemoveHiddenSolids() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::Transform
//|
//| Purpose: This method applies a matrix transformation to all the Solids in this
//|          Space.
//|
//| Parameters: m: the matrix of the transformation to apply.
//|_________________________________________________________________________________

void Space::Transform(const AMatrix& m)
{

  // Transform all solids in this space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
    (*solid)->Transform(m);

} //==== Space::Transform() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawIntoVoxelArray
//|
//| Purpose: This method draws all the solids in this Space by scan converting them
//|          into a voxel array.
//|
//| Parameters: voxel_array: the voxel array; receives the scan converted Space
//|             minimum:     an array of minima, one for each coordinate, determining
//|                          the hyper-region to scan convert.
//|             maximum:     an array of maxima, one for each coordinate, determining
//|                          the hyper-region to scan convert.
//|___________________________________________________________________________________

void Space::DrawIntoVoxelArray(Voxel *voxel_array, long *minimum, long *maximum)
{

  // Draw all solids in this space into the voxel array
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
    (*solid)->ScanConvert(voxel_array, minimum, maximum);

} //==== Space::DrawIntoVoxelArray() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawUsingOpenGL3D
//|
//| Purpose: This method draws all the Solids in this Space using OpenGL.
//|          It assumes the space is a 3D space.
//|
//| Parameters: outline: true iff polygons should be outlined by outline.
//|             fill: true iff polygons should be filled.
//|___________________________________________________________________________________

void Space::DrawUsingOpenGL3D(bool outline, bool fill)
{

  // Draw all Solids in this space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
    (*solid)->DrawUsingOpenGL3D(lights, ambient, outline, fill);
    
} //==== Space::DrawUsingOpenGL3D() ====//


void Space::EnsureAdjacencies(void) {

  // Draw all Solids in this space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
    (*solid)->EnsureAdjacencies();
    
}  //==== Space::EnsureAdjacencies() ====//


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawUsingOpenGL2D
//|
//| Purpose: This method draws all the Solids in this Space using OpenGL.
//|          It assumes the space is a 2D space.
//|
//| Parameters: none
//|___________________________________________________________________________________

void Space::DrawUsingOpenGL2D(bool outline, bool fill)
{

  // Draw all Solids in this space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
    (*solid)->DrawUsingOpenGL2D(outline, fill);

} //==== Space::DrawUsingOpenGL2D() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawUsingOpenGL1D
//|
//| Purpose: This method draws all the Solids in this Space using OpenGL.
//|          It assumes the space is a 1D space.
//|
//| Parameters: none
//|___________________________________________________________________________________

void Space::DrawUsingOpenGL1D(bool outline, bool fill)
{

  // Draw all Solids in this space
  for (vector<Solid *>::iterator solid = solids.begin(); solid != solids.end(); solid++)
    (*solid)->DrawUsingOpenGL1D(outline, fill);

} //==== Space::DrawUsingOpenGL1D() ====//


#if 0
//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawOntoBitmapFilled
//|
//| Purpose: This method draws all the Solids in this Space into a Bitmap as filled
//|          polygons.  Since a  Bitmap is a two-dimensional object, this Space must
//|          be 2D as well.
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|___________________________________________________________________________________

void Space::DrawOntoBitmapFilled(Bitmap& bitmap)
{

 register long i;
 for (i = 1; i <= solids.NumElements(); i++)   //  Loop through all Solids in this Space
  ((Solid *) solids.Element(i))->
       DrawPolygonFilled(bitmap); //  Draw this polygon

} //==== Space::DrawOntoBitmapFilled() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawOntoBitmapWireframe
//|
//| Purpose: This method draws all the Solids in this Space into a Bitmap as wireframe
//|          polygons.  Since a Bitmap is a two-dimensional object, this Space must
//|          be 2D as well.
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|___________________________________________________________________________________

void Space::DrawOntoBitmapWireframe(Bitmap& bitmap)
{

 register long i;
 for (i = 1; i <= solids.NumElements(); i++)   //  Loop through all Solids in this Space
  ((Solid *) solids.Element(i))->
      DrawPolygonWireframe(bitmap); //  Draw this polygon wireframe

} //==== Space::DrawOntoBitmapWireframe() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::DrawOntoBitmapX
//|
//| Purpose: This method draws all the Solids in this Space onto the x axis of a
//|          Bitmap.  Since the x axis of a Bitmap is a one-dimensional object,
//|          this Space must be 1D as well.
//|
//| Parameters: bitmap: the Bitmap to draw onto
//|___________________________________________________________________________________

void Space::DrawOntoBitmapX(Bitmap& bitmap)
{

 register long i;
 for (i = 1; i <= solids.NumElements(); i++)   //  Loop through all Solids in this Space
  ((Solid *) solids.Element(i))->
       DrawLineSegment(bitmap); //  Draw this polygon

} //==== Space::DrawOntoBitmapX() ====//
#endif


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::IntersectHyperplanes
//|
//| Purpose: This method finds the intersection between dimension hyperplanes.
//|          The hyperplanes are manipulated using their equations.  intersection
//|          is set to the solution of the dimension equations.  This method treats
//|          intersection as a point.
//|
//| Parameters: hyperplanes:  the hyperplanes to intersect
//|             intersection: receives the intersection
//|             returns false if there is no solution, or multiple solutions
//|_________________________________________________________________________________

bool Space::IntersectHyperplanes(vector<Hyperplane *>& hyperplanes, Vector& intersection) {

  //  Allocate space for system of equations
  Equation *system = (Equation *) malloc(dimension*sizeof(Equation));
  ASSERT(system != NULL);

  int i = 1;
  for (vector<Hyperplane *>::iterator hyperplane = hyperplanes.begin();
       hyperplane != hyperplanes.end();
       hyperplane++) {

    // Allocate space for one equation
    system[i] = (Equation) malloc((dimension+1)*sizeof(double));
    ASSERT(system[i] != NULL);

    memmove(system[i], (*hyperplane)->coordinates, (dimension+1)*sizeof(double));

    i++;

  }

  // Solve the equations
  return (!SolveEquations(dimension, system, intersection.coordinates));

} //==== Space::IntersectHyperplanes() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::PointInsideHalfspace
//|
//| Purpose: This method returns true point is inside the halfspace.  Points which
//|          lie on or very close to the bounding hyperplane are considered to be
//|          outside the halfspace.  This method treats point as a point (not a
//|          vector).
//|
//| Parameters: halfspace: the halfspace to check point against
//|             point: the point to check
//|             returns true if point is inside halfspace
//|_________________________________________________________________________________

bool Space::PointInsideHalfspace(Halfspace& halfspace, Vector& point)
{
  
  //
  //  The point is on the inside side of the halfspace if
  //
  //  a x  + a x  + ... + a x + k  <  0
  //   1 1    2 2          n n   
  //
  //  where all ai are the same as in the equation of the hyperplane.
  //  The following code evaluates the left side of this inequality.
  //
  
  double result = 0;
  long i;
  for(i = 0; i < dimension; i++)
    result += point.coordinates[i] * halfspace.coordinates[i];
  
  // Add the constant
  result += halfspace.coordinates[i];
 
  return (result > VERY_SMALL_NUM);
  
} //==== Space::PointInsideHalfspace() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::PointInsideHalfspaces
//|
//| Purpose: This method returns true if point is inside all the specified halfspaces.
//|          Points which lie on or very close to the bounding hyperplane are
//|          considered to be outside the halfspace.  This method treats point as
//|          a point (not a vector).
//|
//| Parameters: halfspaces: the halfspaces to check this against
//|             point: the point to check
//|             returns true if point is inside all halfspaces
//|_________________________________________________________________________________

bool Space::PointInsideHalfspaces(vector<Halfspace *>& halfspaces, Vector& point)
{

  for (vector<Halfspace *>::iterator halfspace = halfspaces.begin(); halfspace != halfspaces.end(); halfspace++)

    // this is not inside this halfspace, so it's not inside them all.
    if (!PointInsideHalfspace(**halfspace, point))
      return false;

  // Inside all halfspaces
  return true;
 
} //==== Space::PointInsideHalfspaces() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::PointInsideOrOnHalfspace
//|
//| Purpose: This method returns true if point  is inside the Halfspace or on the
//|          boundary.  This method treats point as a point (not a vector).
//|
//| Parameters: halfspace: the halfspace to check this against
//|             point: the point to check
//|             returns true if point is inside or on halfspace
//|_________________________________________________________________________________

bool Space::PointInsideOrOnHalfspace(Halfspace& halfspace, Vector& point)
{

  //
  //  The point is on the inside side or the boundary of the halfspace if
  //
  //  a x  + a x  + ... + a x + k  <=  0
  //   1 1    2 2          n n   
  //
  //  where all ai are the same as in the equation of the hyperplane.
  //  The following code evaluates the left side of this inequality.
  //
  
  // Add this term
  double result = 0;
  long i;
  for(i = 0; i < dimension; i++)
    result += point.coordinates[i] * halfspace.coordinates[i];
  
  // Add the constant
  result += halfspace.coordinates[i];
  
  return (result > -VERY_SMALL_NUM);

} //==== Space::PointInsideOrOnHalfspace() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::PointInsideOrOnHalfspaces
//|
//| Purpose: This method returns true if for each of the specified halfspaces,
//|          point is either inside or on the boundary.  This method treats point
//|          as a point (not a vector).
//|
//| Parameters: halfspaces: the halfspaces to check this against
//|             point: the point to check
//|             returns true if point is inside or on all halfspaces
//|_________________________________________________________________________________

bool Space::PointInsideOrOnHalfspaces(vector<Halfspace *>& halfspaces, Vector& point)
{

  for (vector<Halfspace *>::iterator halfspace = halfspaces.begin(); halfspace != halfspaces.end(); halfspace++)

    // this is not inside or on this halfspace, so it's not inside or on them all.
    if (!PointInsideOrOnHalfspace(**halfspace, point))
      return false;

  // Inside or on all halfspaces
  return true;
 
} //==== Space::PointInsideOrOnHalfspaces() ====//


