#include "coordinate.h"
#include <cmath> 
#include <iostream>
/*
	Having something that stores coordinates together is much convenient wow.
*/
coordinate::coordinate(){
	x = 0; y = 0; z = 0;
	theta = 0;
};

coordinate::coordinate(double x_, double y_, double z_, double theta_){
	x = x_; y = y_; z = z_;
	theta = theta_;
};

//discounts theta, returns the length of the line between the two coords
double coordinate::distance(coordinate coordA, coordinate coordB){
	double X = std::pow( (coordA.x - coordB.x), 2);
	double Y = std::pow( (coordA.y - coordB.y), 2);
	double Z = std::pow( (coordA.z - coordB.z), 2);
return std::sqrt(X + Y + Z);
};

double coordinate::angle_towards(coordinate src, coordinate dest){
	//pretend that src is the origin of it's own plane
	//get the difference, this is the same as setting the origin to src
	coordinate diff = dest - src;
	//get the angle between the src and destination
return atan(diff.y/diff.x);
};

void coordinate::print(){
	std::cout << "x:" << x
			  << " y:"<< y 
			  << " z:"<< z 
			  << " theta:" << theta 
			  << std::endl;
};

coordinate coordinate::operator-(const coordinate &other){
	double x = this->x - other.x;
	double y = this->y - other.y;
	double z = this->z - other.z;
	double theta = this->theta - other.theta;
return coordinate(x,y,z,theta);
};

coordinate coordinate::operator+(const coordinate &other){
	double x = this->x + other.x;
	double y = this->y + other.y;
	double z = this->z + other.z;
	double theta = this->theta + other.theta;
return coordinate(x,y,z,theta);
};

bool coordinate::operator==(const coordinate &other){
	bool retVal = true;
		retVal = retVal && (this->x == other.x);
		retVal = retVal && (this->y == other.y);
		retVal = retVal && (this->z == other.z);
		retVal = retVal && (this->theta == other.theta); 
return retVal;
};

bool coordinate::operator!=(const coordinate &other){
	return !(*this == other);
};
