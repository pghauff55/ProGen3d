#include "Grammar.h"
#include "NonTerminalShape.h"
#include "R.h"
#include "S.h"
#include "T.h"
#include "Pop.h"
#include "Push.h"
#include "I.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::vector<Variable *> variable_list;

Grammar::Grammar(const std::string &filePath)
{
    // TODO: Implement
    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        std::cerr << "E: Could not open file " << filePath << std::endl;
        return;
    }

    std::string line;
    while(std::getline(fin, line)) {
        std::istringstream lin(line);

        char arrow;
        std::string leftShapeType, rightShapeType;
        lin >> leftShapeType >> arrow >> arrow;
        Rule *rule = new Rule(false);
        rule->left = leftShapeType;
        rule->right = {{}};
        int rightIdx = 0;
        while (lin >> rightShapeType) {

            if (rightShapeType == ":") {
                float prob;
                std::string sep;
                lin >> prob >> sep;
                rule->rightProbs.push_back(prob); 
                rightIdx++;
                if (sep != "")
                    rule->right.push_back({});

                continue;
            }

            Shape* shape;
            if (rightShapeType == "S" || rightShapeType == "T" || rightShapeType == "Tr" || rightShapeType == "I" || rightShapeType == "Rx" || rightShapeType == "Ry" || rightShapeType == "Rz") { // TO DO: If rightShapeType in *list of terminal shapes*
                shape = parseShapeParameters(rightShapeType, lin);
            } else if (rightShapeType == "[") {
                shape = new Push();
            } else if (rightShapeType == "]") {
                shape = new Pop();
            } else if (rightShapeType == "Subdiv") {
                parseSubdivisionParameters(lin, rule, rightIdx);
                continue;
            } else {
                shape = new NonTerminalShape(rightShapeType);
            }

            rule->right[rightIdx].push_back(shape);
        }
        substitutionRules[leftShapeType] = rule;
    }
}

Grammar::~Grammar()
{

}

std::unordered_map<std::string, Rule*> Grammar::substitutionRules;

Rule* Grammar::findRule(Shape *shape)
{
    return substitutionRules[shape->getShapeType()];
}

// TODO: Implement non terminal shape S
Shape* Grammar::initialShape() const
{
    return new NonTerminalShape("X");
}

Shape* Grammar::parseShapeParameters(std::string shapeType, std::istringstream& lin)
{
    if (shapeType == "S") {
        float rangeValues[6];
        bool randoms[3];
        std::vector<std::string> var_names;
        var_names=parseRangeValues(lin, OUT rangeValues, OUT randoms,var_names);
      
        return new S(randoms[0],randoms[1],randoms[2],var_names[0],var_names[1],var_names[2],glm::vec3(rangeValues[0],rangeValues[1],rangeValues[2]), glm::vec3(rangeValues[3],rangeValues[4],rangeValues[5])); 
    }

    else if (shapeType == "T" || shapeType == "Tr") {
        bool relative = (shapeType == "Tr"); 
        float rangeValues[6];
        bool randoms[3];
        std::vector<std::string> var_names;
        var_names=parseRangeValues(lin, OUT rangeValues,OUT randoms,var_names);
        return new T(glm::vec3(rangeValues[0], rangeValues[1], rangeValues[2]),
                     glm::vec3(rangeValues[3], rangeValues[4], rangeValues[5]), relative);
    }

    else if (shapeType == "Rx" || shapeType == "Ry"  || shapeType == "Rz") {
        float rangeValue[2];
        parseRangeValue(lin, OUT rangeValue);
        if (shapeType == "Rx") {
            return new R(1, rangeValue[0], rangeValue[1]);
        }
        else if (shapeType == "Ry") {
            return new R(2, rangeValue[0], rangeValue[1]);
        }
        else if (shapeType == "Rz") {
            return new R(3, rangeValue[0], rangeValue[1]);
        }
    }

    else if (shapeType == "I") {
        char parenthesis;
        std::string objId;
        lin >> parenthesis >> objId >> parenthesis;
        return new I(objId);
    }
    return NULL;
}

void Grammar::parseSubdivisionParameters(std::istringstream& lin, Rule* rule, int rightIdx) 
{
    char parenthesis;
    int dim;
    float n;
    std::string n_str;
    lin >> parenthesis >> dim >> n_str;
	n=findVariable(n_str);
	if(n==0.0f)n=atof(n_str.c_str());
	//std::cout<<"Subs variable :"<<n_str<<"="<<n<<std::endl;
	
	int val=findVariable(str+"subs"+int(n));
	if(val==0.0f)variable_list.push_back(new Variable(str+"subs"+int(n),1.0f/n));
	
	
	
	

    glm::vec3 dimVector(dim==1, dim==2, dim==3);
    std::vector<Shape*> shapes;
    lin >> parenthesis >> parenthesis;
    std::string shapeType;
    Shape* shape;
    
    float size = 1.0f/n;
        glm::vec3 sizeVec((dim==1) ? size : 1, (dim==2) ? size : 1, (dim==3) ? size : 1);
        
        Shape *subS;
     if(dim==1)subS=new S(false,false,false,n_str+std::to_string((int)n),"","",sizeVec,sizeVec);
     if(dim==2)subS=new S(false,false,false,"",n_str+std::to_string((int)n),"",sizeVec,sizeVec);
     if(dim==3)subS=new S(false,false,false,"","",n_str+std::to_string((int)n),sizeVec,sizeVec); 
    lin >> shapeType;
    
    if (shapeType == "S" || shapeType == "T" || shapeType == "Tr") {
            //shape = parseShapeParameters(shapeType, lin);
        } else {
            shape = new NonTerminalShape(shapeType);
        }
    
    
        
        
        
        
    rule->right[rightIdx].push_back(new Subdiv());
    
    lin >> parenthesis;
}

float Grammar::findVariable(std::string var_name){
	//std::cout<<"Searching for "<<var_name<<std::endl;
	for(int i=0;i<variable_list.size();i++){
	
		if(variable_list[i]->var_name.compare(var_name)==0) {
			
			//std::cout<<"Found "<<var_name<<"="<<variable_list[i]->value<<std::endl;
		return variable_list[i]->value;
		}
	
	}
	return 0.0f;
}
void Grammar::parseRangeValue(std::istringstream &lin, float (&rangeValue)[2])
{
	std::string var_name[3];
	var_name[0]=var_name[1]=var_name[2]="";
	
    char parenthesis;
    lin >> parenthesis;
    std::string s;
    lin >> s;
    if (s.compare("R")==0) {
		std::cout<<"Random"<<std::endl;
		
        lin >>var_name[0]>>parenthesis >> rangeValue[0] >> rangeValue[1] >> parenthesis;
        
        
 
        
        
        variable_list.push_back(new Variable(var_name[0],rand()/(float)RAND_MAX*((rangeValue[1]-rangeValue[0])+rangeValue[0])));
    }
    else {
		
		float value=findVariable(s);
	     if(value==0.0)
			rangeValue[0] = atof(s.c_str());
        else {
			std::cout<<"replacing variable "<<s<<" with value ="<<value<<std::endl;
			rangeValue[0]=value;
        }
        
        rangeValue[1] = rangeValue[0];
    }
    lin >> parenthesis;
}

std::vector<std::string> &Grammar::parseRangeValues(std::istringstream& lin, float (&rangeValues)[6],bool (&randoms)[3],std::vector<std::string> &var_names) 
{
	std::string var_name[3];
	var_name[0]=var_name[1]=var_name[2]="";
	randoms[0]=randoms[1]=randoms[2]=false;
	bool flag=false;
    char parenthesis;
    lin >> parenthesis; 
    std::string s;
    for (int i=0; i<3; i++) {
        lin >> s;
        if (s.compare("R")==0) {
			
			lin >>var_name[i]>>parenthesis >> rangeValues[i] >> rangeValues[i+3] >> parenthesis;
        
			variable_list.push_back(new Variable( var_name[i],rand()/(float)RAND_MAX*((rangeValues[i+3]-rangeValues[i])+rangeValues[i]) ));
			flag=true;
            randoms[i]=true;
            var_names.push_back(var_name[i]);
            //lin >> parenthesis >> rangeValues[i] >> rangeValues[i+3] >> parenthesis; 
        } else {
			
		
		
			rangeValues[i] = atof(s.c_str());
			if(rangeValues[i]==0.0f)var_names.push_back(s);
		    else var_names.push_back("");
            rangeValues[i+3] = rangeValues[i]; 
        }
    }

    lin >> parenthesis;
    return var_names;
}
