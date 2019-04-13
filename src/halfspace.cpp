//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace.cp
//|
//| This is the implementation of the Halfspace class.  An Halfspace is half of an n-space.
//| it is described by the equation of the bounding hyperplane.  A point is considered
//| to be inside the halfspace if the left side of the equation is greater than the
//| right side.  The first n coefficients can also be viewed as the normal vector.
//|_______________________________________________________________________________________


#include "face.h"
#include "halfspace.h"
#include "amatrix.h"
#include "solid.h"

#include <stdlib.h>
#include <iostream.h>
#include <string.h>


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::Halfspace
//|
//| Purpose: This method creates an Halfspace.
//|
//| Parameters: dim: dimension of the Halfspace
//|_________________________________________________________________________________

Halfspace::Halfspace(long dim) :

  //  Call Vector constructor, and pretend this is an
  //  (n+1)-dimensional point.  This forces Vector to
  //  allocate an extra coordinate slot, which we will
  //  use for the constant
  Vector(dim+1)

{

  dimension = dim;

}  //==== Halfspace::Halfspace() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::Halfspace
//|
//| Purpose: This method creates a Halfspace which is a copy of the passed Halfspace.
//|
//| Parameters: halfspace: the Halfspace of which to make a copy
//|_________________________________________________________________________________

Halfspace::Halfspace(Halfspace &halfspace) :

  //  Call Vector constructor, and pretend this is an
  //  (n+1)-dimensional point.  This forces Vector to
  //  allocate an extra coordinate slot, which we will
  //  use for the constant
  Vector(halfspace.dimension+1)

{

  // Copy the dimension
  dimension = halfspace.dimension;
 
  // Copy the coefficients
  memmove(coordinates, halfspace.coordinates, (dimension + 1) * sizeof(double));

} //==== Halfspace::Halfspace() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::~Halfspace
//|
//| Purpose: This method disposes of a Halfspace.
//|
//| Parameters: none
//|_________________________________________________________________________________

Halfspace::~Halfspace(void)
{

} //==== Halfspace::~Halfspace() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::Negate
//|
//| Purpose: This method negates all terms in the equation of this halfspace.  This
//|          flips the normal without changing the boundary halfplane.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Halfspace::Negate(void)
{

  // Negate all terms in the equation
  register long i;
  for (i = 0; i <= dimension; i++)
    coordinates[i] = -coordinates[i];

} //==== Halfspace::Negate() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::Translate
//|
//| Purpose: This method translates a halfspace through n-space.
//|
//| Parameters: offset: the vector indicating the direction and distance to 
//|                     translate this halfspace.
//|_________________________________________________________________________________

void Halfspace::Translate(Vector& offset)
{
  
  //
  //  Given a halfspace
  //  
  //    a1*x1 + ... + an*xn + k = 0
  //
  //  We can translate by vector (v1, v2, ..., vn) by substituting (xi - vi) for
  //  all xi, yielding
  //
  //    a1*(x1-v1) + ... + an*(xn-vn) + k = 0
  //
  //  This simplifies to
  //
  //    a1*x1 + ... + an*xn + (k - a1*v1 - ... - an*vn) = 0
  //
  //  So all we have to do is change the constant term.  This is as expected,
  //  since translating should not change the normal vector (the first n-1 terms).
  //
  
  // Get the constant
  double k = coordinates[dimension];
  
  // Subtract terms to compute the new constant
  long i;
  for (i = 0; i < dimension; i++)
    k -= coordinates[i]*offset.coordinates[i];
  
  // Set the constant
  coordinates[dimension] = k;
  
} //==== Halfspace::Translate() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::Transform
//|
//| Purpose: This method applies a matrix transformation to this Halfspace.
//|
//| Parameters: m: the matrix of the transformation to apply.
//|_________________________________________________________________________________

void Halfspace::Transform(const AMatrix& m)
{

  //
  //  The normal of the tranformed halfspace can be found with a simple matrix
  //  multiplication.  The constant is more difficult.  Here I have solved this
  //  by finding a point on the original halfspace (by checking axes for intersections)
  //  and transforming that point as well.  The transformed point lies on the
  //  transformed halfspace, so the constant term can be computed by plugging the
  //  transformed point into the equation of the transformed halfspace (the coefficients
  //  being the coordinates of the transformed normal and the constant unknown) and
  //  solving for the constant.
  //
  
  // Look for a non-zero coordinate
  register long i;
  for (i = 0; i < dimension; i++)
    if (coordinates[i] != 0.0)
      break;
   
  // Find the intercept for this axis
  register double intercept = -coordinates[dimension] / coordinates[i];
 
  // Transform the normal
  Vector transformedNormal(dimension);
  transformedNormal = (m * (*this)); 

  //
  //  At this point we have found a point on the halfspace.  This point is
  //  (0, 0, ..., intercept, ..., 0, 0), where intercept is the ith coordinate
  //  and all other coordinates are 0.  Since this is a highly sparse and
  //  predictable vector.  We will NOT actually plug all these coordinates
  //  into a Vector and use matrix multiplication; rather, we will take
  //  advantage of the fact that multiplication by such a vector yields
  //  a vector which is simply the ith column of m multiplied by intercept.
  //  We skip another step by plugging the coordinates of this product
  //  directly into the transformed equation.
  //
  
  register double sum = 0.0;
  register long j;
  for (j = 0; j < dimension; j++) {
    
    // Get a coordinate of the transformed normal
    register double coordinate = transformedNormal.coordinates[j];

    // Copy it to this' coordinates
    coordinates[j] = coordinate;

    // Find this coordinate of the transformed
    // point, and plug it into the transformed
    // equation.
    sum += m.elements[j][i] * intercept * coordinate;

  }
 
  // Set the constant equal to the negative of the rest of the equation (so it adds to zero).
  coordinates[dimension] = -sum;
              
} //==== Halfspace::Transform() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::DumpEquation
//|
//| Purpose: This method dumps the equation of this halfspace to cout.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Halfspace::DumpEquation(void)
{

  long i;
  for (i = 0; i < dimension; i++) {

    // Don't dump this term if it's 0
    if (coordinates[i] == 0.)
      continue;
  
    // Just output - for -1
    if (coordinates[i] == -1.)
      cout << '-';

    // Don't show coefficient if it's 1
    else if (coordinates[i] != 1.)
      cout << coordinates[i];
   
    // Dump this coefficient
    cout << 'x' << i+1 << " + ";

  }

  // Don't dump constant if it's 0
  if (coordinates[i] != 0.)
    cout << coordinates[i];
 
  // Dump end of equation
  cout << " > 0";

} //==== Halfspace:DumpEquation() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Halfspace::SliceSolid
//|
//| Purpose: This procedure slices a solid into two solids along the boundary of
//|          this Halfspace.
//|
//| Parameters: solid:         the solid to slice
//|             inside_solid:  receives the half of the solid inside halfspace
//|             outside_solid: receives the half of the solid outside halfspace
//|_________________________________________________________________________________

void Halfspace::SliceSolid(Solid& solid, Solid*& inside_solid, Solid*& outside_solid)
{

  // Make inside_solid a copy of solid
  inside_solid = new Solid(solid);

  // Make outside_solid a copy of solid
  outside_solid = new Solid(solid);
 
  // Make a new face, using this as the halfspace.
  Face *face = new Face(*this);
             
  // Add the face to inside_solid's description.
  inside_solid->AddFace(face);
 
  // Make another face, using this as the halfspace.
  face = new Face(*this);
             
  // Flip the normal for this face (so the
  // other half of n-space is inside).
  face->Negate();

  // Add the flipped hyperplane to outside_solid's description.
  outside_solid->AddFace(face);

} //==== Halfspace::SliceSolid() ====//

