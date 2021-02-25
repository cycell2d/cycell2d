#include <SFML/Graphics.hpp>
#define SFML_NO_DEPRECATED_WARNINGS
#include <TGUI/TGUI.hpp>
#include <filesystem>

#include <iostream>
#include <iomanip>
#include <stdio.h>
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
#include "Cell.h"
#include "Lab.h"
#include "parameters.h"
#include "User_Input.h"
#include "Window_Manager.h"
#include "RuleParser.h"
//#include "exptrk.h"

using namespace std;



string debug_print_sfml_vector(sf::Vector2f v) {
	stringstream result;

	result << "(" << v.x << ", " << v.y << ")";
	return result.str();
}
string debug_print_sfml_vector(sf::Vector2i v) {
	stringstream result;

	result << "(" << v.x << ", " << v.y << ")";
	return result.str();
}

string debug_print_vector(vector <string> v) {
	stringstream result;
	for (auto vi : v) {
		result << vi << "\n";
	}
	result << "\n";
	return result.str();
}


float random_float() {
	return  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float random_float_range(float LO, float HI) {
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}
float squared_distance_between_vectors(sf::Vector2f a, sf::Vector2f b) {

	return ((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));

};

float smallest_spread_between_two_angles(float a, float b)
{

	//returns absolute value.

	float result;
	if (a < 0) { a += 360; }
	if (b < 0) { b += 360; }
	if (a == b) { return 0; }

	if (fabs(a - b) == 180) { return 180; }
	if (a > b) {
		if (a - b > 180) {
			result = 360 - a + b;
			return result;
		}
		if (a - b < 180) {
			result = a - b;
			return result;
		}
	}
	if (a < b) {

		if (b - a > 180) {
			result = 360 - b + a;
			return result;
		}
		if (b - a < 180) {
			result = b - a;
			return result;
		}
	}
	return 0;
}
;
sf::Vector2f rotate_vector_using_degrees(sf::Vector2f v, float angle) {

	if (angle == 0) { return v; }
	angle = angle * 0.0174532f; //convert to radians
	float new_x, new_y, cos_po, sin_po;
	cos_po = cos(angle);
	sin_po = sin(angle);

	//Remember! The normal rotation matix is negative signs on Y as the SFML Y axis is reversed from normal trig
	new_x = v.x * cos_po - v.y * sin_po;
	new_y = v.x * sin_po + v.y * cos_po;

	return sf::Vector2f(new_x, new_y);
}
float wrap_around_float_mod(float i, float N) {
	i = fmod(i, N);
	if (i < 0) {
		i = N + i;
	}
	return i;
}
int wrap_around_mod(int i, int N) {
	bool wasNegative = false;
	if (i < 0) {
		wasNegative = true;
		i = -i; //there's definitely a bithack for this.
	}
	int offset = i % N;
	if (wasNegative) {
		if (offset == 0) {
			return 0;
		}
		else {
			return N - offset;
		}
	}
	return offset;
}


void wrap_around_sfvector_fmod(sf::Vector2f & i, sf::Vector2f& N) {

	if ((i.x < 0) || (i.x >= N.x)) {
		i.x = fmod(i.x, N.x);
		if (i.x < 0) {
			i.x = N.x + i.x;
		}
	}
	if ((i.y < 0) || (i.y >= N.y)) {
		i.y = fmod(i.y, N.y);
		if (i.y < 0) {
			i.y = N.y + i.y;
		}
	}




	return;

}

float add_a_smidgen_of_an_angle_to_original(float original, float angle_to_add, float smidgen) {


	original = wrap_around_float_mod(original, 360);
	angle_to_add = wrap_around_float_mod(angle_to_add, 360);
	float spread_angle=0;
	if ((original == angle_to_add) || (fabs(original - angle_to_add) == 180) || (original - angle_to_add == 0)) { return original; }
	//we only want to deal with the smaller angle on the circle.
	//So we first get that... and then we need to figure out if it is to the left or right of the original in order
	//to imnplement the correct sign.

	if (original > angle_to_add) {
		if (original - angle_to_add < 180) {
			spread_angle = -(original - angle_to_add);
		}
		if (original - angle_to_add > 180) {
			spread_angle = (360 - original) + angle_to_add;
		}

	}
	if (original < angle_to_add) {
		if (angle_to_add - original < 180) {
			spread_angle = angle_to_add - original;
		}
		if (angle_to_add - original > 180) {
			spread_angle = -((360 - angle_to_add) + original);
		}
	}


	original += spread_angle * smidgen;
	return wrap_around_float_mod(original, 360);

}
void normalize_vector(sf::Vector2f & v) {

	float mag = (float)(pow(v.x*v.x + v.y*v.y, .5));
	if (mag == 0) {
		v.x = 0;
		v.y = 0;
		return;
	}
	v = v / mag;
}

float magnitude_vector(sf::Vector2f & v)
{
	return 	 (float)pow(v.x*v.x + v.y*v.y, .5);
}

float dot_product_vectors(sf::Vector2f & a, sf::Vector2f & b) {

	return a.x*b.x + a.y*b.y;
}


float angle_in_degrees_of_vector(sf::Vector2f v) {
	//For some stupid reason, this program deals with global angles as follows:
	//12:00 is 0 degrees. Angles increases as you turn clockwise.
	float angle;
	angle = 57.3f * atan2(v.x, -v.y);
	if (angle <= 0) { angle = -angle; }
	else { angle = 360 - angle; }
	return angle;

}
sf::Vector2f vector_from_degrees(float angle) {
	//For some stupid reason, this program deals with global angles as follows:
//12:00 is 0 degrees. Angles increases as you turn clockwise.
	//Also, the coorindates of the vector match up with the inverted y-axis on the graphics screen.

	angle = angle / 57.3;
	return sf::Vector2f(sin(angle), -cos(angle));

}
int positive_int_mod(int x, int m) {
	return ((x%m + m) % m);
}


bool cheapo_Poisson(float x) {
	if (x == 0) { return 0; }
	if (x == 1) { return 1; }
	float r = 1 / x;

	if (random_float_range(0, 1) < r) {
		return 1;
	}
	return 0;
}
int main()
{
	//float ts = 38;

	//int count = 1;
	//vector <int> intervals;
	//intervals.clear();
	//for (int x = 0; x < 100000; x++) {
	//	bool test = poss(ts);

	//	//db(test);


	//	if (test == 1) {
	//	//	cout << "push back: " << count << "\n";
	//		intervals.push_back(count);
	//		count = 1;
	//	}
	//	else {
	//		count++;
	//	}
	//}
	//
	//float average = 0;
	//for (auto in : intervals) {
	//	average += in;
	//}
	//average = average / (float)intervals.size();
	//db(average);
	//sp();
	////create code from Syntax.txt
	////(Got tired of trying to get resources to work... kept crashing VS for some reason.)
	//stringstream result;
	//string line;
	//ifstream myfile("Syntax.txt");
	//if (myfile.is_open()) {
	//	while (myfile.good()) {
	//		getline(myfile, line);
	//		string s_holder = "";
	//		for (int x = 0; x < line.size(); x++) {
	//			if (line.substr(x,1) == "\"") {
	//				s_holder+="\\\"";
	//			}
	//			else { s_holder += line.substr(x, 1); }
	//		}
	//		cout << "syntax_holder.push_back(\"" << s_holder << "\\n\");\n";
	//	}
	//}
	//else { cout << "bad file name\n"; }
	//myfile.close();

	//sp();




	////create code from TGUI_form.txt
	////(Got tired of trying to get resources to work... kept crashing VS for some reason.)
	//stringstream result;
	//string line;
	//ifstream myfile("tgui2.txt");
	//if (myfile.is_open()) {
	//	while (myfile.good()) {
	//		getline(myfile, line);
	//		string s_holder = "";
	//		for (int x = 0; x < line.size(); x++) {
	//			if (line.substr(x,1) == "\"") {
	//				s_holder+="\\\"";
	//			}
	//			else { s_holder += line.substr(x, 1); }
	//		}
	//		cout << "form_holder.push_back(\"" << s_holder << "\\n\");\n";
	//	}
	//}
	//else { cout << "bad file name\n"; }
	//myfile.close();

	//sp();



	srand(143223);

	User_Input * ui = new User_Input();


	Window_Manager * window_manager = new Window_Manager(1000, 850);



	ui->create_widgets(window_manager->getWindow());


	Lab * my_lab = new Lab(8 * 40, window_manager, ui);











	while (window_manager->getWindow().isOpen())
	{

		ui->poll(window_manager->getWindow());
		window_manager->update(ui);
		if (window_manager->isResizeNeeded()) {
			my_lab->window_resize_update();
		}
		my_lab->update();
	

		my_lab->draw();
		window_manager->getWindow().display();
	}


	return 0;
}








