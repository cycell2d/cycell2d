#include "Cell.h"
#include <cstdint>
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
#include "Lab.h"
#include "PetriDish.h"
#include "Membrane_Handler.h"
#include "Field.h"


using namespace std;

int Cell::count = -1;





Cell::Cell(int GOAL_AREA_i, int state, PetriDish * p)
{

	neighbor_angle_holder_old.clear();

	parent_petri = p;

	ppv.location = parent_petri->FieldObject->default_location;
	ppv.orientation = parent_petri->FieldObject->default_orientation;
	ppv.scale_v = sf::Vector2f(1,1);// random_float_range(0.3, 3);
	//plv.orientation = rand()%360;
	count++; ID = count;

	if (state != 0) {
		local_memory = vector <float>(parent_petri->rule_parser->LocalMemoryNames.size(), 0);
		local_gradient_memory = vector <float>(parent_petri->rule_parser->GradientVariableNames.size(), 0);
		V_VAR_data x;
		local_vector_memory = vector <V_VAR_data>(parent_petri->rule_parser->VectorVariableNames.size(), x);
		new_local_memory = local_memory;
	}
	else {
		local_memory = vector <float>(50, 0);
		local_gradient_memory = vector <float>(50, 0);
		V_VAR_data x;
		local_vector_memory = vector <V_VAR_data>(50, x);
		new_local_memory = local_memory;
	}

	membrane_wrangler = new Membrane_Handler(this);
	
	desired_migration_vector = sf::Vector2f(0, 0);
}
void Cell::update(stringstream & debug_message)
{

	hasBeenUpdated = 1;

	if (monitored) {

		parent_petri->rule_parser->evaluate(this, debug_message, parent_petri->parent_lab->isDebugOn);
	}
	else {
		parent_petri->rule_parser->evaluate(this, debug_message, false);
	}


}





void Cell::clear_and_set_everything_for_new_cycle()
{


change_cell_state_if_needed();
clearAllVectorDataInLVM();
	
local_memory[PP_SCALED_WIDTH_Index] = ppv.scale_v.x;
local_memory[PP_SCALED_HEIGHT_Index] = ppv.scale_v.y;

grow_vector = sf::Vector2f(0, 0);
shrink_vector = sf::Vector2f(0, 0);

index_of_V_VAR_to_be_used_for_mm_PP_loc = -1;
index_of_V_VAR_to_be_used_for_mitosis = -1;
}



void Cell::set_desired_migration_vector()
{

	sf::Vector2f holder;
	desired_migration_vector = sf::Vector2f(0,0);

	for (size_t j = 0; j < local_vector_memory.size();j++) {

		if (local_vector_memory[j].force == 0) { continue; }
		if (local_vector_memory[j].dir_vector == sf::Vector2f(0, 0)) { continue; }
		holder = rotate_vector_using_degrees(local_vector_memory[j].dir_vector, local_vector_memory[j].offset_angle);
		holder *= -local_vector_memory[j].force;
		desired_migration_vector += holder;

	}
}

vector<Node*> Cell::getEdges() { return edges.getData(); }

vector<Node*> Cell::getEdgesTouchingMedium()
{
	vector <Node *> result;
	result.clear();
	vector<Node*> edge_holder = edges.getData();

	for (auto e : edge_holder) {

		for (int j = 0; j < Order_I; j++) {
			if (e->neighbors[j]->cell == parent_petri->SUBSTRATE) {
				result.push_back(e);
				break;
			}
		}
	}
	return result;
}

vector<Node*> Cell::getEdgesTouchingMediumOrObstruction()
{
	vector <Node *> result;
	result.clear();
	vector<Node*> edge_holder = edges.getData();

	for (auto e : edge_holder) {

		for (int j = 0; j < Order_I; j++) {
			if ((e->neighbors[j]->cell == parent_petri->SUBSTRATE) || (e->neighbors[j]->cell->getState() == parent_petri->default_OBSTRUCTION_index)) {
				result.push_back(e);
				break;
			}
		}
	}
	return result;
}

void Cell::clear_edges() { edges.clear(); }

void Cell::insert_node_to_edges(Node* n) {
	edges.insert(n);


}

void Cell::erase_node_from_edges(Node* n) {
	edges.erase(n);

}



void Cell::clearAllVectorDataInLVM()
{

	for (size_t j = 0; j < local_vector_memory.size(); j++) {
		local_vector_memory[j].clear();
	}
}

void Cell::debugLocalMemory() {
	for (size_t x = 0; x < local_memory.size(); x++) {
		cout << "x= " << x << ", " << parent_petri->rule_parser->LocalMemoryNames[x] << " = " << local_memory[x] << "\n";

	}
	cout << "\n\n\n";
}

float Cell::get_G_VAR_value_at_center_node(int x) {

	Node* center_node;

	center_node = parent_petri->flat_nodes[int(cell_center_cartesian.y) * parent_petri->cols + int(cell_center_cartesian.x)];
	return center_node->old_gradients[x];
}

int Cell::get_adjacent_cell_count() {
	return membrane_wrangler->total_cell_neighbors() - is_touching_medium;
}

int Cell::get_adjacent_cell_count(int _state) {
	return membrane_wrangler->total_cell_neighbors_of_particular_state(_state);
}

int Cell::get_adjacent_membrane_count() {
	return membrane_wrangler->total_membranes();
}

int Cell::get_adjacent_membrane_count(int _state) {
	return membrane_wrangler->total_membranes_of_cells_with_particular_state(_state);
}

int Cell::get_adjacent_membranes_percent(int _state) {
	return (int)(100.0f * float(get_adjacent_membrane_count(_state)) / float(get_adjacent_membrane_count()));
}

void Cell::set_mitosis_flag(int x)
{
	local_memory[Mitosis_Flag_Index] = (float)x;
}

void Cell::change_cell_state_if_needed()
{

	if (local_memory[Current_State_Index] != local_memory[State_to_Change_Into_Index]) {
		local_memory[Current_State_Index] = local_memory[State_to_Change_Into_Index];
		local_memory[Entering_State_Index] = 1.0f;
	}

}

int Cell::getFieldValueAtPresumedPosition()
{
	return parent_petri->FieldObject->getFieldValueAtParticularCartesianLocation(ppv.location);

}

int Cell::getNumberOfEdgeNodesWithThisFieldValue(int fv)
{
	vector <Node *> my_edges = edges.getData();

	int result = 0;
	for (auto ce : my_edges) {

		if (parent_petri->getFieldValueAtNode(this, ce) == fv) {
			result++;
		}
	}

	return result;
}

void Cell::adjust_center_due_to_added_component(Node * n)
{
	if ((int)local_memory[Actual_Area_Index] == 1) {

		cell_center_cartesian.x = float(n->col) + 0.5f;
		cell_center_cartesian.y = float(n->row) + 0.5f;
		cell_center_node = parent_petri->flat_nodes[int(cell_center_cartesian.y) * parent_petri->cols + int(cell_center_cartesian.x)];
		return;
	}

	sf::Vector2f n_pos = sf::Vector2f((float)n->col, (float)n->row);


	//**********Wrap_Around_Stuff*************
	sf::Vector2f wrap_around_offset = parent_petri->getWrapAroundOffset(n_pos, cell_center_cartesian);
	parent_petri->applyWrapAroundOffset(n_pos, wrap_around_offset);
	parent_petri->applyWrapAroundOffset(cell_center_cartesian, wrap_around_offset);
	//**********Wrap_Around_Stuff*************


	//This assumes n has already been added to all_components.
	float average_row = cell_center_cartesian.y * (local_memory[Actual_Area_Index] - 1.0f);
	average_row = average_row + n_pos.y;
	cell_center_cartesian.y = average_row / (local_memory[Actual_Area_Index]);
	float average_col = cell_center_cartesian.x * (local_memory[Actual_Area_Index] - 1.0f);
	average_col = average_col + n_pos.x;
	cell_center_cartesian.x = average_col / (local_memory[Actual_Area_Index]);


	//**********Wrap_Around_Stuff*************
	parent_petri->undoWrapAroundOffset(cell_center_cartesian, wrap_around_offset);
	//**********Wrap_Around_Stuff*************


	cell_center_node = parent_petri->flat_nodes[int(cell_center_cartesian.y) * parent_petri->cols + int(cell_center_cartesian.x)];

}

void Cell::adjust_center_due_to_removed_component(Node * n)
{

	if ((int)local_memory[Actual_Area_Index] == 0) {
		cell_center_node = NULL;
		return;
	}
	if ((int)local_memory[Actual_Area_Index] == 1) {

		cell_center_cartesian.x = float(n->col) + 0.5f;
		cell_center_cartesian.y = float(n->row) + 0.5f;
		cell_center_node = parent_petri->flat_nodes[int(cell_center_cartesian.y) * parent_petri->cols + int(cell_center_cartesian.x)];

		return;
	}

	sf::Vector2f n_pos = sf::Vector2f((float)n->col, (float)n->row);


	//**********Wrap_Around_Stuff*************
	sf::Vector2f wrap_around_offset = parent_petri->getWrapAroundOffset(n_pos, cell_center_cartesian);
	parent_petri->applyWrapAroundOffset(n_pos, wrap_around_offset);
	parent_petri->applyWrapAroundOffset(cell_center_cartesian, wrap_around_offset);
	//**********Wrap_Around_Stuff*************



	//This assumes n has already been removed from all_components.
	float average_row = cell_center_cartesian.y * (local_memory[Actual_Area_Index] + 1.0f);
	average_row = average_row - n_pos.y;
	cell_center_cartesian.y = average_row / (local_memory[Actual_Area_Index]);
	float average_col = cell_center_cartesian.x * (local_memory[Actual_Area_Index] + 1.0f);
	average_col = average_col - n_pos.x;
	cell_center_cartesian.x = average_col / (local_memory[Actual_Area_Index]);


	//**********Wrap_Around_Stuff*************
	parent_petri->undoWrapAroundOffset(cell_center_cartesian, wrap_around_offset);
	//**********Wrap_Around_Stuff*************


	cell_center_node = parent_petri->flat_nodes[int(cell_center_cartesian.y) * parent_petri->cols + int(cell_center_cartesian.x)];

}


void Cell::add_component(Node * n)
{



	local_memory[Actual_Area_Index]++;
	adjust_center_due_to_added_component(n);

}

void Cell::remove_component(Node * n)
{

	local_memory[Actual_Area_Index]--;
	adjust_center_due_to_removed_component(n);
	
}

float Cell::get_area_energy_component(int flag) //0 is actual, -1 is energy absent one node, 1 is energy plus one node.
{



	if (flag == 0) {
		return local_memory[Bulk_Modulus_Index] *
			((local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index])*(local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index]));
	}
	if (flag == 1) {
		return local_memory[Bulk_Modulus_Index] *
			(((local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index])*(local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index]) +
				2 * (local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index]) + 1));
	}
	if (flag == -1) {
		return local_memory[Bulk_Modulus_Index] *
			(((local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index])*(local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index]) -
				2 * (local_memory[Actual_Area_Index] - local_memory[Goal_Area_Index]) + 1));
	}
	cout << "Wrong flag type Error.\n";
	return -1;
}


int Cell::get_die_flag()
{

	return (int)local_memory[Die_Flag_Index];

}
int Cell::getState()
{
	return (int)local_memory[Current_State_Index];
	//return (int)local_memory[parent_petri->rule_parser->LMIM[CURRENT_STATE]];

}
int Cell::get_change_state()
{

	return (int)local_memory[State_to_Change_Into_Index];

}
int Cell::get_mitosis_flag()
{

	return (int)local_memory[Mitosis_Flag_Index];

}
void Cell::turn_off_entering_state_flag()
{

	local_memory[Entering_State_Index] = 0;

}
sf::Color Cell::get_color()
{

	if ((!haveRulesProcessedRGB)) {
		switch (getState()) {

		case 0:
			return sf::Color(255-50, 255 - 50, 255 - 50);
		case 1:
			return sf::Color(255 - 90, 255 - 50, 255 - 50);
		case 2:
			return sf::Color(255 - 110, 255 - 60, 255 - 60);
		case 3:
			return sf::Color(255 - 130, 255 - 70, 255 - 70);
		case 4:
			return sf::Color(255 - 170, 255 - 80, 255 - 80);
		case 5:
			return sf::Color(255 - 190, 255 - 50, 255 - 50);
		case 6:
			return sf::Color(255 - 110, 255 - 160, 255 - 60);
		case 7:
			return sf::Color(255 - 130, 255 - 70, 255 - 170);
		case 8:
			return sf::Color(255 - 70, 255 - 180, 255 - 80);
		default:
			return sf::Color(255 , 20,20);
		}


	}
	if (monitored) {

		return sf::Color(220, 50, 50);
	}
	
	return sf::Color((uint8_t)local_memory[Red_Index], (uint8_t)local_memory[Green_Index], (uint8_t)local_memory[Blue_Index], 200);

}


sf::Color Cell::get_edge_color()
{

	if (monitored) {

		return sf::Color::Red;
	}
	if ((!haveRulesProcessedRGB) ) {
		switch (getState()) {

		case 0:
			return sf::Color(50, 50, 200);
		case 1:
			return sf::Color(90, 50, 200);
		case 2:
			return sf::Color(110, 60, 200);
		case 3:
			return sf::Color(130, 70, 200);
		case 4:
			return sf::Color(170, 80, 200);
		default:
			return sf::Color(220, 220, 200);
		}
	}
	return sf::Color((uint8_t)std::max(local_memory[Red_Index] - 80, 0.0f), (uint8_t)std::max(local_memory[Green_Index] - 80, 0.0f), (uint8_t)std::max(local_memory[Blue_Index] - 80, 0.0f));
}


string Cell::info() {
	stringstream result;
	string ln;
	result << "Cell No. " << ID << "\n";
	result << "  touching obstructed = " << isTouchingObstructed << "\n";
	result << "State:         " << parent_petri->rule_parser->StateNames[getState()] << "\n";
	result << "Goal Area:           " << local_memory[Goal_Area_Index] << "\n";
	result << "Actual Area:          " << local_memory[Actual_Area_Index] << "\n";
	result << "Mitosis Flag:           " << local_memory[Mitosis_Flag_Index] << "\n";
	result << "RGB:          (" << local_memory[Red_Index] << ", " << local_memory[Green_Index] << ", " << local_memory[Blue_Index] << ")\n";
	result << "Desired_Vector:    " << debug_print_sfml_vector(desired_migration_vector) << "\n";
	result << "Desired_Vector Mag:    " << magnitude_vector(desired_migration_vector) << "\n";
	result << "Edges:          " << edges.size() << "\n";
	result << "Presumed Scale:      (" << ppv.scale_v.x << ","<<ppv.scale_v.y<<")\n";
	result << "Neighbor Cell IDs: \n";
	vector <Cell *> neighbors = membrane_wrangler->get_all_neighbor_cells();
	for (auto nn : neighbors) {
		result << "   ID No.  " << nn->ID << "\n";
	}
	result << " Number of Membranes:  " << membrane_wrangler->total_membranes() << "\n";
	result << "User_Defined_Variables:\n";
	for (auto uvn : parent_petri->rule_parser->completeUserVariableNames) {
		result << "  " << uvn << ":  " << local_memory[parent_petri->rule_parser->LMIM[uvn]] << "\n";
	}
	result << "Gradient_Variables:\n";
	for (auto uvn : parent_petri->rule_parser->GradientVariableNames) {
		result << "  " << uvn << ":  " << get_G_VAR_value_at_center_node(parent_petri->rule_parser->GVIM[uvn]) << "\n";
	}



	return result.str();
}

Medium::Medium(int GOAL_AREA_i, int state, PetriDish * p) : Cell(GOAL_AREA_i, state, p)
{




}


float Medium::get_area_energy_component(int flag)
{
	return 0.0f;
}
string Medium::info()
{
	stringstream result;
	result << "[Substrate Medium]\n";
	return result.str();
}

Cell::~Cell()
{

	delete membrane_wrangler;
}


