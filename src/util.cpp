#include "solid.h"
#include "face.h"

// Create a 3-dimensional cube
Solid *New3Cube(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax) {

  Solid *cube = new Solid(3);

  //   x < xmax
  //  -x + 0y + 0z + xmax > 0
  Face *face = new Face(3);
  face->coordinates[0] = -1;
  face->coordinates[1] = 0;
  face->coordinates[2] = 0;
  face->coordinates[3] = xmax;
  cube->AddFace(face);
  
  //   x > xmin
  //   x + 0y + 0z - xmin > 0
  face = new Face(3);
  face->coordinates[0] = 1;
  face->coordinates[1] = 0;
  face->coordinates[2] = 0;
  face->coordinates[3] = -xmin;
  cube->AddFace(face);
  
  //   y > ymin
  //   0x + 1y + 0z - ymin > 0
  face = new Face(3);
  face->coordinates[0] = 0;
  face->coordinates[1] = 1;
  face->coordinates[2] = 0;
  face->coordinates[3] = -ymin;
  cube->AddFace(face);
  
  //   y < ymax
  //   0x + -y + 0z + ymax > 0
  face = new Face(3);
  face->coordinates[0] = 0;
  face->coordinates[1] = -1;
  face->coordinates[2] = 0;
  face->coordinates[3] = ymax;
  cube->AddFace(face);

  //   z > zmin
  //   0x + 0y + 1z - zmin > 0
  face = new Face(3);
  face->coordinates[0] = 0;
  face->coordinates[1] = 0;
  face->coordinates[2] = 1;
  face->coordinates[3] = -zmin;
  cube->AddFace(face);
  
  //   z < zmax
  //   0x + 0y + -z + zmax > 0
  face = new Face(3);
  face->coordinates[0] = 0;
  face->coordinates[1] = 0;
  face->coordinates[2] = -1;
  face->coordinates[3] = zmax;
  cube->AddFace(face);
  
  return cube;

}  //==== New3Cube() ====//



//  Create a 4-dimensional cube
Solid *New4Cube(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, double wmin, double wmax) {

  Solid *hypercube = new Solid(4);

  //   x < xmax
  //  -x + 0y + 0z + 0w + xmax > 0
  Face *face = new Face(4);
  face->coordinates[0] = -1;
  face->coordinates[1] = 0;
  face->coordinates[2] = 0;
  face->coordinates[3] = 0;
  face->coordinates[4] = xmax;
  hypercube->AddFace(face);
  
  //   x > xmin
  //   x + 0y + 0z + 0w - xmin > 0
  face = new Face(4);
  face->coordinates[0] = 1;
  face->coordinates[1] = 0;
  face->coordinates[2] = 0;
  face->coordinates[3] = 0;
  face->coordinates[4] = -xmin;
  hypercube->AddFace(face);
  
  //   y > ymin
  //   0x + 1y + 0z + 0w - ymin > 0
  face = new Face(4);
  face->coordinates[0] = 0;
  face->coordinates[1] = 1;
  face->coordinates[2] = 0;
  face->coordinates[3] = 0;
  face->coordinates[4] = -ymin;
  hypercube->AddFace(face);
  
  //   y < ymax
  //   0x + -y + 0z + 0w + ymax > 0
  face = new Face(4);
  face->coordinates[0] = 0;
  face->coordinates[1] = -1;
  face->coordinates[2] = 0;
  face->coordinates[3] = 0;
  face->coordinates[4] = ymax;
  hypercube->AddFace(face);

  //   z > zmin
  //   0x + 0y + 1z + 0w - zmin > 0
  face = new Face(4);
  face->coordinates[0] = 0;
  face->coordinates[1] = 0;
  face->coordinates[2] = 1;
  face->coordinates[3] = 0;
  face->coordinates[4] = -zmin;
  hypercube->AddFace(face);
  
  //   z < zmax
  //   0x + 0y + -z + zmax > 0
  face = new Face(4);
  face->coordinates[0] = 0;
  face->coordinates[1] = 0;
  face->coordinates[2] = -1;
  face->coordinates[3] = 0;
  face->coordinates[4] = zmax;
  hypercube->AddFace(face);
  
  //   w > wmin
  //   0x + 0y + 0z + 1w - wmin > 0
  face = new Face(4);
  face->coordinates[0] = 0;
  face->coordinates[1] = 0;
  face->coordinates[2] = 0;
  face->coordinates[3] = 1;
  face->coordinates[4] = -wmin;
  hypercube->AddFace(face);
  
  //   w < wmax
  //   0x + 0y + 0z -w + zmax > 0
  face = new Face(4);
  face->coordinates[0] = 0;
  face->coordinates[1] = 0;
  face->coordinates[2] = 0;
  face->coordinates[3] = -1;
  face->coordinates[4] = wmax;
  hypercube->AddFace(face);
  
  return hypercube;

}  //==== New4Cube() ====//


//
//  Builds a polygon
//

void build_polygon(Solid *polygon)
{
  
  Face *face = new(Face)(2);
  face->coordinates[0] = 1.;					//  Add face x + y + 100 > 0
  face->coordinates[1] = 1.;
  face->coordinates[2] = 100.;
  polygon->AddFace(face);
  
  face = new(Face)(2);
  face->coordinates[0] = -1.;					//  Add face -x - y + 100 > 0
  face->coordinates[1] = -1.;
  face->coordinates[2] = 100.;
  polygon->AddFace(face);
  
  face = new(Face)(2);
  face->coordinates[0] = 1.;					//  Add face x - y + 100 > 0
  face->coordinates[1] = -1.;
  face->coordinates[2] = 100.;
  polygon->AddFace(face);
  
  face = new(Face)(2);
  face->coordinates[0] = -1.;					//  Add face -x + y + 100 > 0
  face->coordinates[1] = 1.;
  face->coordinates[2] = 100.;
  polygon->AddFace(face);
  
  face = new(Face)(2);
  face->coordinates[0] = 1.;					//  Add face x + 50 > 0
  face->coordinates[1] = 0.;
  face->coordinates[2] = 50.;
  polygon->AddFace(face);
  
  face = new(Face)(2);
  face->coordinates[0] = -2.;					//  Add face -2x - y + 140 > 0
  face->coordinates[1] = -1.;
  face->coordinates[2] = 140.;
  polygon->AddFace(face);
	
}  //==== build_polygon() ====//
