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
using namespace std;


#define db(x) cout << #x " = " << x << "\n";
#define mh() cout<<"Made it to "<<__FILE__<<"  Line: "<<__LINE__<<"\n";
//#define mh() cout<<"Made it to "<<__LINE__<<"\n";
#define sp() system("pause");

#define CURRENT_AREA_ENERGY 0
#define AREA_ENERGY_ABSENT_SINGLE_NODE -1
#define AREA_ENERGY_PLUS_SINGLE_NODE 1
#define MAX_NUMBER_OF_FIELDS 5

#define Order_I 4
#define Order_II 8
#define Order_III 12
#define Order_IV 20

#define Current_State_Index 0
#define Entering_State_Index 1

#define State_to_Change_Into_Index 2
#define Die_Flag_Index 3

#define Field_Active_Flag_Index 4

#define Mitosis_Flag_Index 5
#define Mitosis_Cleavage_Vector_Signifier_Index 6
#define Mitosis_Cleavage_Index 7
#define Mitosis_State_to_Change_Into_Index 8
#define Mitosis_Cleavage_Offset_Angle_Index 9
#define Mitosis_CW_0_CCW_1_Index 10

#define Actual_Area_Index 11
#define Red_Index 12
#define Green_Index 13
#define Blue_Index 14
#define Goal_Area_Index 15
#define Bulk_Modulus_Index 16
#define Typical_Area_Index 17
#define PP_SCALED_WIDTH_Index 18
#define PP_SCALED_HEIGHT_Index 19
#define First_User_Variable_Index 20


#define		gui_MOUSE_RIGHT_BUTTON 0
#define		gui_MOUSE_LEFT_BUTTON 1


#define		gui_TAB_SORTER 2
#define		gui_TAB_MONITOR 3
#define		gui_TAB_FIELD 4
#define		gui_TAB_MANIPULATE 5
#define		gui_TAB_EDITOR 6

#define		gui_LD_LOAD_BUTTON 7
#define		gui_SD_SAVE_BUTTON 8

#define		gui_ED_LOAD_BUTTON 9
#define		gui_ED_COMPILE_BUTTON 10
#define		gui_ED_SAVE_BUTTON 11
#define		gui_ED_TEXT_CHANGED 12

#define		gui_WTS_SAVE 13
#define		gui_WTS_CANCEL 14
#define		gui_WTS_DISCARD 15

#define		gui_IT_ITERATE_BUTTON 16
#define		gui_IT_STEP_RADIO_BUTTON 17
#define		gui_IT_MOMENTARY_RADIO_BUTTON 18
#define		gui_IT_TOGGLE_RADIO_BUTTON 19
#define		gui_IT_MEMBRANE_FLUX_SLIDER 20
#define		gui_IT_MEMBRANE_SPEED_SLIDER 21
#define		gui_IT_STATE_COMBOBOX 22
#define		gui_IT_MANUAL_PLACEMENT_CHECKBOX 23
#define		gui_IT_CLEAR 24
#define		gui_IT_DRAW_GRADIENT_CHECKBOX 25
#define		gui_IT_USE_FIELD 26
#define		gui_IT_DRAW_FIELD 27
#define		gui_IT_FANCY 28


#define		gui_CR_DEBUGCHECKBOX 30

#define		gui_CS_CLEAR_AND_PLACE 31


#define		gui_FIELD_ADJUST_PL_CHECKBOX 36
#define		gui_FIELD_LOAD 37
#define		gui_FIELD_SAVE 38
#define		gui_FIELD_CLEAR 39
#define		gui_FIELD_FV_0_RADIO 40
#define		gui_FIELD_FV_1_RADIO 41
#define		gui_FIELD_FV_2_RADIO 42
#define		gui_FIELD_FV_3_RADIO 43
#define		gui_FIELD_FV_4_RADIO 44

#define		gui_MANIP_SELECT_RADIO 46
#define		gui_MANIP_DESELECT_RADIO 47
#define		gui_MANIP_MOVE_RADIO 48
#define		gui_MANIP_KILL_BUTTON 49

#define		gui_SLIDER_TEMP 50
#define		gui_SLIDER_SPEED 51
#define		gui_SLIDER_YELLOW_SELF 52
#define		gui_SLIDER_YELLOW_YELLOW 53
#define		gui_SLIDER_YELLOW_MEDIUM 54
#define		gui_SLIDER_YELLOW_SIZE 55
#define		gui_SLIDER_BLUE_SELF 56
#define		gui_SLIDER_BLUE_BLUE 57
#define		gui_SLIDER_BLUE_MEDIUM 58
#define		gui_SLIDER_BLUE_SIZE 59
#define		gui_SLIDER_YELLOW_BLUE 60
#define		gui_SLIDER_BRUSH_SIZE 61
#define		gui_KNOB_ORIENTATION 62
#define		gui_SLIDER_ROWS 63
#define		gui_SLIDER_COLS 64
#define		gui_DRAW_VECTOR_CHECKBOX 65

#define		gui_SYNTAX_GUIDE_BUTTON 66

#define gui_IT_DRAW_ORIENTS 67
#define		gui_LAST_ENTRY 68

#define Mitosis_Cleavage_Using_Cells 0
#define Mitosis_Cleavage_Using_Variable 1
#define Mitosis_Cleavage_Using_Gradient 2

class Cell;

bool cheapo_Poisson(float x);
int positive_int_mod(int x, int m);
float random_float();
float random_float_range(float LO, float HI);
float squared_distance_between_vectors(sf::Vector2f a, sf::Vector2f b);
template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

string debug_print_sfml_vector(sf::Vector2f v);
string debug_print_sfml_vector(sf::Vector2i v);
string debug_print_vector(vector <string> v);
int wrap_around_mod(int i, int N);
float wrap_around_float_mod(float i, float N);
void wrap_around_sfvector_fmod(sf::Vector2f&  i, sf::Vector2f& N);
float smallest_spread_between_two_angles(float a, float b);
float add_a_smidgen_of_an_angle_to_original(float original, float angle_to_add, float smidgen);
//this container is like an unordered Set. Can only take unique values.
//No special flags or errors raised if trying to delete something that's not
//there or trying in insert a duplicate.
//Its a bit faster than a real Set for small numbers (maybe less than 100) of members.
//There's no good way to iterate through data without 'getting' the data and then iterating
//through the returned vector... that's sort of costly. Probably a way to override something
//and create a special iterator for this custom class. Haven't done that yet.
template <class T>
class special_vector {
private:
	vector <T> data;
public:
	vector <T> getData() { return data; }
	void clear() {
		data.clear();
	}

	void insert(T x) {
		if (std::find(data.begin(), data.end(), x) == data.end()) {
			data.push_back(x);
		}
	}
	void erase(T x) {
		if (find(data.begin(), data.end(), x) != data.end()) {
			data.erase(find(data.begin(), data.end(), x));
		}
	}
	bool isInSpecialVector(T x) {
		if (find(data.begin(), data.end(), x) != data.end()) { return true; }
		return false;
	}
	unsigned int size() {
		return data.size();
	}
	void append(special_vector<T>& v) {
		for (auto dd : v.data) {
			insert(dd);
		}
	}

};
struct Rule_Text {

	string text;
	string filename;
	bool isSaved;
	bool isCompiled;
	bool getIsSaved() {
		if ((text == "") || (isSaved)) { return true; }return false;
	}
	bool getIsCompiled() { return isCompiled; }
	Rule_Text() { text = ""; filename = "*.cyr"; isSaved = 0; isCompiled = 0; }
};

struct Init_Placement {
	int cell_type;
	int size;
	int row;
	int col;
};


void normalize_vector(sf::Vector2f & v);
float magnitude_vector(sf::Vector2f & v);
float dot_product_vectors(sf::Vector2f & a, sf::Vector2f & b);
class Presumed_Position_Vector {
private:
	
public:
float orientation;

	sf::Vector2f scale_v;
	sf::Vector2f location;

		Presumed_Position_Vector() {
		location = sf::Vector2f(0, 0);
		scale_v = sf::Vector2f(1, 1);
		orientation = 0;
		
	}
};

sf::Vector2f rotate_vector_using_degrees(sf::Vector2f v, float angle);

float angle_in_degrees_of_vector(sf::Vector2f v); 

sf::Vector2f vector_from_degrees(float angle);



