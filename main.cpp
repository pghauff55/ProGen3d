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
#include <random>


#include <stdio.h>
#include <bits/stdc++.h>
#include <termios.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>



#include <epoxy/gl.h>
#include <GL/gl.h>
#include <GL/glu.h>







#include <gtk/gtk.h>


//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "PLYWriter.h"
#include "Context.h"
#include "Scope.h"
#include "grammar.h"
#include "MathParser.h"
#include "LBAssembler.h"




#define STB_IMAGE_IMPLEMENTATION
#include "stbimage/stb_image.h"

//#include "hex_planet.h"
#include "textures.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                                                   char appname[]="ProGen3d 0.4c";                //////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace cv;


std::mt19937_64 rng;





//===========================================================================================================================

//												 Globals 

//===========================================================================================================================

Grammar *grammar;

static int SCREEN_WIDTH=1600;
static int SCREEN_HEIGHT=1200;

std::string grammar_filename("./test.grammar");

extern std::vector<Variable *> variable_list;
extern std::vector<Variable *> full_variable_list;


static GtkWidget *window;
static GtkWidget *notebook,*layout,*layout2,*layout3,*layout4;
static GtkWidget *gl_area;
static GtkTextBuffer *mybuffer;
static GtkWidget *view;
static GtkWidget *file_label;
static GtkWidget *scale_slider3;

char const * filterPatterns[1] = { "*.grammar"  };

float angle_view=0.0f;
float scale_global=0.3f;

std::vector<int> texture_list;
std::vector<std::string> texture_filenames;


bool play=true;

void display( void );

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
"uniform vec3 pos;\n"
"uniform vec3 scale_vec;\n"
"void main(){\n"
"       tex_coord.x=texture.x;	\n"
"       tex_coord.y=texture.y;	\n"
"    fN = (model*view*vec4(normal,1.0)).xyz ;\n"
"    fV = - (model*view*vec4(position+pos, 1.0)).xyz;\n"
"    fL = lightposition.xyz - (model*view*vec4(position+pos, 1.0)).xyz ;\n"
"    gl_Position = projection * view *(   (model * vec4(position+pos, 1.0)) );\n"
"}\n";






const GLchar *FRAGMENT_SOURCE2 =
"#version 330\n"
"in vec3 fN ;\n"
"in vec3 fL ;\n"
"in vec3 fV ;\n"
"in vec2 tex_coord;\n"
"uniform vec4 ambientproduct, diffuseproduct, specularproduct ;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform vec3 lightposition ;\n"
"uniform float shinyness ;\n"
"uniform sampler2D texture1;\n"
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
"}\n"; 


/* the GtkGLArea widget */


/* The object we are drawing */


long current_frame = 0.0;
long delta_time = 0.0;
GDateTime *last_frame;
int dt = 0;

static GLuint position_buffer,position_buffer1[50];
static GLuint program;
static GLuint vao,vao1[50];
int tex_count[50];





mat4 model = mat4(1.0);

static const GLfloat vertex_data[] = {
  
  //Y
  
  1.0, -1.0, -1.0, 0.0, -1.0, 0.0, 1.0,-1.0,
    1.0, -1.0, 1.0, 0.0, -1.0, 0.0,  1.0, 1.0,
  -1.0, -1.0, 1.0, 0.0, -1.0, 0.0,-1.0, 1.0,
  
  
    -1.0, -1.0, 1.0, 0.0, -1.0, 0.0,-1.0, 1.0, 
  -1.0, -1.0,-1.0, 0.0, -1.0, 0.0,-1.0,-1.0,
  1.0, -1.0, -1.0, 0.0, -1.0, 0.0, 1.0,-1.0,
  

  -1.0, 1.0, 1.0, 0.0, 1.0, 0.0, -1.0,1.0,
   1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0,1.0,
   1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 1.0,-1.0,
   
1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 1.0,-1.0,
  -1.0, 1.0,-1.0, 0.0, 1.0, 0.0, -1.0,-1.0,
-1.0, 1.0, 1.0, 0.0, 1.0, 0.0, -1.0,1.0,  





 //X 
  -1.0, 1.0, -1.0, -1.0, 0.0, 0.0, -1.0,1.0,
  -1.0, -1.0,-1.0, -1.0, 0.0, 0.0, -1.0,-1.0,
  -1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 1.0,-1.0,//cw
  
  
-1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 1.0,-1.0,
  -1.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0,1.0,
-1.0, 1.0, -1.0, -1.0, 0.0, 0.0, -1.0,1.0,//cw
  
  1.0, -1.0, 1.0,1.0, 0.0, 0.0, 1.0,-1.0,
  1.0, -1.0,-1.0,1.0, 0.0, 0.0, -1.0,-1.0,
  1.0, 1.0, -1.0,1.0, 0.0, 0.0,  -1.0,1.0,//ccw
  
      
  1.0, 1.0,-1.0 ,1.0, 0.0, 0.0, -1.0,1.0,
  1.0,  1.0, 1.0 ,1.0, 0.0, 0.0, 1.0,1.0,//ccw
 1.0, -1.0, 1.0 ,1.0, 0.0, 0.0,1.0,-1.0,

//Z
  
  
  
  -1.0, 1.0, 1.0, 0.0, 0.0, 1.0, -1.0,1.0,
  -1.0, -1.0,1.0, 0.0, 0.0, 1.0, -1.0,-1.0,
  1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 1.0,-1.0,

  
  
  1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 1.0,-1.0,
  1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0,1.0,
  -1.0, 1.0, 1.0, 0.0, 0.0, 1.0, -1.0,1.0,
  
  
  
  -1.0, 1.0, -1.0, 0.0, 0.0, -1.0, -1.0, 1.0,
  1.0,  1.0, -1.0, 0.0, 0.0, -1.0,  1.0, 1.0,
  1.0, -1.0, -1.0, 0.0, 0.0, -1.0,  1.0,-1.0,
  
  
  1.0, -1.0, -1.0, 0.0, 0.0, -1.0,   1.0,-1.0,
  -1.0, -1.0, -1.0, 0.0, 0.0, -1.0, -1.0,-1.0,
  -1.0, 1.0, -1.0, 0.0, 0.0, -1.0,  -1.0,1.0
};

int generateTexture(int index)
{
	/*GLuint mTexture;
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
*/
return index;
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
    unsigned char* image = stbi_load(filename, &picWidth, &picHeight, &n,STBI_rgb_alpha);
    if (image == NULL ) {
        std::cout<<"Failed to load image: "<<stbi_failure_reason()<<std::endl;
    }
    //Generate the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , picWidth , picHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    //texture_data_list.push_back(image);
    
   // std::cout<<"Image START#####"<<picWidth<<":"<<picHeight<<":"<<picWidth*picHeight*3<<"#####################"<<std::endl;
   // for(int i=0;i<picWidth*picHeight*3;i++)std::cout<<(unsigned int)image[i]<<",";
   // std::cout<<"Image END##########################"<<std::endl;

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
    //std::cout << std::fixed << std::showpoint;
	std::cout.precision(2);
	
    grammar=new Grammar(grammar_filename.c_str());
    
    
    
		grammar->addContext();
	
	
	std::cout<<"Read files..."<<std::endl;
	
	
	std::vector<std::string> files = globVector("./*.png");
	texture_filenames=files;
	std::cout<<"files: "<<files.size()<<std::endl;

	
	
	
	int texid;
	
	int counter=0;
    for (int i=0;i<files.size();i++){
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


gtk_label_set_text((GtkLabel *)file_label,(grammar_filename+"    tokens: "+std::to_string(grammar->tokens_new.size())).c_str());


//grammar->context->getScene().calc_normals();
 float mydata[36*8];
    for(int i=0;i<36*8;i++){
		mydata[i]=vertex_data[i]; 
	}
	
	
	for(int i=0;i<36;i++){
		for(int j=0;j<8;j++){
			if(j==0 || j==2)mydata[i*8+j]*=0.5f;
			if(j==1){
				if(mydata[i*8+j]<0)mydata[i*8+j]=0.0f;
			}
			if(j==6 && mydata[i*8+j]<0)mydata[i*8+j]=0.0f;
			if(j==7 && mydata[i*8+j]<0)mydata[i*8+j]=0.0f;
		}
	}
	for(int i=0;i<texture_list.size();i++){
		
		float *my_vertex_data=grammar->context->calc(mydata,i);
		//tex_count[i]=texcount;
		
		  glGenVertexArrays (1, &vao1[i]);
		  glBindVertexArray (vao1[i]);

		  /* This is the buffer that holds the vertices */
		  glGenBuffers (1, &position_buffer1[i]);
		  glBindBuffer (GL_ARRAY_BUFFER, position_buffer1[i]);
		  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*36*8*tex_count[i],my_vertex_data,GL_DYNAMIC_DRAW);
		  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		  glEnableVertexAttribArray (0);
		  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		  glEnableVertexAttribArray (1);
		   // texture coord attribute
		  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		  glEnableVertexAttribArray(2);
		  glBindBuffer (GL_ARRAY_BUFFER, 0);
		  delete my_vertex_data;
	  

	}



}
  
  
  


















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
  //GdkGLContext *context;
  gtk_gl_area_make_current (GTK_GL_AREA (widget));
  if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
    return;
  //context = gtk_gl_area_get_context (GTK_GL_AREA (widget));
   gtk_gl_area_attach_buffers(GTK_GL_AREA (widget));
  gtk_gl_area_set_auto_render (GTK_GL_AREA (widget),TRUE);
  setup();
     
      std::string text="";
  for(int i=0;i<grammar->lines.size();i++)text+=grammar->lines[i]+"\n";
  

  gtk_text_buffer_set_text (mybuffer,text.c_str(),text.length());
     
    float mydata[36*8];
    for(int i=0;i<36*8;i++){
		mydata[i]=vertex_data[i]; 
	}
	
	
	for(int i=0;i<36;i++){
		for(int j=0;j<8;j++){
			if(j==0 || j==2)mydata[i*8+j]*=0.5f;
			if(j==1){
				if(mydata[i*8+j]<0)mydata[i*8+j]=0.0f;
			}
		}
	}
   
   /* We only use one VAO, so we always keep it bound */
  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  /* This is the buffer that holds the vertices */
  glGenBuffers (1, &position_buffer);
  glBindBuffer (GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*36*8,mydata,GL_STATIC_DRAW);
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


  glEnable(GL_DEPTH_TEST);

glDepthFunc(GL_LESS);  
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   




  
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
	for(int i=0;i<texture_list.size();i++){
		glDeleteBuffers (1, &position_buffer1[i]);
	}
  glDeleteBuffers (1, &position_buffer);
  glDeleteProgram (program);
}

int GLAREA_WIDTH;
int GLAREA_HEIGHT;


void
 resize_glarea (GtkGLArea *area,
               int        width,
               int        height,
               gpointer   user_data){
				   
				   GLAREA_WIDTH=width;
				GLAREA_HEIGHT=height;
				   
				   
			   }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//                      CUBE


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_box(float angle_cube,vec3 scale_vec,vec3 position_vec,vec3 pos,int tex_index,float texscale)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_list[tex_index]);
  /* Use our shaders */
  glUseProgram (program);
  glUniform1i(glGetUniformLocation(program, "texture1"), 0);
 
 
 
 position_vec=vec3(position_vec.x*scale_global,position_vec.y*scale_global,position_vec.z*scale_global);
 
  model = glm::mat4(1.0);
  model = rotate(model, angle_cube, vec3(0,1,0));
  model = translate(model,position_vec);
  vec3 scale_vec2=vec3(scale_vec.x*scale_global,scale_vec.y*scale_global,scale_vec.z*scale_global);
  model = scale(model,scale_vec2);
  
  
  scale_vec=vec3(scale_vec.x*texscale,scale_vec.y*texscale,scale_vec.z*texscale);
  
  
  //vec3 pos(0,0,0);
  
  //model2 = rotate(model2, angle_view, vec3(0,1,0));
  //pos2=model2*pos2;
  //pos=vec3(pos2.x,pos2.y,pos2.z);
  
  
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
  
  
 
  
  vec3 position = vec3(5.0*cos(angle_view*M_PI/180.0),0,5.0*sin(angle_view*M_PI/180.0));
  
  vec3 up = vec3(0,-1,0);
  mat4 view = lookAt(position, vec3(0.0,0.0,0.0), up);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
  
  mat4 projection = perspective(43.0, double(GLAREA_WIDTH)/double(GLAREA_HEIGHT), 0.01, 100.0);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);
  
  vec3 lightposition=vec3(1.1,5.7,-1.3);
  glUniform3fv(glGetUniformLocation(program,"lightposition"),1,&lightposition[0]);
  
  //pos=position_vec;
  glUniform3fv(glGetUniformLocation(program,"pos"),1,&pos[0]);
  glUniform3fv(glGetUniformLocation(program,"scale_vec"),1,&scale_vec[0]);  
  
  glm::vec4 ambientproduct=glm::vec4(0.7,0.7,0.7,1.0);
  glUniform4fv(glGetUniformLocation(program,"ambientproduct"),1,&ambientproduct[0]);
  
  glm::vec4 diffuseproduct=glm::vec4(0.5,0.5,0.5,1.0);
  glUniform4fv(glGetUniformLocation(program,"diffuseproduct"),1,&diffuseproduct[0]);
  
  glm::vec4 specularproduct=glm::vec4(0.3,0.3,0.3,1.0);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//                      BUFFER


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_buffer(float angle_cube,vec3 scale_vec,vec3 position_vec,vec3 pos,int tex_index,float texscale)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_list[tex_index]);
  /* Use our shaders */
  glUseProgram (program);
  glUniform1i(glGetUniformLocation(program, "texture1"), 0);
 
 
 
 position_vec=vec3(position_vec.x*scale_global,position_vec.y*scale_global,position_vec.z*scale_global);
 
  model = glm::mat4(1.0);
  model = rotate(model, angle_cube, vec3(0,1,0));
  model = translate(model,position_vec);
  vec3 scale_vec2=vec3(scale_vec.x*scale_global,scale_vec.y*scale_global,scale_vec.z*scale_global);
  model = scale(model,scale_vec2);
  
  
  scale_vec=vec3(scale_vec.x*texscale,scale_vec.y*texscale,scale_vec.z*texscale);
  
  
  //vec3 pos(0,0,0);
  
  //model2 = rotate(model2, angle_view, vec3(0,1,0));
  //pos2=model2*pos2;
  //pos=vec3(pos2.x,pos2.y,pos2.z);
  
  
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
  
  
 
  
  vec3 position = vec3(5.0*cos(angle_view*M_PI/180.0),0,5.0*sin(angle_view*M_PI/180.0));
  
  vec3 up = vec3(0,-1,0);
  mat4 view = lookAt(position, vec3(0.0,0.0,0.0), up);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
  
  mat4 projection = perspective(43.0, double(GLAREA_WIDTH)/double(GLAREA_HEIGHT), 0.01, 100.0);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);
  
  vec3 lightposition=vec3(1.1,5.7,-1.3);
  glUniform3fv(glGetUniformLocation(program,"lightposition"),1,&lightposition[0]);
  
  //pos=position_vec;
  glUniform3fv(glGetUniformLocation(program,"pos"),1,&pos[0]);
  glUniform3fv(glGetUniformLocation(program,"scale_vec"),1,&scale_vec[0]);  
  
  glm::vec4 ambientproduct=glm::vec4(0.7,0.7,0.7,1.0);
  glUniform4fv(glGetUniformLocation(program,"ambientproduct"),1,&ambientproduct[0]);
  
  glm::vec4 diffuseproduct=glm::vec4(0.5,0.5,0.5,1.0);
  glUniform4fv(glGetUniformLocation(program,"diffuseproduct"),1,&diffuseproduct[0]);
  
  glm::vec4 specularproduct=glm::vec4(0.3,0.3,0.3,1.0);
  glUniform4fv(glGetUniformLocation(program,"specularproduct"),1,&specularproduct[0]);
  float shinyness=128.0f;
  glUniform1f(glGetUniformLocation(program,"shinyness"),shinyness);
  
  
  glBindVertexArray(vao1[tex_index]);
  /* Use the vertices in our buffer */

  /* Draw the three vertices as a triangle */
  glDrawArrays (GL_TRIANGLES, 0, 36*tex_count[tex_index]);

  /* We finished using the buffers and program */
  glBindVertexArray(0);
  glDisableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, 0);
  glUseProgram (0);
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//              RENDER


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
gboolean
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
 if(play)angle_view+=(float)delta_time/1000.0;
  if(angle_view>360.0)angle_view=0.0;
  
  
  /* Clear the viewport */
  glClearColor (1.0, 1.0, 1.0, 1.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* Draw our object */
  //draw_box (angle_view,vec3(0.5,1.0,0.5),vec3(0.0,0.0,0.0),0);
  //draw_box (delta_time,vec3(1.0,0.1,1.0),vec3(0.0,0.0,0.6),1);
  
  //grammar->context->draw();
  for(int i=0;i<texture_list.size();i++){
		if(tex_count[i]>0)draw_buffer(0,vec3(1,1,1),vec3(0,0,0),vec3(0,0,0),i,1);
	}
  
//display();
//glFlush ();
  gtk_gl_area_queue_render (area);
return TRUE;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//             MAIN



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GtkWidget *labels[2000];
GtkWidget *ruleslabel[200];
bool setup_textures=true;
bool setup_vars=true;
bool setup_rules=true;
GtkWidget *frames[40];
GtkBox *token_box[40];
GtkComboBoxText *combo1[40],*combo2[40],*combo3[40];
GtkWidget *token_label[40];
 GtkActionBar *actionbar2;
char *rulenames[200];
GtkWidget *view2;

GtkTextBuffer *buffer2;
GtkWidget *images[50];
int frame_counter=0;
bool init_buffer=false;
GtkAdjustment * vadj;

  GtkWidget *rulescombo;

GtkFileChooserNative *native;
GtkFileChooser *chooser;
void generate_widgets();
void activate_add(GtkWidget *item) ;




std::vector<std::string> breakup_into_lines(std::string input,std::string delimiter){
		std::vector<std::string> output;
	
		int pos=-1;
		
		while( (pos = input.find(delimiter))!=-1){
	
			std::string str=input.substr(0, pos);
			
			input.erase(0, pos + delimiter.length());
		
		    output.push_back(str);
		}
		output.push_back(input);
		
		
		
		return output;
}

  void save_to_file(const char *filename){
   std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "E: Could not open file for saving" << filename << std::endl;
        return;
    }
    
    for(int i=0;i<grammar->lines.size();i++){
		fout<<grammar->lines[i]<<std::endl;
	}	
fout.close();
}








////////////////////////////////////////////////////////////////////////////////////////////////////////////////





void activate(GtkButton *item) {

 for(int i=0;i<full_variable_list.size();i++){
	gtk_container_remove((GtkContainer *)layout3,labels[i]);
	}

 

	GtkTextIter start, end;
		gtk_text_buffer_get_start_iter(mybuffer, &start);
	gtk_text_buffer_get_end_iter(mybuffer, &end);
	 grammar->lines=breakup_into_lines((std::string)gtk_text_buffer_get_text(mybuffer, &start, &end, FALSE),"\n");
 
 
      grammar->Reread();    
    
    
		grammar->addContext();

 
	
	

setup_vars=true;
//setup_rules=true;
	
	generate_widgets();
	

		std::cout<<"generate primitives"<<std::endl;
		grammar->context->genPrimitives();
		
		grammar->generateGeometry();
		std::cout<<"Finished generating geometry..."<<std::endl;
		init_buffer=false;



gtk_container_remove((GtkContainer *)actionbar2,rulescombo);
//g_free(rulescombo);
rulescombo=gtk_combo_box_text_new ();
gtk_action_bar_pack_start (actionbar2,rulescombo);
    gtk_widget_set_size_request((GtkWidget *)actionbar2, 950, 12);
g_signal_connect (rulescombo ,"changed", G_CALLBACK (activate_add), NULL);    
gtk_widget_show(rulescombo);
for(int i=0;i<grammar->rule_list.size();i++){
	
	std::string rule_str=grammar->rule_list[i]->rule_name;
	
	strcpy(rulenames[i],rule_str.c_str());
	gtk_combo_box_text_append_text((GtkComboBoxText *)rulescombo,rulenames[i]);
	
	
	}

gtk_label_set_text((GtkLabel *)file_label,(grammar_filename+"    tokens: "+std::to_string(grammar->tokens_new.size())).c_str());
	//for(int i=0;i<texture_list.size();i++){
		//std::cout<<i<<texture_list[i]<<";";
		//glDeleteBuffers (1, &position_buffer1[i]);
	//}

		
	float mydata[36*8];
    for(int i=0;i<36*8;i++){
		mydata[i]=vertex_data[i]; 
	}
	
	
	for(int i=0;i<36;i++){
		for(int j=0;j<8;j++){
			if(j==0 || j==2)mydata[i*8+j]*=0.5f;
			if(j==1){
				if(mydata[i*8+j]<0)mydata[i*8+j]=0.0f;
			}
		}
	}
	for(int i=0;i<texture_list.size();i++){
		
		float *my_vertex_data=grammar->context->calc(mydata,i);
		//tex_count[i]=texcount;
		if(tex_count[i]>0){
		  //glGenVertexArrays (1, &vao1[i]);
		  glBindVertexArray (vao1[i]);

		  /* This is the buffer that holds the vertices */
		  //glGenBuffers (1, &position_buffer1[i]);
		  glBindBuffer (GL_ARRAY_BUFFER, position_buffer1[i]);
		  glBufferData(GL_ARRAY_BUFFER,sizeof(float)*36*8*tex_count[i],my_vertex_data,GL_DYNAMIC_DRAW);
		  
		  delete my_vertex_data;
	  }

	}
	
	save_to_file(grammar_filename.c_str());


}

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void scalesliderupdate(GtkRange *range){
	
	
	scale_global=  gtk_range_get_value (range);
	
}
void scalesliderupdate3(GtkRange *range){
	
	
	angle_view=  gtk_range_get_value (range);
	
}


void activate_add(GtkWidget *item) {
	Rule *active_rule;
	std::string selected_rule_name=gtk_combo_box_text_get_active_text((GtkComboBoxText *)rulescombo);
	for(int i=0;i<grammar->rule_list.size();i++){
		if(grammar->rule_list[i]->rule_name==selected_rule_name){
				active_rule=grammar->rule_list[i];
				break;
		}
	}
	
	
	for(int i=0;i<frame_counter;i++){
	gtk_container_remove((GtkContainer *)layout,(GtkWidget *)combo1[i]);
	gtk_container_remove((GtkContainer *)layout,(GtkWidget *)combo2[i]);
	gtk_container_remove((GtkContainer *)layout,(GtkWidget *)combo3[i]);
	gtk_container_remove((GtkContainer *)layout,(GtkWidget *)token_label[i]);
	}
	frame_counter=0;
	//frames[frame_counter++]=gtk_frame_new ("Repeat");
	//GtkWidget *box5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, FALSE);
	
	//GtkWidget *label=gtk_label_new("Repeat:");
	//GtkWidget *entry1=gtk_entry_new ();
	
	
	
	//GtkComboBoxText combo1=gtk_combo_box_text_new_with_entry ();
	//gtk_combo_box_text_append_text(std::string(active_rule->repeat).c_str());
	
	//std::cout<<"frames"<<std::endl;
	int token_counter=0;
	for(int k=0;k<3;k++){
		
		//section labels
		if(k==1)token_label[frame_counter]=gtk_label_new("Start Section");
		else if(k==2)token_label[frame_counter]=gtk_label_new("Repeat Section");
		else token_label[frame_counter]=gtk_label_new("End Section");
		gtk_layout_put ((GtkLayout *)layout,(GtkWidget *)token_label[frame_counter],20,30*token_counter);
		combo1[frame_counter]=(GtkComboBoxText *)gtk_combo_box_text_new_with_entry ();
		combo2[frame_counter]=(GtkComboBoxText *)gtk_combo_box_text_new_with_entry ();
		combo3[frame_counter]=(GtkComboBoxText *)gtk_combo_box_text_new_with_entry ();
		frame_counter++;
		token_counter++;
		
		std::vector<Token *> section_tokens=active_rule->section_tokens[k];
		for(int i=0;i<section_tokens.size();i++){
			  //std::cout<<section_tokens[i]->token_name.c_str()<<std::endl;
				
				
				token_label[frame_counter]=gtk_label_new(section_tokens[i]->token_name.c_str());
				
				combo1[frame_counter]=(GtkComboBoxText *)gtk_combo_box_text_new_with_entry ();
				combo2[frame_counter]=(GtkComboBoxText *)gtk_combo_box_text_new_with_entry ();
				combo3[frame_counter]=(GtkComboBoxText *)gtk_combo_box_text_new_with_entry ();
				gtk_widget_set_size_request((GtkWidget *)combo1[frame_counter],60,20);
				gtk_widget_set_size_request((GtkWidget *)combo2[frame_counter],60,20);
				gtk_widget_set_size_request((GtkWidget *)combo3[frame_counter],60,20);
				
				if(section_tokens[i]->var_names[0]!="")gtk_combo_box_text_append_text(combo1[frame_counter],(section_tokens[i]->var_names[0]).c_str());
				else if(section_tokens[i]->arguments.size()>0)gtk_combo_box_text_append_text(combo1[frame_counter],std::to_string(section_tokens[i]->arguments[0]).c_str());
				
				if(section_tokens[i]->var_names[1]!="")gtk_combo_box_text_append_text(combo2[frame_counter],(section_tokens[i]->var_names[1]).c_str());
				else if(section_tokens[i]->arguments.size()>1)gtk_combo_box_text_append_text(combo2[frame_counter],std::to_string(section_tokens[i]->arguments[1]).c_str());
				
				if(section_tokens[i]->var_names[2]!="")gtk_combo_box_text_append_text(combo3[frame_counter],(section_tokens[i]->var_names[2]).c_str());
				else if(section_tokens[i]->arguments.size()>2)gtk_combo_box_text_append_text(combo3[frame_counter],std::to_string(section_tokens[i]->arguments[2]).c_str());
				
				
				gtk_combo_box_set_active((GtkComboBox *)combo1[frame_counter],0);
				gtk_combo_box_set_active((GtkComboBox *)combo2[frame_counter],0);
				gtk_combo_box_set_active((GtkComboBox *)combo3[frame_counter],0);
				
				gtk_layout_put ((GtkLayout *)layout,(GtkWidget *)token_label[frame_counter],20,30*token_counter);
				gtk_layout_put ((GtkLayout *)layout,(GtkWidget *)combo1[frame_counter],200,30*token_counter);
				gtk_layout_put ((GtkLayout *)layout,(GtkWidget *)combo2[frame_counter],400,30*token_counter);
				gtk_layout_put ((GtkLayout *)layout,(GtkWidget *)combo3[frame_counter],600,30*token_counter);
				
				
				
				
				gtk_widget_show(token_label[frame_counter]);
				gtk_widget_show((GtkWidget *)combo1[frame_counter]);
				gtk_widget_show((GtkWidget *)combo2[frame_counter]);
				gtk_widget_show((GtkWidget *)combo3[frame_counter]);
				frame_counter++;
				token_counter++;
				//std::cout<<frame_counter<<std::endl;
		}
		
	}
		
	guint layout_width,layout_height;
	gtk_layout_get_size ((GtkLayout *)layout,&layout_width,&layout_height);
	gtk_layout_set_size ((GtkLayout *)layout,layout_width,(token_counter+1)*30);
	//gtk_entry_set_text(entry1,std::string(active_rule->repeat).c_str())
	
	
	//GtkWidget *label2=gtk_label_new("StartSection:");
	//GtkWidget *textview1=gtk_text_view_new();
	//GtkWidget *scrollwin=gtk_scrolled_window_new (NULL,NULL);
	
	
	//GtkTextBuffer *start_section_text=gtk_text_view_get_buffer (textview1);
	//gtk_text_buffer_set_text(start_section_text,active->)
	
	
	//gtk_widget_set_size_request(entry1,60,20);
	//gtk_widget_set_size_request(scrollwin,260,180);
	
	//gtk_container_add (GTK_CONTAINER (scrollwin), textview1);
	//gtk_text_view_set_wrap_mode((GtkTextView *)textview1,GTK_WRAP_WORD);
	
	
	//gtk_layout_put ((GtkLayout *)layout,scrollwin,80,80);
	//gtk_layout_put ((GtkLayout *)layout,label2,20,80);
	
	//gtk_layout_put ((GtkLayout *)layout,entry1,80,40);
	//gtk_layout_put ((GtkLayout *)layout,label,20,50);
	
	
	
	
	
	
	//GtkWidget *myimage=gtk_image_new_from_file("129_concrete brick wall texture-seamless.png");
	//gtk_layout_put ((GtkLayout *)layout,myimage,80,130);
	//gtk_widget_show (myimage);
	
	
	
	//gtk_widget_show (scrollwin);
	//gtk_widget_show (textview1);
	//gtk_widget_show (entry1);
	//gtk_widget_show (label);
	 //gtk_widget_set_size_request (frames[frame_counter-1], 220, 175);
	//gtk_widget_show(frames[frame_counter-1]);
	
	gtk_widget_show (layout);
}

void generate_widgets(){
if(setup_textures){				 
	std::cout<<"setup textures"<<std::endl;  
	for(int i=0;i<texture_filenames.size();i++){
		images[i]=gtk_image_new_from_file(texture_filenames[i].c_str());
		int j=i%7;
		int k=i/7;
		gtk_layout_put ((GtkLayout *)layout2,images[i],80+k*130,j*135);
		gtk_widget_show (images[i]);
	
	}
	setup_textures=false;  
		
}
if(setup_vars){				   
	std::cout<<"setup variables"<<std::endl;  
	std::cout<<full_variable_list.size()<<std::endl;
for(int i=0;i<full_variable_list.size();i++){
		labels[i]=gtk_label_new ((full_variable_list[i]->var_name+":="+std::to_string(full_variable_list[i]->value)).c_str());
		gtk_layout_put ((GtkLayout *)layout3,labels[i],80,i*20);
		gtk_widget_show (labels[i]);
	
	}
	guint layout_width,layout_height;
	gtk_layout_get_size ((GtkLayout *)layout3,&layout_width,&layout_height);
	gtk_layout_set_size ((GtkLayout *)layout3,layout_width,full_variable_list.size()*20);
	gtk_adjustment_configure (vadj,0,0,full_variable_list.size()*20,20,layout_height,layout_height);
	setup_vars=false;
}
if(setup_rules){				
	std::cout<<"setup rules"<<std::endl;     
for(int i=0;i<grammar->rule_list.size();i++){
	
	std::string rule_str=grammar->rule_list[i]->rule_name;
	
	strcpy(rulenames[i],rule_str.c_str());
	gtk_combo_box_text_append_text((GtkComboBoxText *)rulescombo,rulenames[i]);
	
	
	}
	setup_rules=false;
}
}


gboolean
layout_focus (GtkWidget       *widget,
               GtkDirectionType direction,
               gpointer         user_data){
generate_widgets();
return true;
}

gboolean
rule_focus (GtkWidget       *widget,
               GtkDirectionType direction,
               gpointer         user_data){
generate_widgets();
return true;
}

gboolean
vars_focus (GtkWidget       *widget,
               GtkDirectionType direction,
               gpointer         user_data){
generate_widgets();
return true;
}


gboolean
texture_focus (GtkWidget       *widget,
               GtkDirectionType direction,
               gpointer         user_data){
generate_widgets();
return true;				   
}



void activate_stop(GtkButton *item) {
	
	play=false;
	
}

void activate_play(GtkButton *item) {
	
	play=true;
	
}


			   
void activate_saveas(GtkButton *item) {
	
 
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
     GtkFileFilter *filter;

    native = gtk_file_chooser_native_new("Save File",
                                         (GtkWindow *)window,
                                         action,
                                         "_Save",
                                         "_Cancel");

    // Filters
    filter = gtk_file_filter_new(); // filter 1
    gtk_file_filter_set_name(filter, "Grammar file");
    gtk_file_filter_add_pattern(filter, "*.grammar");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter);
    filter = gtk_file_filter_new(); // filter 2
    gtk_file_filter_set_name(filter, "All files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter);

    // default file name
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(native), "test.grammar");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        grammar_filename = std::string(gtk_file_chooser_get_filename(chooser));

        // save the file
        save_to_file(grammar_filename.c_str());

activate(NULL);
        //g_free(grammar_filename);
    }

    g_object_unref(native);

}

GdkPixbuf *pix=NULL;
cv::Mat image;			   
void activate_choose_image_button(GtkButton *item) {
	
 
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
     GtkFileFilter *filter;

    native = gtk_file_chooser_native_new("Open Image File",
                                         (GtkWindow *)window,
                                         action,
                                         "_Open",
                                         "_Cancel");

    // Filters
    filter = gtk_file_filter_new(); // filter 1
    gtk_file_filter_set_name(filter, "Image file");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter);
    filter = gtk_file_filter_new(); // filter 2
    gtk_file_filter_set_name(filter, "All files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter);

    // default file name
    //gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(native), "test.grammar");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        std::string image_filename = std::string(gtk_file_chooser_get_filename(chooser));

        
		GError *err = NULL;
			/* Create pixbuf */
			if(pix!=NULL)g_object_unref(pix);
			
			image = cv::imread(image_filename.c_str(), cv::IMREAD_COLOR);
			
			//pix = gdk_pixbuf_new_from_file(image_filename.c_str(), &err);
			if(err)
			{
				printf("Error : %s\n", err->message);
				g_error_free(err);
			pix=NULL;
			}
	
        //g_free(grammar_filename);
    }

    g_object_unref(native);

}

void
txt_changed (GtkTextBuffer *textbuffer,
               gpointer       user_data){
GtkTextIter start, end;
GtkTextTag *tag,*tag2,*boldtag;
bool keyword_initiated=false;
char keyword[100];
int start_of_keyword=0,end_of_keyword=0;
int keyword_counter=0;
tag = gtk_text_buffer_create_tag (textbuffer, "blue_foreground",
	   		            "foreground", "blue", NULL); 
tag2 = gtk_text_buffer_create_tag (textbuffer, "purple_foreground",
	   		            "foreground", "purple", NULL); 
boldtag = gtk_text_buffer_create_tag (textbuffer, "bold", "weight", 600, NULL);

if(init_buffer==false){
	gtk_text_buffer_get_start_iter (textbuffer,&start);
	gtk_text_buffer_get_iter_at_offset (textbuffer, &end, 0);

	int counter=1;
	char character;
	while((character=gtk_text_iter_get_char (&end))!=0){
		
		
		if( !((character>='a' && character<='z') || (character>='A' && character<='Z')) ){
			keyword_initiated=false;
			end_of_keyword=counter;
			keyword[keyword_counter]=0;
			keyword_counter=0;
			bool matched=false;
			if(matched==false){
			 if('R'==keyword[0]  || 'S'==keyword[0] || 'T'==keyword[0] || 'I'==keyword[0] ){
				 	 gtk_text_buffer_get_iter_at_offset (textbuffer, &end, end_of_keyword);
					 gtk_text_buffer_get_iter_at_offset (textbuffer, &start, start_of_keyword);
					 gtk_text_buffer_apply_tag (textbuffer, boldtag, &start, &end);
					 matched=true;
			 }
			}
			if(matched==false){
					for(int i=0;i<variable_list.size();i++){
				   if(variable_list[i]->var_name==std::string(keyword)){
					 gtk_text_buffer_get_iter_at_offset (textbuffer, &end, end_of_keyword);
					 gtk_text_buffer_get_iter_at_offset (textbuffer, &start, start_of_keyword);
					 gtk_text_buffer_apply_tag (textbuffer, tag2, &start, &end);
					 matched=true;
					 break;	   
				   }	
				}
			}
			if(matched==false){
				for(int i=0;i<grammar->rule_list.size();i++){
				 if(grammar->rule_list[i]->rule_name==std::string(keyword)){
					 gtk_text_buffer_get_iter_at_offset (textbuffer, &end, end_of_keyword);
					 gtk_text_buffer_get_iter_at_offset (textbuffer, &start, start_of_keyword);
					 gtk_text_buffer_apply_tag (textbuffer, tag, &start, &end);
					 matched=true;
					 break;					 
				 }
				}
			}
		}
		else {
			if(keyword_initiated==false){
				keyword_initiated=true;
				start_of_keyword=counter;
			}
			keyword[keyword_counter]=character;
			keyword_counter++;
			
		}
		
		counter++;
		gtk_text_buffer_get_iter_at_offset (textbuffer, &end, counter);
	}
	init_buffer=true;
}

}
GtkWidget *drawing_area;
int mx[20],my[20],mouse_index=0,mousex,mousey;
Point p1,p2;
bool mouse_pressed=false;
std::vector<std::vector<Point>> squares;
std::vector<Point2f> perspective_points;

gboolean
keypressed_textview (GtkWidget *widget,
               GdkEvent  *event,
               gpointer   user_data){
				   //gtk_text_view_reset_cursor_blink((GtkTextView *)view);
				   //std::cout<<"*";
				   return TRUE;
			   }

bool perspective_transform=false;
void activate_perspective_transform_button(GtkButton *button){
	if(perspective_transform==false){
	perspective_transform=true;
	mouse_index=0;

	}
}

			   
void activateply(GtkButton *item) {
GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
     GtkFileFilter *filter;

    native = gtk_file_chooser_native_new("Save File",
                                         (GtkWindow *)window,
                                         action,
                                         "_Save",
                                         "_Cancel");

    // Filters
    filter = gtk_file_filter_new(); // filter 1
    gtk_file_filter_set_name(filter, "PLY file");
    gtk_file_filter_add_pattern(filter, "*.ply");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter);
    filter = gtk_file_filter_new(); // filter 2
    gtk_file_filter_set_name(filter, "All files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter);

    // default file name
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(native), "test.ply");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        filename = gtk_file_chooser_get_filename(chooser);

         PLYWriter::writeMesh( filename, grammar->context->getScene());
         
        g_free(filename);
    }

    g_object_unref(native);

			  
}		


gboolean mouse_press_callback(GtkWidget *da, GdkEventButton *event, gpointer userdata)
{
 int w = gtk_widget_get_allocated_width (da);
  int h = gtk_widget_get_allocated_height (da);
		//std::cout<<"mouse pressed"<<event->x<<event->y<<std::endl;
        if(mouse_pressed==false){
			if(perspective_transform){
				mouse_index++;
				perspective_points.push_back(Point2f(event->x*image.cols/w,event->y*image.rows/h));
				if(mouse_index==4){
						 int w = gtk_widget_get_allocated_width (da);
						int h = gtk_widget_get_allocated_height (da);
						std::vector<Point2f> screen_points;
						screen_points.push_back(Point2f(0,0));
						screen_points.push_back(Point2f(w,0));
						screen_points.push_back(Point2f(w,h));
						screen_points.push_back(Point2f(0,h));
						
						
						Mat M = getPerspectiveTransform(perspective_points,screen_points);
						 warpPerspective(image,image,M,Size(w,h),INTER_LINEAR,BORDER_CONSTANT,Scalar(1.0));
						perspective_transform=false;
								mouse_index=0;
				}
			}
			else {
			p1=Point(event->x,event->y);
			mouse_pressed=true;
			}
		}
		else {
			p2=Point(event->x,event->y);
			std::vector<Point> points;
			points.push_back(Point(p1.x*image.cols/w,p1.y*image.rows/h));
			points.push_back(Point(p1.x*image.cols/w,p2.y*image.rows/h));
			points.push_back(Point(p2.x*image.cols/w,p2.y*image.rows/h));
			points.push_back(Point(p2.x*image.cols/w,p1.y*image.rows/h));
			std::string grammar_snippet="Rule ->  [ T ( "+std::to_string((float)(w-p1.x*image.cols/w)/(float)h)+" "+std::to_string((float)(p2.y*image.rows/h-h)/(float)h-1.0f)+" z ) S ( "+std::to_string((float)(p2.x*image.cols/w-p1.x*image.cols/w)/(float)(h*4.0f))+" "+std::to_string((float)(p2.y*image.rows/h-p1.y*image.rows/h)/(float)(h*4.0f))+" Z ) I ( CubeX TextureIndex ) ] \n";
			gtk_text_buffer_set_text (buffer2, grammar_snippet.c_str(), -1);
			squares.push_back(points);
			mouse_pressed=false;
		}
		gtk_widget_queue_draw(da);

    return TRUE;
}
static gboolean
handle_mouse (GtkWidget *da, GdkEventButton *event, gpointer t){
	 int w = gtk_widget_get_allocated_width (da);
  int h = gtk_widget_get_allocated_height (da);
  mousex=event->x;
  mousey=event->y;
	gtk_widget_queue_draw_area (da, 0, 0, w, h);
	return TRUE;
}
	   
gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  guint width, height;
  GdkRGBA color;
  GtkStyleContext *context;

  context = gtk_widget_get_style_context (widget);

  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);

  gtk_render_background (context, cr, 0, 0, width, height);
  
  if(perspective_transform==true){
	  for(int i=0;i<perspective_points.size();i++){
			cv::circle(image,perspective_points[i],3,Scalar(0,255,0),1);
	  }
  }
  else cv::polylines(image, squares, true, Scalar(0, 255, 0), 3, LINE_AA);
  
  pix = gdk_pixbuf_new_from_data((const guchar*)image.data,GDK_COLORSPACE_RGB,FALSE,8,image.cols,image.rows,image.cols * 3,NULL,NULL);
  
	 if(pix!=NULL)pix = gdk_pixbuf_scale_simple((GdkPixbuf *)pix,width,height,GDK_INTERP_BILINEAR); 
    if(pix!=NULL){
		gdk_cairo_set_source_pixbuf(cr, pix, 0, 0);
		cairo_paint(cr);
   
	}
	
	    
  
  
   
  //cairo_fill (cr);

 return FALSE;
}
			   
void activate_app(GtkApplication *app){
		GtkWidget *box,*paned,*vpaned,*box3,*box4,*box5;
		
	
	uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    
    
   /* Create new top level window. */
  window = gtk_application_window_new (app);//gtk_window_new( GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW(window),SCREEN_WIDTH,SCREEN_HEIGHT);
  gtk_window_set_title(GTK_WINDOW(window), appname);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, FALSE);
  //box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, FALSE);
  paned=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  vpaned=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  box3=gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
  box4=gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
  box5=gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
  //box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, FALSE);
  
  //g_object_set (box, "margin", 12, NULL);
  //gtk_box_set_spacing (GTK_BOX (box), 6);
//  g_object_set (paned, "margin", 12, NULL);
 // gtk_box_set_spacing (GTK_BOX (paned), 6);
  
 actionbar2=(GtkActionBar *)gtk_action_bar_new ();
 GtkActionBar *actionbar3=(GtkActionBar *)gtk_action_bar_new ();
 GtkActionBar *actionbar4=(GtkActionBar *)gtk_action_bar_new ();

  gtk_box_pack_start (GTK_BOX(box4), (GtkWidget *)actionbar2,0,0, 0);

  
  gtk_container_add (GTK_CONTAINER (window), paned);
  
  
  gl_area = gtk_gl_area_new ();
  GtkWidget *scrollwin=gtk_scrolled_window_new (NULL,NULL);
  view = gtk_text_view_new();
  view2= gtk_text_view_new();
  gtk_container_add (GTK_CONTAINER (scrollwin), view);
  gtk_text_view_set_wrap_mode((GtkTextView *)view,GTK_WRAP_WORD);
  gtk_text_view_set_monospace ((GtkTextView *)view,TRUE);
  gtk_text_view_set_monospace ((GtkTextView *)view2,TRUE);
  
  
  
  GtkWidget *scrollwin2 = gtk_scrolled_window_new(NULL, NULL);
  GtkWidget *scrollwin3 = gtk_scrolled_window_new(NULL, NULL);
  
    
    gtk_widget_set_size_request((GtkWidget *)actionbar2, 950, 12);
notebook=gtk_notebook_new ();
    file_label=gtk_label_new("");
    gtk_widget_set_size_request((GtkWidget *)actionbar3, 950, 12);
    gtk_box_pack_start(GTK_BOX(box),file_label, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(box),notebook, 0, 0, 0);
    gtk_notebook_append_page ((GtkNotebook *)notebook,
                          scrollwin,
                          gtk_label_new ("Grammar Text"));
    layout=gtk_layout_new (NULL,NULL);
layout2=gtk_layout_new (NULL,NULL);

gtk_widget_set_size_request(notebook, 950, 1250);
gtk_widget_set_size_request(file_label, 950, 12);

vadj=gtk_adjustment_new (0,0,2000,1,100,100);
                    
layout3=gtk_layout_new (NULL,NULL);
layout4=gtk_layout_new (NULL,NULL);


gtk_container_add (GTK_CONTAINER (scrollwin2), layout3);
gtk_container_add (GTK_CONTAINER (scrollwin3), layout);

gtk_box_pack_start (GTK_BOX(box4),scrollwin3,1,1, 0);

gtk_notebook_append_page ((GtkNotebook *)notebook,
                          box4,
                          gtk_label_new ("Rule Layout"));
    
    
     drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 900, 500);
                            
 gtk_paned_add1(GTK_PANED(vpaned), drawing_area);
  gtk_paned_add2 (GTK_PANED(vpaned), view2);
   gtk_widget_show(drawing_area);
   gtk_widget_show(view2);
   
   GtkWidget *choose_image_button=gtk_button_new_with_label("Open Image");
   GtkWidget *perspective_transform_button=gtk_button_new_with_label("Perspective Transform");
   
   gtk_action_bar_pack_start (actionbar4,choose_image_button);  
   gtk_action_bar_pack_start (actionbar4,perspective_transform_button);  
   
  gtk_box_pack_start(GTK_BOX(box5),(GtkWidget *)actionbar4, 0, 0, 0);
  gtk_box_pack_start(GTK_BOX(box5),vpaned, 1, 1, 1);
  
gtk_notebook_append_page ((GtkNotebook *)notebook,
                          box5,
                          gtk_label_new ("SketchImport"));
  
  
  
  g_signal_connect (G_OBJECT (drawing_area), "draw",
                    G_CALLBACK (draw_callback), NULL);
    
  GtkWidget *addbutton=gtk_button_new_with_label("Add");
rulescombo=gtk_combo_box_text_new ();

 gtk_action_bar_pack_start (actionbar2,addbutton);    
   gtk_action_bar_pack_start (actionbar2,rulescombo);
       
                  //gtk_widget_set_size_request((GtkWidget *)actionbar2, 950,50);
                      gtk_widget_set_size_request((GtkWidget *)scrollwin3,950, 1000);
  gtk_widget_set_size_request(addbutton,10, 30);
  
gtk_widget_add_events (drawing_area,GDK_POINTER_MOTION_MASK|GDK_BUTTON_PRESS_MASK);

gtk_notebook_append_page ((GtkNotebook *)notebook,
                          layout2,
                          gtk_label_new ("Texture"));


gtk_notebook_append_page ((GtkNotebook *)notebook,
                          scrollwin2,
                          gtk_label_new ("Variables"));

//gtk_notebook_append_page ((GtkNotebook *)notebook,
  //                        layout4,
    //                      gtk_label_new ("Rules"));
                          
                          
 gtk_paned_add1(GTK_PANED(paned), box3);
  gtk_paned_add2 (GTK_PANED(paned), box);
  
  
   GtkActionBar *actionbar=(GtkActionBar *)gtk_action_bar_new ();
   GtkWidget *runbutton=gtk_button_new_with_label("Run");
  GtkWidget *savebutton=gtk_button_new_with_label("SaveAs");
  GtkWidget *plybutton=gtk_button_new_with_label("Export PLY");
  GtkWidget *scale_slider=gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,0.3,3.0,0.05);
   gtk_widget_set_hexpand (scale_slider, TRUE);
  gtk_action_bar_pack_start (actionbar,runbutton);
  gtk_action_bar_pack_start (actionbar,savebutton);
  gtk_action_bar_pack_start (actionbar,plybutton);
  gtk_action_bar_pack_start (actionbar,scale_slider);
  
  gtk_widget_set_size_request((GtkWidget *)actionbar, 650,50);
    gtk_widget_set_size_request((GtkWidget *)gl_area,650, 1100);
  gtk_widget_set_size_request(runbutton,10, 30);
  
  gtk_box_pack_start (GTK_BOX(box3), (GtkWidget *)actionbar,1,1, 0);
  gtk_box_pack_start (GTK_BOX(box3), gl_area,1,1, 0);
  
  
  
  gtk_widget_show((GtkWidget *)actionbar);
  gtk_widget_show(runbutton);
  



  
   GtkWidget *playbutton3=gtk_button_new_with_label("Play");
  GtkWidget *stopbutton3=gtk_button_new_with_label("Stop");
  
  scale_slider3=gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,0.0,360.0,2.0);
   gtk_widget_set_hexpand (scale_slider3, TRUE);
  gtk_action_bar_pack_start (actionbar3,playbutton3);
  gtk_action_bar_pack_start (actionbar3,stopbutton3);
  
  gtk_action_bar_pack_start (actionbar3,scale_slider3);
gtk_widget_set_size_request((GtkWidget *)actionbar3, 650,50);
    
  gtk_widget_set_size_request(playbutton3,10, 30);
    gtk_box_pack_start (GTK_BOX(box3), (GtkWidget *)actionbar3,1,1, 0);
  
  
  
  
  gtk_widget_show((GtkWidget *)actionbar);
  gtk_widget_show((GtkWidget *)actionbar3);
  gtk_widget_show(playbutton3);
  gtk_widget_show(stopbutton3);
  gtk_widget_show(rulescombo);
  
   gtk_text_view_set_editable((GtkTextView *)view,TRUE);
    //gtk_text_view_set_overwrite((GtkTextView *)view,TRUE);

 
   
  buffer2 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view2));
    
    mybuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    
    g_signal_connect(drawing_area,"motion_notify_event",G_CALLBACK(handle_mouse),   NULL);
    g_signal_connect(drawing_area,"button_press_event",G_CALLBACK(mouse_press_callback),   NULL);
    
    g_signal_connect (mybuffer ,"changed", G_CALLBACK (txt_changed), NULL);  
    //g_signal_connect (layout4 ,"focus", G_CALLBACK (rule_focus), NULL);  
    g_signal_connect (layout2 ,"focus", G_CALLBACK (texture_focus), NULL);  
    g_signal_connect (layout3 ,"focus", G_CALLBACK (vars_focus), NULL); 
    g_signal_connect (box4 ,"focus", G_CALLBACK (layout_focus), NULL);   
    
    g_signal_connect (perspective_transform_button,"clicked",G_CALLBACK (activate_perspective_transform_button), NULL);    
    g_signal_connect (rulescombo ,"changed", G_CALLBACK (activate_add), NULL);    
    g_signal_connect (runbutton ,"clicked", G_CALLBACK (activate), NULL);
    g_signal_connect (savebutton ,"clicked", G_CALLBACK (activate_saveas), NULL); 
    
    g_signal_connect (choose_image_button ,"clicked", G_CALLBACK (activate_choose_image_button), NULL); 
    g_signal_connect (plybutton ,"clicked", G_CALLBACK (activateply), NULL); 
    
    g_signal_connect (playbutton3 ,"clicked", G_CALLBACK (activate_play), NULL);
    g_signal_connect (stopbutton3 ,"clicked", G_CALLBACK (activate_stop), NULL); 
    
    
    g_signal_connect (scale_slider ,"value-changed", G_CALLBACK (scalesliderupdate), NULL); 
    g_signal_connect (scale_slider3 ,"value-changed", G_CALLBACK (scalesliderupdate3), NULL); 
    
  
  /* We need to initialize and free GL resources, so we use
  * the realize and unrealize signals on the widget
  */
  gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA (gl_area), TRUE);
  g_signal_connect (gl_area, "realize", G_CALLBACK (realize), NULL);
  g_signal_connect (gl_area, "unrealize", G_CALLBACK (unrealize), NULL);
  g_signal_connect (gl_area, "resize", G_CALLBACK (resize_glarea), NULL);

  /* The main "draw" call for GtkGLArea */
  g_signal_connect (gl_area, "render", G_CALLBACK (render), NULL);
  /* Quit form main if got delete event */
  g_signal_connect(G_OBJECT(window), "delete-event",
                 G_CALLBACK(gtk_main_quit), NULL);
                 
                 
  g_signal_connect(view,"leave-notify-event",G_CALLBACK(keypressed_textview), NULL);
                 
  gtk_widget_show_all(GTK_WIDGET(window));
 
   
   
    gtk_window_present (GTK_WINDOW (window));
   
   
}
   
int main( int argc, char* argv[] ){

GtkApplication *app;

for(int i=0;i<100;i++)rulenames[i]=new char(40);//due to GTK combobox error

	
	 app= gtk_application_new ("au.org.progen3d", G_APPLICATION_FLAGS_NONE);
	
	
       g_signal_connect (app ,"activate", G_CALLBACK (activate_app), NULL);
   
  
  
     int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
  
	
}

































