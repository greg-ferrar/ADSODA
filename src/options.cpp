#include <stdlib.h>
#include <string.h>
#include "state.h"
#include <iostream.h>


void parseArgs(State &state, int numArgs, const char **args) {

  for (int i = 0; i < numArgs; i++) {

    const char *option = args[i];
    
    //    cout << ">>>> parsing ADSODA options: " << option << endl;

    if (!strcasecmp(option, "-dim")) {
      i++;
      state.dimension = atoi(args[i]);
    }
    
    else if (!strcasecmp(option, "-removeHidden2D"))
      state.removeHidden2D = true;
    
    else if (!strcasecmp(option, "-removeHidden3D"))
      state.removeHidden3D = true;
    
    else if (!strcasecmp(option, "-removeHidden4D"))
      state.removeHidden4D = true;
    
    else if (!strcasecmp(option, "-rotate2D"))
      state.rotate2D = true;
    
    else if (!strcasecmp(option, "-rotate3D"))
      state.rotate3D = true;
    
    else if (!strcasecmp(option, "-rotate4D"))
      state.rotate4D = true;
    
    else if (!strcasecmp(option, "-draw1D"))
      state.draw1D = true;
    
    else if (!strcasecmp(option, "-draw2D"))
      state.draw2D = true;
    
    else if (!strcasecmp(option, "-draw3D"))
      state.draw3D = true;
    
    else if (!strcasecmp(option, "-nofill"))
      state.fillPolys = false;

    else if (!strcasecmp(option, "-nooutline"))
      state.outlinePolys = false;

    else if (!strcasecmp(option, "-drawcube"))
      state.drawcubeFlag = true;

    else
      cout << "#### ERROR unknown ADSODA option: #" << option << "#" << endl;

  }  // for

} 


void parseArgs(State &state, char *arg_string) {

  char *args[100];
  
  char *p = arg_string;
  long i = 0;
  while (*p == ' ')
    p++;
  char *tokenstart = p;
  while (*p) {

    if (*p == '\n')
      *p = 0;

    if (*p == ' ') {
      *p = 0;
      //      cout << "Got token: " << tokenstart << endl;
      args[i++] = tokenstart;
      tokenstart = p + 1;
    }

    p++;
    
  }  // while p

  if (tokenstart != p) {
    //    cout << "Got last token: " << tokenstart << endl;
    args[i++] = tokenstart;
  }

  parseArgs(state, i, args);

}  //=== parseArgs() ====//
