#pragma once

#include "Scope.h"
#include "Mesh.h"

#include <stack>

class Context
{
public:
    Context();
    ~Context();
    void pushScope();
    Scope *popScope();
    void newScope();
    Scope *getCurrentScope();
    Mesh &getScene();
private:
    Scope *current_scope=NULL;
    std::stack<Scope *> scopes;
    Mesh scene;
};
