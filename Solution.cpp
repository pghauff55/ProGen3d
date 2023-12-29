

#include "Solution.h"
#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
#include <cmath>
#include <sstream>


Solution::Solution(){
	
	   ops[0]="+";
	    ops[1]="*";
	    ops[2]="/";
	    ops[3]="(";
	    ops[4]=")";
	    ops[5]="^";
	
	
}
float Solution::Process(std::string input){
	//std::cout<<input<<std::endl;
		std::string A="";
		int pos1=input.find("-");
		if(pos1==0){
			A="-";
			input.erase(0,  1);
			}
		while((pos1=input.find("-"))!=-1){
			std::string B;
				B=input.substr(0, pos1);
					input.erase(0, pos1 + 1);
				A+=B+"+-";
				
			
		}
		A+=input;
	//std::cout<<A<<std::endl;	
	  float afloat=process(A);
	  if(afloat<0.000001 && afloat >-0.000001)afloat=0.0f;
		return afloat;
}
float Solution::process(std::string input){
		
		
		//std::cout<<input<<std::endl;
	 	int pos[10];
		
		bool isnumeric=true;
		for(int i=0;i<6;i++){
			pos[i]=input.find(ops[i]);
			if(pos[i]!=-1)isnumeric=false;
		}
		if(isnumeric==true)return atof(input.c_str());
		
		
			if(pos[0]!=-1){
					std::string A=input.substr(0, pos[0]);
					input.erase(0, pos[0] + 1);
					
				return process(A)+process(input);
				
			}
			else {
				if(pos[1]!=-1 && pos[2]!=-1){
					if(pos[1]<pos[2]){
						std::string A=input.substr(0, pos[1]);
						input.erase(0, pos[1] + 1);
					
						return process(A)*process(input);
					}
					else {
						std::string A=input.substr(0, pos[2]);
						input.erase(0, pos[2] + 1);
					
						float ret=process(input);
					
						if(ret!=0.0f)return process(A)/ret;
						else return 0.0f;
						}
					
				}
				else if(pos[1]!=-1){
					std::string A=input.substr(0, pos[1]);
					input.erase(0, pos[1] + 1);
					
				return process(A)*process(input);
					
				}
				else if(pos[2]!=-1){
					std::string A=input.substr(0, pos[2]);
					input.erase(0, pos[2] + 1);
					float ret=process(input);
					
				if(ret!=0.0f)return process(A)/ret;
				else return 0.0f;
					
				}
				else {
					if(pos[3]!=-1){
						if(pos[4]!=-1){
						std::string A=input.substr(pos[3]+1, pos[4]-1);
						return process(A);
						}
						else return 0.0f;
					}
					else if(pos[5]!=-1){
								std::string A=input.substr(0, pos[5]);
								input.erase(0, pos[5] + 1);
					
						return pow(process(A),process(input));
					}
				}
				
				
			}
			
		return 0.0f;	
}




Noise2d::Noise2d(int numOctaves,int persistence){
	this->numOctaves=numOctaves;
	this->persistence=persistence;
}

double Noise2d::Noise(int i, int x, int y) {
  int n = x + y * 57;
  n = (n << 13) ^ n;
  int a = primes[i][0], b = primes[i][1], c = primes[i][2];
  int t = (n * (n * n * a + b) + c) & 0x7fffffff;
  return 1.0 - (double)(t)/1073741824.0;
}

double Noise2d::SmoothedNoise(int i, int x, int y) {
  double corners = (Noise(i, x-1, y-1) + Noise(i, x+1, y-1) +
                    Noise(i, x-1, y+1) + Noise(i, x+1, y+1)) / 16,
         sides = (Noise(i, x-1, y) + Noise(i, x+1, y) + Noise(i, x, y-1) +
                  Noise(i, x, y+1)) / 8,
         center = Noise(i, x, y) / 4;
  return corners + sides + center;
}

double Noise2d::Interpolate(double a, double b, double x) {  // cosine interpolation
  double ft = x * 3.1415927,
         f = (1 - cos(ft)) * 0.5;
  return  a*(1-f) + b*f;
}

double Noise2d::InterpolatedNoise(int i, double x, double y) {
  int integer_X = x;
  double fractional_X = x - integer_X;
  int integer_Y = y;
  double fractional_Y = y - integer_Y;

  double v1 = SmoothedNoise(i, integer_X, integer_Y),
         v2 = SmoothedNoise(i, integer_X + 1, integer_Y),
         v3 = SmoothedNoise(i, integer_X, integer_Y + 1),
         v4 = SmoothedNoise(i, integer_X + 1, integer_Y + 1),
         i1 = Interpolate(v1, v2, fractional_X),
         i2 = Interpolate(v3, v4, fractional_X);
  return Interpolate(i1, i2, fractional_Y);
}

double Noise2d::ValueNoise_2D(double x, double y) {
  double total = 0,
         frequency = pow(2, this->numOctaves),
         amplitude = 1;
  for (int i = 0; i < numOctaves; ++i) {
    frequency /= 2;
    amplitude *= persistence;
    total += InterpolatedNoise((primeIndex + i) % maxPrimeIndex,
        x / frequency, y / frequency) * amplitude;
  }
  return total / frequency;
}

