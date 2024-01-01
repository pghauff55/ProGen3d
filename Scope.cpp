#include "Scope.h"

#include <glm/ext.hpp>

Scope::Scope()
    : position(0.0f)
    , size(1.0f)
    , size2(1.0f)
    , x(1.0f, 0.0f, 0.0f)
    , y (0.0f, 1.0f, 0.0f)
    , z (0.0f, 0.0f, 1.0f)
    {
		this->anglex=0.0f;
		this->angley=0.0f;
		this->anglez=0.0f;
		
		}

Scope::Scope(Scope *other)
    {
		this->position=other->position;
		this->size=other->size;
		this->size2=other->size2;
		this->x=other->x;
		this->y=other->y;
		this->z=other->z;
		this->anglex=other->anglex;
		this->angley=other->angley;
		this->anglez=other->anglez;
		
		}

void Scope::T(const glm::vec3 &translation)
{
	glm::vec3 translate(translation.x*size.x,translation.y*size.y,translation.z*size.z);
	
	
    position += translate;
}

void Scope::S(const glm::vec3 &size_)
{
    size *= size_;
    size2=size;
}
void Scope::D(const glm::vec3 &size_)
{
    size2 = size*size_;
}
void Scope::Rx(float angle)
{
	anglex=angle;
	
	if(anglex>360.0f)anglex=anglex-360.0;
	else if(anglex<0.0f)anglex=360.0f-anglex;
    float c = glm::cos(anglex*M_PI/180.0f);
    float s = glm::sin(anglex*M_PI/180.0f);
    glm::mat3 rotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, c, s), glm::vec3(0.0f, -s, c));
    glm::mat3 basis(x, y, z);
    y = basis * rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    z = basis * rotation * glm::vec3(0.0f, 0.0f, 1.0f);

}

void Scope::Ry(float angle)
{
	angley=angle;
	if(angley>360.0f)angley=angley-360.0;
	else if(angley<0.0f)angley=360.0f-angley;
    float c = glm::cos(angley*M_PI/180.0f);
    float s = glm::sin(angley*M_PI/180.0f);
    glm::mat3 rotation(glm::vec3(c, 0.0f, -s), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(s, 0.0f, c));
    glm::mat3 basis(x, y, z);
    x = basis * rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    z = basis * rotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

void Scope::Rz(float angle)
{
	anglez=angle;
	
	if(anglez>360.0f)anglez=anglez-360.0;
	else if(anglez<0.0f)anglez=360.0f-anglez;
    float c = glm::cos(anglez*M_PI/180.0f);
    float s = glm::sin(anglez*M_PI/180.0f);
    glm::mat3 rotation(glm::vec3(c, s, 0.0f), glm::vec3(-s, c, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat3 basis(x, y, z);
    x = basis * rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    y = basis * rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    
}

glm::mat4 Scope::getTransform()
{
    glm::mat4 rotation(glm::vec4(x, 0.0f), glm::vec4(y, 0.0f), glm::vec4(z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::mat4 translation(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(position, 1.0f));
    glm::mat4 scale(glm::vec4(size.x, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, size.y, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, size.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::mat4 transform = rotation * translation * scale;
    //transform[3][0] = position.x;
    //transform[3][1] = position.y;
    //transform[3][2] = position.z;

    return transform;
}
glm::mat4 Scope::getTransform2()
{
    glm::mat4 rotation(glm::vec4(x, 0.0f), glm::vec4(y, 0.0f), glm::vec4(z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::mat4 translation(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(position, 1.0f));
    glm::mat4 scale(glm::vec4(size2.x, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, size2.y, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, size2.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::mat4 transform = rotation * translation * scale;
   // transform[3][0] = position.x;
    //transform[3][1] = position.y;
    //transform[3][2] = position.z;

    return transform;
}
glm::vec3 Scope::getPosition() const
{
    return position;
}
glm::vec3 Scope::setPosition(glm::vec3 pos) 
{
    this->position=pos;
    return position;
}

glm::vec3 Scope::getSize() const
{
    return size;
}
