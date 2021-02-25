#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include<stdio.h>
#include<stdlib.h>
#include <vector>
#include <map>
#include <set>
#include <math.h>
#include <locale>
#include <list>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <queue>
#include "parameters.h"
#include "PetriDish.h"
using namespace std;

class Node;
class Field
{

private:
	int max_row;
	int max_col;
public:
	sf::Vector2f default_location;
	float default_orientation;

	string filename = "*.png";
	vector < vector <int> > FieldData_row_col;
	void acceptImageAndInitializeEverything(sf::Image im, string n);
	bool isFieldDataInSynchWithTexture = 0;
	void update_field_data_from_rendertexture();
	sf::Image getImageFromFieldData() {
		sf::Image result;
		result = field_RenderTexture.getTexture().copyToImage();
		//add in default location
		sf::Color probe;
		probe = result.getPixel((unsigned int)default_location.x, (unsigned int)default_location.y);

		probe.r = probe.r / 255;
		probe.g = probe.g / 255;
		probe.b = probe.b / 255;
		result.setPixel((unsigned int)default_location.x, (unsigned int)default_location.y, probe);
		return result;
	};
	int getFieldValueAtParticularCartesianLocation(sf::Vector2f v) {
		

		if ((v.x > max_col) || (v.x < 0) || (v.y > max_row) || (v.y < 0)) { return 0; }
	//	db(FieldData_row_col[v.y][v.x]);
		return FieldData_row_col[(unsigned int)v.y][(unsigned int)v.x];
	}

	void clear();
	
	sf::Image field_image;
	sf::Image field_image_with_white_transparent;
	sf::Texture field_texture_with_white_transparent;
	sf::RenderTexture field_RenderTexture;
	sf::Sprite field_sprite;
	sf::Sprite field_sprite_with_white_transparent;
	sf::Sprite getFieldSprite() {
		return field_sprite;	}

	Field(int rows, int cols);
	~Field();
};

