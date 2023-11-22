#include "Context.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>



	extern void draw_box(float angle_cube,glm::vec3 scale_vec,glm::vec3 position_vec,glm::vec3 pos,int tex_index);

Primitive::Primitive(std::string type,GLuint texId, bool x,bool y,bool z){
this->texId=texId;

this->type=type;
this->x=x;
this->y=y;
this->z=z;


}

void Primitive::draw(Scope *scope,int tex,int rotate,float scaletex){
	
	

 /*glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D,tex);


	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();


	
	glTranslatef(scope->position[0],scope->position[1],scope->position[2]);
	glRotatef(90.0f*float(rotate),0,1,0);
	glScalef(scope->size.x,scope->size.y,scope->size.z);
	
	*/
	
	
	if(type=="Sphere"){	
	//GLUquadric *qobj = gluNewQuadric();
	//gluQuadricTexture(qobj, GL_TRUE);
	//gluSphere(qobj, 0.5, 20, 20);
	//gluDeleteQuadric(qobj);
	}
	else if(type=="Cylinder"){
	
	
	//glTranslatef(0.0f,1.0f,0.0f);	
	//glRotatef(90,1,0,0);	
	//GLUquadric *qobj = gluNewQuadric();
	//gluQuadricTexture(qobj, GL_TRUE);
	//gluCylinder(qobj,0.5f,0.5f,1.0f,32,32);
	//gluDeleteQuadric(qobj);
		
	
		
	}
	else if(type=="Cube" || type=="CubeX" || type=="CubeY"){
	
	std::cout<<"draw"<<std::endl;
	
	
	//float y_val=0.0f,x_val=0.0f,z_val=0.0f;
	//if(y)y_val=0.5f;
	//if(x)x_val=0.5f;
	//if(x)z_val=0.5f;
	//glTranslatef(x_val,y_val,z_val);
	
	//glScalef(0.5f,0.5f,0.5f);
	
	
	float SCALE=0.5f;
	
	float x=fabs(scope->size.x);
	float z=fabs(scope->size.z);
	float y=fabs(scope->size.y);
	
	float X=scope->position.x;
	float Z=scope->position.z;
	float Y=scope->position.y;
	
	if(type=="Cube")
	draw_box(90.0f*float(rotate),glm::vec3(x,y,z),glm::vec3(X,Y,Z),glm::vec3(0,0,0),tex);
	else if(type=="CubeX")
	draw_box(90.0f*float(rotate),glm::vec3(x,y,z),glm::vec3(X,Y,Z),glm::vec3(0.5,0,0),tex);
	else if(type=="CubeY")
	draw_box(90.0f*float(rotate),glm::vec3(x,y,z),glm::vec3(X,Y,Z),glm::vec3(0,0,0.5),tex);
	
	/*
	
	glBegin(GL_QUADS);                
        
        //std::cout<<"*";
        // Top face (y = 1.0f)
										  // Define vertices in counter-clockwise (CCW) order with normal pointing out
		//glColor3f(0.0f, 1.0f, 0.0f);     // Green
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(x,-z);
		glVertex3f(1.0f, 1.0f, -1.0f);
		
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(-x,-z);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(-x,z);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(x,z);
		glVertex3f(1.0f, 1.0f, 1.0f);



		// Bottom face (y = -1.0f)

		//glColor3f(1.0f, 0.5f, 0.0f);     // Orange
		glNormal3f(0.0f,-1.0f,0.0f);
		
		
		glTexCoord2f(x,z);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(-x,z);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(-x,-z);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(x,-z);
		glVertex3f(1.0f, -1.0f, -1.0f);

		// Front face  (z = 1.0f)
		//glColor3f(1.0f, 0.0f, 0.0f);     // Red
		glNormal3f(0.0f,0.0f,1.0f);
		
		
		glTexCoord2f(x,y);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(-x,y);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(-x,-y);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(x,-y);
		glVertex3f(1.0f, -1.0f, 1.0f);

		// Back face (z = -1.0f)
		//glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
		glNormal3f(0.0f,0.0f,-1.0f);
		
		
		glTexCoord2f(x,-y);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(-x,-y);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(-x,y);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(x,y);
		glVertex3f(1.0f, 1.0f, -1.0f);

		// Left face (x = -1.0f)
		//glColor3f(0.0f, 0.0f, 1.0f);     // Blue
		glNormal3f(-1.0f,0.0f,0.0f);
		
		glTexCoord2f(y,z);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(y,-z);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(-y,-z);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(-y,z);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		// Right face (x = 1.0f)
		//glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
		glNormal3f(1.0f,0.0f,0.0f);
		
		glTexCoord2f(y,-z);
		glVertex3f(1.0f, 1.0f, -1.0f);
		
		glTexCoord2f(y,z);
		glVertex3f(1.0f, 1.0f, 1.0f);
		
		glTexCoord2f(-y,z);
		glVertex3f(1.0f, -1.0f, 1.0f);
		
		glTexCoord2f(-y,-z);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glEnd();  // End of drawing color-cube*/
	}
	//glPopMatrix();
	
	 //glDisable(GL_TEXTURE_2D);
}



void Context::addPrimitive(std::string type,Scope *scope,int texindex, int rotate, float texscale){
	
	
	
	
	if(type=="Cube"){
		std::cout<<"Adding Primitive "<<Cube->type<<std::endl;
		primitives.push_back(Cube);
		}
	
	else if(type=="CubeX"){
		std::cout<<"Adding Primitive "<<Cube->type<<std::endl;
		primitives.push_back(CubeX);
		}
	else if(type=="CubeY"){
		std::cout<<"Adding Primitive "<<Cube->type<<std::endl;
		primitives.push_back(CubeY);
		}
	else if(type=="Cylinder"){
		primitives.push_back(Cylinder);
		std::cout<<"Adding Primitive "<<Cylinder->type<<std::endl;
		}
	else if(type=="Sphere") {
		primitives.push_back(Sphere);
		std::cout<<"Adding Primitive "<<Sphere->type<<std::endl;
		}
	else {
		std::cout<<"primitive not added!"<<std::endl;
		return;
	}
	
	glm::vec3 pos=scope->getPosition();
	glm::vec3 size=scope->getSize();
	
	std::cout<<pos.x<<","<<pos.y<<","<<pos.z<<std::endl;
	std::cout<<size.x<<","<<size.y<<","<<size.z<<std::endl;
	
	primitive_scopes.push_back(new Scope(scope));
	texindexes.push_back(texids[texindex]);
	rotates.push_back(rotate);
	texscales.push_back(texscale);
	
	
}



GLfloat *Context::calc(const GLfloat *vertex_data){
	GLfloat SCALE=1.0f;
	int NUM=36;
	GLfloat *vertex_buffer=new GLfloat[NUM*8*primitives.size()];
	
	for(int i=0;i<primitives.size();i++){
		float x=primitive_scopes[i]->size.x*SCALE;
		float y=primitive_scopes[i]->size.y*SCALE;
		float z=primitive_scopes[i]->size.z*SCALE;
		float X=primitive_scopes[i]->position.x*SCALE;
		float Y=primitive_scopes[i]->position.y*SCALE;
		float Z=primitive_scopes[i]->position.z*SCALE;
		
		for (int j=0;j<NUM;j++){
			vertex_buffer[i*NUM*8+j*8]=vertex_data[j*8]*x+X;
			vertex_buffer[i*NUM*8+j*8+1]=vertex_data[j*8+1]*y+Y;
			vertex_buffer[i*NUM*8+j*8+2]=vertex_data[j*8+2]*z+Z;
			vertex_buffer[i*NUM*8+j*8+3]=vertex_data[j*8+3];
			vertex_buffer[i*NUM*8+j*8+4]=vertex_data[j*8+4];
			vertex_buffer[i*NUM*8+j*8+5]=vertex_data[j*8+5];
			if(vertex_data[j*NUM+3]==1.0f){
				vertex_buffer[i*NUM*8+j*8+6]=vertex_data[j*8+6];
				vertex_buffer[i*NUM*8+j*8+7]=vertex_data[j*8+7];
			}
			else if(vertex_data[j*NUM+4]==1.0f){
				vertex_buffer[i*NUM*8+j*8+6]=vertex_data[j*8+6];
				vertex_buffer[i*NUM*8+j*8+7]=vertex_data[j*8+7];
				
			}
			else {
				vertex_buffer[i*NUM*8+j*8+6]=vertex_data[j*8+6];
				vertex_buffer[i*NUM*8+j*8+7]=vertex_data[j*8+7];
			}
			
			//for(int k=0;k<8;k++)std::cout<<vertex_buffer[i*NUM*8+j*8+k]<<":";
			//std::cout<<std::endl;
		}
	}
 return vertex_buffer;	
}


void Context::draw(){
	
	for(int i=0;i<primitives.size();i++){
		
		
		primitives[i]->draw(primitive_scopes[i],texindexes[i],rotates[i],texscales[i]);
	}
	
}



GLuint Context::loadTexture(GLuint tex){
	
		

	texids.push_back(tex);
	return tex;
}



void Context::genPrimitives(){
Cube=new Primitive("Cube",0,false,true,false);
CubeX=new Primitive("CubeX",0,true,true,false);
CubeY=new Primitive("CubeY",0,false,true,true);
	Cylinder=new Primitive("Cylinder",0,false,false,false);
	Sphere=new Primitive("Sphere",0,false,false,false);
}

Context::Context()

{
	
	
	
	
	
	if(current_scope==NULL)current_scope=new Scope();
    
}

Context::~Context()
{
	for(int i=0;i<primitives.size();i++){
		
		
		delete primitives[i];
	}
 	for(int i=0;i<primitives.size();i++){
		
		delete primitive_scopes[i];
	}

}
void Context::newScope(){
	
	
	scopes.push(current_scope);
	glm::vec3 pos=current_scope->getPosition();//preserve position not scale
	current_scope=new Scope();
	current_scope->setPosition(pos);
}
void Context::pushScope() 
{
    if(current_scope!=NULL){
		scopes.push(current_scope);
		current_scope=new Scope(current_scope);//copy consructor
	}
	else {
		current_scope=new Scope();
    }
}

Scope *Context::popScope() 

{

	if(current_scope!=NULL)delete current_scope;
    current_scope=scopes.top();
    scopes.pop();
    
    return current_scope;

}

Scope *Context::getCurrentScope()
{
    
    return current_scope;
}

Mesh &Context::getScene()
{
    return scene;
}
