#include "map.h"
#include "robot.h"
#include "lodepng.h"
#include "map_utilities.h"
#include <iostream>
#include <vector>
#include <cmath>

/************Region Start*****************/
//something for the sections to point when asked what region they belong to.
	//a region should be something that is 
Map::Region::Region(){
	_explored = false;
	_id = next_id;
		next_id++;
};

	//what does it do? I wonder.
void Map::Region::add_section(Section * new_section){
	//if the region has the section already
		//then return and do nothing.
	for(int i = 0; i < _section_list.size(); i++)
		if(new_section == _section_list[i])
			return;
//otherwise add the section to the list
_section_list.push_back(new_section);	
};
	
unsigned Map::Region::next_id = 0;
/*****************Region END*************************/
/***************Section START*********************/
Map::Section::Section(){
	_corner_meter = coordinate(0,0,0,0);
	_explorable = false;
	_region = NULL;
};

	//takes in map, width, length and the coordinate the corner pixel resides.
Map::Section::Section(
	unsigned char ** map, 
	unsigned map_w, unsigned map_l, 
	coordinate corner_pixel

){
	_region = NULL;
	//translate the coordinate from pixels to meters
		double x_meters = corner_pixel.x * pixels_per_meter;
		double y_meters = corner_pixel.y * pixels_per_meter;
	_corner_meter = coordinate(x_meters, y_meters, 0, 0);

	//find the length/width of the section in pixels
	double length_pixels = ceil(_length_meters*pixels_per_meter);
	double width_pixels = ceil(_width_meters*pixels_per_meter);

	//assume the section is non-explorable, innocent until proven guilty
	_explorable = false;

	//if the section's boundaries extend past the image's boundaries
		double section_y_upper = corner_pixel.y+length_pixels;
	if(corner_pixel.y < 0 || section_y_upper > map_l) return;
		
		double section_x_upper = corner_pixel.x+width_pixels;
	if(corner_pixel.x < 0 || section_x_upper > map_w) return;

	//for each pixel in the square surrounding section_center
		//if there is an obstacle, the section is indeed non-explorable.
	for(int i = corner_pixel.y; (i < section_y_upper); i++)
		for(int j = corner_pixel.x; (j < section_x_upper); j++)
			if(map[i][j] != 255) return;
	
	//if all goes well the section is explorable
	_explorable = true;
};
	
	//returns false if it failed to set region
bool Map::Section::set_region( Region * new_region){
	if(_region != NULL) return false;
	_region = new_region;	
return true;	
};

bool Map::Section::contains_coord(coordinate coord){
	double range_x_lower = _corner_meter.x; 
	double range_x_higher = range_x_lower + width_meters();
	if(coord.x < range_x_lower || coord.x > range_x_higher)
		return false;

	double range_y_lower = _corner_meter.y;
	double range_y_higher = range_y_lower + length_meters();
	if(coord.y < range_y_lower || coord.y > range_y_higher)
		return false;
return true;
};
//it is best to tune NEAREST to produce the smallest map but also so that
	//it doesn't produce extreme false positives 0.5 and 1 seem to work
#define NEAREST 1
double Map::Section::_length_meters = ceil_to(NEAREST, Robot::length());
double Map::Section::_width_meters = ceil_to(NEAREST, Robot::width());
/******************Section END***************/

/****************Start Map********************/
/* The functions in class Map */

/*map constructor*/
Map::Map(const char * mapFilename){
	// load the pixel map
	unsigned char * pixel_map = NULL;
		unsigned width = 0;
		unsigned length = 0;

	load_pixel_map(mapFilename, pixel_map, width, length);
	load_section_map(pixel_map, length, width, 
					_section_map, _section_map_w, _section_map_l);
	assign_regions(_section_map, _section_map_w, _section_map_l, 
					_region_list, &_region_list_size);
};


//this is really just a wrapper function for the lodepng
	//I should probably kill it.
void Map::load_pixel_map(
	const char * mapFilename, 
	unsigned char * &pixel_map, 
	unsigned &w, unsigned &l
){
	//for the sake of safty I'm using the function that returns an error
	std::vector<unsigned char> image;
	unsigned error = lodepng::decode(image, w, l, mapFilename, LCT_GREY);
	if(error != 0) 
		std::cout << "error " << lodepng_error_text(error) << std::endl;
	//set the pixel map to be the value that the vector holds
	pixel_map = &image[0];	
};

void Map::load_section_map(
	unsigned char * pixel_map_1D, 
	unsigned pixel_w, unsigned pixel_l, 
	Section ** &_section_map,
	unsigned &section_w, unsigned &section_l
){
	//translate the 1D pixel_map into a 2D pixel_map
	unsigned char ** pixel_map_2D = 
		alloc_2D_array(pixel_map_1D, pixel_w, pixel_l);

	//calculate the width and length of the _section_map
		double width_pixels = ceil(Section::width_meters()*pixels_per_meter);
	section_w = ceil(pixel_w/width_pixels);

		double length_pixels = ceil(Section::length_meters()*pixels_per_meter);
	section_l = ceil(pixel_l/length_pixels);
	
		//set up the section map and fill it up
	_section_map = new Section*[section_l];
	for(int i = 0; i < section_l; i++){
			//new row in the section map
		_section_map[i] = new Section[section_w];
			//the y coordinate of the center
		double corner_y = i*length_pixels;
		
		for(int j = 0; j < section_w; j++){
				//the x coordinate of the center
			double corner_x = j*width_pixels;
			coordinate corner = coordinate(corner_x, corner_y, 0, 0);
				//set the specified section to be a new section
			_section_map[i][j] = 
				Section(pixel_map_2D, pixel_w, pixel_l, corner);
		}
	}
//deallocate the 2D array made earlier. No need for the raw image.
dealloc_2D_array(pixel_map_2D, pixel_w, pixel_l);
};

//on the to do list obviously
void Map::assign_regions(
	Section ** section_map, unsigned w, unsigned l,
	Region * region_array, unsigned * num_regions
){
		

};

//returns a random coordinate whose section is unexplored.
coordinate Map::generate_random_coord(){

return coordinate(0,0,0,0);
};

//prints out a map of traversible and non-traversible sections
void Map::print_section_map(){
	//for each section in section map
	for(int i = 0; i < _section_map_l; i++){
		for(int j = 0; j < _section_map_w; j++){
			Section s = _section_map[i][j];
			std::cout << s.explorable();
		}
		std::cout << std::endl; 
	}
};

//prints out a map of sections designated by their region's id
void Map::print_region_map(){
	for(int i = 0; i < _section_map_l; i++){
		for(int j = 0; j < _section_map_w; j++)
			std::cout << (_section_map[i][j].region())->id();
		std::cout << std::endl;
	}
};

bool Map::map_explored(){
	//for now only it only returns false;
	//for each region A in the region list
		//if that region is unexplored return false
//if all else fails the map has been explored
return false;
};

/***********************End Map*****************/
