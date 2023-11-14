#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <list>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>
#include <filesystem>
#include <glob.h>


#include <stdio.h>
#include <bits/stdc++.h>
#include <termios.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>



#include <epoxy/gl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


// openGL and related includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include	 <SDL_image.h>
#include <SDL_ttf.h>



#include <gtk/gtk.h>



#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


#include "PLYWriter.h"
#include "Context.h"
#include "Scope.h"
#include "Grammar.h"
#include "MathParser.h"
#include "LBAssembler.h"




#define STB_IMAGE_IMPLEMENTATION
#include "stbimage/stb_image.h"

#include "hex_planet.h"
#include "textures.h"


bool dialog_open=false;

	GtkWidget    *window;
static GtkWidget *gl_area = NULL;

static void dialog_result(GtkWidget *dialog, gint resp, gpointer data)
{
    if (resp == GTK_RESPONSE_OK) {
        // do nothing
    } else {
        gtk_widget_destroy(dialog);
    }
    dialog_open=true;
}

static void open_dialog()
{
	    //static struct data callback_params;
    GtkWidget *dialog=NULL;
    GtkFileFilter *filter=NULL;

    dialog = gtk_file_chooser_dialog_new("Choose a file:", GTK_WINDOW(window), 
                        GTK_FILE_CHOOSER_ACTION_SAVE, "_Save",
                         GTK_RESPONSE_OK, "_Cancel",
                         GTK_RESPONSE_CANCEL, NULL);
     
     if(dialog==NULL){
		 std::cout<<"Error opening dialog."<<std::endl;
		 return;
	 }
     
    std::cout<<"opening filechooser dialog"<<std::endl;
    filter = gtk_file_filter_new();
    if(filter==NULL){
		std::cout<<"Error new filter."<<std::endl;
		 return;
	}
    gtk_file_filter_add_pattern(filter, "*.grammar");
	
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "test.Grammar");

    std::cout<<"connect dialog to response"<<std::endl;
    g_signal_connect(dialog,"response",G_CALLBACK (dialog_result),NULL);
     gtk_window_set_modal (GTK_WINDOW(dialog), TRUE);
    gtk_widget_show_all (dialog);
    
}
//===========================================================================================================================

//												 Globals 

//===========================================================================================================================

Grammar *grammar;

static int SCREEN_WIDTH=1600;
static int SCREEN_HEIGHT=1200;

static int SCREEN_LINES=60;
GLuint tex_2d,tex_moon;
  float A1x=0.0,A1y=0.0,A1z=0.0;

    //float scale = 0.1;
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









char const * filterPatterns[1] = { "*.grammar"  };



std::vector<int> texture_list;





void display( void );




int generateTexture(int index)
{
	GLuint mTexture;
    glGenTextures(1 , &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);// Bind our 2D texture so that following set up will be applied

    //Set texture wrapping parameter
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    //Set texture Filtering parameter
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    //Load the image
    
    unsigned char *image=NULL;
    if(index==0)image=texture_image_array0;
    else if(index==1)image=texture_image_array1;
    else if(index==2)image=texture_image_array2;
    //Generate the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB , 128 , 128, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    
    
    
    glBindTexture(GL_TEXTURE_2D,0); //Unbind 2D textures
    
    return mTexture;

}


int generateTexture(const char * filename)
{
	GLuint mTexture;
    glGenTextures(1 , &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);// Bind our 2D texture so that following set up will be applied

    //Set texture wrapping parameter
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    //Set texture Filtering parameter
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    //Load the image
    int picWidth,picHeight,n;
    unsigned char* image = stbi_load(filename, &picWidth, &picHeight, &n,STBI_rgb);
    if (image == NULL ) {
        std::cout<<"Failed to load image: "<<stbi_failure_reason()<<std::endl;
    }
    //Generate the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB , picWidth , picHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    
    
    std::cout<<"Image START#####"<<picWidth<<":"<<picHeight<<":"<<picWidth*picHeight*3<<"#####################"<<std::endl;
    for(int i=0;i<picWidth*picHeight*3;i++)std::cout<<(unsigned int)image[i]<<",";
    std::cout<<"Image END##########################"<<std::endl;

    stbi_image_free(image);// Free the reference to the image
    glBindTexture(GL_TEXTURE_2D,0); //Unbind 2D textures
    
    return mTexture;

}



std::vector<std::string> globVector(const std::string& pattern){
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));
    
     int return_value,count=0;
    while((return_value=glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result))!=0){
		 
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if(count++>20){
			std::cout<<"glob_error"<<std::endl;
			exit(0);
		}
	}
    
    
    std::vector<std::string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}


void   setup(){
  
    
    

    
   

	srand(time(NULL));
    std::cout << std::fixed << std::showpoint;
	std::cout.precision(2);
    grammar=new Grammar("./test.grammar");
    
    
    
		grammar->addContext();
	
	
	std::cout<<"Read files..."<<std::endl;
	
	
	std::vector<std::string> files = globVector("./*.png");
	
	std::cout<<"files: "<<files.size()<<std::endl;

	
	
	
		GLuint texid=generateTexture(0);
		texture_list.push_back(texid);
	
	texid=generateTexture(1);
		texture_list.push_back(texid);
		
		 texid=generateTexture(2);
		texture_list.push_back(texid);
	
	
	int counter=0;
    for (int i=0;i<files.size()-3;i++){
		std::cout<<counter;
		counter++;
		texid=generateTexture(files[i].c_str());
		texture_list.push_back(texid);
	
	}
	/*std::vector<std::string> files2 = globVector("./*.jpg");
	
	
	std::cout<<"files: "<<files2.size()<<std::endl;
	
	
    for (int i=0;i<files2.size();i++){
		std::cout<<counter;
		counter++;
		GLuint texid=generateTexture(files2[i].c_str());
		texture_list.push_back(texid);
	
	}*/
	
		for(int i=0;i<texture_list.size();i++){
					grammar->context->loadTexture(texture_list[i]);
			
		}
		
	//back_texture=load_texture("paper.png");	
		std::cout<<"generate primitives"<<std::endl;
		grammar->context->genPrimitives();
		
		grammar->generateGeometry();
		std::cout<<"Finished generating geometry..."<<std::endl;

//grammar->context->getScene().calc_normals();





	glEnable( GL_BLEND );
	
	   
    //	straight alpha
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //	premultiplied alpha (remember to do the same in glColor!!)
    //glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

    //	do I want alpha thresholding?
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.5f );
    glPointSize(4);

  glEnable(GL_LIGHTING);

 /* Enable a single OpenGL light. */
	GLfloat light_ambient[] =
	  {0.1, 0.1, 0.1 ,0.1};
	  GLfloat light_diffuse[] =
	  {0.8, 0.8, 0.8, 0.8};
	  GLfloat light_specular[] =
	  {1.0, 1.0, 1.0, 0.9};


	  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.2f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.08f);
	glEnable(GL_LIGHT0);
	  
	  
	  
  GLfloat ambient1[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
  GLfloat diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
  GLfloat specular1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
  GLfloat light_position1[] = { 1.0f, 4.2f, 0.2f, 0.0f };
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  GLfloat spotExp = 20.0;
  glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spotExp);
  GLfloat spotCutoff = 60.0;
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutoff);
	
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);
	
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	  glDepthFunc(GL_LESS);
	
	 glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	
	  //glEnable(GL_NORMALIZE);
	  //glEnable(GL_RESCALE_NORMAL);
	  glShadeModel(GL_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
  
	

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
//  Display Func
//================================================================================================================





GLuint back_texture = 0;
void background()
{
	glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, back_texture ); 

    glMatrixMode( GL_PROJECTION );
glLoadIdentity();
glMatrixMode( GL_MODELVIEW );
glLoadIdentity();




    glPushMatrix();
glBegin( GL_QUADS );
glTexCoord2f(0,0); glVertex2f(-1,-1);
glTexCoord2f(1,0); glVertex2f( 1,-1);
glTexCoord2f(1,1); glVertex2f( 1, 1);
glTexCoord2f(0,1); glVertex2f(-1, 1);
glEnd();
    glPopMatrix();

   

}



void display( void )//-------------------------------------------------------------------------------------
{  
	
glClearColor(1,1,1,1);

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 glViewport(SCREEN_WIDTH/2,0,SCREEN_WIDTH/2,SCREEN_HEIGHT);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  gluPerspective( 70.0f, 1.0f, 0.1f, 5000.0f );


 float dt=0.0007/2;
float dsx=0,dsy=0,dsz=0;



//dsx=(SCREEN_WIDTH/2.0f)*dt;


//if(button_down){

//	button_down=false;

 if( mouse_x>SCREEN_WIDTH/2 && mouse_x<SCREEN_WIDTH) dsx=(mouse_x-SCREEN_WIDTH*0.75)*dt;

 //dsy=-(mouse_y-SCREEN_HEIGHT/2)*dt;

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









gluLookAt(3.0f*svec[0], 3.0f*svec[1], 3.0f*svec[2], 0.0, 0.0, 0.0,svec5[0],svec5[1],svec5[2]);


light_position[0]=10.0f*svec5[0];
light_position[1]=10.0f*svec5[1];
light_position[2]=10.0f*svec5[2];

  glLightfv(GL_LIGHT1, GL_POSITION, light_position);

 glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

//background();




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





glPushMatrix();

//glScalef(scale,scale,scale);




//glColor4f(1,1,1,0.6);			
//grammar->context->getScene().draw();

glColor4f(1,1,1,1);

grammar->context->draw();


glPopMatrix();







  //SDL_GL_SwapBuffers( );

  
}
//----------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------------


//==================================================================================================================================================

//											 		Main program



//==================================================================================================================================================


#define iWidth 16
#define iHeight 16
#define iDepth 16


static GLubyte image[iDepth][iHeight][iWidth][3];


//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font* gFont = NULL;



//Scene textures
LTexture gPromptTextTexture;
LTexture gtext_inputTexture;
LTexture imagefromfileTexture;
LTexture MousexyTexture;
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	
	if(textureText=="")return false;
	
	//Get rid of preexisting texture
	free();
	
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		//printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}











bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		
		//Create window
		gWindow = SDL_CreateWindow( "ProGen3d 0.2 Beta", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
		 SDL_GLContext Context = SDL_GL_CreateContext(gWindow);
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
		
		
	//	int posx=-1,posy=-1;
	//	SDL_GetWindowPosition(gWindow , &posx,&posy);

		
		//Create window
		//if( !gWindows[ 0 ].init(posx,posy) )
		//{
		//	printf( "Window 1 could not be created!\n" );
		//	success = false;
		//}
		
		
  setup();
	}

	return success;
}





bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont( "CourierPrime-Regular.ttf", 13 );
	if( gFont == NULL )
	{
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Render the prompt
		SDL_Color textColor = { 0, 0, 0, 0xFF };
		if( !gPromptTextTexture.loadFromRenderedText( "Enter Text:", textColor ) )
		{
			printf( "Failed to render prompt text!\n" );
			success = false;
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	gPromptTextTexture.free();
	gtext_inputTexture.free();

	//Free global font
	TTF_CloseFont( gFont );
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	//gWindows[ 0 ].free();
	gWindow = NULL;
	
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}





//cursor I page up / page down Iy screen up/down J screen left/right
int I=0;
int Iy=0;
size_t J=0;

int main1()
{
	
		SDL_Color textColor = { 0, 0, 0, 0xFF };		
			SDL_Color textColor2 = { 0xFF, 0, 0, 0xFF };		
	
  
  //SDL_Window *Window = SDL_CreateWindow("OpenGL Test", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
  //assert(Window);
  //SDL_GLContext Context = SDL_GL_CreateContext(Window);
  
  
  
  
  
	if( !init() )
		{
		printf( "Failed to initialize!\n" );
		}
	
	if( !loadMedia() )
		{
		printf( "Failed to load media!\n" );
		}
  
 
  
  setup();
  
  imagefromfileTexture.loadFromFile("textbackground.jpg");
  SDL_StartTextInput();
  
  
  //Set text color as black
	

		//The current input text.
		
		
		
		std::string text_input,text_input_add;
		
	
	//gtext_inputTexture.loadFromRenderedText( text_input.c_str(), textColor );

			//Enable text input

  	
  bool Running = 1;
  bool FullScreen = 0;
  
  while (Running)
  {
	  
	bool renderText=false;  
	  bool delete_character=false;
	  
	  if(!dialog_open){
	   std::cout<<"GetMouseState"<<std::endl;
	  SDL_GetMouseState( &mouse_x, &mouse_y );
		}
		
		std::string mouseText="J:"+std::to_string(J)+"I:"+std::to_string(I)+"Iy:"+std::to_string(Iy)+"X:"+std::to_string(mouse_x)+" Y:"+std::to_string(mouse_y);
		MousexyTexture.loadFromRenderedText(mouseText,textColor2);
		
		
		
		
		int width_text=0;
				
	std::cout<<"gtk non blocking main iteration"<<std::endl;
	Running=gtk_main_iteration_do(true);		//run gtk main window events in non-blocking  	
				
				
				
			text_input_add="";	
	float x_size=0.0f;
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
		
		
     if (e.type == SDL_MOUSEMOTION){
		

			if (e.motion.state & SDL_BUTTON_LMASK)
			{
				// mouse moved with left button down
			}
		}
		else if(e.type == SDL_MOUSEWHEEL){
        if(e.wheel.y > 0) // scroll up
        {
             // Put code for handling "scroll up" here!
             //scale*=1.0f/1.05f;
        }
        else if(e.wheel.y < 0) // scroll down
        {
             // Put code for handling "scroll down" here!
             //scale*=1.05f;
        }

        if(e.wheel.x > 0) // scroll right
        {
             
        }
        else if(e.wheel.x < 0) // scroll left
        {
             
        }
	}
	else if(e.type==SDL_MOUSEBUTTONDOWN){
		
		if(e.button.button==SDL_BUTTON_LEFT){
			
			if(mouse_x>0 && mouse_x<SCREEN_WIDTH/2 && mouse_y>30 && mouse_y<SCREEN_HEIGHT){//text area
				
				Iy=(mouse_y-50.0f)/MousexyTexture.getHeight();
				x_size=float(mouseText.length())/float(MousexyTexture.getWidth());
				J=(float(mouseText.length())*(float)mouse_x)/float(MousexyTexture.getWidth());
			}
		 else if(mouse_x>0 && mouse_x<40 && mouse_y<25 && mouse_y>0){
			open_dialog();
			dialog_open=true;
			}	
				
				
		}
		
	}
     else if( e.type == SDL_KEYDOWN )
					{
						//Handle backspace
						if( e.key.keysym.sym == SDLK_BACKSPACE && text_input.length() > 0 )
						{
							//lop off character
							
							delete_character=true;
							
							
							renderText = true;
						}
						//Handle copy
						else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
						{
							SDL_SetClipboardText( text_input.c_str() );
						}
						//Handle paste
						else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
						{
							text_input = SDL_GetClipboardText();
							renderText = true;
						}
						else if( e.key.keysym.sym == SDLK_RIGHT){
							J++;
							if(J>grammar->lines[I+Iy].npos)J=grammar->lines[I+Iy].npos;
						}
						else if( e.key.keysym.sym == SDLK_LEFT){
						
						     J--;
						     if(J<0)J=0;
						
						}
						else if( e.key.keysym.sym == SDLK_PAGEDOWN){
							I+=SCREEN_LINES;
							if(I>grammar->lines.size()-SCREEN_LINES)I=grammar->lines.size()-SCREEN_LINES;
						}
						else if( e.key.keysym.sym == SDLK_PAGEUP){
						
						     I-=SCREEN_LINES;
						     if(I<0)I=0;
						
						}
						else if( e.key.keysym.sym == SDLK_DOWN){
							Iy++;
							if(Iy>SCREEN_LINES)Iy=SCREEN_LINES;
						}
						else if( e.key.keysym.sym == SDLK_UP){
						
						     Iy--;
						     if(Iy<0)Iy=0;
						
						}
						else if( e.key.keysym.sym == SDLK_F1){
						
							grammar->addContext();
							for(int i=0;i<texture_list.size();i++){
									grammar->context->loadTexture(texture_list[i]);
			
							}
							grammar->Reread();
								
							grammar->context->genPrimitives();
		
							grammar->generateGeometry();
						}
						else {
						//	text_input+=e.key.toString();
						}
					}
					
	 else if( e.type == SDL_TEXTINPUT ){
						//Not copy or pasting
						if( !( SDL_GetModState() & KMOD_CTRL && ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' || e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) ) )
						{
							
							
							
							
							
							//Append character
							text_input_add = e.text.text;
							J++;
							if(J>grammar->lines[I+Iy].npos)J=grammar->lines[I+Iy].npos;
							renderText = true;
						}
	  } 
      else if (e.type == SDL_QUIT)
      {
        Running = 0;
      }
    }
    
		Running=gtk_main_iteration_do(false);		//run gtk main window events in non-blocking  
		std::cout<<"Finished Polling"<<std::endl;
		if(J<grammar->lines[I+Iy].npos) {
					
					
					try {
						text_input= grammar->lines[I+Iy].substr(0, J);
					}
					catch (const std::out_of_range& e) {
						std::cout << "Out of Range error1.";
						text_input="";
					}
				}
				else {
					
					try {
					text_input= grammar->lines[I+Iy].substr(0, grammar->lines[I+Iy].npos);
					}
					catch (const std::out_of_range& e) {
						std::cout << "Out of Range error2.";
						text_input="";
					}
				}
				
				text_input+=text_input_add;
				
				gtext_inputTexture.loadFromRenderedText( text_input, textColor );
							
				std::string  text_input_trailing="";
				if(J<grammar->lines[I+Iy].npos){
					
					
					try {
					text_input_trailing=grammar->lines[I+Iy].substr(J,grammar->lines[I+Iy].npos );
					}
					catch (const std::out_of_range& e) {
						std::cout << "Out of Range error3.";
						text_input_trailing="";
					}
					
				}

  
  //SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	//			SDL_RenderClear( gRenderer );
	
	  SDL_Rect viewport;
  viewport.x=SCREEN_WIDTH/2;
  viewport.y=0;
  viewport.w=SCREEN_WIDTH/2;
  viewport.h=SCREEN_HEIGHT;
  SDL_RenderSetViewport(gRenderer, &viewport);
  glEnable(GL_COLOR_MATERIAL);
	
		glEnable(GL_LIGHTING);
		//glEnable(GL_NORMALIZE);
		//glEnable(GL_RESCALE_NORMAL);
		glEnable( GL_BLEND );
		//glEnable(GL_LIGHT1);
		//glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glEnable( GL_ALPHA_TEST );	
		  glEnable(GL_LIGHTING);

 /* Enable a single OpenGL light. */
	GLfloat light_ambient[] =
	  {0.1, 0.1, 0.1 ,0.1};
	  GLfloat light_diffuse[] =
	  {0.8, 0.8, 0.8, 0.8};
	  GLfloat light_specular[] =
	  {1.0, 1.0, 1.0, 0.9};


	  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.2f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.08f);
	glEnable(GL_LIGHT0);
	  GLfloat ambient1[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
  GLfloat diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
  GLfloat specular1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
  GLfloat light_position1[] = { 1.0f, 4.2f, 0.2f, 0.0f };
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  GLfloat spotExp = 20.0;
  glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spotExp);
  GLfloat spotCutoff = 60.0;
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutoff);
	
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);
	
	glEnable(GL_LIGHT1);
					glEnable(GL_TEXTURE_2D);
					
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					
					display();
					
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		//glDisable(GL_NORMALIZE);
		//glDisable(GL_RESCALE_NORMAL);
		glDisable( GL_BLEND );
		//glDisable(GL_LIGHT1);
		//glDisable(GL_LIGHT0);
		glDisable(GL_DEPTH_TEST);
		glDisable( GL_ALPHA_TEST );				
		glDisable(GL_COLOR_MATERIAL);
  
  //Render text textures

  viewport.x=0;
  viewport.y=0;
  viewport.w=SCREEN_WIDTH/2;
  viewport.h=SCREEN_HEIGHT;
  SDL_RenderSetViewport(gRenderer, &viewport);
  glViewport(0,0,SCREEN_WIDTH/2,SCREEN_HEIGHT);
  
				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				
					//imagefromfileTexture.setAlpha(0.5);
					imagefromfileTexture.render(0,0);
					MousexyTexture.render(SCREEN_WIDTH/2-250,0);
					
					int OFFSET =40;
				
				  
				//Render text Up to cursor position
				
				//gtext_inputTexture.setAlpha(0.5);
				gtext_inputTexture.setBlendMode(SDL_BLENDMODE_MOD);
				char cursor_char;
				bool add_char=false;
				if(text_input!=""){
				
					cursor_char=text_input.back();
					add_char=true;
					text_input.pop_back();
					if( !gtext_inputTexture.loadFromRenderedText( text_input, textColor )){
						
						}
					else {
					width_text=gtext_inputTexture.getWidth();
					if(text_input!="")gtext_inputTexture.render( 10 , OFFSET +(Iy+1)*gtext_inputTexture.getHeight() );
					}
				
				}
				
					
					int width_text2=0;
					//render cursor
					if( !gtext_inputTexture.loadFromRenderedText( std::string(1,cursor_char), textColor2 )){
						
					}
					else {
						width_text2=gtext_inputTexture.getWidth();
						gtext_inputTexture.render( 10+width_text , OFFSET +(Iy+1)*gtext_inputTexture.getHeight() );
					}
					
				if(add_char)text_input+=cursor_char;
    
				//gPromptTextTexture.render( ( SCREEN_WIDTH/2 - gPromptTextTexture.getWidth() ) / 2, 0 );
				//gtext_inputTexture.render( ( SCREEN_WIDTH/2 - gtext_inputTexture.getWidth() ) / 2, gPromptTextTexture.getHeight() );
				int I_max=I+SCREEN_LINES;
				
				
				if(delete_character){
					text_input.pop_back();
						J--;
							if(J<0)J=0;
					delete_character=false;
				}
				
				grammar->lines[I+Iy]=text_input+text_input_trailing;//update changes to lines[I+Iy] current line ****************************
				
				
				
				
				//render trailing from cursor current line
				if(text_input!=""){
					if( !gtext_inputTexture.loadFromRenderedText( text_input_trailing, textColor )){
					
					}
					else{
					
					gtext_inputTexture.render( 10+width_text+width_text2 , OFFSET +(Iy+1)*gtext_inputTexture.getHeight() );
					}
				}
				
				
				
				if(I_max>grammar->lines.size())I_max=grammar->lines.size();//limit I up/down lines
				
				
				float max_length=0.0f;
				//draw lines
				for(int i=I+1;i<I_max;i++){
					if(float(grammar->lines[i].npos)>max_length)max_length=grammar->lines[i].npos;
					if(grammar->lines[i]!="" && i!=I+Iy){
						gtext_inputTexture.loadFromRenderedText( grammar->lines[i], textColor );
						gtext_inputTexture.render( 10 , OFFSET +gtext_inputTexture.getHeight()*(i+1-I) );
					}
				}
				
				
				
				
				
				
				glMatrixMode(GL_TEXTURE);
				
				glPopMatrix();
				
				
				
				
				
				
				

				//Update screen
				SDL_RenderPresent( gRenderer );
					
 
			
  
 
  
  

  
   std::cout<<"Swap GL window buffer"<<std::endl;
   SDL_GL_SwapWindow(gWindow);
 
  }
  
  
  std::cout<<"Cancelled"<<std::endl;
  		
  		
  		close();
  		
  		
  return 0;


}


using glm::mat4;
using glm::vec3;
using glm::lookAt;
using glm::perspective;
using glm::rotate;
using glm::translate;
using glm::scale;

const GLchar  *VERTEX_SOURCE =
"#version 330\n"
"in vec3 position;\n"
"in vec3 normal;\n"
"in vec2 texture;\n"
"out vec3 fN;\n"
"out vec3 fV;\n"
"out vec3 fL;\n"
"out vec2 tex_coord;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"uniform vec3 lightposition;\n"
"void main(){\n"
"    tex_coord = texture ;\n"
"    fN = (model*view*vec4(normal,1.0)).xyz ;\n"
"    fV = - (model*view*vec4(position, 1.0)).xyz;\n"
"    fL = lightposition.xyz - (model*view*vec4(position, 1.0)).xyz ;\n"
"    gl_Position =  projection * view * model * vec4(position, 1.0);\n"
"}\n";






const GLchar *FRAGMENT_SOURCE2 =
"#version 330\n"
"in vec3 fN ;\n"
"in vec3 fL ;\n"
"in vec3 fV ;\n"
"in tex_coord;\n"
"uniform vec4 ambientproduct, diffuseproduct, specularproduct ;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform vec3 lightposition ;\n"
"uniform float shinyness ;\n"
"uniform sampler2D ourTexture;\n"
"void main(){vec3 N = normalize(fN) ;\n"
"vec3 V = normalize(fV) ;\n"
"vec3 L = normalize(fL) ;\n"
"vec3 H = normalize( L + V ) ;\n"
"vec4 ambient = ambientproduct ;\n"
"float Kd = max(dot(L, N), 0.0) ;\n"
"vec4 diffuse = Kd*diffuseproduct ;\n"
"float Ks = pow(max(dot(N, H), 0.0), shinyness) ;\n"
"vec4 specular = Ks*specularproduct ;\n"
"if( dot(L, N) < 0.0 )specular = vec4(0.0, 0.0, 0.0, 1.0) ;\n"
"gl_FragColor = texture(texture1, tex_coord)*(ambient + diffuse + specular) ;\n"
"gl_FragColor.a = 1.0;}\n"; 


/* the GtkGLArea widget */
//static GtkWidget *gl_area = NULL;

/* The object we are drawing */
static const GLfloat vertex_data[] = {
  1.0, -1.0, -1.0, 0.0, -1.0, 0.0, 1.0,-1.0,
  1.0, -1.0, 1.0, 0.0, -1.0, 0.0,  1.0, 1.0,
  -1.0, -1.0, 1.0, 0.0, -1.0, 0.0,-1.0, 1.0,
  1.0, -1.0, -1.0, 0.0, -1.0, 0.0, 1.0,-1.0,
  -1.0, -1.0, 1.0, 0.0, -1.0, 0.0,-1.0, 1.0, 
  -1.0, -1.0,-1.0, 0.0, -1.0, 0.0,-1.0,-1.0,

  -1.0, 1.0, 1.0, 0.0, 1.0, 0.0, -1.0,1.0,
  1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0,1.0,
  1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 1.0,-1.0,
  -1.0, 1.0, 1.0, 0.0, 1.0, 0.0, -1.0,1.0,
  1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 1.0,-1.0,
  -1.0, 1.0,-1.0, 0.0, 1.0, 0.0, -1.0,-1.0,

  -1.0, -1.0,-1.0, -1.0, 0.0, 0.0, -1.0,-1.0,
  -1.0, -1.0, 1.0, -1.0, 0.0, 0.0, -1.0,1.0,
  -1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0,-1.0,
  -1.0, -1.0, 1.0, -1.0, 0.0, 0.0, -1.0,1.0,
  -1.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0,1.0,
  -1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0,-1.0,

  -1.0, -1.0,1.0, 0.0, 0.0, 1.0, -1.0,-1.0,
  1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 1.0,-1.0,
  -1.0, 1.0, 1.0, 0.0, 0.0, 1.0, -1.0,1.0,
  1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 1.0,-1.0,
  1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0,1.0,
  -1.0, 1.0, 1.0, 0.0, 0.0, 1.0, -1.0,1.0,

  1.0, 1.0, -1.0, 0.0, 0.0, -1.0,   1.0,1.0,
  1.0,-1.0, -1.0, 0.0, 0.0, -1.0,  1.0,-1.0,
  -1.0, -1.0, -1.0, 0.0, 0.0, -1.0, -1.0,-1.0,
  1.0, 1.0, -1.0, 0.0, 0.0, -1.0,   1.0,1.0,
  -1.0, -1.0, -1.0, 0.0, 0.0, -1.0, -1.0,-1.0,
  -1.0, 1.0, -1.0, 0.0, 0.0, -1.0,  -1.0,1.0,

  1.0, 1.0, 1.0,1.0, 0.0, 0.0,  1.0,1.0,
  1.0, -1.0, 1.0,1.0, 0.0, 0.0, -1.0,1.0,
  1.0, -1.0,-1.0,1.0, 0.0, 0.0, -1.0,-1.0,
  1.0, 1.0, 1.0,1.0, 0.0, 0.0,  1.0,1.0,
  1.0, -1.0, -1.0,1.0, 0.0, 0.0, -1.0,-1.0,
  1.0, 1.0, -1.0,1.0, 0.0, 0.0, 1.0,-1.0

};

long current_frame = 0.0;
long delta_time = 0.0;
GDateTime *last_frame;
int dt = 0;

static GLuint position_buffer;
static GLuint program;
static GLuint vao;

mat4 model = mat4(1.0);


/* Create and compile a shader */
static GLuint
create_shader (int  type)
{
  GLuint shader;
  int status;
  shader = glCreateShader (type);
  if (type== GL_FRAGMENT_SHADER){
    glShaderSource (shader, 1, &FRAGMENT_SOURCE2, NULL);
  }
  if (type== GL_VERTEX_SHADER){
	  std::cout<<VERTEX_SOURCE<<std::endl;
    glShaderSource (shader, 1, &VERTEX_SOURCE, NULL);
  }
  glCompileShader (shader);

  glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    int log_len;
    char *buffer;
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_len);
    buffer = (char*)g_malloc (log_len + 1);
    glGetShaderInfoLog (shader, log_len, NULL, buffer);
    g_warning ("Compile failure in %s shader:\n%s",
               type == GL_VERTEX_SHADER ? "vertex" : "fragment",
               buffer);
    g_free (buffer);
    glDeleteShader (shader);
    return 0;
  }

  return shader;
}

static void
realize (GtkWidget *widget)
{
  GdkGLContext *context;
  gtk_gl_area_make_current (GTK_GL_AREA (widget));
  if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
    return;
  context = gtk_gl_area_get_context (GTK_GL_AREA (widget));
   
   
   
   /* We only use one VAO, so we always keep it bound */
  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  /* This is the buffer that holds the vertices */
  glGenBuffers (1, &position_buffer);
  glBindBuffer (GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_data),vertex_data,GL_STATIC_DRAW);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray (0);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray (1);
   // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glBindBuffer (GL_ARRAY_BUFFER, 0);

  GLuint vertex, fragment;
  int status;
  vertex = create_shader (GL_VERTEX_SHADER);

  if (vertex == 0)
    {
      return;
    }

  fragment = create_shader (GL_FRAGMENT_SHADER);

  if (fragment == 0)
    {
      glDeleteShader (vertex);
      return;
    }

  program = glCreateProgram ();
  glAttachShader (program, vertex);
  glAttachShader (program, fragment);

  glLinkProgram (program);

  glGetProgramiv (program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    int log_len;
    char *buffer;

    glGetProgramiv (program, GL_INFO_LOG_LENGTH, &log_len);

    buffer = (char*)g_malloc (log_len + 1);
    glGetProgramInfoLog (program, log_len, NULL, buffer);

    g_warning ("Linking failure:\n%s", buffer);

    g_free (buffer);

    glDeleteProgram (program);
    program = 0;

    glDeleteShader (vertex);
    glDeleteShader (fragment);

    return;
  }

  glDetachShader (program, vertex);
  glDetachShader (program, fragment);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);  
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  last_frame = g_date_time_new_now_local();  
current_frame = g_date_time_get_microsecond(last_frame);
  
}
/* We should tear down the state when unrealizing */
static void
unrealize (GtkWidget *widget)
{
  gtk_gl_area_make_current (GTK_GL_AREA (widget));

  if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
    return;

  glDeleteBuffers (1, &position_buffer);
  glDeleteProgram (program);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//                      CUBE


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float angle_cube=0.0f;
static void
draw_box (long delta_time,vec3 scale_vec,vec3 position_vec,int tex_index)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_list[tex_index]);
  /* Use our shaders */
  glUseProgram (program);
  glUniform1i(glGetUniformLocation(program, "texture1"), 0);
  angle_cube+=(float)delta_time/1000;

  if(angle_cube>360.0)angle_cube=0.0f;
  model = glm::mat4(1.0);
  model = rotate(model, angle_cube, vec3(1,1,0));
  model = translate(model,position_vec);
  model = scale(model,scale_vec);
  
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
  
  vec3 position = vec3(0,0,5);
  vec3 front = vec3(0,0,-1);
  vec3 up = vec3(0,1,0);
  mat4 view = lookAt(position, position + front, up);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
  
  mat4 projection = perspective(45.0, double(SCREEN_WIDTH)/double(SCREEN_HEIGHT), 0.1, 100.0);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);
  
  vec3 lightposition=vec3(1.1,5.7,-1.3);
  glUniform3fv(glGetUniformLocation(program,"lightposition"),1,&lightposition[0]);
  
  
  glm::vec4 ambientproduct=glm::vec4(0.2,0.2,0.2,1.0);
  glUniform4fv(glGetUniformLocation(program,"ambientproduct"),1,&ambientproduct[0]);
  
  glm::vec4 diffuseproduct=glm::vec4(0.5,0.5,0.5,1.0);
  glUniform4fv(glGetUniformLocation(program,"diffuseproduct"),1,&diffuseproduct[0]);
  
  glm::vec4 specularproduct=glm::vec4(0.8,0.8,0.8,1.0);
  glUniform4fv(glGetUniformLocation(program,"specularproduct"),1,&specularproduct[0]);
  float shinyness=128.0f;
  glUniform1f(glGetUniformLocation(program,"shinyness"),shinyness);
  
  
  glBindVertexArray(vao);
  /* Use the vertices in our buffer */

  /* Draw the three vertices as a triangle */
  glDrawArrays (GL_TRIANGLES, 0, 36);

  /* We finished using the buffers and program */
  glBindVertexArray(0);
  glDisableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, 0);
  glUseProgram (0);
}



static void
on_axis_value_change (void)
{
  gtk_widget_queue_draw (gl_area);
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//              RENDER


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static gboolean
render (GtkGLArea    *area,
        GdkGLContext *context)
{
gdk_gl_context_make_current (context);
GDateTime *date_time;



  date_time = g_date_time_new_now_local();  
  current_frame = g_date_time_get_microsecond(date_time);
  delta_time = g_date_time_difference(date_time, last_frame) / 1000;
  last_frame = date_time;

  if (gtk_gl_area_get_error (area) != NULL)
    return FALSE;

  /* Clear the viewport */
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* Draw our object */
  draw_box (delta_time,vec3(0.5,1.0,0.5),vec3(0.0,0.0,0.0),0);
  draw_box (delta_time,vec3(1.0,0.1,1.0),vec3(0.0,0.0,0.6),1);

//display();
glFlush ();
  gtk_gl_area_queue_render (area);
return TRUE;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//             MAIN



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////








int main( int argc, char* argv[] ){
	GtkWidget *window, *box;
  /* initialize gtk */
  gtk_init(&argc, &argv);
  /* Create new top level window. */
  window = gtk_window_new( GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW(window),SCREEN_WIDTH,SCREEN_HEIGHT);
  gtk_window_set_title(GTK_WINDOW(window), "GL Area");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, FALSE);
  g_object_set (box, "margin", 12, NULL);
  gtk_box_set_spacing (GTK_BOX (box), 6);
  gtk_container_add (GTK_CONTAINER (window), box);
  gl_area = gtk_gl_area_new ();
  gtk_box_pack_start (GTK_BOX(box), gl_area,1,1, 0);
  /* We need to initialize and free GL resources, so we use
  * the realize and unrealize signals on the widget
  */
  g_signal_connect (gl_area, "realize", G_CALLBACK (realize), NULL);
  g_signal_connect (gl_area, "unrealize", G_CALLBACK (unrealize), NULL);

  /* The main "draw" call for GtkGLArea */
  g_signal_connect (gl_area, "render", G_CALLBACK (render), NULL);
  /* Quit form main if got delete event */
  g_signal_connect(G_OBJECT(window), "delete-event",
                 G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_main();
   
   return 0;
	
}

































