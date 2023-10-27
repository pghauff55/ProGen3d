#include "Context.h"

#include <iostream>

Context::Context()

{
	
	if(current_scope==NULL)current_scope=new Scope();
    
}

Context::~Context()
{

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
