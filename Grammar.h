#pragma once

#include "Rule.h"

#include <string>
#include <unordered_map>
#include <stack>
#include <vector>

#define OUT


class Variable{
	public:
	std::string var_name;
	float value;
	Variable(std::string name,float value){
		this->var_name=name;
		this->value=value;
	}
	
};



class Grammar
{
public:
    Grammar(const std::string &filePath);
    ~Grammar();
    static Rule* findRule(Shape *shape);
    Shape* initialShape() const;
    Shape* parseShapeParameters(std::string shapeType, std::istringstream& lin);
    void parseSubdivisionParameters(std::istringstream& lin, Rule* rule, int rightIdx);
    float findVariable(std::string var_name);
private:
    using ShapeInstanceType = std::string;
    static std::unordered_map<ShapeInstanceType, Rule*> substitutionRules;
    void parseRangeValue(std::istringstream &lin, float (&rangeValues)[2]);
    std::vector<std::string> &parseRangeValues(std::istringstream& lin, float (&rangeValues)[6],bool (&randoms)[3],std::vector<std::string> &);


};
