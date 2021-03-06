#include "navigator.h"
#include "coordinate.h"
#include "map.h"
	#include <vector>
	#include <stack>
	#include <limits>
	#include <iostream>
#define ROOT_INDEX 0;
using namespace std;
Navigator::Navigator(Map * map, coordinate root){
	//set the map
	_map = map;
	robot_location = root;	
	//construct the root node and add it to the tree
	add_node(root, NULL);
	//initialize the waypoint stack with a new goal
	coordinate goal = next_goal();
	plan_path_to_goal(goal);
};

//get the waypoint at the top of the queue given the current location
	//this function assumes that the robot has either reached it waypoint
	//or completely failed to reach it. No in betweens.
coordinate Navigator::next_waypoint(coordinate current_location){
	//set the robot_location to be location.
	robot_location = current_location;
	//if the robot is not in tree then it either it failed or 
		//achieved it's goal. start over.
	if(!in_tree(robot_location)){
		int parent = closest_in_tree(robot_location);
		add_node(robot_location, parent);
		waypoints.clear();
	}

	if(!waypoints.empty()){
		//if the waypoint has been reached remove from stack 
			//otherwise the robot was blocked
		if(robot_location == waypoints.top()) waypoints.pop();
	}
	//if the queue is empty
		//get the next goal
		//plan the path to that goal
	if(waypoints.empty()){
		coordinate goal = next_goal();
		plan_path_to_goal(goal);
	}
//return the next element in the stack
return waypoints.top();
};

//get's the next goal checks
coordinate Navigator::next_goal(){
	//if the map has been explored then the robot is done.
	if(_map->map_explored()) return robot_location;	
	//generate a new goal if the old one is inaccessible
	coordinate goal; bool cond;	
	do{ 
		goal = _map->generate_random_coord();
		// construct the end condition
		cond = !(_map->accessible(goal));
			cond = cond && !(in_tree(goal));
	}while(cond);	

return goal;
};

void Navigator::plan_path_to_goal(coordinate goal){
	//add the goal coordinate to the waypoint stack
	waypoints.push(goal);	
	//find the closest node in the tree to goal
	node n = *closest_in_tree(goal);
	//add that node's coordinate to the waypoint stack
	waypoints.push(n.coord);
	//while the current node is not the root node
	while(!root_node(n)){
		//set the current node to be the current node's parent
		n = tree[n.parent];
		//add the current node's coordinate to the waypoint stack
		waypoints.push(n.coord);
	}
	//create a new stack called sub_stack NOT replacing the waypoint stack
	std::stack<coordinate> s;
	//find the closest node in the tree to the current location
	n = *closest_in_tree(robot_location);
	//add that node's coordinate to the sub_stack
	s.push(n.coord);
	//while the current node is not the root node
	while(!root_node()){
		//set the current node to be the current node's parent
		n = tree[n.parent];
		//add the current node's coordinate to the sub_stack
		s.push(n.coord);
	}
	//until the sub_stack is empty
	while(!s.empty() && s.top() != tree[0].coord){
		//add the front of the sub_stack to the waypoint stack
		waypoints.push(s.top());
		//pop the front of the sub_stack
		s.pop();
	}
};

//create a new node and add it to the tree
void Navigator::add_node(coordinate coord, int parent_index){
	if(in_tree(coord)) return;
	node n;
		n.coord = coord;
		n.parent = parent_index;
		n.index = tree.size();
	tree.push_back(n);
	node y;
};

//checks to see if the coordinate is in the tree
bool Navigator::in_tree(coordinate coord){
//if the find_node function returns NULL return false
return find_node(coord) != -1;
};

//finds the node with the coresponding coordinate
int Navigator::find_node(coordinate coord){
	if(tree.empty()) return -1;
	//for each element in tree 
		//if the coordinate matches return the tree
	for(int i = 0; i < tree.size(); i++)
		if(tree[i].coord == coord) return i;
//all else fails return null
return -1;
};

int Navigator::closest_in_tree(coordinate goal){
	node * closest = &tree[0]; 
	double lowest_distance = std::numeric_limits<double>::max();
	for(int i = 0; i < tree.size(); i++){
		node n = tree[i]; 
		double cur_dist = coordinate::distance(goal, n.coord); 
		if(cur_dist < lowest_distance){
			lowest_distance = cur_dist;
			closest = &tree[i];
		}
	}
return closest.index;
};

void Navigator::print_tree(){
	//for each element in the tree
		//print the coordinate and the index of it's parent
	for(int i = 0; i < tree.size(); i++){
		node n = tree[i];
		(n.coord).print();
		std::cout << "\tparent index:"
				  << n.parent  
				  << std::endl;
	}
		
};
