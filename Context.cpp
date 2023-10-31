#include "Context.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>


	
	

Primitive::Primitive(std::string type,GLuint texId){
this->texId=texId;

this->type=type;


}

void Primitive::draw(Scope *scope){
	
	
	glPushMatrix();
 glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D,this->texId);
	
	glTranslatef(scope->position[0],scope->position[1],scope->position[2]);
	glScalef(scope->size.x,scope->size.y,scope->size.z);
	
	
	if(type=="Sphere"){	
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricTexture(qobj, GL_TRUE);
	gluSphere(qobj, 0.5, 20, 20);
	gluDeleteQuadric(qobj);
	}
	else if(type=="Cylinder"){
	
	
	glTranslatef(0.0f,1.0f,0.0f);	
	glRotatef(90,1,0,0);	
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricTexture(qobj, GL_TRUE);
	gluCylinder(qobj,0.5f,0.5f,1.0f,32,32);
	gluDeleteQuadric(qobj);
		
	
		
	}
	else if(type=="Cube"){
	
	glTranslatef(0.0f,0.5f,0.0f);
	
	glScalef(0.5f,0.5f,0.5f);
	
	
	glBegin(GL_QUADS);                
        
        //std::cout<<"*";
        // Top face (y = 1.0f)
										  // Define vertices in counter-clockwise (CCW) order with normal pointing out
		//glColor3f(0.0f, 1.0f, 0.0f);     // Green
		glNormal3f(0.0f,1.0f,0.0f);
		
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(-1.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(-1.0f,-1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f,-1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);



		// Bottom face (y = -1.0f)

		//glColor3f(1.0f, 0.5f, 0.0f);     // Orange
		glNormal3f(0.0f,-1.0f,0.0f);
		
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(-1.0f,1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(-1.0f,-1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f,-1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		// Front face  (z = 1.0f)
		//glColor3f(1.0f, 0.0f, 0.0f);     // Red
		glNormal3f(0.0f,0.0f,1.0f);
		
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(-1.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(-1.0f,-1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1.0f,-1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

		// Back face (z = -1.0f)
		//glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
		glNormal3f(0.0f,0.0f,-1.0f);
		
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(-1.0f,1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(-1.0f,-1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(1.0f,-1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);

		// Left face (x = -1.0f)
		//glColor3f(0.0f, 0.0f, 1.0f);     // Blue
		glNormal3f(-1.0f,0.0f,0.0f);
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(-1.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(-1.0f,-1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f,-1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		// Right face (x = 1.0f)
		//glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
		glNormal3f(1.0f,0.0f,0.0f);
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(-1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(-1.0f,-1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(1.0f,-1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glEnd();  // End of drawing color-cube
	}
	glPopMatrix();
	 glDisable(GL_TEXTURE_2D);
}



void Context::addPrimitive(std::string type,Scope *scope){
	
	
	
	
	if(type=="Cube"){
		std::cout<<"Adding Primitive "<<Cube->type<<std::endl;
		primitives.push_back(Cube);
		}
	else if(type=="Cylinder"){
		primitives.push_back(Cylinder);
		std::cout<<"Adding Primitive "<<Cylinder->type<<std::endl;
		}
	else if(type=="Sphere") {
		primitives.push_back(Sphere);
		std::cout<<"Adding Primitive "<<Sphere->type<<std::endl;
		}
	else return;
	
	glm::vec3 pos=scope->getPosition();
	glm::vec3 size=scope->getSize();
	
	std::cout<<pos.x<<","<<pos.y<<","<<pos.z<<std::endl;
	std::cout<<size.x<<","<<size.y<<","<<size.z<<std::endl;
	
	primitive_scopes.push_back(new Scope(scope));
	
	
}


void Context::draw(){
	
	for(int i=0;i<primitives.size();i++){
		
		
		primitives[i]->draw(primitive_scopes[i]);
	}
	
}



GLuint Context::loadTexture(GLuint texid){
	
		

	texids.push_back(texid);
	return texid;
}



void Context::genPrimitives(){
Cube=new Primitive("Cube",texids[0]);
	Cylinder=new Primitive("Cylinder",texids[1]);
	Sphere=new Primitive("Sphere",texids[2]);
}

Context::Context()

{
	
	
	
	
	
	if(current_scope==NULL)current_scope=new Scope();
    
}

Context::~Context()
{
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
