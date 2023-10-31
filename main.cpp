#include <fstream>
#include <iostream>
#include <sstream>

#include <list>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>



#include <stdio.h>
#include <bits/stdc++.h>
#include <termios.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// openGL and related includes
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>

#include "PLYWriter.h"
#include "Context.h"
#include "Scope.h"
#include "Grammar.h"
#include "MathParser.h"
#include "LBAssembler.h"




#define STB_IMAGE_IMPLEMENTATION
#include "stbimage/stb_image.h"


// local includes
#include "hex_planet.h"





//using namespace std;


//===========================================================================================================================

//												 Globals 

//===========================================================================================================================

Grammar *grammar;

static int SCREEN_WIDTH=800;
static int SCREEN_HEIGHT=600;
GLuint tex_2d,tex_moon;
  float A1x=0.0,A1y=0.0,A1z=0.0;

    float scale = 0.1;
    int scale_level=0;
int g_glutMainWin;
float g_aspect;
HexPlanet *m_planet,*m_moon;

  //set of sides that are travelable or not per type
  std::vector<unsigned int> vec_type1;
  std::vector<unsigned int> vec_type2;
  std::vector<unsigned int> vec_type3;
  std::vector<unsigned int> vec_type4;
  std::vector<unsigned int> vec_type0;
 

GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, 15.0, 5.0, 0.0};  /* Infinite light location. */
int mouse_x=0,mouse_y=0;

GLfloat texcoord[][2] = {{0.5, 0.0},
{0.0669875,0.25},
{0.0669875, 0.75},
{0.5, 1.0},
{0.9330125, 0.75},
{0.9330125, 0.25}};  

int tiles[3000][3];
GLuint textile[5];
GLuint number_tile[10];

float anglex=0.0;
float angley=0.0;
float sz=1.0f;
float sx=0.0f;
float sy=0.0f;
float sX=0.0f,sY=0.0f,sZ=1.0f;
int current_axes=5;
int last_axes=5;
bool invert_y=false;
float theta2=atan(sz/sx);
Imath::V3f svec4,svec(0.0f,0.0f,1.0f),SVEC(0.0f,0.0f,1.0f);
Imath::V3f X(1.0f,0.0f,0.0f),Y(0.0f,1.0f,0.0f),Z(0.0f,0.0f,1.0f);

bool button_down=false;
bool flipped=false;
bool moving=false;
int select_hex=0;
int current_select_hex=0;


Imath::V3f Align;


std::vector<int> hex_tile_list;
std::vector<int> hex_path;
std::vector<int>::iterator it;
int count_path=0;
int overflow_count=0;









class HexGrid{
public:
    HexGrid(Imath::V3f center,int size){
		this->size=size;
		this->center=center;
		
	}
	void setcolor(float r,float g,float b){
		this->r=r/256.0f;
		this->g=g/256.0f;
		this->b=b/256.0f;
	}
	void add(Imath::V3f p){
		this->points[this->counter]=p;
		this->counter++;
		
	}
	Imath::V3f center,points[6];
	int size;
	int counter=0;
	float r,g,b;
	
};
std::vector<HexGrid *> hextiles;
std::vector<HexGrid *> globalhextiles;













/***    drawing text on screen  ***/     
void drawString(char *string)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();             
    glLoadIdentity();   
    int w = glutGet( GLUT_WINDOW_WIDTH );
    int h = glutGet( GLUT_WINDOW_HEIGHT );
    glOrtho( 0, w, 0, h, -1, 1 );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable( GL_DEPTH_TEST ); 

    glDisable( GL_LIGHTING );
    glColor3f(1, 0, 0);

    glRasterPos2i(20, 20);
    void *font = GLUT_BITMAP_HELVETICA_18; 
    for (char* c=string; *c != '\0'; c++) 
    {
        glutBitmapCharacter(font, *c); 
    }

    glEnable( GL_LIGHTING );

    glEnable (GL_DEPTH_TEST);     

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();  
}
//===========================================================================================================
//to_polar
//================================================================================================================
Imath::V3f topolar(Imath::V3f a){
	Imath::V3f p;
	p.x=1.0f;
	p.y=atan2f(a.z,a.x);
	p.z=atanf(a.y/sqrtf(a.x*a.x+a.z*a.z));
	return p;
}
Imath::V3f frompolar(float theta,float phi,float theta_2,float phi_2){
	Imath::V3f p(0,1,0);
	Imath::M33f M1,M2,M3,M4;
	float st=sin(theta);
	float ct=cos(theta);
	float sp=sin(phi);
	float cp=cos(phi);
	
	
	M1.makeIdentity ();
	M1[0][0]=ct;
	M1[0][1]=-st;
	M1[1][0]=st;
	M1[1][1]=ct;
	
	M2.makeIdentity ();
	M2[0][0]=cp;
	M2[0][2]=sp;
	M2[2][0]=-sp;
	M2[2][2]=cp;
	
	st=sin(theta_2);
	ct=cos(theta_2);
	sp=sin(phi_2);
	cp=cos(phi_2);
	
	
	M3.makeIdentity ();
	M3[0][0]=ct;
	M3[0][1]=-st;
	M3[1][0]=st;
	M3[1][1]=ct;
	
	M4.makeIdentity ();
	M4[0][0]=cp;
	M4[0][2]=sp;
	M4[2][0]=-sp;
	M4[2][2]=cp;
	
	Imath::V3f out=(p*M3*M4)*M1*M2;
	
	
	return out;
	
}
//===========================================================================================================
//satelite
//================================================================================================================
float angleSat[10];


Imath::V3f Sat(Imath::V3f axis,int num,float D_ANGLE,float phi,float theta){
	angleSat[num]+=D_ANGLE;
if(angleSat[num]>=2.0f*M_PI)
	angleSat[num]=0.0f;
float dt=0.000007/2;
	axis=topolar(axis);
for(int i=0;i<30;i++){
	if(i==0)glPointSize(14.0f);
	else glPointSize(1.0f);
	Imath::V3f points;

	points=frompolar(theta+angleSat[num]-5000.0f*dt*float(i),phi,axis.y,axis.z);
	
	
	points.normalize();
	points*=m_planet->kPlanetRadius + 1.75+angleSat[num]*0.005f;
	glPushMatrix();


	
	glBegin(GL_POINTS); //starts drawing of points
		  glVertex3f(points[0],points[1],points[2]);//upper-right corner
		  
	glEnd();//end drawing of points
	glPopMatrix();
}
Imath::V3f points;
	
	points=frompolar(theta+angleSat[num],phi,axis.y,axis.z);
	
	return points;
}

int selected_hex;
//tileNode *selected_hex_tile=NULL;
//===========================================================================================================
// glut Display Func
//================================================================================================================



void glut_Display( void )//-------------------------------------------------------------------------------------
{  

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 70.0f, 1.0f, 0.1f, 5000.0f );
 // int x_vp=-400,y_vp=-400;
glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
  
 float dt=0.000007/2;
float dsx=0,dsy=0,dsz=0;

//if(button_down){

//	button_down=false;

 dsx=(mouse_x-SCREEN_WIDTH/2.0f)*dt;

 dsy=-(mouse_y-SCREEN_HEIGHT/2)*dt;

//}


if(fabsf(SVEC[1])>0.207f){

Imath::V3f x,y,z,Yt,Xt,Zt;

//printf("   SVEC(%2.3f  %2.3f   %2.3f ) X(%2.3f   %2.3f   %2.3f )  Y(%2.3f  %2.3f   %2.3f )   Z(%2.3f  %2.3f   %2.3f )\n",SVEC[0],SVEC[1],SVEC[2],X[0],X[1],X[2],Y[0],Y[1],Y[2],Z[0],Z[1],Z[2]);


if(SVEC[1]>0.0f){
y[0]=-SVEC[0];

y[1]=(SVEC[0]*SVEC[0]+SVEC[2]*SVEC[2])/SVEC[1];


y[2]=-SVEC[2];
}
else {
y[0]=SVEC[0];

y[1]=-(SVEC[0]*SVEC[0]+SVEC[2]*SVEC[2])/SVEC[1];


y[2]=SVEC[2];
	
	
	
}


y.normalize();




//printf("(%2.3f   %2.3f   %2.3f ) ",y[0],y[1],y[2]);
z=SVEC;
z.normalize();
x=y%z;
x.normalize();

//y=z%x;
//y.normalize();

float square_angle=acos((y^z)/(sqrt(z^z)*sqrt(y^y)))*180.0f/M_PI;

//printf("  %2.3f   ",square_angle);


Yt[0]=y[0]*X[0]+y[1]*Y[0]+y[2]*Z[0];
Yt[1]=y[0]*X[1]+y[1]*Y[1]+y[2]*Z[1];
Yt[2]=y[0]*X[2]+y[1]*Y[2]+y[2]*Z[2];

Zt[0]=z[0]*X[0]+z[1]*Y[0]+z[2]*Z[0];
Zt[1]=z[0]*X[1]+z[1]*Y[1]+z[2]*Z[1];
Zt[2]=z[0]*X[2]+z[1]*Y[2]+z[2]*Z[2];

Xt[0]=x[0]*X[0]+x[1]*Y[0]+x[2]*Z[0];
Xt[1]=x[0]*X[1]+x[1]*Y[1]+x[2]*Z[1];
Xt[2]=x[0]*X[2]+x[1]*Y[2]+x[2]*Z[2];

X=Xt;Y=Yt;Z=Zt;

X.normalize();
Y.normalize();
Z.normalize();



//printf("   SVEC(%2.3f  %2.3f   %2.3f ) X(%2.3f   %2.3f   %2.3f )  Y(%2.3f  %2.3f   %2.3f )   Z(%2.3f  %2.3f   %2.3f )\n",SVEC[0],SVEC[1],SVEC[2],X[0],X[1],X[2],Y[0],Y[1],Y[2],Z[0],Z[1],Z[2]);


float SY,SX,SZ;
SVEC[0]=0.0f;
SVEC[1]=0.0f;
SVEC[2]=1.0f;
SX=SVEC[0]*X[0]+SVEC[1]*Y[0]+SVEC[2]*Z[0];
SY=SVEC[0]*X[1]+SVEC[1]*Y[1]+SVEC[2]*Z[1];
SZ=SVEC[0]*X[2]+SVEC[1]*Y[2]+SVEC[2]*Z[2];
printf("SX SY SZ (%2.3f   %2.3f   %2.3f ) ",SX,SY,SZ);


//getchar();
}



//up std::vector
Imath::V3f SVEC2;
SVEC2=SVEC-Imath::V3f(0.0f,1.0f,0.0f);

 SVEC2.normalize();

Imath::V3f SVEC3=SVEC%SVEC2;
SVEC3.normalize();

Imath::V3f SVEC5;
SVEC5=SVEC%SVEC3;
SVEC5.normalize();

Imath::V3f SVEC_POINTER,svec_pointer;

SVEC_POINTER[0]=SVEC[0]+250.0f*dsx*SVEC3[0]+250.0f*dsy*SVEC5[0];
SVEC_POINTER[1]=SVEC[1]+250.0f*dsx*SVEC3[1]+250.0f*dsy*SVEC5[1];
SVEC_POINTER[2]=SVEC[2]+250.0f*dsx*SVEC3[2]+250.0f*dsy*SVEC5[2];

svec_pointer[0]=SVEC_POINTER[0]*X[0]+SVEC_POINTER[1]*Y[0]+SVEC_POINTER[2]*Z[0];
svec_pointer[1]=SVEC_POINTER[0]*X[1]+SVEC_POINTER[1]*Y[1]+SVEC_POINTER[2]*Z[1];
svec_pointer[2]=SVEC_POINTER[0]*X[2]+SVEC_POINTER[1]*Y[2]+SVEC_POINTER[2]*Z[2];

SVEC[0]+=dsx*SVEC3[0]+dsy*SVEC5[0];
SVEC[1]+=dsx*SVEC3[1]+dsy*SVEC5[1];
SVEC[2]+=dsx*SVEC3[2]+dsy*SVEC5[2];

SVEC.normalize();



sx=SVEC[0]*X[0]+SVEC[1]*Y[0]+SVEC[2]*Z[0];
sy=SVEC[0]*X[1]+SVEC[1]*Y[1]+SVEC[2]*Z[1];
sz=SVEC[0]*X[2]+SVEC[1]*Y[2]+SVEC[2]*Z[2];

Imath::V3f svec3;
svec3[0]=SVEC3[0]*X[0]+SVEC3[1]*Y[0]+SVEC3[2]*Z[0];
svec3[1]=SVEC3[0]*X[1]+SVEC3[1]*Y[1]+SVEC3[2]*Z[1];
svec3[2]=SVEC3[0]*X[2]+SVEC3[1]*Y[2]+SVEC3[2]*Z[2];

Imath::V3f svec5;
svec5[0]=SVEC5[0]*X[0]+SVEC5[1]*Y[0]+SVEC5[2]*Z[0];
svec5[1]=SVEC5[0]*X[1]+SVEC5[1]*Y[1]+SVEC5[2]*Z[1];
svec5[2]=SVEC5[0]*X[2]+SVEC5[1]*Y[2]+SVEC5[2]*Z[2];

Imath::V3f svec4(sx,sy,sz);

svec=svec4;

svec.normalize();
svec5.normalize();

char string1[100];
sprintf(string1," tile index: %d (%2.3f   %2.3f   %2.3f )  (%2.3f  %2.3f   %2.3f )   (%2.3f  %2.3f   %2.3f )",select_hex,SVEC[0],SVEC[1],SVEC[2],X[0],X[1],X[2],Y[0],Y[1],Y[2]);
drawString(string1);



gluLookAt(3.0f*svec[0], 3.0f*svec[1], 3.0f*svec[2], 0.0, 0.0, 0.0,svec5[0],svec5[1],svec5[2]);


light_position[0]=10.0f*svec5[0];
light_position[1]=10.0f*svec5[1];
light_position[2]=10.0f*svec5[2];

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

 glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();






glPushMatrix();
glScalef(0.8,0.8,0.8);
glDisable(GL_LIGHTING);
glBegin( GL_LINES );
	
	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 1.0f, 0.0f, 0.0f );


	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 1.0f, 0.0f );

	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 1.0f );

	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( X[0], X[1], X[2] );


	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( Y[0], Y[1], Y[2] );

	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( Z[0], Z[1], Z[2] );

	//glScalef(25.0f,25.0f,25.0f);
glColor3f( 0.0f, 1.0f, 1.0f );
	glVertex3f( svec[0], svec[1], svec[2] );
	glVertex3f( svec[0]+10.0f*dsy*svec5[0], svec[1]+10.0f*dsy*svec5[1], svec[2]+10.0f*dsy*svec5[2] );
	glVertex3f( svec[0], svec[1], svec[2] );
	glVertex3f( svec[0]+10.0f*dsx*svec3[0], svec[1]+10.0f*dsx*svec3[1], svec[2]+10.0f*dsx*svec3[2] );

	glEnd();
glPopMatrix();



glEnable(GL_LIGHTING);
glScalef(scale,scale,scale);
glPushMatrix();



//glColor4f(1,1,1,0.6);			
//grammar->context->getScene().draw();

glColor4f(1,1,1,1);

grammar->context->draw();


glPopMatrix();








  glutSwapBuffers(); 
glutPostRedisplay();
}
//----------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------------




void pressNormalKey(unsigned char key, int x, int y){
	
}
void releaseNormalKey(unsigned char key, int x, int y){
	
	
}
void pressSpecialKey(int key, int x, int y){
	switch (key) {
      case GLUT_KEY_F1:    // F1: Toggle between full-screen and windowed mode
		 delete grammar;
		 grammar=new Grammar("./test.grammar");
		 grammar->generateGeometry();
		 grammar->context->getScene().calc_normals();
         break;
      case GLUT_KEY_RIGHT:    // Right: increase x speed
          break;
      case GLUT_KEY_LEFT:     // Left: decrease x speed
          break;
      case GLUT_KEY_UP:       // Up: increase y speed
          break;
      case GLUT_KEY_DOWN:     // Down: decrease y speed
          break;
      case GLUT_KEY_PAGE_UP:  // Page-Up: increase ball's radius
         break;
      case GLUT_KEY_PAGE_DOWN: // Page-Down: decrease ball's radius
         break;
   }
}
void releaseSpecialKey(int key, int x, int y){
	
}






//----------------------------------------------------------------------------------------------------------------------------------------
void OnMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
  { 
		button_down=true;
  }
  else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
  { 
		button_down=false;
  }	
  else   if ((button == 3) || (button == 4)) // It's a wheel event
   {
       // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
       if (state == GLUT_UP){
		   
		    return;
		} 
       if(button==3){
       scale_level++;
		scale*=1.0f/1.05f;
		}
       else { 
		scale*=1.05f;
		scale_level--;
		}
       if(scale>1.0f){
		scale=1.0f;
		scale_level=0;
		}
   }
}

void glut_Motion(int x, int y)
{

mouse_x = x;
mouse_y = y;
glutPostRedisplay();
}

int load_texture(const char *name){
 int tex_2D=SOIL_load_OGL_texture
	(
		name,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	/* check for an error during the load process */
	if( 0 == tex_2D )
	{
		std::cout<<"SOIL loading error:"<< SOIL_last_result()<<std::endl;
	}
	else {
		std::cout<<"LOADED TEXTURE"<<std::endl;

	
    glBindTexture(GL_TEXTURE_2D, tex_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
	}
return tex_2D;
}

//==================================================================================================================================================

//											 		Main program



//==================================================================================================================================================


#define iWidth 16
#define iHeight 16
#define iDepth 16


static GLubyte image[iDepth][iHeight][iWidth][3];






int main( int argc, char *argv[])
{












		
		
	
	
	// Initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
 
	g_glutMainWin = glutCreateWindow( "RaceToMars" );

	//glutFullScreen();  
	glutDisplayFunc( glut_Display );
	glutPassiveMotionFunc( glut_Motion );
	glutMouseFunc(OnMouseClick); 
	
	glutKeyboardFunc(pressNormalKey);
    glutKeyboardUpFunc(releaseNormalKey);
    glutSpecialFunc(pressSpecialKey);
    glutSpecialUpFunc(releaseSpecialKey);
    
    
	glEnable( GL_BLEND );
    //glDisable( GL_BLEND );
    //	straight alpha
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //	premultiplied alpha (remember to do the same in glColor!!)
    //glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

    //	do I want alpha thresholding?
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.5f );
    glPointSize(4);
    
    // LoadGLTextures(); 
	

srand(time(NULL));
    std::cout << std::fixed << std::showpoint;
	std::cout.precision(2);
    grammar=new Grammar("./test.grammar");
		grammar->addContext();
		GLuint texid=load_texture("paper.png");
		grammar->context->loadTexture(texid);
		
		texid=load_texture("ice.png");
		grammar->context->loadTexture(texid);
		
		texid=load_texture("cherrywood.png");
		grammar->context->loadTexture(texid);
		
		
		grammar->context->genPrimitives();
		
		grammar->generateGeometry();
		std::cout<<"Finished generating geometry..."<<std::endl;

grammar->context->getScene().calc_normals();

    
	/* load an image file directly as a new OpenGL texture */
/*
	tex_2d=load_texture("earth.png");
	tex_moon=load_texture("moon.png");

	textile[0]=load_texture("sky-patch-1.png");
	textile[1]=load_texture("sky-patch-2.png");
	textile[2]=load_texture("sky-patch-3.png");
	textile[3]=	load_texture("sky-patch-4.png");
	textile[4]=load_texture("sky-patch-5.png");


	number_tile[0]=load_texture("n0.png");
	number_tile[1]=load_texture("n1.png");
	number_tile[2]=load_texture("n2.png");
	number_tile[3]=load_texture("n3.png");
	number_tile[4]=load_texture("n4.png");
	number_tile[5]=load_texture("n5.png");
	number_tile[6]=load_texture("n6.png");
	number_tile[7]=load_texture("n7.png");
	number_tile[8]=load_texture("n8.png");
	number_tile[9]=load_texture("n9.png");
*/
	 /* Enable a single OpenGL light. */
	GLfloat light_ambient[] =
	  {0.2, 0.2, 0.2, 0.2};
	  GLfloat light_diffuse[] =
	  {1.0, 1.0, 1.0, 0.8};
	  GLfloat light_specular[] =
	  {1.0, 1.0, 1.0, 0.9};


	  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	  glEnable(GL_LIGHT0);
	  glDepthFunc(GL_LESS);
	  glEnable(GL_DEPTH_TEST);
	 glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	  glEnable(GL_LIGHT1);
	  glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Call glut main loop  
		glutMainLoop();



	return 1;
}













