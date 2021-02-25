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
#include "Window_Manager.h"
#include "User_Input.h"
#include "RuleParser.h"
#include "parameters.h"
#include "Field.h"
using namespace std;

struct IO_Flags;
class Gui;
class PetriDish;
class Node;

#define Start_State 0
#define Sorter_State 1
#define Editor_State 2
#define Field_State 3
#define Manipulate_State 4
#define Monitor_State 5
#define Wish_To_Save_Text_State 6
#define Save_Text_State 7
#define Load_Text_State 8
#define Save_Field_State 9
#define Load_Field_State 10

class Lab
{

private:
	int markov_count = 0;
	int picture_count = 0;
	Window_Manager * window_manager;
	
	float speed;
	PetriDish* petri;

	Node * node_mouse_is_on();
	string debug_info;
	bool cell_is_selected_by_mouse = 0;

	float petri_width;
	float petri_height;
	sf::Vector2f petri_pos;
	sf::Vector2f petri_ratio;
	float status_bar_height;
	float status_bar_width;
	float border;
	tgui::Gui gui;
	User_Input * ui;
	string rule_text = "";
	string current_file_name = "";
	string load_file(string fn);
	bool save_file(string name);
	

	Rule_Text user_rules;
	vector <Init_Placement> cell_placement_coordinates;
	void place_cells_automatically();
	int file_type_flag;  //0 = rules, 1 = positions


	Cell * cell_being_dragged = NULL;
	sf::Time lab_time;
	sf::Time petri_time;
	sf::Clock lab_clock;
	sf::Clock petri_clock;
	vector <float> petri_time_average;
	vector <float> lab_time_average;


	int coming_from_lab_state = 0;
	void state_change_New_Former(int x, int y) { entering_lab_state = 1; coming_from_lab_state = y; lab_state = x; }
	bool isState(int x) { if (lab_state == x) { return 1; }return 0; }
	stringstream debug_message2;



	vector <bool> one_shot_switch;
	vector <bool> momentary_switch;
	vector <bool> toggle_switch;
	vector <float> values_switch;


	bool entering_lab_state = 1;
	bool moving_cells_obstructed = 1;
	bool moving_cells_mouse_down_toggle = 0;
	float old_orientation = 0;
public:
	Field* FieldObject;
	bool areMovingCellsObstructed() {
		return moving_cells_obstructed;
	}
	vector <pair<Cell *,sf::Vector2f > > cells_being_moved;

	int lab_state = 0;

	bool isManipPanelOpen = 0;
	special_vector <Cell*> cells_selected;

	Node * oldNodeMouseWasOn;
	Node* newNodeMouseIsOn;
	bool isDebugOn = 0;
	bool isDrawGradientOn() {
		return toggle_switch[gui_IT_DRAW_GRADIENT_CHECKBOX];
	}
	bool isDrawFieldOn() {
		return toggle_switch[gui_IT_DRAW_FIELD];
	}
	bool isFieldInUse() {
		return toggle_switch[gui_IT_USE_FIELD];
	}
	bool isDrawVectorOn() {
		return toggle_switch[gui_DRAW_VECTOR_CHECKBOX];
	}
	bool isDrawOrientOn() {
		return toggle_switch[gui_IT_DRAW_ORIENTS];
	}
	bool load_field_file(sf::Image & im, string n);
	float blue_goal_area;
	float yellow_goal_area;
	void load_state_names_to_GUI(vector <string> s) { ui->load_state_names_to_GUI(s); }
	string get_selected_state_name_from_combo_box() { return ui->get_selected_state_name_from_combo_box(); }
	float get_petri_width() { return petri_width; }
	float get_petri_height() { return petri_height; }
	sf::Vector2f get_petri_pos() { return petri_pos; }

	void update();


	void start_state();
	void sorter_state();
	void monitor_state();
	void field_state();
	void manipulate_state();
	void editor_state();
	void load_text_state();
	void save_text_state();
	void load_field_state();
	void save_field_state();
	void wish_to_save_text_state();
	void updatePetri();

	void window_resize_update();
	void draw();
	void draw_grid(Window_Manager * w_m);
	Lab(int rows, Window_Manager * w_m, User_Input * u_i);
	void setTemp(float x);

};

