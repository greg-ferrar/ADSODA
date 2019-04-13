#include "space.h"
#include "solid.h"
#include "light.h"
#include "amatrix.h"
#include "face.h"
#include "debug.h"
#include "state.h"


//====  PROTOTYPES

Space *init2DDemo(void);
Space *init3DDemo(void);
Space *init4DDemo(void);

Space *Process1D(State &state, Space *space1D);
Space *Process2D(State &state, Space *space2D);
Space *Process3D(State &state, Space *space3D);
Space *Process4D(State &state, Space *space4D);

void drawcube(void);





void initState(State &state) {

  // Initialize the global state
  state.outlinePolys = false;
  state.fillPolys = true;
  state.dimension = 4;
  state.draw1D = false;
  state.draw2D = false;
  state.draw3D = false;
  state.removeHidden2D = false;
  state.removeHidden3D = false;
  state.removeHidden4D = false;
  state.rotate2D = false;
  state.rotate3D = false;
  state.rotate4D = false;
  state.demoInitialized = false;
  state.drawcubeFlag = false;
  
  state.theta = 0;
  state.rho = 0;
  state.phi = 0;
  
}  //==== initState() ====//


// Initialize the demo
void initDemo(State &state) {

  if (state.dimension == 2) 
    state.demoSpace = init2DDemo();
  
  else if (state.dimension == 3)
    state.demoSpace = init3DDemo();
  
  else if (state.dimension == 4)
    state.demoSpace = init4DDemo();
  
  else {
    std::cerr << "Error: there are only demos for state.dimensions 2, 3, and 4" << std::endl;
    exit(-1);
  }

  state.demoInitialized = true;

}  //==== initDemo() ====//



// Prepare one frame of the demo
void prepareDemoFrame(State &state) {

  // Clear out last frame's spaces
  if (state.draw3DSpace) {
    delete(state.draw3DSpace);
    state.draw3DSpace = NULL;
  }

  if (state.draw2DSpace) {
    delete(state.draw2DSpace);
    state.draw2DSpace = NULL;
  }
  
  if (state.draw1DSpace) {
    delete(state.draw1DSpace);
    state.draw1DSpace = NULL;
  }

  // Initialize the demo, if we haven't yet
  if (!state.demoInitialized)
    initDemo(state);

  Space *workingSpace = state.demoSpace;

  if (workingSpace && (state.dimension >= 4) && (workingSpace->Dimension() == 4))
    workingSpace = Process4D(state, state.demoSpace);

  Space *workingSpace3D = workingSpace;

  if (workingSpace && (state.dimension >= 3) && (workingSpace->Dimension() == 3))
    workingSpace = Process3D(state, workingSpace);

  Space *workingSpace2D = workingSpace;

  if (workingSpace && (state.dimension >= 2) && (workingSpace->Dimension() == 2))
    workingSpace = Process2D(state, workingSpace);

  Space *workingSpace1D = workingSpace;

  if (workingSpace && (state.dimension >= 1) && (workingSpace->Dimension() == 1))
    workingSpace = Process1D(state, workingSpace);

  if (workingSpace3D && (workingSpace3D != state.demoSpace))
    delete(workingSpace3D);

  if (workingSpace2D && (workingSpace2D != state.demoSpace))
    delete(workingSpace2D);

  if (workingSpace1D && (workingSpace1D != state.demoSpace))
    delete(workingSpace1D);


  // Make sure all the adjacencies of all the objects are ready,
  // so we can draw without modifying the space object
  if (state.draw3DSpace)
    state.draw3DSpace->EnsureAdjacencies();

  if (state.draw2DSpace)
    state.draw2DSpace->EnsureAdjacencies();

  if (state.draw1DSpace)
    state.draw1DSpace->EnsureAdjacencies();

}  //==== prepareDemoFrame() ====//



// Display one frame of the demo
void drawDemoFrame(State &state) {

  if (state.drawcubeFlag)
    drawcube();

  if (state.draw3DSpace)
    state.draw3DSpace->DrawUsingOpenGL3D(state.outlinePolys, state.fillPolys);

  if (state.draw2DSpace)
    state.draw2DSpace->DrawUsingOpenGL2D(state.outlinePolys, state.fillPolys);
  
  if (state.draw1DSpace)
    state.draw1DSpace->DrawUsingOpenGL1D(state.outlinePolys, state.fillPolys);

}  //==== drawDemoFrame() ====//



Space *Process1D(State &state, Space *space1D) {

  // If we're drawing, remember the space to draw
  if (state.draw1D)
    state.draw1DSpace = new Space(*space1D);
  else
    state.draw1DSpace = NULL;

  return NULL;

}  //==== Process1D() ====//



Space *Process2D(State &state, Space *space2D) {

  // Copy the space if we're going to change it
  Space *space2Dcopy = new Space(*space2D);

  // Rotate the space if requested
  if (state.rotate2D) {

    // Scale and rotate the space
    Vector scaleVector(2);
    scaleVector.coordinates[0] = 1;
    scaleVector.coordinates[1] = 1;
    AMatrix scaleMatrix2D(2, 2);
    scaleMatrix2D.CreateScaleMatrix(scaleVector);
    AMatrix rotationMatrix2D(2, 2);
    rotationMatrix2D.CreateRotationMatrix(1, 2, state.theta);
    AMatrix transformMatrix2D(2, 2);
    transformMatrix2D = scaleMatrix2D * rotationMatrix2D;
    space2Dcopy->Transform(transformMatrix2D);

    state.theta += 0.01;

  }  // if rotate

  if (state.removeHidden2D) 
    space2Dcopy->RemoveHiddenSolids();

  // Project to 1D, if we're going to draw it there
  Space *space1D = NULL;
  if (state.draw1D) {
    space1D = new Space(1, Color(0.1, 0.1, 0.1));
    space2Dcopy->Project(space1D);
  }

  // If we're drawing, remember the space to draw; else, delete the copy space
  if (state.draw2D)
    state.draw2DSpace = space2Dcopy;
  else {
    state.draw2DSpace = NULL;
    delete(space2Dcopy);
  }

  return space1D;

}  //==== Process2D() ====//



Space *Process3D(State &state, Space *space3D) {

  // Copy the space if we're going to change it
  Space *space3Dcopy = new Space(*space3D);

  // Rotate the space if requested
  if (state.rotate3D) {

    // Rotate the space
    AMatrix rotationMatrix3D(3, 3);
    rotationMatrix3D.CreateRotationMatrix(2, 3, state.theta);
    state.theta += -0.01;
    AMatrix rotationMatrix3D2(3, 3);
    rotationMatrix3D2.CreateRotationMatrix(1, 3, 2*state.theta);
    state.theta += -0.01;
    space3Dcopy->Transform(rotationMatrix3D*rotationMatrix3D2);

  }  // if rotate

  // Remove hidden solids, if requested
  if (state.removeHidden3D) 
    space3Dcopy->RemoveHiddenSolids();

  // Project to 3D, if we're going to draw it there
  Space *space2D = NULL;
  if (state.draw2D) {
    space2D = new Space(2, Color(0.1, 0.1, 0.1));
    space3Dcopy->Project(space2D);
  }

  // If we're drawing, remember the space to draw; else delete the copy space
  if (state.draw3D)
    state.draw3DSpace = space3Dcopy;
  else {
    state.draw3DSpace = NULL;
    delete(space3Dcopy);
  }

  return space2D;

}  //==== Process3D() ====//



Space *Process4D(State &state, Space *space4D) {

  Space *space4Dcopy;

  // Copy the space if we're going to change it
  if (state.rotate4D || state.removeHidden4D)
    space4Dcopy = new Space(*space4D);
  else
    space4Dcopy = space4D;

  // Rotate the space if requested
  if (state.rotate4D) {

    // Rotate the copy
    AMatrix rotationMatrix4D1(4, 4);
    AMatrix rotationMatrix4D2(4, 4);
    AMatrix rotationMatrix4D3(4, 4);
    AMatrix rotationMatrix4D(4, 4);
    rotationMatrix4D1.CreateRotationMatrix(1, 4, 3*state.rho);
    rotationMatrix4D2.CreateRotationMatrix(1, 3, 2*state.theta);
    rotationMatrix4D3.CreateRotationMatrix(1, 2, state.phi);
    rotationMatrix4D = rotationMatrix4D1 * rotationMatrix4D2 * rotationMatrix4D3;
    space4Dcopy->Transform(rotationMatrix4D);

    state.theta += 0.01;
    state.rho += 0.02;
    state.phi -= 0.015;

  }  // if rotate

  if (state.removeHidden4D) 
    space4Dcopy->RemoveHiddenSolids();

  // Project to 3D, if we're going to draw it there
  Space *space3D = NULL;
  if (state.draw3D) {
    space3D = new Space(3, Color(0.2, 0.2, 0.2));
    Light *light = new Light(3, 1.0, 1.0, 1.0);
    light->coordinates[0] = -100;
    light->coordinates[1] = -100;
    light->coordinates[2] = -100;
    space3D->AddLight(light);
    space4Dcopy->Project(space3D);
  }

  if (space4Dcopy != space4D)
    delete(space4Dcopy);

  state.theta += -0.01;

  return space3D;

}  //==== Process4D() ====//


