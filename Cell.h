#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include <vector>
#include <sstream>
#include <string>


#include "parameters.h"
#include "Membrane_Handler.h"
#include "PetriDish.h"
#include "RuleParser.h"
#include "Field.h"

using namespace std;


class Node;
class PetriDish;

class Cell
{

private:

	bool hasBeenUpdated = 0;
	bool haveRulesProcessedRGB = 0;
	struct V_VAR_data {
		sf::Vector2f dir_vector;
		float offset_angle;
		float force;
		float translation_amount;
		void clear() {
			dir_vector.x = 0;
			dir_vector.y = 0;
			offset_angle = 0;
			force = 0;
			translation_amount = 0;
		};
		V_VAR_data() {
			clear();
		};
	};



public:

	void clear_and_set_everything_for_new_cycle();


	

	sf::Vector2f shrink_vector;
	sf::Vector2f grow_vector;
	float grow_shrink_amount = 0;

	int index_of_V_VAR_to_be_used_for_mm_PP_loc;
	float mod_PP_location_rotational_offset = 0;
	float mod_PP_location_amount = 0;

	map <Cell*, float> neighbor_angle_holder_old;
	map <Cell*, float> neighbor_angle_holder_new;

	bool isTouchingObstructed = 0;
	static int count;
	int ID;


	PetriDish* parent_petri;
	Membrane_Handler* membrane_wrangler;
	Presumed_Position_Vector ppv;

	int is_touching_medium = 0;
	bool monitored = 0;
	bool selected = 0;


	void set_haveRulesProcessedRGB(bool x) { haveRulesProcessedRGB = x; }


	void set_desired_migration_vector();
	sf::Vector2f desired_migration_vector;




	special_vector <Node*> edges;
	vector <Node*> getEdges();
	vector <Node*> getEdgesTouchingMedium();
	vector <Node*> getEdgesTouchingMediumOrObstruction();
	inline virtual void clear_edges();
	inline virtual void insert_node_to_edges(Node* n);
	virtual void erase_node_from_edges(Node* n);



	vector <float> local_memory;
	vector <float> new_local_memory;
	vector <float> local_gradient_memory;
	vector <V_VAR_data> local_vector_memory;

	void clearAllVectorDataInLVM();
	void debugLocalMemory();





	void set_mitosis_flag(int x);
	int index_of_V_VAR_to_be_used_for_mitosis;
	bool daughter_mother_position_determinor;

	void change_cell_state_if_needed();






	sf::Vector2f cell_center_cartesian;
	Node * cell_center_node;


	void adjust_center_due_to_added_component(Node * n);
	void adjust_center_due_to_removed_component(Node * n);


	virtual void add_component(Node * n);
	virtual void remove_component(Node * n);

	virtual float get_area_energy_component(int flag); //0 is actual, -1 is energy absent one, 1 is energy plus one.	
	
	virtual void update(stringstream & debug_message);


	void turn_off_entering_state_flag();




	int getFieldValueAtPresumedPosition();
	int getNumberOfEdgeNodesWithThisFieldValue(int fv);
	float get_G_VAR_value_at_center_node(int x);
	vector <Cell *> get_all_neighbor_cells() {return membrane_wrangler->get_all_neighbor_cells(); }
	int get_adjacent_cell_count();
	int get_adjacent_cell_count(int _state);
	int get_adjacent_membrane_count();
	int get_adjacent_membrane_count(int _state);
	int get_adjacent_membranes_percent(int _state);
	int get_total_nodes() { return local_memory[Actual_Area_Index]; }
	sf::Color get_color();
	sf::Color get_edge_color();
	int get_die_flag();
	 int getState();
	int get_change_state();
	int get_mitosis_flag();



	virtual string info();
	Cell(int GOAL_AREA_i, int state, PetriDish * p);
	~Cell();
};

class Medium : public Cell {
public:
	Medium(int GOAL_AREA_i, int state, PetriDish * p);
	string info();
	void clear_edges() {}
	void insert_node_to_edges(Node* n) {  }
	void erase_node_from_edges(Node* n) { }
	float get_area_energy_component(int flag);
	float get_migrate_energy_component_at_Node_n_if_it_were_this_cell(Node * n) { return 0; }
	// float get_adhesion_energy_component_at_Node_n_if_it_were_cell_y(Node * n, Cell * c) { return 0; }
	void update(stringstream & debug_message) { return; }
	void add_component(Node * n) { return; }
	void remove_component(Node * n) { return; }
	void change_type(int x) { return; }
};
