/****************************************************************/
/****            noosh.c = ishell97 to be                    ****/
/****           descended from ishell95                      ****/
/****            14 February 1995                            ****/
/****            29 August   1995                            ****/
/****             1 November 1995                            ****/ 
/****            15 november 1996   ishell96.c               ****/
/****            27 november 1996   ishell96.c               ****/
/****            23 march    1997   newsh.c                  ****/
/****    (C) 1994 Board of Trustees University of Illinois   ****/ 
/****    A Model Real-Time Interactive CAVE Application      ****/
/****    George Francis, Glenn Chappell, Chris Hartman       ****/
/****    e-mail  gfrancis@math.uiuc.edu                      ****/
/****    Generic RTICA by GGC 7/25/94 revised gkf 12.30.94   ****/ 
/****    Revised by GGC and GKF august 95                    ****/
/****    "Opengl ed" by AVB, nov 14-15, 1996                 ****/
/****    Reformatted by AVB, nov 16, 1996                    ****/
/****    adaptation to CAVE 2.6Beta started 6feb97 gkf       ****/
/****    New version to fix many bugs and clean up 17mar97 cmh  */
/****    Rationalization begun 26mar97 gkf                   ****/    
/****************************************************************/
/* CAVE trials 1apr97 bugs:
   1. At 24x24x2 triangles = 1152 we get 24fps with all 8 processors
      At 30x30x2 triangles = 1800 this drops to 16 fps on 8 processors
      With fewer processors (mpunlock) the dropoff is disastrous, 
   2. Current version (= 31mar97) does ambient only on process 2 (right
      wall and floor) .... no paint at all!
 
   Some emerging conventions on the path of rationalization 

   draw<what> == this function uses GL calls for drawing something
   cons       == pertaining to the console mode only
   cave       == pertaining to the CAVE mode(s)
                 all such functions are protected by #ifdef CAVEs for
                 CAVE less compilation
   audio      == pertaining to the vss audio stuff required three
                 matched files: vssClient.h libsnd.a and vss 
                 should also be protected by #ifdef SOUNDs
   autotymer  == is (now) called only by the master process in the CAVE
                 with the result that non-shared globals are not updated
                 on the other walls. So we share them. 
   <factor>  == a factor is a function that has been separated out for 
                 (1) convenience (i.e. is not a factor of any other function) 
                 (2) necessity (i.e. is a factor of another function )
   <depth>   == from main() in the factor graph. 

   Of necessity, C requires that the factor graph be treelike with the leaves
   at the top of the program code. For a C++ conversion, this tree will be 
   turned upside down.  
*/

/* Factor graph for this program. Please indicate structural modifications 
   The asterisk indicates a factor that has more than one function calling it. 
       main  
         arguments ....................command line switches and parameters 
           getopt  
         CAVEconfigure
         getmem .......................allocate shared memory  
           Malloc(share_var)
         dataprep .....................done only once   
           audioprep
             AUDinit
          *deFault 
         ifCAVE
           FrameFunc(cavetrack)........done once every frame 
                       *deFault........on the reset button combination
                       *autotymer .....automatic clockwork animation
                       *audiofunc .....sends messages to soundserver
           InitApplic(drawcaveinit)....done each frame 
           Display(drawcave)...........done each frame
                    *drawstars
                     graffiti..........on the front CAVE wall
                        char2wall
                    *drawall
                       drawtor.........compressed version of tr1.c 
                         dovert........do everything for one vertex
                       drawcube........transfer from skel.c as exercise 
           while(notESC) cavekeybo.....done asynchronously from display
                           *deFault
           audioclean .................shut down audio server
           Exit........................shut down CAVE properly 
         ifCONS
           gluttery....................we use the GLUT library
              drawcons.................analogue to drawcave
                *drawstars
                *drawall 
                    drawtor
                      dovert
                    drawcube
                messages...............analogue to grafitti
                  char2wall
                  speedometer..........unix based speedometer
              keyboard.................GLUT distinguishes between ascii keys 
                *deFault
                *audioclean
              special_keybo............and other kinds of keys
              mousepushed..............mouse buttons were pressed
              mousemoved...............mouse was moved       
              reshaped.................what to do if window is reshaped
              idle.....................done when nothing else is being done 
                 chaptrack.............updates the two affine matrices 
                *autotymer.............updates the animation parameters
                *audiofunc.............updates the sound servers  
         endIF 
*/
#include <stdlib.h>
/*#include <sys/types.h>*/
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <glut.h>
#include <unistd.h>

#ifdef SOUND
/*
#include <device.h>
*/
#include <string.h>
#include "vssClient.h"
#endif
#ifdef CAVE
#include <cave_ogl.h>
#include <cave.macros.h>
#endif 

#include "state.h"

/* Examples of useful geometric macros, use inliners in C++ */
#define  MAX(x,y)         (((x)<(y))?(y):(x))
#define  MIN(x,y)         (((x)<(y))?(x):(y))
#define  FOR(a,b,c)       for((a)=(b);(a)<(c);(a)++)
#define  DOT(p,q)         ((p)[0]*(q)[0]+(p)[1]*(q)[1]+(p)[2]*(q)[2])
#define  NRM(p)           fsqrt(DOT((p),(p)))
#define  DG            M_PI/180
#define  S(u)          fsin(u*DG)
#define  C(u)          fcos(u*DG)
#define  CLAMP(x,u,v) (x<u? u : (x>v ? v: x))

/* global variables ... add new ones on top of the stack */
char which[20]; /* audio needs this */
int delta; /* new variables for autotymer to change torus */
float gap, gap0=1.; /*very bad kludge ... for letting arguments reset default gap value */
int audiohandle; /* for telling vss the name of the .aud file */
float lux[3]={1.,2.,3.};             /*light source direction vector        */
float lu[3];     /* this needs to be shared current light direction vector*/
float mysiz,speed, torq, focal, far; /*console navigation variables         */
int win = 2;                         /* full screen, use 0 for demand sized */
unsigned int BUT,XX,YY,SHIF;     /* used in chaptrack gluttery           */ 
int xt,yt;                       /* once was xt,yt,xm,ym for viewportery */
int caveyes=0;                      /* rename? ----------> caveflag */
int mode,morph,msg,binoc;            /* pretty global */
int th0, th1, dth, ta0, ta1, dta;    /* torus parameters */
#define FLYMODE  (0)
#define TURNMODE (1) 

/*Shared memory variables as per Stuart Levy 1994 */  
struct share_var{ 
   float  s_siz;         /*final scaling factor before projection      */  
   int s_wnd;            /*wand or maus flag                           */
   int s_gnd;            /*background color                            */
   float s_lu[3];         /* current light direction */ 
   int s_mauspaw ;      /*binary for mouse button state                     */
   int s_th0 ;           /* since the autotymer changes these          */
   int s_th1 ;           /* they need to be shared                      */
   int s_ta0 ;           /*                                             */
   int s_ta1 ;           /*                                             */
   float s_gap ;           /*                                             */
   GLfloat s_aff[16],s_starmat[16]; /*affine matrices for object, stars*/
} *s_var; 

State s_state;  /* ADSODA state */


#define siz     (s_var->s_siz)
#define wnd     (s_var->s_wnd)
#define gnd     (s_var->s_gnd)  /* use only if printframe function is present */
#define lu      (s_var->s_lu )
#define aff     (s_var->s_aff)  
#define starmat (s_var->s_starmat)
#define mauspaw (s_var->s_mauspaw)
#define th0 (s_var->s_th0)
#define th1 (s_var->s_th1)
#define ta0 (s_var->s_ta0)
#define ta1 (s_var->s_ta1)
#define gap (s_var->s_gap)

/* these are used only in console mode      */
//int binoc;    /* flag                       */
float nose;   /* to eye distance in console */

void getmem(void){
  if(caveyes)
#ifdef CAVE
     s_var = (share_var *) CAVEMalloc(sizeof(struct share_var))
#endif
     ;
   else
     s_var = (share_var *) malloc(sizeof(struct share_var));
  if(s_var==NULL)
    {fprintf(stderr,"No room to share! %x \n",s_var); exit(1);}
}

void char2wall(GLfloat x,GLfloat y, GLfloat z, char buf[]){
     char *p; glRasterPos3f(x,y,z);
     for(p = buf;*p;p++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*p);
}

void autotymer(int);  /* to allow deFault to call the autotymer later */

void deFault(void){
int ii; float tmp;
th0=5; th1=355;  dth=15; ta0=5; ta1=355; dta=15; gap = gap0; 
msg=1; binoc=0; nose=.06; mauspaw = 0; wnd = 1; siz = 1.; mode=FLYMODE;  
speed=.1; torq=.02; focal = 2.; far =13.; mysiz=.01; morph=0; 
for(ii=0;ii<16;ii++)  starmat[ii]=aff[ii] = (ii/4==ii%4) ? 1 : 0;   
tmp=NRM(lux);  for(ii=0;ii<3;ii++)lux[ii]/=tmp;
if(caveyes){aff[12]=0; aff[13]= 5; aff[14]= -10 ; } /*5ft beyond front wall*/ 
       else{aff[12]=0; aff[13]= 0; aff[14]= -4.2; } /*inside the console */ 
autotymer(1); /* reset autotymer to start at the beginning */
}

void drawvert(int th, int ta){  
/* exercise ... float all the angles for a smoother homotopy */ 
float lmb,spec,nn[3], dog, cat;
nn[0] = C(th)*C(ta); /* unit sphere vector */  
nn[1] = S(th)*C(ta);
nn[2] =       S(ta);
lmb = MAX(CLAMP(nn[0]*lu[0] + nn[1]*lu[1] + nn[2]*lu[2],0.,1.),.3);
spec = MIN(1, 1*(1-10+10*lmb));                    /* clamp spec below maximum       */
dog = (ta-ta0)/(float)(ta1-ta0); cat = (th-th0)/(float)(th1-th0);
glColor3f(MAX(lmb*dog,spec),                     /* map R^2(dog,cat)->R^3(RGBspace */
          MAX(lmb*(.25 + fabsf(cat -.5)),spec),  /* dog cat model of Hartman       */
          MAX(lmb*(1 - cat ),spec));             /* illiLight by Ray Idaszak 1989  */
glVertex3f(C(th) + .5*nn[0],S(th) + .5*nn[1],.5*nn[2]);
}

void drawtor(void){
int th, ta;
dth = (int)((th1-th0)/24);  /* 24  meridian strips */ 
dta = (int)((ta1-ta0)/24);  /* 24  triangle pairs per strip */
for(th=th0; th < th1; th += dth){
  glBegin(GL_TRIANGLE_STRIP);
  for(ta=ta0; ta < ta1; ta += dta){ drawvert(th,ta); drawvert(th+gap*dth,ta); }
  glEnd();
  }
}

extern void drawall(void);

void drawstars(void){
int i;
static float star[1000][3];
static int virgin=1;
if(virgin){
  int ii,jj;
  float fact;
  virgin=0;
  for(ii=0;ii<1000;ii++)
     {
     for(jj=0;jj<3;jj++) star[ii][jj] =   random()/(float)0x40000000-1.;
     if(caveyes){
           fact = 90./NRM(star[ii]);
           for(jj=0;jj<3;jj++) star[ii][jj] *= fact;
           }   
     }
  }
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf(starmat);
  glColor3f(1.0,1.0,0.0);    
  glBegin(GL_POINTS);
    for(i=0;i<1000;i++) glVertex3fv(star[i]);
  glEnd();
  glPopMatrix();
  glClear(GL_DEPTH_BUFFER_BIT);
}

void audioprep(void){
#ifdef SOUND
   if (!FBgnSoundServer())
      fprintf(stderr,"UDP connection to sound server failed\n");
   else if ((audiohandle = AUDinit("noosh.aud")) == -1)
      fprintf(stderr,"Couldn't open noosh.aud\n");
#endif
}

void dohoot(void) {strcpy(which,"hoot");}
void dotrump(void){strcpy(which,"trump");}
void doflute(void){strcpy(which,"flute");}
void dodrum(void) {strcpy(which,"drum");}

void audiofunc(void){
#ifdef SOUND 
#define LIRP(X,A,B,x,a,b)  (X = A + (x-a)*(B-A)/(b-a))
  static float freq=440., ampl=1., indx=20., cmra=2.; 
  static float data[4];
  static omode =0; static ofocal = 0;
  static beep =0;  /* the sound server */

  if(omode > mode){dohoot(); beep = 1;}
  if(omode < mode){dodrum(); beep = 1;}

  LIRP(freq, 440, 1760, th0,  10, 170); /* frequeny mimics th1 */ 
  LIRP(indx, 10, 1,  focal, .03, 2.0); /* index  mimics focal */ 
  if(ofocal != focal){dodrum(); beep = 1;}    /* there is an fp bug here */

if(beep){data[0] = freq;  /* pitch in Herz      */ 
          data[1] = ampl;  /* the sum of all amplitudes must be < 1.0 */ 
          data[2] = indx;    
          data[3] = cmra;
          AUDupdate(audiohandle, which, 4, data);  /* send message  to vss */ 
          beep = 0; 
}
  omode = mode;  ofocal = focal; 
#endif
}

void audioclean(void){
#ifdef SOUND
   AUDterminate(audiohandle);
   EndSoundServer();
#endif
}

void parseArgs(State &state, int numArgs, const char **args);

void arguments(int argc,char **argv){

#if 0
  extern char *optarg;
  extern int optind; 
  int chi;   
  /* w: needs ONE number after -w, c<nothing> means NO number follows*/ 
  while ((chi = getopt(argc,argv,"w:cd:g:")) != -1) 
  switch(chi)  {case 'c': caveyes=1; break;  
                case 'w': win=atoi(optarg); break; 
                case 'g': gap0=atof(optarg); break;
                }
  if (optind!=argc) fprintf(stderr,"%s: Incorrect usage\n",argv[0]);
#endif

  parseArgs(s_state, argc, argv);

}

void dataprep(void){
   audioprep();
   deFault();
}

void autotymer(int reset){
#define  TYME(cnt,max,act) {static int cnt; if(first)cnt=max; else\
                            if(cnt?cnt--:0){ act ; goto Break;}}
  static int first = 1;  /* the first time autymer is called */
  if(reset)first=1;  /* or if it is reset to start over  */
  TYME( shrink , 150,th0++;th1--;ta0++;ta1--)
  TYME( pause  , 20,          )
  TYME( grow   , 150,th0--;th1++;ta0--;ta1++)
  TYME( dwell  , 30,            )
  TYME(finish  , 1 , first = 1  )
  first = 0;
  Break:   ;   /* yes Virginia, C has gotos */
}

/*Beware of competing keys in CAVEsimulation and wnd==1   */
/* you may want to remap these keys to avoid the conflict */

void keyboard(unsigned char key, int x, int y){
#define  IF(K)            if(key==K)
#define  PRESS(K,A,b)     IF(K){b;}IF((K-32)){A;}  
/*was backwards in previous versions */
#define  TOGGLE(K,flg)    IF(K){(flg) = 1-(flg); }
#define  CYCLE(K,f,m)     PRESS((K), (f)=(((f)+(m)-1)%(m)),(f)=(++(f)%(m)))
#define  SLIDI(K,f,m,M)   PRESS(K,(--f<m?m:f), (++f>M?M:f))
#define  SLIDF(K,f,m,M,d) PRESS(K,((f -= d)<m?m:f), ((f += d)>M?M:f))
/* Only ASCII characters can be processes by this GLUT callback function */ 
   TOGGLE('v',binoc);                            /* cross-eyed STEREO   */
   TOGGLE('w',msg);                              /* writing on/off      */
   TOGGLE('h',morph);                            /* autotymer on/off    */
   PRESS('n', nose -= .001 , nose += .001 );     /* for binoculars      */
   CYCLE(' ', mode,TURNMODE+1);                  /* fly/turn modes      */
   PRESS('i', mysiz /= 1.1, mysiz *= 1.1)        /* rescale the world   */
   PRESS('o', focal *= 1.1 , focal /= 1.1)       /* telephoto           */
   PRESS('p', far *= 1.01 , far   /= 1.01)       /* rear clipping plane */
   PRESS('s',speed /= 1.02, speed *= 1.02);      /* flying speed        */
   PRESS('q',torq /= 1.02, torq *= 1.02);        /* turning speed       */
   PRESS('g',gap /= .99, gap *= .99);             /* gap parameter       */
   PRESS('z', deFault(), deFault());             /* zap changes         */
   IF(27) {audioclean(); exit(0); }              /* ESC exit            */
}

void special_keybo(int key, int x, int y){
/*non-ASCII keypresses go here, if you're lucky enough to know their names */
 fprintf(stderr," non-ASCII character was pressed.\n");
 fprintf(stderr," use special_keybo() to process it\n");
}
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
#ifdef CAVE
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

void cavekeybo(void){
#define  CAVEIF(K)         if(CAVEgetbutton(K))
#define  CAVEPRESS(K,A,b)  CAVEIF(K){if(CAVEgetbutton(CAVE_LEFTSHIFTKEY)||\
                           CAVEgetbutton(CAVE_RIGHTSHIFTKEY)){b;}else{A;}}
#define  CAVETOGGLE(K,flg) CAVEIF(K){(flg) = 1-(flg); }
#define  CAVECYCLE(K,f,m)  CAVEPRESS((K),(f)=(((f)+(m)-1)%(m)),(f)=(++(f)%(m)) )
#define  CAVESLIDI(K,f,m,M)  CAVEPRESS(K,(--f<m?m:f), (++f>M?M:f))
#define  CAVESLIDF(K,f,m,M,d) CAVEPRESS(K,((f -= d)<m?m:f), ((f += d)>M?M:f))
  CAVEPRESS(CAVE_PKEY, far *= 1.01,   far   /= 1.01) /* rear clipping plane*/
  CAVEPRESS(CAVE_QKEY, speed /= 1.02, speed *= 1.02);/* flying speed       */
  CAVEPRESS(CAVE_RKEY, torq /= 1.02,  torq *= 1.02); /* turning speed      */
  CAVETOGGLE(CAVE_YKEY,wnd);                         /* mauspaw vs wandpaw */
/* not yet implemented */ 
   CAVEPRESS(CAVE_ZKEY, deFault(), deFault());        /* zap changes       */
}

void graffiti(void){             /* used to be called speedo  */
  char buf[256];                 /*messages written into here */ 
#define  LABEL3(x,y,z,W,u){sprintf(buf,(W),(u));char2wall(x,y,z,buf);}
  static float last=0.,fps;      /*for measuring time         */ 
  if(!CAVEMasterWall()) return;  /* USE CAVEMasterWall in papeBeta */
  if(floor(2.*(*CAVETime))>floor(2.*last)){  /* rationalize this */ 
     last=*CAVETime;
     fps=*CAVEFramesPerSecond;
     }
  if(mode==TURNMODE){
        glColor3f(1.,0.,1.);
        LABEL3(-3.8,1.,-5.0,\
          "4=auto 2=mode 3=shrink 6=grow 5=freeze 7=reset %s","");
        } else {
        glColor3f(1.,1.,0.);
        LABEL3(-3.8,1.0,-5.0,
          " push thumb-button to fly, 2=tractor gap %s","gap");
        }
     LABEL3(-4.8,8.0,-5.0, "ADSODA, by Greg Ferrar; based on illiOpenSkelGLUT\
    by Francis, Bourd, Hartman & Chappell, (C) 1995..1997 U.Illinois %s","")
       LABEL3(-4.8,1.0,-5.0,"%5.1f fps",fps);
}

void cavetrack(void){
  float azi,ele,rol,hx,hy,hz,wx,wy,wz;
  static float ohx,ohy,ohz;   /*old head position */
  static float owx,owy,owz;   /*old wand position */
  static int opaw = 0,paw, joy = 0, friz;

  if(!CAVEMasterDisplay()) return;  /* doit once per frame */
  paw = wnd ? (CAVEBUTTON1*2+CAVEBUTTON2)*2+CAVEBUTTON3 : mauspaw;
  if(paw) joy = 1; /* activate joystick by clicking any button*/
  if(opaw!=2 && paw==2) mode = (mode+1)%(TURNMODE+1); /* click modes */
  if(mode==TURNMODE){   /* hold middle button and click right to shrink */
     if (opaw == 2 && paw == 3)siz /= 1.5;     /*shrink object */
     if (opaw == 2 && paw == 6)siz *= 1.5;     /* grow   object */
     if (opaw != 4 && paw == 4)morph = 1-morph;/* shape changer */
     if (opaw != 5 && paw == 5)friz = 1-friz;  /* parking option */
     } 
  
  if(opaw==5 && paw == 5)gap *= 1.1;  /* hold right and  */ 
  if(opaw==3 && paw == 3)gap /= 1.1;  /* +left -right    */ 

  if(opaw==2 && paw == 7){ /* restart, the most useful feature */
     deFault(); ohx=ohy=ohz=0; owx=owy=owz=0;
     } 
  if(wnd){ 
      CAVEGetWandOrientation(azi,ele,rol);
      CAVEGetHead(hx,hy,hz);  /*position*/
      CAVEGetWand(wx,wy,wz);  /*position*/ 
      }
   else{  /* fix this for CAVE mouse flying */ 
      azi = .5*512; ele = .5*-512;
      } 
   glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

   glTranslatef(ohx-hx,ohy-hy,ohz-hz); 
   if(mode==TURNMODE) glTranslatef(aff[12],aff[13],aff[14]);
   if(!friz){  /* if not parked */
      glRotatef(rol*(mode?torq:-torq),0.,0.,1.); 
      glRotatef(ele*-torq, 1.,0.,0.); 
      glRotatef(azi*-torq,0.,1.,0.);
      }
   if(joy&&((fabsf(CAVE_JOYSTICK_Y)>.02)||(fabsf(CAVE_JOYSTICK_X)>.02)))
       glTranslatef(CAVE_JOYSTICK_X*speed,0.,CAVE_JOYSTICK_Y*speed);
 
 if(opaw==2 && paw==2)glTranslatef(wx-owx, wy-owy, wz-owz); /*wand tractor */

 if(mode == TURNMODE) glTranslatef(-aff[12],-aff[13],-aff[14]);
   glMultMatrixf(aff); 
   glGetFloatv(GL_MODELVIEW_MATRIX,aff);

  {int ii,jj; for(ii=0;ii<3;ii++){ lu[ii]=0; /* calculite */
   for(jj=0;jj<3;jj++) lu[ii] += aff[ii*4+jj]*lux[jj];} }

   if(mode==FLYMODE && !friz) /*make the star matrix rotate*/
      {
      glLoadIdentity();
      glRotatef(rol*(mode?torq:-torq),0.,0.,1.); 
      glRotatef(ele*-torq,1.,0.,0.); 
      glRotatef(azi*-torq,0.,1.,0.);
      glMultMatrixf(starmat); 
      glGetFloatv(GL_MODELVIEW_MATRIX,starmat);
      }
   glPopMatrix();
   opaw=paw; ohx=hx; ohy=hy; ohz=hz;
             owx=wx; owy=wy; owz=wz;
   if(morph) autotymer(0);  /* advance autotymer */
   audiofunc();
}

void drawcaveinit(void){   /* kludge? needed why ? */  
   CAVEFar = 1000.;
   glClearColor(0.,0.,0.,0.);
   glClearDepth(1.);
}

void drawcave(void){
  float hx,hy,hz;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawstars();
  graffiti();
  CAVEGetHead(hx,hy,hz);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(hx,hy,hz);
  glMultMatrixf(aff);
  glScalef(siz,siz,siz);
  drawall();
  glPopMatrix();
}
#endif

float speedometer(void){
double dbl; static double rate; static int ii=0;
static struct timezone notused; static struct timeval now, then;
   if(++ii % 8 == 0){  /* 8 times around measure time */
      gettimeofday(&now, &notused); /* elapsed time */
      dbl =  (double)(now.tv_sec - then.tv_sec)
         +(double)(now.tv_usec - then.tv_usec)/1000000;
      then = now;  rate = 8/dbl;
      }
   return((float)rate);
}
 
void messages(void){     
  char buf[256]; /* console messages are done differently from cave */
#define  LABEL2(x,y,W,u) {sprintf(buf,(W),(u));char2wall(x,y,0.,buf);}
  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
  gluOrtho2D(0,3000,0,3000);
  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
      /*bull's eye*/
      if(mode==0) glColor3f(0x22/255.,0x88/255.,0xdd/255.);
        else      glColor3f(.8,.8,.8);
      LABEL2(1500,1500,"%s","o");
      /* writings */
      if(mode==0) glColor3f(1.,0.,1.);
        else      glColor3f(1.,1.,0.);
      LABEL2(80,80,"%4.1f fps",speedometer());
      LABEL2(80,2840,\
      "(ESC)ape (V)Binoc (MAUS2)Fore (BAR)Flymode %s (H)omotopy (W)riting",
             mode?"FLYING":"CONTROL");
      LABEL2(10,10,"ADSODA by Greg Ferrar, based on illiOpenSkelGLUT \
   by Francis, Bourd, Hartman & Chappell, U Illinois, 1995..1997 %s","");
      LABEL2(80,2770,"(N)ose   %0.3f",nose);
      LABEL2(80,2700,"(S)peed  %0.4f",speed);
      LABEL2(80,2630," tor(Q) %0.4f",torq);
      LABEL2(80,2560,"near clipper %g", mysiz*focal);
      LABEL2(80,2490,"f(O)cal factor %g",focal);
      LABEL2(80,2420,"my s(I)ze %.2g",mysiz);
      LABEL2(80,2350,"far cli(P)er= %.2g",far);
      LABEL2(80,2280,"(Z)ap %s","");
      LABEL2(80,2210,"(G)ap %.2g",gap);
    glPopMatrix();
glMatrixMode(GL_PROJECTION); glPopMatrix();
} 

void chaptrack(int but,int xx,int yy,int shif){  
   long dx,dy; 
   dx = xx -.5*xt; dx = abs(dx)>5?dx:0;
   dy = yy -.5*yt; dy = abs(dy)>5?dy:0;

   glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

   if(mode==FLYMODE) glTranslatef(aff[12],aff[13],aff[14]);
   glRotatef(dx*torq,0.,1.,0.); glRotatef(dy*torq,1.,0.,0.);
   if(but&(1<<GLUT_RIGHT_BUTTON ))glRotatef(shif?-10:-1,0.,0.,1.);
   if(but&(1<<GLUT_LEFT_BUTTON  ))glRotatef(shif?10:1,0.,0.,1.);
   if(mode==TURNMODE){
      glPushMatrix();
      glMultMatrixf(starmat);
      glGetFloatv(GL_MODELVIEW_MATRIX,starmat);
      glPopMatrix();
   }
   if(but&(1<<GLUT_MIDDLE_BUTTON)) glTranslatef(0.,0.,shif ? -speed : speed);
   if(mode==FLYMODE) glTranslatef(-aff[12],-aff[13],-aff[14]);
   glMultMatrixf(aff); 
   glGetFloatv(GL_MODELVIEW_MATRIX,aff);
   {int ii,jj; for(ii=0;ii<3;ii++){ lu[ii]=0; /* calculite */
          for(jj=0;jj<3;jj++) lu[ii] += aff[ii*4+jj]*lux[jj];}}
   glPopMatrix();
}

void reshaped(int xx, int yy){xt=xx ; yt=yy;}   /* what is this for ? */

void drawcons(void){  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if(binoc) glViewport(0,yt/4,xt/2,yt/2);
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  glFrustum(-mysiz*xt/yt,mysiz*xt/yt,-mysiz,mysiz,mysiz*focal,far); 
  glMatrixMode(GL_MODELVIEW); glLoadIdentity();
  drawstars();
  glTranslatef(-binoc*nose,0.0,0.0);
  glMultMatrixf(aff);
  drawall();
  if(binoc){
    glViewport(xt/2,yt/4,xt/2,yt/2); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawstars();
    glTranslatef(binoc*nose,0.0,0.0);
    glMultMatrixf(aff);
    drawall();
    }
  glViewport(0,0,xt,yt);
  if(msg) messages();
  glutSwapBuffers();
}

void idle(void){ /*do this when nothing else is happening*/
   if(morph) autotymer(0);  /* advance autotymer */ 
   glutPostRedisplay();  /*redraw the window*/
   chaptrack(BUT,XX,YY,SHIF);
   audiofunc();  
}

void mousepushed(int but,int stat,int x,int y){
  if(stat==GLUT_DOWN) BUT |= (1<<but);
  else BUT &= (-1 ^ (1<<but));  
  XX=x; YY=y; SHIF=(glutGetModifiers()==GLUT_ACTIVE_SHIFT)?1:0;
}

void mousemoved(int x,int y){  XX=x; YY=y; }

#ifndef CAVE
#include "state.h"
State *nooshState = NULL;
#endif

void initState(State &state);

void main(int argc, char **argv){  
  //   arguments(argc,argv); 
   if(caveyes)
#ifdef CAVE
       CAVEConfigure(&argc, argv, NULL)
#endif
       ;
   getmem();
   dataprep();

   // Set up the ADSODA state
   initState(s_state);
   parseArgs(s_state, argc - 1, &(argv[1]));
   //   cout << "NOW: " << s_state.draw3D << endl;

 
   if(caveyes){
#ifdef CAVE
       CAVEInit(); /* Each wall is (part of) a forked process from here on.*/ 
       CAVEFrameFunction(cavetrack,0);  
       /*is restricted to MasterWall, so once per frame */
       CAVEInitApplication(drawcaveinit, 0); 
       CAVEDisplay(drawcave, 0);
       while(!CAVEgetbutton(CAVE_ESCKEY))
         {
         cavekeybo();  /* is asynchronous from display processes */ 
	 sginap(10); 
	 }
       audioclean();
       CAVEExit();
#endif
       ;}
     else{ /*console main*/
       glutInit(&argc, argv);
       glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH);
       switch(win)
         { case 0: break;
           case 1: glutInitWindowSize(640, 480);
                   glutInitWindowPosition(0,1024-480);
                   break;
           case 2: glutInitWindowPosition(0,0);
	           break;
         }
       glutCreateWindow("<* illiSkel in C/OpenGL/GLUT *>");
       if(win==2) glutFullScreen();
       glEnable(GL_DEPTH_TEST);
       glutDisplayFunc(drawcons);       
       glutKeyboardFunc(keyboard);
       glutSpecialFunc(special_keybo);
       glutMouseFunc(mousepushed);
       glutMotionFunc(mousemoved);       
       glutPassiveMotionFunc(mousemoved); 
       glutReshapeFunc(reshaped);
       glutIdleFunc(idle);             
       glutMainLoop();
       }
} 


void drawDemoFrame(State &state);
void prepareDemoFrame(State &state);
void drawDemoFrame(void);

void drawall(void) {

  prepareDemoFrame(s_state);
  drawDemoFrame(s_state);

}



extern "C" void ADSODA_dsp(void);
extern "C" void ADSODA_ppr(void);
extern "C" void ADSODA_ini(char *options);
extern "C" void ADSODA_ffn(void);
extern "C" void ADSODA_nav(void);

void drawDemoFrame(void);
void parseArgs(State &state, char *arg_string);
void initState(State &state);

#include <iostream.h>

void ADSODA_dsp() {

  drawDemoFrame(s_state);

}

void ADSODA_ini(char *options) {

  caveyes = 1;
  getmem();
  dataprep();

  initState(s_state);
  parseArgs(s_state, options);

}

void ADSODA_ppr(void) {

}

void ADSODA_ffn(void) {

  static float last_joyx;
  static float last_joyy;

  //  cout << "CAVE_JOYSTICK_Y: " << CAVE_JOYSTICK_Y << endl;

  if ((CAVE_JOYSTICK_Y < -0.8) && (last_joyy >= -0.8))
    s_state.outlinePolys = !s_state.outlinePolys;

  if ((CAVE_JOYSTICK_Y > 0.8) && (last_joyy <= 0.8))
    s_state.fillPolys = !s_state.fillPolys;

  if ((CAVE_JOYSTICK_X < -0.8) && (last_joyx >= -0.8)) {
    if (s_state.dimension == 2) 
      s_state.removeHidden2D = !s_state.removeHidden2D;
    else if (s_state.dimension == 3) 
      s_state.removeHidden3D = !s_state.removeHidden3D;
    if (s_state.dimension == 4) 
      s_state.removeHidden4D = !s_state.removeHidden4D;
  }

  if ((CAVE_JOYSTICK_X > 0.8) && (last_joyx <= 0.8)) {
    if (s_state.dimension == 2) 
      s_state.rotate2D = !s_state.rotate2D;
    else if (s_state.dimension == 3) 
      s_state.rotate3D = !s_state.rotate3D;
    if (s_state.dimension == 4) 
      s_state.rotate4D = !s_state.rotate4D;
  }

  last_joyx = CAVE_JOYSTICK_X;
  last_joyy = CAVE_JOYSTICK_Y;


  //  if (!CAVEMasterDisplay())
  prepareDemoFrame(s_state);

}
