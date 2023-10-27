#include <fstream>
#include <iostream>
#include <sstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>

#include "PLYWriter.h"
#include "Context.h"
#include "Scope.h"

#include "MathParser.h"
#include "LBAssembler.h"





bool checkAlpha(std::string str){
    for(int i = 0; i < str.size(); i++)
        if( !isalpha(str[i]) || !isspace(str[i]) )
            return true;
    return false;
}
bool isnumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && ( (*it)=='-' || (*it)=='.' || std::isdigit(*it)) ) ++it;
    return !s.empty() && it == s.end();
}


class Variable{
	public:
	std::string var_name;
	float value;
	int instance_count=0;
	Variable(std::string name,float min,float max){
		this->var_name=name;
		this->min=min;
		this->max=max;
		this->value=rand()/(float)RAND_MAX*(max-min)+min;
	}
	Variable(std::string name,float value){
		this->var_name=name;
		this->min=0;
		this->max=0;
		this->value=value;
	}
	float getValue(){
		//this->value=rand()/(float)RAND_MAX*(max-min)+min;
		return this->value;
	}
	float getRandom(){
		
		return rand()/(float)RAND_MAX*(max-min)+min;
	}
	float min,max;
};


    std::vector<Variable *> variable_list;
    
int findVariable(std::string var_name){
	std::cout<<"Searching Back for "<<var_name<<std::endl;
	for(int i=variable_list.size()-1;i>=0;i--){
	
		if(variable_list[i]->var_name.compare(var_name)==0) {
	
			std::cout<<"Found "<<var_name<<"="<<variable_list[i]->value<<std::endl;
		return i;
		}
	
	}
	return -1;
}
int findVariableForward(std::string var_name){
	std::cout<<"Searching Forward for "<<var_name<<std::endl;
	for(int i=0;i<variable_list.size();i++){
	
		if(variable_list[i]->var_name.compare(var_name)==0) {
	
			std::cout<<"Found "<<var_name<<"="<<variable_list[i]->value<<std::endl;
		return i;
		}
	
	}
	
	return -1;
}

void removeVariable(std::string var_name){
	
	
	for(int i=variable_list.size()-1;i>=0;i--){
		if(variable_list[i]->var_name.compare(var_name)==0) {
			if(variable_list[i]->max==variable_list[i]->min){
				variable_list.erase (variable_list.begin()+i);
				return;
			}
		}
		
	}
	
}


    
void addVariable(std::string var_name,float min,float max){
	
	
	int index=findVariableForward(var_name);
	if(index==-1){
		Variable *var=new Variable(var_name,min,max);
		var->value=var->getRandom();
		variable_list.push_back(var);
	}
	
	
}

void addVariable(std::string var_name){
	
	std::cout<<"Adding variable "<<var_name;
	int index=findVariableForward(var_name);
	if(index!=-1){
	
		Variable *var=new Variable(var_name,variable_list[index]->getRandom());
		var->instance_count=variable_list[index]->instance_count+1;
		variable_list.push_back(var);
		std::cout<<" SUCCESS"<<var->instance_count<<std::endl;
	}
	else 
		std::cout<<" FAILED"<<std::endl;
}
    
    
class Token{
	public:
	Token(Token *t){
		this->token_name=t->token_name;
		for(int i=0;i<t->arguments.size();i++){
			this->arguments.push_back(t->arguments[i]);
		}
		this->var_names[0]=t->var_names[0];
		this->var_names[1]=t->var_names[1];
		this->var_names[2]=t->var_names[2];
		this->var_name=t->var_name;
		
		this->instance_type=t->instance_type;
		
	}
	Token(std::string token_name){
		this->token_name=token_name;
	}
	Token(std::string token_name,int instance_count){
		this->token_name=token_name;
		this->instance_count=instance_count;
	}
	void addArgument(float value){
		
		arguments.push_back(value);
	}
	void addInstanceType(std::string instance){
		
			this->instance_type=instance;
	}
	int getArgCount(){
		return arguments.size();
	}
	std::string isRule(){
		
		if(token_name!="{" && token_name!="}" && token_name!="+" && token_name!="*" && token_name!="[" && token_name!="]" && instance_type=="" && arguments.size()==0)return token_name;
		else return "";
	}
	void print(){
		std::cout<<"Token: "<<token_name<<" -> ";
		if(var_name!="")std::cout<<var_name<<":";
		if(arguments.size()>0){
			std::cout<<" ( ";
			for(int i=0;i<arguments.size();i++){
			std::cout<<arguments[i]<<" ";
			}
			std::cout<<" ) ";
		}
		if(var_names[0]!=""){
		
		}
		if(var_names[1]!=""){
	            	
		}
		if(var_names[2]!=""){
		
		}
		
		std::cout<<"["<<var_names[0]<<" "<<var_names[1]<<" "<<var_names[2]<<"]";
		if(instance_type!=""){
			std::cout<<instance_type<<":"<<instance_type;
		}
		std::cout<<std::endl;
		
	}
	
	void performAction(Context &context);
	
	
	std::string token_name;
	std::vector<float> arguments;
	std::string instance_type;
	int instance_count=0;
	std::string var_name;
	std::string var_names[3];
	bool modify[3];
	bool divideby2[3];
};

class Rule {
	public:
	Rule(std::string rule_name,int repeat){
		this->rule_name=rule_name;
		this->repeat=repeat;
		this->count=0;
	}
	void addToken(Token *token,int section){
		
		section_tokens[section].push_back(token);
		
		if(section==1){
		  tokens.push_back(token);	
		}
		
	}
	bool increment(){
		if(repeat==0)return false;
		count++;
		if(count==repeat)return false;
		else return true;
	}
	
	int repeat=1,count=0;
	std::string rule_name;
	std::vector<Token *> tokens,section_tokens[3];
	std::string var_name;
	std::string var_names[10];
	bool modify;
	bool divideby2;
	int var_counter=0;
	float probability=1.0f;
};



std::string MathS2(std::string input){
		mathS::Assembler assembler;
				std::cout<<"IN:"<<input<<";";
			//replace variables with floats
		for(int i=variable_list.size()-1;i>=0;i--){
			std::string str2 = variable_list[i]->var_name;
					
		      if(input.find(str2)!=-1){
				  std::cout<<"===="<<str2;
				  input.replace(input.find(str2),str2.length(),std::to_string(variable_list[i]->value));	
				 break;
			  }
		}
		std::cout<<"IN:"<<input<<";";
		
			//replace variables with floats
		for(int i=variable_list.size()-1;i>=0;i--){
			std::string str2 = variable_list[i]->var_name;
					
		      if(input.find(str2)!=-1){
				  std::cout<<"===="<<str2;
				  input.replace(input.find(str2),str2.length(),std::to_string(variable_list[i]->value));	
				 break;
			  }
		}
		std::cout<<"IN:"<<input<<";";
		
		std::vector<std::string> params = {};
		auto mobj = mathS::Parse(input);
		auto f = assembler.Assemble(mobj, params);
				
		std::cout<<"mathS2 :"<<f({})->GetString()<<std::endl;
	
	std::string out=f({})->GetString();
	if(isnumber(out))
		return out;
	else
	return input;
}

std::string MathS(std::string input){
		mathS::Assembler assembler;
				std::cout<<"IN:"<<input<<";";
			//replace variables with floats
		for(int i=variable_list.size()-1;i>=0;i--){
			if(variable_list[i]->max==variable_list[i]->min){
			std::string str2 = variable_list[i]->var_name;
					
		      if(input.find(str2)!=-1){
				  std::cout<<"===="<<str2;
				  input.replace(input.find(str2),str2.length(),std::to_string(variable_list[i]->value));	
				 break;
			  }
		  }
		}
		std::cout<<"IN:"<<input<<";";
		
			//replace variables with floats
		for(int i=variable_list.size()-1;i>=0;i--){
			if(variable_list[i]->max==variable_list[i]->min){
			std::string str2 = variable_list[i]->var_name;
					
		      if(input.find(str2)!=-1){
				  std::cout<<"===="<<str2;
				  input.replace(input.find(str2),str2.length(),std::to_string(variable_list[i]->value));	
				 break;
			  }
		  }
		}
		std::cout<<"IN:"<<input<<";";
		
		
		
		
		std::vector<std::string> params = {};
		auto mobj = mathS::Parse(input);
		auto f = assembler.Assemble(mobj, params);
				
		std::cout<<"mathS :"<<f({})->GetString()<<std::endl;
	std::string out=f({})->GetString();
	if(isnumber(out))
		return out;
	else
	return input;
}



void Token::performAction(Context &context){
	
	if(token_name=="R"){
		
		addVariable(var_name,arguments[0],arguments[1]);
		
	}
	else if(token_name=="S" || token_name=="Sr"){
		
		
		Scope *newScope = context.getCurrentScope();
		
		std::cout<<"***001";
		glm::vec3 size;
		
		if(var_names[0]!=""){
				
				size.x=atof(MathS2(var_names[0]).c_str());
				
				
		}
		else size.x=arguments[0];
		
		if(var_names[1]!=""){
				size.y=atof(MathS2(var_names[1]).c_str());
		}
		else size.y=arguments[1];
		
		if(var_names[2]!=""){
				
				size.z=atof(MathS2(var_names[2]).c_str());
		}
		else size.z=arguments[2];
		
		//std::cout << "S (" << size.x << ", " << size.y << ", " << size.z << ") ";
		
		//size *= newScope->getSize();
		//size = glm::vec3(abs(size.x), abs(size.y), abs(size.z));
		
		//std::cout << "S* (" << size.x << ", " << size.y << ", " << size.z << ") "<<std::endl;
		newScope->S(size);
		glm::vec3 pos = context.getCurrentScope()->getPosition();
    glm::vec3 contextSize = context.getCurrentScope()->getSize();
    std::cout << " -- Current scope -> POS: (" << pos.x << ", " << pos.y << ", " << pos.z << ") SIZE: (" << contextSize.x << ", " << contextSize.y << ", " << contextSize.z << ") " << std::endl;
		
	}
	else if(token_name=="T"){
		Scope *newScope = context.getCurrentScope();
		glm::vec3 position;
		
		if(var_names[0]!=""){
			position.x=atof(MathS2(var_names[0]).c_str());
				
		}
		else position.x=arguments[0];
		
		if(var_names[1]!=""){
				position.y=atof(MathS2(var_names[1]).c_str());
		}
		else position.y=arguments[1];
		
		if(var_names[2]!=""){
				position.z=atof(MathS2(var_names[2]).c_str());
		}
		else position.z=arguments[2];
		
		
				
		newScope->T(position);
		
	}
	else if(token_name=="*"){
		std::cout<<"***002";
			if(var_name!=""){
				int index=findVariable(var_name);
				if(index!=-1 ){
					if(variable_list[index]->max==variable_list[index]->min) removeVariable(var_name);
				}
				else std::cout<<"not found";
			}
	}
	else if(token_name=="+"){
		std::cout<<"***003";
			if(var_name!=""){
				
				 addVariable(var_name);
				}
				
			
	}
	else if(token_name=="I"){
	    glm::mat4 transform = context.getCurrentScope()->getTransform();
		Mesh instance = Mesh::getInstance(instance_type);
		instance.apply(transform);
		context.getScene().add(instance);
	}
	else if(token_name=="["){
		 context.pushScope();
		
	}
	else if(token_name=="]"){
		 context.popScope();
		
	}
	else if(token_name=="{"){
		 context.newScope();
		
	}
	else if(token_name=="}"){
		 context.popScope();
		
	}
	
	
	
}




class Grammar
{
public:
    Grammar(std::string filePath);
    ~Grammar(){}

    
	int findRule(std::string rule_name);
	
	std::vector<Token *> Recurse(Rule *rule);
	void generateGeometry();
	void ReadTokens(Rule *rule,std::string token_str,int i);

    std::vector<Rule *> rule_list;
	std::vector<Token *> tokens_new;
};






//////////////////////////////////Read Grammar Script from Grammar file/////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Grammar::ReadTokens(Rule *rule,std::string rule_str,int i){
	std::istringstream lin(rule_str);
			
			std::string token_str;
			float value;
			
			while(lin>>token_str){
				Token *token;
				if(token_str=="S" || token_str=="T" || token_str=="Sr" ){
					token=new Token(token_str);
					
					lin>>token_str;
					if(token_str=="("){
						
						
						
						for(int i=0;i<3;i++){
							lin>>token_str;
							
							
							
							if(!isnumber(token_str)){
								
								token->var_names[i]=token_str;
							     value=0.0f;
							}
							else {
								token->var_names[i]="";
								value=atof(token_str.c_str());
							}
							token->addArgument(value);
						}
						lin>>token_str;
						if(token_str!=")"){
							std::cout<<"Error-expected ')' 001 "<<std::endl;
							exit(1);
							}
					rule->addToken(token,i);
					}
					else {
						std::cout<<"Error-expected '(' 001"<<std::endl;
						exit(1);
						
					}
				}
				else if(token_str=="R"){
					token=new Token(token_str);
					lin>>token_str;
				
					token->var_name=token_str;
					
					
					
					
					lin>>token_str;
					
					if(token_str=="("){
						
						for(int i=0;i<2;i++){
							lin>>token_str;
							
							value=atof(token_str.c_str());
							token->addArgument(value);
						}
						
						lin>>token_str;
						if(token_str!=")"){
							std::cout<<"Error-expected ')' 002 "<<std::endl;
							exit(1);
						}
					
					
		
					addVariable(token->var_name,token->arguments[0],token->arguments[1]);
					
					rule->addToken(token,i);
					}
					else {
						std::cout<<"Error-expected '(' 002"<<std::endl;
						exit(1);
					}
				}
				else if(token_str=="I"){
					token=new Token(token_str);
					lin>>token_str;
					if(token_str=="("){
						lin>>token_str;
						if(token_str=="Cube" || token_str=="Sphere" || token_str=="Cylinder"){
							
							token->addInstanceType(token_str);
						}
						else {
							std::cout<<"Error-expected ')' 003 "<<std::endl;
							exit(1);
						}
						
						
						lin>>token_str;
						if(token_str!=")"){
							std::cout<<"Error-expected ')' 003 "<<std::endl;
							exit(1);
							}
						rule->addToken(token,i);
			
					}
					
				}
				else if(token_str=="*"){
					token=new Token(token_str);
					lin>>token->var_name;
					
					
					
					rule->addToken(token,i);					
				}
				else if(token_str=="+"){
					token=new Token(token_str);
					lin>>token->var_name;
					
					
					
					rule->addToken(token,i);					
				}
				else if(token_str=="["){
					token=new Token(token_str);
					rule->addToken(token,i);
					
				}
				else if(token_str=="]"){
					token=new Token(token_str);
					rule->addToken(token,i);
				}
				else if(token_str=="{"){
					token=new Token(token_str);
					rule->addToken(token,i);
					
				}
				else if(token_str=="}"){
					token=new Token(token_str);
					rule->addToken(token,i);
				}
				else {
					
					token=new Token(token_str);
					rule->addToken(token,i);
					
						
					
					
				}
			
			std::cout<<"Token :"<<i<<":"<<token->token_name<<std::endl;	
			
			}
}


Grammar::Grammar(std::string filePath)
{

    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        std::cerr << "E: Could not open file " << filePath << std::endl;
        return;
    }

    std::string line;
    while(std::getline(fin, line) ) {

        if(line!=""){
			size_t pos = 0;
			std::string token;

			std::string delimiter = "->";

			pos = line.find(delimiter);
			
			std::string rule_str = line.substr(0, pos);
			line.erase(0, pos + delimiter.length());
			std::istringstream lin(rule_str+" -> ");
			
			std::cout<<"line"<<line<<".";
			
			
			char token_char;
			std::string token_str;
			float value;
			std::string rulename;
			lin >> rulename;
			Rule *rule;
			if(rulename!=""){
			
				rule=new Rule(rulename,1);
				
				lin >> token_str;
				
				int repeat;
				
			
			
				if(token_str!=";" && token_str!="->"){
				if(!isnumber(token_str)){
					
					rule->var_name=token_str;
					repeat=1;
				}
				else {
					rule->repeat=atoi(token_str.c_str());
				}
				lin >> token_str;
				}
					
			
				do
				{
				 if(token_str==";" || token_str=="->")break;
					
					if(findVariableForward(token_str)!=-1){
						rule->var_names[rule->var_counter]=token_str;
						rule->var_counter++;
					}
					lin >> token_str;
				//	std::cout<<token_str;
				}
				while(true);
				
				if(token_str==";"){
					lin >> token_str;
				
					if(isnumber(token_str)){
						rule->probability=atof(token_str.c_str());
					std::cout<<"PROBABILITY:"<<rule->probability<<std::endl;
					}
				}
				
				
				std::cout<<"Rule :"<<rulename<<std::endl;
				
				rule_list.push_back(rule);
			
			
			
				delimiter = "|";

				std::vector<std::string> sections;
				int num_sections=0;
				while ((pos = line.find(delimiter)) != std::string::npos) {
					sections.push_back( line.substr(0, pos));
						
					line.erase(0, pos + delimiter.length());
					std::cout<<"line"<<line<<".";
					num_sections++;
				}
				
				if( line != "") sections.push_back( line.substr(0, pos));


				if(num_sections==0){
					ReadTokens(rule,sections[0],1);
				}
				else {
					num_sections++;
					if(num_sections>3)num_sections=3;
					
					for(int i=0;i<num_sections;i++){
					
						ReadTokens(rule,sections[i],i);
					
					}
			
				}
			}
	  }
	}
	
	
	

		
		
		tokens_new=Recurse(rule_list[0]);
		
	
		
		
	
	
	
	
}




std::vector<Token *> Grammar::Recurse(Rule *rule){
	
		
		std::cout<<"Rule Name: "<<rule->rule_name;
		
		std::vector<Token *> new_tokens;
		
	
		
		
		for(int i=0;i<rule->var_counter;i++){
			
			int index=findVariableForward(rule->var_names[i]);
		
			addVariable(variable_list[index]->var_name);
		}
		
		std::cout<<"Rule Var Name: "<<rule->var_name;
		
		
		int index=findVariable(rule->var_name);
		if(index!=-1){
		
		

				float val=variable_list[index]->getValue();
				rule->repeat=floorf(val);	
			
			
			
		}
	
		
		
		std::cout<<"REPEAT="<<rule->repeat<<std::endl;
			
		for(int m=0;m<3;m++){
		
		
				if(m==1){
					for(int l=0;l<rule->repeat;l++){
		
						for(int j=0;j<rule->section_tokens[m].size();j++){
						
							if(rule->section_tokens[m][j]->isRule()!=""){
								int index=findRule(rule->section_tokens[m][j]->token_name);
								if(index!=-1){
									Rule *fromtoken = rule_list[index];
									float prob=(rand()/(float)RAND_MAX);
									std::cout<<"PROB"<<prob<<">"<<fromtoken->probability<<std::endl;
									if(fromtoken->probability>=prob){
									std::vector<Token *> more_tokens=Recurse(fromtoken);
									for(int k=0;k<more_tokens.size();k++){
										Token *check_token=more_tokens[k];
										if(check_token->token_name=="S" || check_token->token_name=="T"){ //replace variable with value
											
											for(int i=0;i<3;i++){
												if(check_token->var_names[i]!=""){
													
													check_token->arguments[i]=atof(MathS(check_token->var_names[i]).c_str());
													
													check_token->var_names[i]="";
												}
											}
										}		
										
										new_tokens.push_back(more_tokens[k]);
									}
								}
							  }
						
							}
							else {
								Token *check_token=new Token(rule->section_tokens[m][j]);
								if(check_token->token_name=="S" || check_token->token_name=="T"){ //replace variable with value
									
									for(int i=0;i<3;i++){
										if(check_token->var_names[i]!=""){
											check_token->arguments[i]=atof(MathS(check_token->var_names[i]).c_str());
											
											check_token->var_names[i]="";
										}
									}
								}
								new_tokens.push_back(check_token);
							}
							
							
						  }
					}
				}
				else {
						for(int j=0;j<rule->section_tokens[m].size();j++){
						
							if(rule->section_tokens[m][j]->isRule()!=""){
								int index=findRule(rule->section_tokens[m][j]->token_name);
								if(index!=-1){
									Rule *fromtoken = rule_list[index];
									std::vector<Token *> more_tokens=Recurse(fromtoken);
									for(int k=0;k<more_tokens.size();k++){
										Token *check_token=more_tokens[k];
										if(check_token->token_name=="S" || check_token->token_name=="T"){ //replace variable with value
											
											for(int i=0;i<3;i++){
												if(check_token->var_names[i]!=""){
													
													check_token->arguments[i]=atof(MathS(check_token->var_names[i]).c_str());
													
													check_token->var_names[i]="";
												}
											}
										}		
										new_tokens.push_back(more_tokens[k]);
									}
								}
						
							}
							else {
								
								
								
								Token *check_token=new Token(rule->section_tokens[m][j]);
								if(check_token->token_name=="S" || check_token->token_name=="T"){ //replace variable with value
									
									for(int i=0;i<3;i++){
										if(check_token->var_names[i]!=""){
											
											check_token->arguments[i]=atof(MathS(check_token->var_names[i]).c_str());
											
											check_token->var_names[i]="";
										}
									}
								}
								
								
								new_tokens.push_back(check_token);
							}
							
							
						  }
					
				} 
		}
		
		for(int i=0;i<rule->var_counter;i++){
			
			removeVariable(rule->var_names[i]);
		}

		return new_tokens;
		
}
	
	
	
	



int Grammar::findRule(std::string rule_name){
	std::cout<<"Searching for rule:  "<<rule_name<<std::endl;
	for(int i=0;i<rule_list.size();i++){
	
		if(rule_list[i]->rule_name.compare(rule_name)==0) {
			
			std::cout<<"Found "<<rule_name<<std::endl;
		return i;
		}
	
	}
	return -1;
}



void Grammar::generateGeometry()
{
    Context context;
    for(int k=0;k<tokens_new.size();k++){
		//tokens_new[k]->print();
			tokens_new[k]->performAction(context);
			
	}		
    PLYWriter::writeMesh( "test.ply", context.getScene());
}



int main(){
    srand(time(NULL));
    std::cout << std::fixed << std::showpoint;
	std::cout.precision(2);
    Grammar grammar("./test.grammar");
		grammar.generateGeometry();
}




















