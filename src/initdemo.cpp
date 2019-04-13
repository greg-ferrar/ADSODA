#include "solid.h"
#include "space.h"

//====  PROTOTYPES

void build_polygon(Solid *polygon);
Solid *New3Cube(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
Solid *New4Cube(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, double wmin, double wmax);


//==== GLOBALS

extern Space *demoSpace;


// Initialize the 2D demo space
Space *init2DDemo(void) {

  Solid *polygon = new Solid(2);
  build_polygon(polygon);
  polygon->SetColor(0, 1, 1);

  Solid *polygon2 = new Solid(*polygon);
  polygon2->SetColor(1, 0, 1);

  Solid *polygon3 = new Solid(*polygon);
  polygon3->SetColor(0, 1, 0);

  Space *space2D = new Space(2, Color(0.1, 0.1, 0.1));
  space2D->AddSolid(polygon);
  space2D->AddSolid(polygon2);	
  space2D->AddSolid(polygon3);	

  Vector *offset = new Vector(2);
  offset->coordinates[0] = 50;
  offset->coordinates[1] = 80;
  polygon->Translate(*offset);

  offset->coordinates[0] = -80;
  offset->coordinates[1] = -20;
  polygon2->Translate(*offset);
	
  offset->coordinates[0] = 80;
  offset->coordinates[1] = -130;
  polygon3->Translate(*offset);
  
  delete(offset);

  Light *light1;
  light1 = new Light(2, 1.0, 1.0, 1.0);
  light1->coordinates[0] = 0;
  light1->coordinates[1] = -100;
  space2D->AddLight(light1);

  return space2D;

}  //==== init2DDemo() ====//



// Initialize the 3D demo space
Space *init3DDemo(void) {

  Space *space3D = new Space(3, Color(0.2, 0.2, 0.2));

  // Add a cube to the space
  Solid *cube = New3Cube(10, 130, 10, 130, 10, 130);
  cube->SetColor(0, 1, 1);
  space3D->AddSolid(cube);

  // Add a cube to the space
  cube = New3Cube(-130, -10, -130, -10, -130, -10);
  cube->SetColor(1, 1, 0);
  space3D->AddSolid(cube);

  Light *light1;
  light1 = new Light(3, 1.0, 1.0, 1.0);
  light1->coordinates[0] = -100;
  light1->coordinates[1] = -100;
  light1->coordinates[2] = -100;
  space3D->AddLight(light1);

  return space3D;

}  //==== init3DDemo() ====//



// Initialize the 4D demo space
Space *init4DDemo(void) {

  Space *space4D = new Space(4, Color(0.3, 0.3, 0.3));

  // Add a hypercube to the space
  Solid *hypercube = New4Cube(10, 150, 10, 150, 10, 150, 10, 150);
  hypercube->SetColor(0.7, 0.7, 0.7);
  space4D->AddSolid(hypercube);

  Solid *hypercube2 = New4Cube(-150, -10, -150, -10, -150, -10, -150, -10);
  hypercube2->SetColor(1, 0.0, 0.0);
  space4D->AddSolid(hypercube2);

  Light *light = new Light(4, 0.3, 1.0, 0.3);
  light->coordinates[0] = -100;
  light->coordinates[1] = -100;
  light->coordinates[2] = -100;
  light->coordinates[3] = -100;
  space4D->AddLight(light);

#if 0
  light2 = new Light(3, 1.0, 1.0, 1.0);
  light2->coordinates[0] = -100;
  light2->coordinates[1] = -100;
  light2->coordinates[2] = -100;
  space3D->AddLight(light2);
#endif
  
  return space4D;

}  //==== init4DDemo() ====//
