#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include<stdlib.h>
#include <vector>
#include <set>
#include <math.h>
#include <string>
#include <algorithm>
#include "parameters.h"
#include "Membrane_Handler.h"
#include "Field.h"
#include <thread>
using namespace std;




class Cell;
class Medium;
class RuleParser;
class Lab;
class Window_Manager;
class Field;
class Node {
public:
	Cell* cell;
	sf::Vector2f presumed_position_value_holder;
	bool isEdge;
	
	int col;
	int row;
	vector <float> old_gradients;
	vector <float> new_gradients;
	Node* neighbors[20];
	Node(int r, int c) {
		cell = NULL;
		row = r; col = c;
		isEdge = 0;
		

	}

};
class PetriDish {

private:
	struct PAS {
		Cell * neighbor;
		Cell* cell;
		sf::Vector2f neighbor_edge_centroid;
		sf::Vector2f cell_edge_centroid;

	};
	float getScoreAtNode(Cell* c, Presumed_Position_Vector& plv, Node* n);
	void adjustPPs();
	void updatePPInAllEdges();
	void updatePPInSingleCellEdges(Cell* c);

	void adjustPPs2();

	void adjustPPLbasedOnObstruction();

	void applyRulesToCells(stringstream& debug_message);
	void updateCellsDesiredMigrationVector();
	void updateCellPP_SCALEIfNeeded();
	void updateCellPP_Mod_LocationIfNeeded();
	void updateGapJunctions();
	void iterate();
	bool areRulesSuccesfullyLoadedAndCompiled = 0;
	void updateSetOfEdgesAndGrid(Node* n, Cell* former);

	vector <pair <Cell*, int> > gradients_to_set_to_one;

	float cos_theta_between_two_vectors(sf::Vector2f a, sf::Vector2f b);
	sf::Vector2f getAdjustedDeltaVectorDueToWrapAround(sf::Vector2f a, sf::Vector2f b);
	bool need_to_update_cell_edges = 0;
	vector <sf::Vector2f> fancy;
	sf::Vector2f TL, TR, BR, BL;

public:
	bool pause_mitosis = 0;
	bool levin_juice = 0;
	bool randomize_orientation_one_shot = 0;
	int default_Blue_index;
	int default_Yellow_index;
	int default_OBSTRUCTION_index;
	bool take_picture = 0;
	void updateCellEdgesAndMembranes();
	bool fancy_graphics = 0;
	int getFieldValueAtNode(Cell* c, Node* n);
	Node * PetriCartesianToNode(sf::Vector2f v);
	Field* FieldObject;

	sf::Vector2f getWrapAroundOffset(sf::Vector2f& N, sf::Vector2f& i);
	void applyWrapAroundOffset(sf::Vector2f& i, sf::Vector2f offset);
	void undoWrapAroundOffset(sf::Vector2f& i, sf::Vector2f offset);

	void setEmittingCellGradients();
	void diffuse();

	sf::Vector2f getVectorTowardsNeighborStates(Cell* c, int state);
	sf::Vector2f getVectorTowardsUserVariables(Cell* c, int index);
	sf::Vector2f getVectorTowardsField(Cell* c, int f);
	sf::Vector2f getVectorTowardsGradient(Cell* c, int g);

	sf::Vector2f getNodeLocalCartesian(Cell* c, Node* n);

	void addToGradientsToSetToOne(pair <Cell*, int> x) { gradients_to_set_to_one.push_back(x); }

	int getDefault_Blue_Index() { return default_Blue_index; }
	int getDefault_Yellow_Index() { return default_Yellow_index; }
	void loadDefaultRules();
	RuleParser* rule_parser;
	void assignTextToRuleParser(string s);
	void compileRuleParser(stringstream& message, int& line_number);
	vector <string> getRawRules();
	bool getCompiledFlag() {
		return areRulesSuccesfullyLoadedAndCompiled;
	}
	void setCompiledFlag(bool x) { areRulesSuccesfullyLoadedAndCompiled = x; }

	int MCS_count;



	int adhesion_order = Order_IV;
	vector <float> adhesion_cells_present;

	sf::Vector2f mouse_position;

	Medium* SUBSTRATE;
	float flux;


	int cols;
	int rows;


	set <Node* > edges;


	Lab* parent_lab;

	vector <Cell*> baby_cells;
	vector <Cell*> cells;
	vector < vector < Node*> > nodes;
	vector <Node*> flat_nodes;

	bool isNodeTouchingThisCellState(Node * n, int x);

	bool can_n_be_removed_connectivity_check(Node* n);
	bool can_n_be_changed_to_C_connectivity_check(Node* n, Cell* c);

	Cell* selected_cell = NULL;
	Cell* monitored_cell = NULL;
	int get_number_of_cells() {
		return cells.size();
	}

	void update(stringstream& debug_message);


	void perform_mitosis_if_needed_and_possible();
	void perform_mitosis2(Cell* c);
	int getMitosisCleavageSide(Cell* c, sf::Vector2f v, Node* n);
	void get_all_nodes_of_a_cell(Cell* c, vector <Node*>& result);
	void clear_petri_dish();

	void spawn_cell(Cell*& c, int size, int type, vector <Cell*>& where_to_put_it);
	void spawn_medium(Medium*& c);
	void manually_place_cell(Node* n, int type);
	void auto_place_cell(Init_Placement ip);


	bool mitosis_contiguous_check(vector <Node*> n);


	bool try_flip(Node* n1, Cell* c);
	bool compare_energies(float x, float y);

	void change_n1_to_c(Node* n1, Cell* c);

	void kill_cell(Cell* c);
	bool createCellSortingGrid(int row, int col);
	void draw(Window_Manager* window_manager);
	PetriDish(int a_cols, int a_rows, Field* f_o);

};

