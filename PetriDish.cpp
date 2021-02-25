#include "PetriDish.h"
#include "Cell.h"
#include "Lab.h"
#include "Membrane_Handler.h"
#include "Window_Manager.h"
#include "User_Input.h"
#include "RuleParser.h"
#include <math.h>
using namespace std;

PetriDish::PetriDish(int a_cols, int a_rows, Field* f_o)

{
	FieldObject = f_o;
	SUBSTRATE = new Medium(0, 0, this);
	cols = a_cols;
	rows = a_rows;


	//used for drawing field 'sprites'
	TL = sf::Vector2f(0, 0);
	TR = sf::Vector2f((float)cols, 0);
	BR = sf::Vector2f((float)cols, (float)rows);
	BL = sf::Vector2f(0, (float)rows);

	rule_parser = new RuleParser();
	nodes.clear();
	vector <Node*> node_row_holder;
	node_row_holder.clear();
	cells.clear();
	baby_cells.clear();
	Node* node_holder;
	flat_nodes.clear();
	flat_nodes.reserve(cols * rows);


	for (int x = 0; x < rows; x++) {
		node_row_holder.clear();
		for (int y = 0; y < cols; y++) {
			node_holder = new Node(x, y);
			node_holder->cell = SUBSTRATE;
			node_row_holder.push_back(node_holder);
		}
		nodes.push_back(node_row_holder);

	}


	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
			node_holder = nodes[x][y];
			flat_nodes.push_back(node_holder);

			node_holder->neighbors[0] = nodes[wrap_around_mod((x - 1), rows)][wrap_around_mod((y - 0), cols)];
			node_holder->neighbors[1] = nodes[wrap_around_mod((x - 0), rows)][wrap_around_mod((y + 1), cols)];
			node_holder->neighbors[2] = nodes[wrap_around_mod((x + 1), rows)][wrap_around_mod((y - 0), cols)];
			node_holder->neighbors[3] = nodes[wrap_around_mod((x - 0), rows)][wrap_around_mod((y - 1), cols)];

			node_holder->neighbors[4] = nodes[wrap_around_mod((x - 1), rows)][wrap_around_mod((y - 1), cols)];
			node_holder->neighbors[5] = nodes[wrap_around_mod((x - 1), rows)][wrap_around_mod((y + 1), cols)];
			node_holder->neighbors[6] = nodes[wrap_around_mod((x + 1), rows)][wrap_around_mod((y + 1), cols)];
			node_holder->neighbors[7] = nodes[wrap_around_mod((x + 1), rows)][wrap_around_mod((y - 1), cols)];


			node_holder->neighbors[8] = nodes[wrap_around_mod((x - 2), rows)][wrap_around_mod((y - 0), cols)];
			node_holder->neighbors[9] = nodes[wrap_around_mod((x - 0), rows)][wrap_around_mod((y + 2), cols)];
			node_holder->neighbors[10] = nodes[wrap_around_mod((x + 2), rows)][wrap_around_mod((y - 0), cols)];
			node_holder->neighbors[11] = nodes[wrap_around_mod((x - 0), rows)][wrap_around_mod((y - 2), cols)];


			node_holder->neighbors[12] = nodes[wrap_around_mod((x - 2), rows)][wrap_around_mod((y - 1), cols)];
			node_holder->neighbors[13] = nodes[wrap_around_mod((x - 2), rows)][wrap_around_mod((y + 1), cols)];
			node_holder->neighbors[14] = nodes[wrap_around_mod((x - 1), rows)][wrap_around_mod((y + 2), cols)];
			node_holder->neighbors[15] = nodes[wrap_around_mod((x + 1), rows)][wrap_around_mod((y + 2), cols)];
			node_holder->neighbors[16] = nodes[wrap_around_mod((x + 2), rows)][wrap_around_mod((y + 1), cols)];
			node_holder->neighbors[17] = nodes[wrap_around_mod((x + 2), rows)][wrap_around_mod((y - 1), cols)];
			node_holder->neighbors[18] = nodes[wrap_around_mod((x + 1), rows)][wrap_around_mod((y - 2), cols)];
			node_holder->neighbors[19] = nodes[wrap_around_mod((x - 1), rows)][wrap_around_mod((y - 2), cols)];

		}
	}
	nodes.clear();
	vector< vector <Node* > >().swap(nodes);
	edges.clear();


	fancy.clear();
	fancy.push_back(sf::Vector2f(-0.333f, 0)); //a
	fancy.push_back(sf::Vector2f(0, 0)); //b
	fancy.push_back(sf::Vector2f(0, -0.333f)); //c
	fancy.push_back(sf::Vector2f(0.333f, 0)); //dh k
	fancy.push_back(sf::Vector2f(0.666f, 0)); //e
	fancy.push_back(sf::Vector2f(1.0f, -0.333f)); //f
	fancy.push_back(sf::Vector2f(1.0f, 0)); //g
	fancy.push_back(sf::Vector2f(1.333f, 0)); //h
	fancy.push_back(sf::Vector2f(1.0f, .333f)); //i
	fancy.push_back(sf::Vector2f(1, .666f)); //j
	fancy.push_back(sf::Vector2f(1.333f, 1.0f)); //k
	fancy.push_back(sf::Vector2f(1.0f, 1.0f)); //l
	fancy.push_back(sf::Vector2f(1.0f, 1.333f)); //m
	fancy.push_back(sf::Vector2f(0.666f, 1.0f)); //n
	fancy.push_back(sf::Vector2f(0.333f, 1.0f)); //o
	fancy.push_back(sf::Vector2f(0, 1.333f)); //p
	fancy.push_back(sf::Vector2f(0, 1.0f)); //q
	fancy.push_back(sf::Vector2f(-0.333f, 1.0f)); //r
	fancy.push_back(sf::Vector2f(0, .666f)); //s
	fancy.push_back(sf::Vector2f(0, .333f)); //t
	fancy.push_back(sf::Vector2f(0.333f, .333f)); //u
	fancy.push_back(sf::Vector2f(0.666f, .333f)); //v
	fancy.push_back(sf::Vector2f(0.333f, .666f)); //w
	fancy.push_back(sf::Vector2f(0.666f, .666f)); //x

}



void PetriDish::clear_petri_dish()
{
	MCS_count = 0;
	selected_cell = NULL;
	monitored_cell = NULL;

	for (size_t x = 0; x < cells.size(); x++) {
		delete cells[x];
	}

	cells.clear();
	for (auto j : flat_nodes) {
		j->isEdge = 0;
		j->new_gradients = vector <float>(rule_parser->GradientVariableNames.size(), 0);
		j->old_gradients = vector <float>(rule_parser->GradientVariableNames.size(), 0);
	}

	if (SUBSTRATE != NULL) {
		SUBSTRATE->membrane_wrangler->clear();
		for (auto j : flat_nodes) {
			j->cell = SUBSTRATE;
			SUBSTRATE->add_component(j);
		}
	}


}

void PetriDish::assignTextToRuleParser(string s) { rule_parser->loadRulesText(s); }
void PetriDish::compileRuleParser(stringstream& message, int& line_number)
{
	clear_petri_dish();
	rule_parser->compile(message, line_number);
	if (message.str() == "") {
		default_Blue_index = rule_parser->SNIM["Default_Blue"];
		default_Yellow_index = rule_parser->SNIM["Default_Yellow"];
		default_OBSTRUCTION_index = rule_parser->SNIM["OBSTRUCTION"];
		for (auto fn : flat_nodes) {
			fn->new_gradients = vector <float>(rule_parser->GradientVariableNames.size(), 0);
			fn->old_gradients = vector <float>(rule_parser->GradientVariableNames.size(), 0);
		}
	}

	adhesion_cells_present = vector <float>(rule_parser->StateNames.size(), 0);

}
void PetriDish::loadDefaultRules()
{
	//string	default_rules = "STATE Default_Blue\nENTERING_STATE; :: RGB(131,196,242); GOAL_AREA=60; \nSTATE Default_Yellow\nENTERING_STATE; :: RGB(227,226,98); GOAL_AREA=60; \n STATE OBSTRUCTION\n RGB(227,226,255);";

	string	default_rules = "";

	assignTextToRuleParser(default_rules);
	stringstream dummy;
	int dummy_int = -1;
	compileRuleParser(dummy, dummy_int);


}

vector<string> PetriDish::getRawRules()
{
	return rule_parser->getRawRules();
}


void PetriDish::update(stringstream& debug_message)
{
	MCS_count++;

	if (need_to_update_cell_edges) {
		updateCellEdgesAndMembranes();
		need_to_update_cell_edges = 0;
	}


	for (auto c : cells) {
		c->clear_and_set_everything_for_new_cycle();
	}


	applyRulesToCells(debug_message);
	for (auto c : cells) {
		if (c->local_memory[Actual_Area_Index] < 5) {
			c->local_memory[Die_Flag_Index] = 1;
		}
	}

	for (auto c : cells) {
		c->ppv.scale_v.x = c->local_memory[PP_SCALED_WIDTH_Index];
		if (c->ppv.scale_v.x <= 0.01) { c->ppv.scale_v.x = 0.01; }
		c->ppv.scale_v.y = c->local_memory[PP_SCALED_HEIGHT_Index];
		if (c->ppv.scale_v.y <= 0.01) { c->ppv.scale_v.y = 0.01; }
	}

	updateCellPP_SCALEIfNeeded();


	///pull all PP_SCALEs together
	for (auto c : cells) {
		float delta;
		if (c->ppv.scale_v.x > c->ppv.scale_v.y) {
			delta = c->ppv.scale_v.x - c->ppv.scale_v.y;
			c->ppv.scale_v.x -= .001 * delta;
			c->ppv.scale_v.y += .001 * delta;
		}
		if (c->ppv.scale_v.x < c->ppv.scale_v.y) {
			delta = c->ppv.scale_v.y - c->ppv.scale_v.x;
			c->ppv.scale_v.x += .001 * delta;
			c->ppv.scale_v.y -= .001 * delta;

		}

	}

	updateCellPP_Mod_LocationIfNeeded();

	updateCellsDesiredMigrationVector();


	setEmittingCellGradients();

	updateGapJunctions();

	iterate();

	if (need_to_update_cell_edges) {
		updateCellEdgesAndMembranes();
		need_to_update_cell_edges = 0;
	}


	for (auto c : cells) {
		c->turn_off_entering_state_flag();
	}


	if (!pause_mitosis) {
		perform_mitosis_if_needed_and_possible();
	}



	//INSERT NEWLY CREATED CELLS (from Mitosis) INTO MAIN CELLS VECTOR
	//**************************************************************************
	if (baby_cells.size() > 0) {
		for (auto bc : baby_cells) { cells.push_back(bc); }
		baby_cells.clear();
	}
	if (need_to_update_cell_edges) {
		updateCellEdgesAndMembranes();
		need_to_update_cell_edges = 0;
	}
	//KILL CELLS	
	vector <Cell*> dead_cells;
	dead_cells.clear();
	for (auto c : cells) {
		if (c->get_die_flag()) {
			dead_cells.push_back(c);
		}
	}
	for (auto dc : dead_cells) {
		kill_cell(dc);
	}
	//****************************************************************

	if (need_to_update_cell_edges) {
		updateCellEdgesAndMembranes();
		need_to_update_cell_edges = 0;
	}
	//Fill in presumed values in all edges
	if (parent_lab->isFieldInUse()) {
		updatePPInAllEdges();
		adjustPPs();
	}

}
void PetriDish::iterate() {

	Node* n;
	Node* nn;
	vector <Node*> sequence_to_iterate_over(edges.begin(), edges.end());
	random_shuffle(sequence_to_iterate_over.begin(), sequence_to_iterate_over.end());
	vector <Cell*> options;



	int sz = sequence_to_iterate_over.size();
	for (int kk = 0; kk < sz; kk++) {

		n = sequence_to_iterate_over[kk];
		if ((!n->isEdge)) { continue; }
		if (n->cell->getState() == default_OBSTRUCTION_index) { continue; }
		if (n->cell != SUBSTRATE) {
			if (!can_n_be_removed_connectivity_check(n)) { continue; }
		}


		//Detour for mouse-dragged cells.
		//If n->cell is being dragged, n WILL be swapped with SUBSTRATE if it will
		//advance the cell center towards target.
		if (n->cell->selected) {
			if (parent_lab->newNodeMouseIsOn != NULL) {
				float cos_theta = cos_theta_between_two_vectors(getNodeLocalCartesian(n->cell, n), getNodeLocalCartesian(n->cell, parent_lab->newNodeMouseIsOn));
				if (cos_theta < -.95) {  // about 30 degree on either side
					change_n1_to_c(n, SUBSTRATE);
					continue;
				}
			}
		}





		options.clear();
		options.push_back(SUBSTRATE);
		bool changed = 0;
		for (int j = 0; j < Order_I; j++) {
			nn = n->neighbors[j];
			if (nn->cell == n->cell) { continue; }
			if (nn->cell->getState() == default_OBSTRUCTION_index) { continue; }

			//detour for mouse-dragged cells. If the 'option' is a mouse-dragged cell, it WILL be 
			//moved into the node if it = the mouse-dragged cell's desired migration.
			//If it doesn't, we don't add it to options (by continuuing). If it does, we return.
			if (nn->cell->selected) {
				if (parent_lab->newNodeMouseIsOn != NULL) {
					if (!can_n_be_changed_to_C_connectivity_check(n, nn->cell)) { continue; }
					float cos_theta = cos_theta_between_two_vectors(getNodeLocalCartesian(nn->cell, n), getNodeLocalCartesian(nn->cell, parent_lab->newNodeMouseIsOn));
					if (cos_theta > .95) {  // about 30 degree on either side
						change_n1_to_c(n, nn->cell);
						changed = 1;
						break;
					}
				}
			}
			options.push_back(nn->cell);
		}
		//************************************************************************
		if (changed) { continue; }


		if (options.size() == 0) {
			sp();
		}


		Cell* opt = options[rand() % options.size()];
		if (opt != n->cell) {
			if (can_n_be_changed_to_C_connectivity_check(n, opt)) {
				try_flip(n, opt);
			}
		}
		//************************************************************************
	}
}
void PetriDish::applyRulesToCells(stringstream& debug_message)
{

	gradients_to_set_to_one.clear();

	int st;
	for (auto c : cells) {

		//THIS USED TO BE WHERE THE DIRECTIONAL VECTORS WERE CLEARED

		st = c->getState();
		if (st == default_Blue_index) {
			c->local_memory[Goal_Area_Index] = parent_lab->blue_goal_area;
		}
		else {
			if (st == default_Yellow_index) {
				c->local_memory[Goal_Area_Index] = parent_lab->yellow_goal_area;
			}
		}
	}
	for (auto c : cells) {
		c->update(debug_message);
	}
}
void PetriDish::updateCellsDesiredMigrationVector()
{
	for (auto c : cells) {

		c->set_desired_migration_vector();
	}
}
void PetriDish::updateCellPP_SCALEIfNeeded()
{

	for (auto c : cells) {
		if ((c->grow_vector.x == 0) &&
			(c->grow_vector.y == 0) &&
			(c->shrink_vector.x == 0) &&
			(c->shrink_vector.y == 0)) {
			continue;
		}


		normalize_vector(c->grow_vector);
		normalize_vector(c->shrink_vector);

		c->grow_vector = rotate_vector_using_degrees(c->grow_vector, c->ppv.orientation);
		c->shrink_vector = rotate_vector_using_degrees(c->shrink_vector, c->ppv.orientation);

		c->ppv.scale_v.x = c->ppv.scale_v.x * (1 + c->grow_shrink_amount * (abs(c->grow_vector.x) - abs(c->shrink_vector.x)));

		c->ppv.scale_v.y = c->ppv.scale_v.y * (1 + c->grow_shrink_amount * (abs(c->grow_vector.y) - abs(c->shrink_vector.y)));
	}

}
void PetriDish::updateCellPP_Mod_LocationIfNeeded()
{
	for (auto c : cells) {

		if (c->index_of_V_VAR_to_be_used_for_mm_PP_loc == -1) {
			continue;
		}



		if (c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mm_PP_loc].dir_vector == sf::Vector2f(0, 0)) {
			continue;
		}

		if (c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mm_PP_loc].translation_amount == 0) {
			continue;
		}

		sf::Vector2f holder = c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mm_PP_loc].dir_vector;

		normalize_vector(holder);
		holder = rotate_vector_using_degrees(holder, c->ppv.orientation);
		c->ppv.location.x += c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mm_PP_loc].translation_amount* holder.x * c->ppv.scale_v.x;

		c->ppv.location.y += c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mm_PP_loc].translation_amount * holder.y * c->ppv.scale_v.y;

	}


}
void PetriDish::updateGapJunctions()
{

	int lm_index;
	vector <Cell*> neighbors;
	for (auto c : cells) {
		neighbors = c->membrane_wrangler->get_all_neighbor_cells();

		for (size_t x = 0; x < rule_parser->completeUserVariableNames.size(); x++) {
			float loss = 0;
			float gain = 0;
			lm_index = x + First_User_Variable_Index;

			for (auto cn : neighbors) {
				if (cn == SUBSTRATE) { continue; }
				loss += c->local_memory[lm_index] * rule_parser->get_GJ_value(c->getState(), cn->getState(), lm_index) * c->membrane_wrangler->total_membranes_of_particular_cell(cn);
				gain += cn->local_memory[lm_index] * rule_parser->get_GJ_value(c->getState(), cn->getState(), lm_index) * c->membrane_wrangler->total_membranes_of_particular_cell(cn);
			}
			c->new_local_memory[lm_index] = c->local_memory[lm_index] + gain - loss;
		}
	}
	for (auto c : cells) {
		for (size_t x = 0; x < rule_parser->completeUserVariableNames.size(); x++) {
			lm_index = x + First_User_Variable_Index;
			c->local_memory[lm_index] = c->new_local_memory[lm_index];
		}
	}

}
void PetriDish::perform_mitosis_if_needed_and_possible()
{

	for (auto c : cells) {

		if (c->get_mitosis_flag() == 1) {

			perform_mitosis2(c);
		}
	}
}

void PetriDish::perform_mitosis2(Cell* c)
{
	//If too small continue
	if (c->get_total_nodes() < 15) {  return; }

	//If the center is outside the cell for some reason, abandon mitosis.	
	if (c->cell_center_node->cell != c) {  return; }

	//if te center node is not within an ORDER_II set of nodes, abandon mitosis.
	for (int j = 0; j < Order_II; j++) {
		if (c->cell_center_node->neighbors[j]->cell != c) {  return; }
	}


	sf::Vector2f cleavage_vector;

	//obtain the relevant V_VAR.
	if (c->index_of_V_VAR_to_be_used_for_mitosis == -1) {
		cleavage_vector = sf::Vector2f(random_float_range(-1, 1), random_float_range(-1, 1));
	}
	else {
		cleavage_vector = c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mitosis].dir_vector;
		cleavage_vector = rotate_vector_using_degrees(cleavage_vector, c->local_vector_memory[c->index_of_V_VAR_to_be_used_for_mitosis].offset_angle);
	}
	if (cleavage_vector == sf::Vector2f(0, 0)) {
		cleavage_vector = sf::Vector2f(random_float_range(-1, 1), random_float_range(-1, 1));
	}

	//normalize vector
	normalize_vector(cleavage_vector);

	//establish the line of nodes that constitute the cleavage line
	set <Node *> cleavage_line;
	cleavage_line.clear();

	float increment = 0;
	Node * holder;

	holder = PetriCartesianToNode(c->cell_center_cartesian + increment * cleavage_vector);
	
	cleavage_line.insert(holder);
	bool is_edge = 0;
	sf::Vector2f increment_vector;
	//extend line bit by bit in one direction 
	while (1) {

		increment++;
		increment_vector = c->cell_center_cartesian + (increment * cleavage_vector);
		holder = PetriCartesianToNode(increment_vector);
	
		if (holder->cell != c) { break; }
		else {
			for (int j = 0; j < Order_I; j++) {
				if (holder->neighbors[j]->cell != c) {
					cleavage_line.insert(holder);
					 is_edge = 1; break;
				}
			}
		}
		if (is_edge) { break; }
		cleavage_line.insert(holder);
	
	}
	//extend line bit by bit in the other direction
	increment = 0;
	is_edge = 0;
	while (1) {

		increment--;
		increment_vector = c->cell_center_cartesian + (increment * cleavage_vector);
	//	cout << "increment = " << increment << ":  increment_vector  = " << debug_print_sfml_vector(increment_vector) << "\n";
		holder = PetriCartesianToNode(increment_vector);
	
		if (holder->cell != c) { break; }
		else {
			for (int j = 0; j < Order_I; j++) {
				if (holder->neighbors[j]->cell != c) {
					cleavage_line.insert(holder);
					 is_edge = 1; break;
				}
			}
		}
		if (is_edge) { break; }
		cleavage_line.insert(holder);
		
	}

	//find a good starting point for the flood fill with d=1.
	Node* seed_node;
	bool found_good_seed = 0;
	bool det_side = 0;
	for (int j = 0; j < Order_I; j++) {
		holder = c->cell_center_node->neighbors[j];
	
		if (cleavage_line.find(holder) == cleavage_line.end()) {
			det_side = getMitosisCleavageSide(c, cleavage_vector, holder);
			seed_node = holder;
				found_good_seed = 1;
				break;
			}
		}
	

	if (!found_good_seed) { // never should reach this.
		cout << "Error with found_good_seed\n";
		cout << "cleavage vector = " << debug_print_sfml_vector(cleavage_vector) << "\n";
		cout << "cell center cartesian = " << debug_print_sfml_vector(c->cell_center_cartesian) << "\n";
		for (int j = 0; j < Order_I; j++) {
			holder = c->cell_center_node->neighbors[j];
			cout << "J = " << j << "\n";
			db(holder->row);
			db(holder->col);
			if (cleavage_line.find(holder) == cleavage_line.end()) {
				cout << "Not in cleavage_line\n";
			}
			else {
				cout << "In cleavage line\n";
			}

			db(getMitosisCleavageSide(c, cleavage_vector, holder));
			}
		
		sp();

		return;
	}


	vector <Node*> all_nodes;
	get_all_nodes_of_a_cell(c, all_nodes);



	vector <Node*> checked;
	checked.clear();
	vector <Node*> toCheck;
	toCheck.clear();

	checked.push_back(seed_node);


	//this checks the first four from the seed node.
	for (int j = 0; j < Order_I; j++) {
		if (seed_node->neighbors[j]->cell == c) {
			if (find(checked.begin(), checked.end(), seed_node->neighbors[j]) == checked.end()) {
				if (cleavage_line.find(seed_node->neighbors[j]) == cleavage_line.end()) {
					toCheck.push_back(seed_node->neighbors[j]);
				}
			}
		}
	}


	Node* node_being_checked;
	while (toCheck.size() != 0) {
		node_being_checked = toCheck[0];
		for (int j = 0; j < Order_I; j++) {
			if (node_being_checked->neighbors[j]->cell == c) {
				if (find(checked.begin(), checked.end(), node_being_checked->neighbors[j]) == checked.end()) {
					if (find(toCheck.begin(), toCheck.end(), node_being_checked->neighbors[j]) == toCheck.end()) {
						if (cleavage_line.find(node_being_checked->neighbors[j]) == cleavage_line.end()) {
							toCheck.push_back(node_being_checked->neighbors[j]);
						}
					}
				}
			}
		}
		if (find(checked.begin(), checked.end(), node_being_checked) == checked.end()) {
			checked.push_back(node_being_checked);
		}
		toCheck.erase(toCheck.begin());
	}




	vector <Node*> mother;
	vector <Node*> daughter;
	mother.clear();
	daughter.clear();

	bool c_det = c->daughter_mother_position_determinor;
	if (!det_side) {
		c_det = !c->daughter_mother_position_determinor;
	}

	if (!c_det) {
		mother = checked;
		for (size_t x = 0; x < all_nodes.size(); x++) {
			if (find(mother.begin(), mother.end(), all_nodes[x]) == mother.end()) {
				daughter.push_back(all_nodes[x]);
			}
		}
	}
	else {
		daughter = checked;
		for (size_t x = 0; x < all_nodes.size(); x++) {
			if (find(daughter.begin(), daughter.end(), all_nodes[x]) == daughter.end()) {
				mother.push_back(all_nodes[x]);
			}
		}
	}

	if (mother.size() + daughter.size() != all_nodes.size()) {
		cout << "checked:\n";
		for (auto ch : checked) {
			cout << "(" << ch->col << "," << ch->row << ")\n";
}
		cout << "mother:\n";
		for (auto ch: mother) {
			cout << "(" << ch->col << "," << ch->row << ")\n";
		}
		cout << "daughter:\n";
		for (auto ch : daughter) {
			cout << "(" << ch->col << "," << ch->row << ")\n";
		}
		cout << "all:\n";
		for (auto ch : all_nodes) {
			cout << "(" << ch->col << "," << ch->row << ")\n";
		}
		sp();
		return;
	}

	for (size_t x = 0; x < all_nodes.size(); x++) {
		change_n1_to_c(all_nodes[x], SUBSTRATE);
	}

	for (size_t x = 0; x < mother.size(); x++) {
		change_n1_to_c(mother[x], c);
	}

	Cell* daughter_cell;
	spawn_cell(daughter_cell, 100, 1, baby_cells); //the values 100 and 1 are placeholders

	for (size_t x = 0; x < daughter.size(); x++) {
		change_n1_to_c(daughter[x], daughter_cell);
	}


	daughter_cell->local_memory[Current_State_Index] = c->local_memory[Current_State_Index];
	daughter_cell->local_memory[Entering_State_Index] = c->local_memory[Entering_State_Index];
	daughter_cell->local_memory[State_to_Change_Into_Index] = c->local_memory[State_to_Change_Into_Index];
	daughter_cell->local_memory[Mitosis_Flag_Index] = c->local_memory[Mitosis_Flag_Index];
	daughter_cell->local_memory[Field_Active_Flag_Index] = c->local_memory[Field_Active_Flag_Index];
	daughter_cell->local_memory[Red_Index] = c->local_memory[Red_Index];
	daughter_cell->local_memory[Green_Index] = c->local_memory[Green_Index];
	daughter_cell->local_memory[Blue_Index] = c->local_memory[Blue_Index];
	daughter_cell->local_memory[Bulk_Modulus_Index] = c->local_memory[Bulk_Modulus_Index];
	daughter_cell->local_memory[Typical_Area_Index] = c->local_memory[Typical_Area_Index];
	daughter_cell->ppv = c->ppv;


	int temp_index;
	for (auto uvn : rule_parser->completeUserVariableNames) {
		temp_index = rule_parser->LMIM[uvn];
		c->local_memory[temp_index] = 0.5f * c->local_memory[temp_index];
		daughter_cell->local_memory[temp_index] = c->local_memory[temp_index];
	}


	daughter_cell->set_mitosis_flag(0);
	c->set_mitosis_flag(0);


	daughter_cell->local_memory[Goal_Area_Index] = daughter_cell->local_memory[Actual_Area_Index];
	c->local_memory[Goal_Area_Index] = c->local_memory[Actual_Area_Index];
	if (c->local_memory[Mitosis_State_to_Change_Into_Index] != c->local_memory[Current_State_Index]) {
		daughter_cell->local_memory[State_to_Change_Into_Index] = c->local_memory[Mitosis_State_to_Change_Into_Index];
	}
	else {
		daughter_cell->set_haveRulesProcessedRGB(1);
	}


	//the grid's 'isEdge' needs to be updated. There is no way an edge could be lost. But, there will be a need to mark new edges at the cleavage.
	for (auto d : daughter) {
		if (d->isEdge) { continue; }
		for (int j = 0; j < Order_I; j++) {
			if (d->neighbors[j]->cell != daughter_cell) {
				d->isEdge = 1;
				edges.insert(d);
				break;
			}
		}
	}

	for (auto m : mother) {
		if (m->isEdge) { continue; }
		for (int j = 0; j < Order_I; j++) {
			if (m->neighbors[j]->cell != c) {
				m->isEdge = 1;
				edges.insert(m);
				break;
			}
		}
	}




}
int PetriDish::getMitosisCleavageSide(Cell * c, sf::Vector2f v, Node* n)
{

	sf::Vector2f node_vec = getNodeLocalCartesian(c, n);
	// A = (x1, y1)
	// B = (x2, y2)
	// d = (x-x1)(y2-y1) - (y-y1)(x2-x1)
	// to see if a point C (x,y) is on one side of the line or the other... d is 
	// negative if its on one side, positive if on the other.

	//in this case, A is at the origin and B is the tip of v. node_vec is d.



	float d = (node_vec.x) * (v.y) - (node_vec.y) * (v.x);

	if (d <= 0) {
		return 0;
	}
	return 1;


};
void PetriDish::updateSetOfEdgesAndGrid(Node* n, Cell* former)
{

	Cell* target = n->cell;
	Cell* source = former;


	//deal with n
	//n->isEdge = 1;
	//edges.insert(n);
	if ((n->neighbors[0]->cell == source) &&
		(n->neighbors[1]->cell == source) &&
		(n->neighbors[2]->cell == source) &&
		(n->neighbors[3]->cell == source)) {
		n->isEdge = 0;
		edges.erase(edges.find(n));
	}


	// for each n neighbor (nn):
	   //			source |  target | other   (that is, nn=source, or nn=target, etc.)
	   //isEdge  | d	   |   O     |   O
	   //!isEdge | O	   |   d     |   O

	   //O = no change necessary
	   //d = check and change

	   //deal with n neighbor being equal to source:

	if ((n->neighbors[0]->cell == source) && (n->neighbors[0]->isEdge)) {
		if ((n->neighbors[0]->neighbors[0]->cell == source) &&
			(n->neighbors[0]->neighbors[1]->cell == source) &&
			(n->neighbors[0]->neighbors[3]->cell == source)) {
			n->neighbors[0]->isEdge = false;
			edges.erase(edges.find(n->neighbors[0]));
		}
	}
	if ((n->neighbors[1]->cell == source) && (n->neighbors[1]->isEdge)) {
		if ((n->neighbors[1]->neighbors[0]->cell == source) &&
			(n->neighbors[1]->neighbors[1]->cell == source) &&
			(n->neighbors[1]->neighbors[2]->cell == source)) {
			n->neighbors[1]->isEdge = false;
			edges.erase(edges.find(n->neighbors[1]));
		}
	}
	if ((n->neighbors[2]->cell == source) && (n->neighbors[2]->isEdge)) {
		if ((n->neighbors[2]->neighbors[1]->cell == source) &&
			(n->neighbors[2]->neighbors[2]->cell == source) &&
			(n->neighbors[2]->neighbors[3]->cell == source)) {
			n->neighbors[2]->isEdge = false;
			edges.erase(edges.find(n->neighbors[2]));
		}
	}
	if ((n->neighbors[3]->cell == source) && (n->neighbors[3]->isEdge)) {
		if ((n->neighbors[3]->neighbors[0]->cell == source) &&
			(n->neighbors[3]->neighbors[2]->cell == source) &&
			(n->neighbors[3]->neighbors[3]->cell == source)) {
			n->neighbors[3]->isEdge = false;
			edges.erase(edges.find(n->neighbors[3]));
		}
	}


	//deal with n neighbor being equal to target:
	if ((n->neighbors[0]->cell == target) && (n->neighbors[0]->isEdge == false)) {
		n->neighbors[0]->isEdge = true;
		edges.insert(n->neighbors[0]);

	}
	if ((n->neighbors[1]->cell == target) && (n->neighbors[1]->isEdge == false)) {
		n->neighbors[1]->isEdge = true;
		edges.insert(n->neighbors[1]);

	}
	if ((n->neighbors[2]->cell == target) && (n->neighbors[2]->isEdge == false)) {

		n->neighbors[2]->isEdge = true;
		edges.insert(n->neighbors[2]);

	}
	if ((n->neighbors[3]->cell == target) && (n->neighbors[3]->isEdge == false)) {

		n->neighbors[3]->isEdge = true;
		edges.insert(n->neighbors[3]);

	}
}
void PetriDish::updateCellEdgesAndMembranes()
{
	for (auto c : cells) {
		c->membrane_wrangler->clear();
		c->edges.clear();
	}

	for (auto e : edges) {
		if (e->cell == SUBSTRATE) { continue; }
		e->cell->insert_node_to_edges(e);
		for (int j = 0; j < Order_I; j++) {
			if (e->neighbors[j]->cell != e->cell) {
				e->cell->membrane_wrangler->add_membrane_associated_with_cell(e->neighbors[j]->cell);
			}
		}

	}


}
void PetriDish::setEmittingCellGradients()
{

	if (gradients_to_set_to_one.size() == 0) { return; }
	for (auto fn : flat_nodes) {

		for (auto g : gradients_to_set_to_one) {

			if (g.first == fn->cell) {
				fn->old_gradients[g.second] = 1;
			}
		}
	}

}
void PetriDish::diffuse()
{
	int s = flat_nodes[0]->old_gradients.size();
	for (auto fn : flat_nodes) {
		for (int x = 0; x < s; x++) {
			fn->new_gradients[x] = 0.25f *
				(fn->neighbors[0]->old_gradients[x] +
					fn->neighbors[1]->old_gradients[x] +
					fn->neighbors[2]->old_gradients[x] +
					fn->neighbors[3]->old_gradients[x]);

			if (fn->new_gradients[x] < 0.0001) { fn->new_gradients[x] = 0; }
		}
	}
	for (auto fn : flat_nodes) {
		fn->old_gradients = fn->new_gradients;

	}

}


sf::Vector2f PetriDish::getNodeLocalCartesian(Cell * c, Node * n)
{

	float delta_x = 0;
	float delta_y = 0;

	int raw_x_distance = abs(n->col - (int)c->cell_center_cartesian.x);
	int raw_y_distance = abs(n->row - (int)c->cell_center_cartesian.y);


	if (raw_x_distance < 0.5*cols) {
		delta_x = (float)n->col - c->cell_center_cartesian.x;
	}
	else {

		if (n->col > (int)c->cell_center_cartesian.x) {

			delta_x = -(c->cell_center_cartesian.x + (float)(cols - n->col));
		}
		if (n->col < (int)c->cell_center_cartesian.x) {
			delta_x = (float)cols - c->cell_center_cartesian.x + (float)(n->col);
		}
	}

	if (raw_y_distance < 0.5*rows) {
		delta_y = (float)n->row - c->cell_center_cartesian.y;
	}
	else {

		if (n->row > (int)c->cell_center_cartesian.y) {

			delta_y = -(c->cell_center_cartesian.y + (float)(rows - n->row));
		}
		if (n->row < (int)c->cell_center_cartesian.y) {
			delta_y = (float)rows - c->cell_center_cartesian.y + (float)(n->row);
		}
	}


	return sf::Vector2f(delta_x, delta_y);

}
sf::Vector2f PetriDish::getVectorTowardsGradient(Cell * c, int g)
{
	sf::Vector2f result(0, 0);

	Node * center_neighbor;

	sf::Vector2f holder(0, 0);
	for (int j = 0; j < Order_I; j++) {
		center_neighbor = c->cell_center_node->neighbors[j];
		switch (j) {
		case 0:
			holder += sf::Vector2f(0, -1) * center_neighbor->old_gradients[g];
			break;
		case 1:
			holder += sf::Vector2f(1, 0) * center_neighbor->old_gradients[g];
			break;
		case 2:
			holder += sf::Vector2f(0, 1) * center_neighbor->old_gradients[g];
			break;
		case 3:
			holder += sf::Vector2f(-1, 0) * center_neighbor->old_gradients[g];
			break;
		}
	}
	normalize_vector(holder);


	return holder;
}


sf::Vector2f PetriDish::getVectorTowardsNeighborStates(Cell* c, int state)
{

	vector<Node*> edges = c->getEdges();

	sf::Vector2f vector_sum(0, 0);
	for (auto e : edges) {

		bool found = 0;
		for (int j = 0; j < Order_I; j++) {
			if (e->neighbors[j]->cell == c) { continue; }
			if (e->neighbors[j]->cell->getState() == state) {
				found = 1;
				break;
			}
		}
		if (!found) { continue; }

		sf::Vector2f Node_vec = getNodeLocalCartesian(c, e);


		vector_sum += Node_vec;

	}

	//If the target neighbors were not found, return with a random cleavage vector
//	if (vector_sum == sf::Vector2f(0, 0)) {
	//	vector_sum.x = random_float_range(-1, 1);
	//	vector_sum.y = random_float_range(-1, 1);
	//}

	normalize_vector(vector_sum);
	return vector_sum;

}
sf::Vector2f PetriDish::getVectorTowardsUserVariables(Cell* c, int index)
{
	sf::Vector2f result(0, 0);
	sf::Vector2f vector_sum(0, 0);



	vector <Node*> edge = c->getEdges();

	for (auto e : edge) {
		for (size_t j = 0; j < Order_I; j++) {
			if (e->neighbors[j]->cell == c) { continue; }
			if (e->neighbors[j]->cell == SUBSTRATE) { continue; }

			if (rule_parser->get_GJ_value(c->getState(), e->neighbors[j]->cell->getState(), index) == 0) { continue; } //this precludes cells that have no gapjunction with present_cell
			sf::Vector2f Node_vec = getNodeLocalCartesian(c, e);




			vector_sum = Node_vec;



			switch (j) {
			case 0:
				vector_sum += sf::Vector2f(0, -.5);
				break;
			case 1:
				vector_sum += sf::Vector2f(.5, 0);
				break;
			case 2:
				vector_sum += sf::Vector2f(0, .5);
				break;
			case 3:
				vector_sum += sf::Vector2f(-.5, 0);
				break;
			default:
				break;
			}

			vector_sum *= e->neighbors[j]->cell->local_memory[index];
			result += vector_sum;
		}

	}
	//If the neighbors had no variables, return with a random cleavage vector
//	if (result == sf::Vector2f(0, 0)) {
//		result.x = random_float_range(-1, 1);
	//	result.y = random_float_range(-1, 1);
//	}

	normalize_vector(result);

	return result;

}
sf::Vector2f PetriDish::getVectorTowardsField(Cell * c, int f)
{
	vector<Node*> edges = c->getEdges();

	sf::Vector2f vector_sum(0, 0);
	int count = 0;
	sf::Vector2f holder(0, 0);
	for (auto e : edges) {
		if (getFieldValueAtNode(c, e) == f) {
			sf::Vector2f Node_vec = getNodeLocalCartesian(c, e);
			holder += Node_vec;
			count++;
		}
	}

	//if all nodes are in field, return rand
	if (count == edges.size()) {
		holder.x = 0;
		holder.y = 0;
	}

	normalize_vector(holder);

	return holder;

}


void PetriDish::change_n1_to_c(Node* n1, Cell* c)
{

	Cell* old_cell = n1->cell;
	Cell* new_cell = c;

	updateSetOfEdgesAndGrid(n1, c);

	n1->cell = c;

	//remove n1 from old_cell
	old_cell->remove_component(n1);

	//add n1 to new_cell
	new_cell->add_component(n1);


	need_to_update_cell_edges = true;

	return;
}
bool PetriDish::try_flip(Node* n, Cell* c)
{

	Cell* target = n->cell;
	Node* target_node = n;
	Cell* source = c;

	int target_state = target->getState();
	int source_state = c->getState();

	for (size_t x = 0; x < adhesion_cells_present.size(); x++) {
		adhesion_cells_present[x] = 0;
	}


	float target_self_count = 0;
	float source_self_count = 0;
	for (int j = 0; j < adhesion_order; j++) {

		if (n->neighbors[j]->cell == target) {
			if (target_state != 0) {
				target_self_count++;
			}
		}
		if (n->neighbors[j]->cell == source) {
			if (source_state != 0) {
				source_self_count++;
			}

		}

		adhesion_cells_present[n->neighbors[j]->cell->getState()]++;
	}



	float Area_J_Original;
	float Adhesion_J_Original = 0;
	float Total_J_Original = 0;

	float Area_J_Hypo;
	float Adhesion_J_Hypo = 0;
	float Total_J_Hypo = 0;


	//******ORIGINAL***********
	Area_J_Original = target->get_area_energy_component(CURRENT_AREA_ENERGY) +
		source->get_area_energy_component(CURRENT_AREA_ENERGY);
	for (size_t x = 0; x < adhesion_cells_present.size(); x++) {
		if ((x == target_state) && (x != 0)) {
			Adhesion_J_Original += (adhesion_cells_present[x] - target_self_count) * rule_parser->get_Adhesion_Value(target_state, x);
			continue;
		}
		Adhesion_J_Original += adhesion_cells_present[x] * rule_parser->get_Adhesion_Value(target_state, x);
	}
	Adhesion_J_Original += target_self_count * rule_parser->get_Adhesion_Self_Value(target_state);
	Total_J_Original += (Area_J_Original)+Adhesion_J_Original;
	//******ORIGINAL***********

	//******HYPO***********
	Area_J_Hypo = target->get_area_energy_component(AREA_ENERGY_ABSENT_SINGLE_NODE) +
		source->get_area_energy_component(AREA_ENERGY_PLUS_SINGLE_NODE);
	for (size_t x = 0; x < adhesion_cells_present.size(); x++) {
		if ((x == source_state) && (x != 0)) {
			Adhesion_J_Hypo += (adhesion_cells_present[x] - source_self_count) * rule_parser->get_Adhesion_Value(source_state, x);
			continue;
		}
		Adhesion_J_Hypo += adhesion_cells_present[x] * rule_parser->get_Adhesion_Value(source_state, x);
	}
	Adhesion_J_Hypo += source_self_count * rule_parser->get_Adhesion_Self_Value(source_state);
	Total_J_Hypo += (Area_J_Hypo)+Adhesion_J_Hypo;
	//******HYPO***********






	if (target != SUBSTRATE) {
		if (target->desired_migration_vector != sf::Vector2f(0, 0)) {
			if (cos_theta_between_two_vectors(getNodeLocalCartesian(target, target_node), target->desired_migration_vector) < -.85f) {
				Total_J_Hypo += -magnitude_vector(target->desired_migration_vector);
			}
		}
	}

	if (source != SUBSTRATE) {
		if (source->desired_migration_vector != sf::Vector2f(0, 0)) {
			if (cos_theta_between_two_vectors(getNodeLocalCartesian(source, target_node), source->desired_migration_vector) > .85f) {
				Total_J_Hypo += -magnitude_vector(source->desired_migration_vector);
			}
		}
	}



	if (compare_energies(Total_J_Original, Total_J_Hypo)) {
		change_n1_to_c(target_node, source);
		return true;
	}
	return false;
}
bool PetriDish::compare_energies(float old_J, float new_J)
{

	float delta_J = new_J - old_J;

	if (delta_J <= 0) {
		return true;
	}

	if (pow(2.718281828459, -delta_J / flux) > random_float()) {
		return true;
	}
	return false;
}





bool PetriDish::mitosis_contiguous_check(vector<Node*> n)
{

	vector <Node*> marked;
	marked.clear();
	vector <Node*> cands;
	cands.clear();
	Node* beginning_node;
	Node* current_node;
	beginning_node = n[rand() % n.size()];
	cands.push_back(beginning_node);
	while (cands.size() > 0) {
		current_node = cands[0];

		cands.erase(cands.begin());

		if (find(marked.begin(), marked.end(), current_node) == marked.end()) {
			marked.push_back(current_node);
		}
		Node* nn;
		for (int j = 0; j < Order_I; j++) {
			nn = current_node->neighbors[j];
			if (find(n.begin(), n.end(), nn) == n.end()) {
				continue;
			}
			if (
				(find(marked.begin(), marked.end(), nn) == marked.end()) &&
				(find(cands.begin(), cands.end(), nn) == cands.end())) {
				cands.push_back(nn);
			}

		}
	}
	if (marked.size() == n.size()) {
		return true;
	}
	return false;
}





void PetriDish::kill_cell(Cell* c) {


	if (c->selected) {
		selected_cell = NULL;
	}
	if (c->monitored) {
		monitored_cell = NULL;
	}

	vector <Node*> all_c_nodes;
	get_all_nodes_of_a_cell(c, all_c_nodes);

	for (auto sp : all_c_nodes) {
		change_n1_to_c(sp, SUBSTRATE);
	}
	cells.erase(find(cells.begin(), cells.end(), c));


}
void PetriDish::spawn_cell(Cell*& c, int size, int type, vector <Cell*>& where_to_put_it)
{
	c = new Cell(64, type, this);
	c->local_memory[Current_State_Index] = (float)type;
	c->local_memory[State_to_Change_Into_Index] = (float)type;
	c->local_memory[Goal_Area_Index] = (float)size;
	c->local_memory[Bulk_Modulus_Index] = 10.0f;
	c->local_memory[Entering_State_Index] = 1.0f;
	where_to_put_it.push_back(c);
}
void PetriDish::spawn_medium(Medium*& m)
{
	m = new Medium(100, rule_parser->SNIM[MEDIUM], this);
}
void PetriDish::manually_place_cell(Node* n, int type)
{


	Node* nn;
	if (n == NULL) { return; }

	if (selected_cell != NULL) {
		if (type != selected_cell->getState()) {
			selected_cell = NULL;
		}
	}

	if (selected_cell == NULL) {
		//See if there is room for initial placement
		if (n->cell != SUBSTRATE) { return; }

		for (int j = 0; j < Order_IV; j++) {
			nn = n->neighbors[j];
			if (nn->cell != SUBSTRATE) {
				return;
			}
		}

		//there is room, so create new cell and fill in Order_IV neighbors.
		spawn_cell(selected_cell, 100, type, cells);
		change_n1_to_c(n, selected_cell);
		for (int j = 0; j < Order_IV; j++) {
			nn = n->neighbors[j];
			change_n1_to_c(nn, selected_cell);
		}
		selected_cell->local_memory[Goal_Area_Index] = selected_cell->local_memory[Actual_Area_Index];
	}

	else {

		//connectivity test
		bool touching_old_cell = false;
		for (int j = 0; j < Order_IV; j++) {
			nn = n->neighbors[j];
			if (nn->cell == selected_cell) {
				touching_old_cell = true;
				break;
			}
		}
		if (!touching_old_cell) { return; }


		if (n->cell == SUBSTRATE) {
			change_n1_to_c(n, selected_cell);
		}
		for (int j = 0; j < Order_IV; j++) {
			nn = n->neighbors[j];
			if (nn->cell != SUBSTRATE) { continue; }
			change_n1_to_c(nn, selected_cell);
		}
		selected_cell->local_memory[Goal_Area_Index] = selected_cell->local_memory[Actual_Area_Index];

	}



}
void PetriDish::auto_place_cell(Init_Placement ip)
{
	Node* n = flat_nodes[int(ip.row) * cols + int(ip.col)];
	Cell* placed_cell;
	spawn_cell(placed_cell, ip.size, ip.cell_type, cells);
	change_n1_to_c(n, placed_cell);
	int size_count = 1;
	Node* nn;
	vector <Node*> to_check;
	to_check.clear();
	to_check.push_back(n);

	for (size_t k = 0; k < to_check.size(); k++) {

		for (int j = 0; j < Order_I; j++) {
			nn = to_check[k]->neighbors[j];
			if (nn->cell != placed_cell) {
				change_n1_to_c(nn, placed_cell);
				size_count++;
				if (size_count >= ip.size) {
					return;
				}
				to_check.push_back(nn);
			}
		}
	}

}

bool PetriDish::createCellSortingGrid(int cs_rows, int cs_cols)
{


	clear_petri_dish();

	Cell* cell_holder;
	Node* nn;
	Node* n;


	int cell_size = 40;  //the diameter is around 8 or so.
	int cell_diameter = 8;

	if (cell_diameter * cs_cols > cols) {
		return 0;
	}
	if (cell_diameter * cs_rows > rows) {
		return 0;
	}

	int col_offset = (int)((float)cols / 2.0f) - 0.5f * (float)cell_diameter * (float)cs_cols;
	int row_offset = (int)((float)rows / 2.0f) - 0.5f * (float)cell_diameter * (float)cs_rows;

	int row_to_place_cell;
	int col_to_place_cell;
	for (int v = 0; v < cs_rows; v++) {
		row_to_place_cell = v + row_offset;
		for (int h = 0; h < cs_cols; h++) {
			col_to_place_cell = h + col_offset;
			n = flat_nodes[row_to_place_cell * cols + col_to_place_cell];
			spawn_cell(cell_holder, cell_size, rand() % 2 + 1, cells);  //1 = blue; 2 = yellow
			for (int j = 0; j < Order_IV; j++) {
				nn = n->neighbors[j];
				change_n1_to_c(nn, cell_holder);
			}

		}
	}
	return 1;
}


sf::Vector2f PetriDish::getWrapAroundOffset(sf::Vector2f& N, sf::Vector2f& i)
{
	sf::Vector2f result(0, 0);

	if (fabs(N.x - i.x) > 0.5f * (float)cols) {
		result.x = 0.5f * (float)cols;
	}
	if (fabs(N.y - i.y) > 0.5f * (float)rows) {
		result.y = 0.5f * (float)rows;
	}
	return result;
}
void PetriDish::applyWrapAroundOffset(sf::Vector2f& i, sf::Vector2f offset)
{
	i += offset;
	if ((i.x < 0) || (i.x >= (float)cols)) {
		i.x = fmod(i.x, (float)cols);
		if (i.x < 0) {
			i.x = (float)cols + i.x;
		}
	}
	if ((i.y < 0) || (i.y >= (float)rows)) {
		i.y = fmod(i.y, (float)rows);
		if (i.y < 0) {
			i.y = (float)rows + i.y;
		}
	}
}
void PetriDish::undoWrapAroundOffset(sf::Vector2f& i, sf::Vector2f offset)
{
	i -= offset;
	if ((i.x < 0) || (i.x >= (float)cols)) {
		i.x = fmod(i.x, (float)cols);
		if (i.x < 0) {
			i.x = (float)cols + i.x;
		}
	}
	if ((i.y < 0) || (i.y >= (float)rows)) {
		i.y = fmod(i.y, (float)rows);
		if (i.y < 0) {
			i.y = (float)rows + i.y;
		}
	}
}





void PetriDish::updatePPInSingleCellEdges(Cell* c) {
	sf::Vector2f offset_vector;
	vector <Node*> c_edges;
	c_edges = c->edges.getData();

	bool isTouchingObstructed = 0;

	for (auto ce : c_edges) {

		for (int j = 0; j < Order_I; j++) {
			if (ce->neighbors[j]->cell->getState() == default_OBSTRUCTION_index) {
				isTouchingObstructed = 1;
				break;
			}
		}
		if (isTouchingObstructed) { break; }
	}


	//db(isTouchingObstructed);
	c->isTouchingObstructed = isTouchingObstructed;

	//short detour to set appropriate orientation offset for cells touching obstruction
	if (isTouchingObstructed) {
		//	c->ppv.offset = 90;
			//calculateOrientationOffset(c);
	}


	c->isTouchingObstructed = isTouchingObstructed;
	for (auto ce : c_edges) {



		//get translation
		//offset_vector = sf::Vector2f(float(ce->col) - c->cell_center_cartesian.x, float(ce->row) - c->cell_center_cartesian.y);
		offset_vector = getAdjustedDeltaVectorDueToWrapAround(sf::Vector2f((float)ce->col, (float)ce->row), c->cell_center_cartesian);


		//rotate

		offset_vector = rotate_vector_using_degrees(offset_vector, c->ppv.orientation);


		//scale
		offset_vector.x *= (1.0f / c->ppv.scale_v.x);
		offset_vector.y *= (1.0f / c->ppv.scale_v.y);

		ce->presumed_position_value_holder = c->ppv.location + offset_vector;
	}

}


void PetriDish::updatePPInAllEdges() {


	for (auto c : cells) {
		if (c->getState() == default_OBSTRUCTION_index) { continue; }
		updatePPInSingleCellEdges(c);
	}
}
float PetriDish::getScoreAtNode(Cell* c, Presumed_Position_Vector& plv, Node* n)
{

	sf::Vector2f result_vector = getAdjustedDeltaVectorDueToWrapAround(sf::Vector2f((float)n->col, (float)n->row), c->cell_center_cartesian);

	//result_vector = (sf::Vector2f(n->col, n->row) - c->cell_center_cartesian);

	result_vector = rotate_vector_using_degrees(result_vector, plv.orientation);
	result_vector.x *= (1.0f / plv.scale_v.x);
	result_vector.y *= (1.0f / plv.scale_v.y);
	result_vector += plv.location;


	float delta = fabs(result_vector.x - n->presumed_position_value_holder.x) + fabs(result_vector.y - n->presumed_position_value_holder.y);


	//	float delta = ((result_vector.x - n->presumed_location_value_holder.x)
	//		* (result_vector.x - n->presumed_location_value_holder.x)) +
	//		((result_vector.y - n->presumed_location_value_holder.y)
	//			* (result_vector.y - n->presumed_location_value_holder.y));
	return delta;
}
int PetriDish::getFieldValueAtNode(Cell* c, Node* n)
{
	sf::Vector2f result_vector = getAdjustedDeltaVectorDueToWrapAround(sf::Vector2f((float)n->col, (float)n->row), c->cell_center_cartesian);

	//result_vector = (sf::Vector2f(n->col, n->row) - c->cell_center_cartesian);
	result_vector = rotate_vector_using_degrees(result_vector, c->ppv.orientation);
	result_vector.x *= (1.0f / c->ppv.scale_v.x);
	result_vector.y *= (1.0f / c->ppv.scale_v.y);

	result_vector += c->ppv.location;
	return FieldObject->getFieldValueAtParticularCartesianLocation(result_vector);
}
Node * PetriDish::PetriCartesianToNode(sf::Vector2f v)
{
	int _row = positive_int_mod(v.y, rows);
	int _col = positive_int_mod(v.x, cols);
	return flat_nodes[_row *cols + _col];

}
sf::Vector2f PetriDish::getAdjustedDeltaVectorDueToWrapAround(sf::Vector2f a, sf::Vector2f b) {
	bool need_to_mod_flag = 0;

	sf::Vector2f wrap_around_offset(0, 0);
	sf::Vector2f mod_value((float)cols, (float)rows);
	if ((fabs(a.x - b.x) > 0.5f * (float)cols)) {
		wrap_around_offset.x = 0.5f * (float)cols;
		need_to_mod_flag = 1;
	}
	if ((fabs(a.y - b.y) > 0.5f * (float)rows)) {
		wrap_around_offset.y = 0.5f * (float)rows;
		need_to_mod_flag = 1;
	}

	if (need_to_mod_flag) {
		a = a + wrap_around_offset;
		b = b + wrap_around_offset;
		wrap_around_sfvector_fmod(a, mod_value);
		wrap_around_sfvector_fmod(b, mod_value);
	}

	return sf::Vector2f(a.x - b.x, a.y - b.y);
}
void PetriDish::adjustPPs() {

	Presumed_Position_Vector best_plv_holder;
	Presumed_Position_Vector tester;
	bool improved = 0;
	float best_score;
	float score;

	set <Node*> edge_neighbors;
	vector <Node*> c_edges;
	vector <Cell*> rs_cells = cells;

	//Orientation
	random_shuffle(rs_cells.begin(), rs_cells.end());


	if (randomize_orientation_one_shot) {

		for (auto c : rs_cells) {
			if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }
	
			c->ppv.orientation = random_float_range(0, 360);
			updatePPInSingleCellEdges(c);
		}

	}


	if (levin_juice) {
		for (auto c : rs_cells) {
			if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }
			if (c->get_adjacent_membrane_count(0) < 4) { continue; }
			sf::Vector2f cut_vec;
			
			cut_vec = getVectorTowardsNeighborStates(c, 0);
			c->ppv.orientation = angle_in_degrees_of_vector(cut_vec);
			updatePPInSingleCellEdges(c);
		}
	}

	//float age = 0;
	float smidge = .015;
	for (auto c : rs_cells) {
		if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }
		if (c->get_adjacent_membrane_count(0) < 4) { continue; }
		vector <Cell *> c_n = c->get_all_neighbor_cells();

		for (auto cn : c_n) {
			if (cn->local_memory[Field_Active_Flag_Index] == 0) { continue; }
			if (cn == SUBSTRATE) { continue; }
			if (cn->getState() == default_OBSTRUCTION_index) { continue; }
			c->ppv.orientation = add_a_smidgen_of_an_angle_to_original(c->ppv.orientation, cn->ppv.orientation, smidge);
		}

		updatePPInSingleCellEdges(c);
	}
	//Scale
	for (auto c : rs_cells) {
		if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }

		edge_neighbors.clear();
		c_edges = c->edges.getData();
		for (auto ce : c_edges) {
			for (int j = 0; j < Order_I; j++) {
				if (ce->neighbors[j]->cell->local_memory[Field_Active_Flag_Index] == 0) { continue; }

				if (ce->neighbors[j]->cell == c) { continue; }
				if (ce->neighbors[j]->cell == SUBSTRATE) { continue; }
				edge_neighbors.insert(ce->neighbors[j]);
			}
		}

		if (edge_neighbors.size() == 0) { continue; }
		sf::Vector2f scale_sum(0, 0);
		for (auto ce : edge_neighbors) {
			scale_sum += ce->cell->ppv.scale_v;
		}
		sf::Vector2f scale_average = scale_sum / (float)edge_neighbors.size();
		c->ppv.scale_v += 0.15f * (scale_average - c->ppv.scale_v);
		updatePPInSingleCellEdges(c);
	}

	//Translation
	float orientation_range = 0;
	for (auto c : rs_cells) {
		if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }

		edge_neighbors.clear();
		c_edges = c->edges.getData();
		for (auto ce : c_edges) {
			for (int j = 0; j < Order_I; j++) {
				if (ce->neighbors[j]->cell->local_memory[Field_Active_Flag_Index] == 0) { continue; }

				if (ce->neighbors[j]->cell == c) { continue; }
				if (ce->neighbors[j]->cell == SUBSTRATE) { continue; }
				edge_neighbors.insert(ce->neighbors[j]);
			}
		}

		if (edge_neighbors.size() == 0) { continue; }


		best_plv_holder = c->ppv;
		best_score = 0;

		best_plv_holder = c->ppv;
		best_score = 0;
		bool big_spread_present = 0;
		for (auto en : edge_neighbors) {
			best_score += getScoreAtNode(c, c->ppv, en);
			if (smallest_spread_between_two_angles(c->ppv.orientation, en->cell->ppv.orientation) > 45) {
				big_spread_present = 1;
			}
		}
		improved = 0;

		if (big_spread_present) {
			orientation_range = 10;
		}
		else { orientation_range = 2; }

		for (int k = 0; k < 30; k++) {
			tester = c->ppv;



			tester.location.x += random_float_range(-2.5, 2.5);
			tester.location.y += random_float_range(-2.5, 2.5);

			tester.orientation = tester.orientation + random_float_range(-orientation_range, orientation_range);
			score = 0;

			for (auto en : edge_neighbors) {


				score += getScoreAtNode(c, tester, en);
			}

			if (score < best_score) {
				best_score = score;
				best_plv_holder = tester;
				improved = 1;
			}

		}

		if (improved) {

			c->ppv = best_plv_holder;
			updatePPInSingleCellEdges(c);

		}


	}
}

void PetriDish::adjustPPs2() {

	//expiremental better version of function.
	//doesn't quite work yet.


	Presumed_Position_Vector best_plv_holder;
	Presumed_Position_Vector tester;
	bool improved = 0;
	float best_score;
	float score;

	set <Node*> edge_neighbors;
	vector <Node*> c_edges;
	vector <Cell*> rs_cells = cells;
	random_shuffle(rs_cells.begin(), rs_cells.end());
	////Orientation
	//
	////float age = 0;
	float smidge = .015;
	for (auto c : rs_cells) {

	    vector <Cell *> c_n = c->get_all_neighbor_cells();

	    for (auto cn : c_n) {

	        if (cn == SUBSTRATE) { continue; }
	        if (cn->getState() == default_OBSTRUCTION_index) { continue; }
	        c->ppv.orientation = add_a_smidgen_of_an_angle_to_original(c->ppv.orientation, cn->ppv.orientation, smidge);
	    }

	    updatePPInSingleCellEdges(c);
	}
	////Scale
	//for (auto c : rs_cells) {
	//    if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }

	//    edge_neighbors.clear();
	//    c_edges = c->edges.getData();
	//    for (auto ce : c_edges) {
	//        for (int j = 0; j < Order_I; j++) {
	//            if (ce->neighbors[j]->cell->local_memory[Field_Active_Flag_Index] == 0) { continue; }

	//            if (ce->neighbors[j]->cell == c) { continue; }
	//            if (ce->neighbors[j]->cell == SUBSTRATE) { continue; }
	//            edge_neighbors.insert(ce->neighbors[j]);
	//        }
	//    }

	//    if (edge_neighbors.size() == 0) { continue; }
	//    sf::Vector2f scale_sum(0, 0);
	//    for (auto ce : edge_neighbors) {
	//        scale_sum += ce->cell->ppv.scale_v;
	//    }
	//    sf::Vector2f scale_average = scale_sum / (float)edge_neighbors.size();
	//    c->ppv.scale_v += 0.15f * (scale_average - c->ppv.scale_v);
	//    updatePPInSingleCellEdges(c);
	//}

	////Scale
	for (auto c : rs_cells) {
		if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }

		edge_neighbors.clear();
		c_edges = c->edges.getData();
		for (auto ce : c_edges) {
			for (int j = 0; j < Order_I; j++) {
				if (ce->neighbors[j]->cell->local_memory[Field_Active_Flag_Index] == 0) { continue; }

				if (ce->neighbors[j]->cell == c) { continue; }
				if (ce->neighbors[j]->cell == SUBSTRATE) { continue; }
				edge_neighbors.insert(ce->neighbors[j]);
			}
		}

		if (edge_neighbors.size() == 0) { continue; }
		sf::Vector2f scale_sum(0, 0);
		for (auto ce : edge_neighbors) {
			scale_sum += ce->cell->ppv.scale_v;
		}
		sf::Vector2f scale_average = scale_sum / (float)edge_neighbors.size();
		c->ppv.scale_v += 0.15f * (scale_average - c->ppv.scale_v);
		updatePPInSingleCellEdges(c);
	}



	//Translation
	for (auto c : rs_cells) {

		sf::Vector2f best_adjustment(0, 0);
		float best = 0;
		float score = 0;
		if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }
		vector <Cell*> cells_touching = c->get_all_neighbor_cells();
		vector <Node*> all_c_edges = c->getEdges();
		vector <PAS> pp_data;
		pp_data.clear();
		PAS pp_holder;

		sf::Vector2f vv;

		for (auto nc : cells_touching) {
			if (nc == SUBSTRATE) { continue; }
			if (nc->local_memory[Field_Active_Flag_Index] == 0) { continue; }
			pp_holder.cell = c;
			pp_holder.neighbor = nc;
			pp_holder.cell_edge_centroid = sf::Vector2f(0, 0);
			pp_holder.neighbor_edge_centroid = sf::Vector2f(0, 0);

			set <Node*> neighbor_nodes;
			neighbor_nodes.clear();
			set <Node*> c_edges_touching_neighbor;
			c_edges_touching_neighbor.clear();

			for (auto ce : all_c_edges) {
				for (int j = 0; j < Order_I; j++) {
					if (ce->neighbors[j]->cell == nc) {
						neighbor_nodes.insert(ce->neighbors[j]);
						c_edges_touching_neighbor.insert(ce);
					}
				}
			}

			for (auto nn : neighbor_nodes) {
				pp_holder.neighbor_edge_centroid += nn->presumed_position_value_holder;
			}
			pp_holder.neighbor_edge_centroid /= (float)neighbor_nodes.size();
			for (auto c_et : c_edges_touching_neighbor) {
				pp_holder.cell_edge_centroid += c_et->presumed_position_value_holder;
			}
			pp_holder.cell_edge_centroid /= (float)c_edges_touching_neighbor.size();

			pp_data.push_back(pp_holder);
		}

		float orient_bias = .1;
		for (int k = 0; k<pp_data.size(); k++) {
			vv = pp_data[k].cell_edge_centroid - pp_data[k].neighbor_edge_centroid;
			score += magnitude_vector(vv);
			score += orient_bias * (fabs(pp_data[k].cell->ppv.orientation - pp_data[k].neighbor->ppv.orientation));
		}

		best = score;
		sf::Vector2f adjustment(0, 0);
		float orient_adjustment = 0;
		float best_orient = 0;;
		for (int x = 0; x < 30; x++) {
			score = 0;
			adjustment.x = random_float_range(-2.5, 2.5);
			adjustment.y = random_float_range(-2.5, 2.5);
			orient_adjustment = random_float_range(-1.3, 1.3);

			for (int k = 0; k < pp_data.size(); k++) {
				sf::Vector2f c_e_holder = pp_data[k].cell_edge_centroid;
				c_e_holder += adjustment;
				c_e_holder = rotate_vector_using_degrees(c_e_holder, orient_adjustment);


				vv = (c_e_holder - pp_data[k].neighbor_edge_centroid);
				score += magnitude_vector(vv);
				score += orient_bias * (fabs((pp_data[k].cell->ppv.orientation + orient_adjustment) - pp_data[k].neighbor->ppv.orientation));
			}
			if (score < best) {
				best_adjustment = adjustment;
				best_orient = orient_adjustment;
				best = score;
			}

		}
		if ((best_orient != 0) || (best_adjustment != sf::Vector2f(0, 0))) {
			c->ppv.location += best_adjustment;
			c->ppv.orientation += best_orient;
			updatePPInSingleCellEdges(c);
		}




	}
}


void PetriDish::adjustPPLbasedOnObstruction() {

	sf::Vector2f vector_towards_obtruction(0, 0);
	for (auto c : cells) {
		if (c->getState() == default_OBSTRUCTION_index) { continue; }

		//if (c->getFieldValueAtPresumedPosition() == 0) { continue; }
		vector<Node*> edges = c->getEdges();
		if (c->getNumberOfEdgeNodesWithThisFieldValue(0) == edges.size()) { continue; }
		sf::Vector2f vector_sum(0, 0);
		for (auto e : edges) {

			bool found = 0;
			for (int j = 0; j < Order_I; j++) {
				if (e->neighbors[j]->cell == c) { continue; }
				if (e->neighbors[j]->cell->getState() == default_OBSTRUCTION_index) {
					found = 1;
					break;
				}
			}
			if (!found) { continue; }

			sf::Vector2f Node_vec = getNodeLocalCartesian(c, e);


			vector_sum += Node_vec;

		}


		if (vector_sum == sf::Vector2f(0, 0)) {
			continue;
		}

		normalize_vector(vector_sum);
		vector_sum = rotate_vector_using_degrees(vector_sum, c->ppv.orientation);
		c->ppv.location += 2.5f*vector_sum;


	}

}


float PetriDish::cos_theta_between_two_vectors(sf::Vector2f a, sf::Vector2f b)
{

	normalize_vector(a);
	normalize_vector(b);
	return dot_product_vectors(a, b);

}


void PetriDish::get_all_nodes_of_a_cell(Cell* c, vector<Node*>& result)
{
	result.clear();
	for (auto fn : flat_nodes) {
		if (fn->cell == c) {
			result.push_back(fn);
		}
	}
}

bool PetriDish::isNodeTouchingThisCellState(Node * n, int x)
{
	for (int j = 0; j < Order_I; j++) {
		if (n->neighbors[j]->cell->getState() == x) { return true; }
	}
	
	return false;
}

bool PetriDish::can_n_be_removed_connectivity_check(Node* n)
{
	Node* nn;
	Cell* c = n->cell;
	if (n->cell == SUBSTRATE) { return true; }
	//4,0,5
	//3,x,1
	//7,2,6

	int count_cn = 0;
	for (int j = 0; j < Order_I; j++) {
		nn = n->neighbors[j];


		if (nn->cell == c) {
			count_cn++;
		}
	}


	if (count_cn == 0) { return true; }
	if (count_cn == 1) { return true; }
	if (count_cn == 2) {

		if (
			((n->neighbors[0]->cell == c) && (n->neighbors[2]->cell == c))

			||
			((n->neighbors[1]->cell == c) && (n->neighbors[3]->cell == c))

			||

			((n->neighbors[0]->cell == c) && (n->neighbors[1]->cell == c) && (n->neighbors[5]->cell != c))

			||

			((n->neighbors[1]->cell == c) && (n->neighbors[2]->cell == c) && (n->neighbors[6]->cell != c))

			||
			((n->neighbors[2]->cell == c) && (n->neighbors[3]->cell == c) && (n->neighbors[7]->cell != c))

			||
			((n->neighbors[3]->cell == c) && (n->neighbors[0]->cell == c) && (n->neighbors[4]->cell != c))

			) {
			return false;
		}
		else { return true; }
	}
	if (count_cn == 3) {
		if (

			(
			((n->neighbors[0]->cell == c) && (n->neighbors[1]->cell == c) && (n->neighbors[2]->cell == c)) &&
				((n->neighbors[5]->cell != c) || (n->neighbors[6]->cell != c))
				)

			||

			(
			((n->neighbors[0]->cell == c) && (n->neighbors[1]->cell == c) && (n->neighbors[3]->cell == c)) &&
				((n->neighbors[5]->cell != c) || (n->neighbors[4]->cell != c))
				)
			||

			(
			((n->neighbors[1]->cell == c) && (n->neighbors[2]->cell == c) && (n->neighbors[3]->cell == c)) &&
				((n->neighbors[6]->cell != c) || (n->neighbors[7]->cell != c))
				)

			||

			(
			((n->neighbors[2]->cell == c) && (n->neighbors[3]->cell == c) && (n->neighbors[0]->cell == c)) &&
				((n->neighbors[4]->cell != c) || (n->neighbors[7]->cell != c))
				)
			) {
			return false;
		}
		else { return true; }
	}


	if (count_cn == 4) {

		if (
			((n->neighbors[4]->cell != c) && (n->neighbors[5]->cell != c)) ||
			((n->neighbors[4]->cell != c) && (n->neighbors[6]->cell != c)) ||
			((n->neighbors[4]->cell != c) && (n->neighbors[7]->cell != c)) ||
			((n->neighbors[5]->cell != c) && (n->neighbors[6]->cell != c)) ||
			((n->neighbors[5]->cell != c) && (n->neighbors[7]->cell != c)) ||
			((n->neighbors[6]->cell != c) && (n->neighbors[7]->cell != c))
			) {
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}
bool PetriDish::can_n_be_changed_to_C_connectivity_check(Node* n, Cell* c)
{
	Node* nn;

	//4,0,5
	//3,x,1
	//7,2,6
	if (c == SUBSTRATE) { return true; } // we don't have to worry about severing something
										//because we have already checked to see if n could be removed.
	int count_cn = 0;
	for (int j = 0; j < Order_I; j++) {
		nn = n->neighbors[j];


		if (nn->cell == c) {
			count_cn++;
		}
	}


	if (count_cn == 0) { return true; }
	if (count_cn == 1) { return true; }
	if (count_cn == 2) {

		if (
			((n->neighbors[0]->cell == c) && (n->neighbors[2]->cell == c))

			||
			((n->neighbors[1]->cell == c) && (n->neighbors[3]->cell == c))

			||

			((n->neighbors[0]->cell == c) && (n->neighbors[1]->cell == c) && (n->neighbors[5]->cell != c))

			||

			((n->neighbors[1]->cell == c) && (n->neighbors[2]->cell == c) && (n->neighbors[6]->cell != c))

			||
			((n->neighbors[2]->cell == c) && (n->neighbors[3]->cell == c) && (n->neighbors[7]->cell != c))

			||
			((n->neighbors[3]->cell == c) && (n->neighbors[0]->cell == c) && (n->neighbors[4]->cell != c))

			) {
			return false;
		}
		else { return true; }
	}
	if (count_cn == 3) {
		if (

			(
			((n->neighbors[0]->cell == c) && (n->neighbors[1]->cell == c) && (n->neighbors[2]->cell == c)) &&
				((n->neighbors[5]->cell != c) || (n->neighbors[6]->cell != c))
				)

			||

			(
			((n->neighbors[0]->cell == c) && (n->neighbors[1]->cell == c) && (n->neighbors[3]->cell == c)) &&
				((n->neighbors[5]->cell != c) || (n->neighbors[4]->cell != c))
				)
			||

			(
			((n->neighbors[1]->cell == c) && (n->neighbors[2]->cell == c) && (n->neighbors[3]->cell == c)) &&
				((n->neighbors[6]->cell != c) || (n->neighbors[7]->cell != c))
				)

			||

			(
			((n->neighbors[2]->cell == c) && (n->neighbors[3]->cell == c) && (n->neighbors[0]->cell == c)) &&
				((n->neighbors[4]->cell != c) || (n->neighbors[7]->cell != c))
				)
			) {
			return false;
		}
		else { return true; }
	}


	if (count_cn == 4) {

		if (
			((n->neighbors[4]->cell != c) && (n->neighbors[5]->cell != c)) ||
			((n->neighbors[4]->cell != c) && (n->neighbors[6]->cell != c)) ||
			((n->neighbors[4]->cell != c) && (n->neighbors[7]->cell != c)) ||
			((n->neighbors[5]->cell != c) && (n->neighbors[6]->cell != c)) ||
			((n->neighbors[5]->cell != c) && (n->neighbors[7]->cell != c)) ||
			((n->neighbors[6]->cell != c) && (n->neighbors[7]->cell != c))
			) {
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}


void PetriDish::draw(Window_Manager* window_manager)
{

	float scale = parent_lab->get_petri_width() / float(cols);

	sf::Vector2f petri_pos = parent_lab->get_petri_pos();
	sf::RectangleShape rect;

	rect.setSize(sf::Vector2f(scale, scale));
	vector <sf::Vertex> grid_line_vertices;
	grid_line_vertices.clear();
	sf::Vertex vertice;

	vertice.color = sf::Color(96, 96, 96);

	sf::CircleShape circ;
	circ.setRadius(1 * scale);
	circ.setOrigin(.5f * scale, .5f * scale);
	//circ.setFillColor(sf::Color(120, 20, 20));
	circ.setFillColor(sf::Color::Red);
	rect.setSize(sf::Vector2f(scale, scale));
	std::vector<sf::Vertex> quad_vertices;
	quad_vertices.clear();

	if (parent_lab->isDrawFieldOn()) {
		FieldObject->field_sprite_with_white_transparent.setScale(scale, scale);
		//vertice.color = sf::Color(255, 255, 255, 20);
		for (auto c : cells) {
			if (c->local_memory[Field_Active_Flag_Index] == 0) { continue; }
			//if (rand() % 10 > 0) { continue; }

		/*	sf::Vector2f ttl = (TL - c->plv.pos);
			sf::Vector2f ttr = (TR - c->plv.pos);
			sf::Vector2f bbr = (BR - c->plv.pos);
			sf::Vector2f bbl = (BL - c->plv.pos);


			ttl = rotate_vector_using_degrees(ttl , c->plv.orientation);
			ttr = rotate_vector_using_degrees(ttr , c->plv.orientation);
			bbr= rotate_vector_using_degrees(bbr , c->plv.orientation);
			bbl = rotate_vector_using_degrees(bbl , c->plv.orientation);



			vertice.position =petri_pos+ scale * (ttl + c->cell_center_cartesian);
			vertice.texCoords = sf::Vector2f(0,0);
			quad_vertices.push_back(vertice);
			vertice.position = petri_pos + scale * (ttr  + c->cell_center_cartesian);
			vertice.texCoords = sf::Vector2f(cols,0);
			quad_vertices.push_back(vertice);
			vertice.position = petri_pos + scale * (bbr  + c->cell_center_cartesian);
			vertice.texCoords = sf::Vector2f(cols, rows);
			quad_vertices.push_back(vertice);
			vertice.position = petri_pos + scale * (bbl  + c->cell_center_cartesian);
			vertice.texCoords = sf::Vector2f(0, rows);
			quad_vertices.push_back(vertice);*/

			FieldObject->field_sprite_with_white_transparent.setScale(scale * c->ppv.scale_v.x, scale * c->ppv.scale_v.y);

			FieldObject->field_sprite_with_white_transparent.setOrigin(sf::Vector2f(c->ppv.location));
			FieldObject->field_sprite_with_white_transparent.setRotation(-(c->ppv.orientation));
			FieldObject->field_sprite_with_white_transparent.setPosition(petri_pos.x + c->cell_center_cartesian.x * scale, petri_pos.y + c->cell_center_cartesian.y * scale);
			FieldObject->field_sprite_with_white_transparent.setColor(sf::Color(255, 255, 255, 100));

			if (take_picture) { window_manager->get_r_texture().draw(FieldObject->field_sprite_with_white_transparent); }
			window_manager->getWindow().draw(FieldObject->field_sprite_with_white_transparent);
		}

		/*if (quad_vertices.size() > 0) {
			window_manager->getWindow().draw(&quad_vertices[0], quad_vertices.size(),sf::Quads, &(parent_lab->FieldObject->field_texture_with_white_transparent) );
		}*/
	}
	else {

		if (monitored_cell != NULL) {
			if (monitored_cell->local_memory[Field_Active_Flag_Index] == 1) {
				FieldObject->field_sprite_with_white_transparent.setScale(scale, scale);

				FieldObject->field_sprite_with_white_transparent.setScale(scale * monitored_cell->ppv.scale_v.x, scale * monitored_cell->ppv.scale_v.y);

				FieldObject->field_sprite_with_white_transparent.setOrigin(sf::Vector2f(monitored_cell->ppv.location));
				FieldObject->field_sprite_with_white_transparent.setRotation(-monitored_cell->ppv.orientation);
				FieldObject->field_sprite_with_white_transparent.setPosition(petri_pos.x + monitored_cell->cell_center_cartesian.x * scale, petri_pos.y + monitored_cell->cell_center_cartesian.y * scale);
				FieldObject->field_sprite_with_white_transparent.setColor(sf::Color(255, 255, 255, 100));


				window_manager->getWindow().draw(FieldObject->field_sprite_with_white_transparent);
			}

		}
	}
	float scaled_col;
	float scaled_row;

	std::vector < sf::Vertex > triangle_vertices;
	triangle_vertices.clear();
	//std::vector<sf::Vertex> quad_vertices;
	quad_vertices.clear();
	std::vector<sf::Vertex> line_vertices;
	line_vertices.clear();
	sf::Vertex vert_holder;
	sf::Color line_color = sf::Color(50, 20, 20);
	sf::Color edge_color;
	sf::Color edge_interior_color;
	sf::Color interior_color;
	bool nn = 0; bool ee = 0; bool ww = 0; bool ss = 0;
	for (auto fn : flat_nodes) {
		if (fn->cell == SUBSTRATE) {
			continue;
			/*	if (fn->isEdge) {
						vert_holder.color = sf::Color(100, 20, 20);
						vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f(fn->col * scale, fn->row * scale));
						quad_vertices.push_back(vert_holder);
						vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((1 + fn->col) * scale, fn->row * scale));
						quad_vertices.push_back(vert_holder);
						vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((1 + fn->col) * scale, (1 + fn->row) * scale));
						quad_vertices.push_back(vert_holder);
						vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f(fn->col * scale, (1 + fn->row) * scale));
						quad_vertices.push_back(vert_holder);
					}


					continue;*/
		}



		interior_color = fn->cell->get_color();

		sf::Color cholder = fn->cell->get_edge_color();
		edge_color.r = cholder.r * .3;
		edge_color.g = cholder.g * .3;
		edge_color.b = cholder.b * .3;
		edge_interior_color = interior_color;


		//	edge_color = sf::Color(90, 10, 10);


		if (parent_lab->lab_state == Manipulate_State) {
			if (parent_lab->cells_selected.isInSpecialVector(fn->cell)) {
				if (fn->isEdge) { edge_color = sf::Color(255, 55, 55, 240); }
				interior_color = sf::Color(0, 0, 0, 0);
			}
		}

		scaled_col = fn->col * scale;
		scaled_row = fn->row * scale;
		sf::Vector2f proper_position = petri_pos + sf::Vector2f(scaled_col, scaled_row);

		if (!fn->isEdge) {
			if (!parent_lab->isDrawOrientOn()) {
				vert_holder.color = interior_color;
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f(scaled_col, scaled_row));
				quad_vertices.push_back(vert_holder);
				vert_holder.color = interior_color;
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((1 + fn->col) * scale, scaled_row));
				quad_vertices.push_back(vert_holder);
				vert_holder.color = interior_color;
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((1 + fn->col) * scale, (1 + fn->row) * scale));
				quad_vertices.push_back(vert_holder);
				vert_holder.color = interior_color;
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f(scaled_col, (1 + fn->row) * scale));
				quad_vertices.push_back(vert_holder);
			}
		}




		int switch_variable;

		if (fn->isEdge) {





			if (fancy_graphics) {
				//3,4,0
				switch_variable = 0;
				if (fn->neighbors[0]->cell == fn->cell) {
					switch_variable += 1;
				}
				if (fn->neighbors[4]->cell == fn->cell) {
					switch_variable += 2;
				}
				if (fn->neighbors[3]->cell == fn->cell) {
					switch_variable += 4;
				}

				switch (switch_variable) {

				case 1:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 0:
				case 2:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					triangle_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 3:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[0] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 4:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					break;

				case 6:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[2] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 5:
				case 7:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[1] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[3] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				}



				//*************************************
	//0,5,1
				switch_variable = 0;
				if (fn->neighbors[1]->cell == fn->cell) {
					switch_variable += 1;
				}
				if (fn->neighbors[5]->cell == fn->cell) {
					switch_variable += 2;
				}
				if (fn->neighbors[0]->cell == fn->cell) {
					switch_variable += 4;
				}

				switch (switch_variable) {


				case 1:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 0:
				case 2:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					triangle_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 3:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[5] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 4:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					break;

				case 6:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[7] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 5:
				case 7:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[6] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[4] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[8] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
					quad_vertices.push_back(vert_holder);
					break;

				}


				//*************************************


				//*************************************

//2,6,1
				switch_variable = 0;
				if (fn->neighbors[1]->cell == fn->cell) {
					switch_variable += 1;
				}
				if (fn->neighbors[6]->cell == fn->cell) {
					switch_variable += 2;
				}
				if (fn->neighbors[2]->cell == fn->cell) {
					switch_variable += 4;
				}

				switch (switch_variable) {


				case 1:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 0:
				case 2:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					triangle_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 3:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[12] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 4:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					break;

				case 6:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[10] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 5:
				case 7:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[9] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[11] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[13] * scale);
					quad_vertices.push_back(vert_holder);
					break;

				}




				//3,7,2
				switch_variable = 0;
				if (fn->neighbors[2]->cell == fn->cell) {
					switch_variable += 1;
				}
				if (fn->neighbors[7]->cell == fn->cell) {
					switch_variable += 2;
				}
				if (fn->neighbors[3]->cell == fn->cell) {
					switch_variable += 4;
				}

				switch (switch_variable) {


				case 1:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 0:
				case 2:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					triangle_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				case 3:
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;

					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[17] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 4:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);
					break;

				case 6:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);



					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);

					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[15] * scale);
					triangle_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					triangle_vertices.push_back(vert_holder);
					break;
				case 5:
				case 7:
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[19] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);


					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[18] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[14] * scale);
					quad_vertices.push_back(vert_holder);
					vert_holder.color = edge_interior_color;
					vert_holder.position = sf::Vector2f(proper_position + fancy[16] * scale);
					quad_vertices.push_back(vert_holder);
					break;
				}




				vert_holder.color = edge_interior_color;
				vert_holder.position = sf::Vector2f(proper_position + fancy[20] * scale);
				quad_vertices.push_back(vert_holder);
				vert_holder.color = edge_interior_color;
				vert_holder.position = sf::Vector2f(proper_position + fancy[21] * scale);
				quad_vertices.push_back(vert_holder);
				vert_holder.color = edge_interior_color;
				vert_holder.position = sf::Vector2f(proper_position + fancy[23] * scale);
				quad_vertices.push_back(vert_holder);
				vert_holder.color = edge_interior_color;
				vert_holder.position = sf::Vector2f(proper_position + fancy[22] * scale);
				quad_vertices.push_back(vert_holder);
			}
			else {


				edge_color = cholder;


				nn = 0;  ee = 0;  ww = 0;  ss = 0;
				vert_holder.color = line_color;
				if (fn->neighbors[0]->cell != fn->cell) {
					nn = 1;
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col) * scale, (float)(fn->row) * scale);
					line_vertices.push_back(vert_holder);
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col + 1) * scale, (float)(fn->row) * scale);
					line_vertices.push_back(vert_holder);
				}
				if (fn->neighbors[1]->cell != fn->cell) {
					ee = 1;
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col + 1) * scale, (float)(fn->row) * scale);
					line_vertices.push_back(vert_holder);
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col + 1) * scale, (float)(fn->row + 1) * scale);
					line_vertices.push_back(vert_holder);
				}
				if (fn->neighbors[2]->cell != fn->cell) {
					ww = 1;
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col) * scale, (float)(fn->row + 1) * scale);
					line_vertices.push_back(vert_holder);
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col + 1) * scale, (float)(fn->row + 1) * scale);
					line_vertices.push_back(vert_holder);
				}
				if (fn->neighbors[3]->cell != fn->cell) {
					ss = 1;
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col) * scale, (float)(fn->row) * scale);
					line_vertices.push_back(vert_holder);
					vert_holder.position = petri_pos + sf::Vector2f((float)(fn->col) * scale, (float)(fn->row + 1) * scale);
					line_vertices.push_back(vert_holder);
				}

				vert_holder.color = edge_color;
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((float)fn->col * scale, (float)fn->row * scale));
				quad_vertices.push_back(vert_holder);
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((float)(1 + fn->col) * scale, (float)fn->row * scale));
				quad_vertices.push_back(vert_holder);
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((float)(1 + fn->col) * scale, (float)(1 + fn->row) * scale));
				quad_vertices.push_back(vert_holder);
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((float)fn->col * scale, (float)(1 + fn->row) * scale));
				quad_vertices.push_back(vert_holder);


			}
		}




	}
	if (fancy_graphics) {
		for (auto c : cells) {
			vert_holder.color = c->get_edge_color();
			vert_holder.position = petri_pos + ((c->cell_center_cartesian + sf::Vector2f(0, 0)) * scale);
			quad_vertices.push_back(vert_holder);
			vert_holder.position = petri_pos + ((c->cell_center_cartesian + sf::Vector2f(1, 0)) * scale);
			quad_vertices.push_back(vert_holder);
			vert_holder.position = petri_pos + ((c->cell_center_cartesian + sf::Vector2f(1, 1)) * scale);
			quad_vertices.push_back(vert_holder);
			vert_holder.position = petri_pos + ((c->cell_center_cartesian + sf::Vector2f(0, 1)) * scale);
			quad_vertices.push_back(vert_holder);
		}
	}

	if (quad_vertices.size() > 0) {
		window_manager->getWindow().draw(&quad_vertices[0], quad_vertices.size(), sf::Quads);
		if (take_picture) {
			for (size_t j = 0; j < quad_vertices.size(); j++) {
				sf::Color holder_c = quad_vertices[j].color;
				holder_c.a = 255;
				quad_vertices[j].color = holder_c;
			}

			window_manager->get_r_texture().draw(&quad_vertices[0], quad_vertices.size(), sf::Quads);
		}
	}
	if (!fancy_graphics) {
		if (line_vertices.size() > 0) {
			window_manager->getWindow().draw(&line_vertices[0], line_vertices.size(), sf::Lines);
			if (take_picture) {
				for (size_t j = 0; j < line_vertices.size(); j++) {
					sf::Color holder_c = line_vertices[j].color;
					holder_c.a = 255;
					line_vertices[j].color = holder_c;
				}
				window_manager->get_r_texture().draw(&line_vertices[0], line_vertices.size(), sf::Lines);
			}
		}

	}
	if (fancy_graphics) {
		if (triangle_vertices.size() > 0) {

			window_manager->getWindow().draw(&triangle_vertices[0], triangle_vertices.size(), sf::Triangles);
			if (take_picture) {
				for (size_t j = 0; j < triangle_vertices.size(); j++) {
					sf::Color holder_c = triangle_vertices[j].color;
					holder_c.a = 255;
					triangle_vertices[j].color = holder_c;;
				}
				window_manager->get_r_texture().draw(&triangle_vertices[0], triangle_vertices.size(), sf::Triangles);
			}
		}
	}






	if (parent_lab->isDrawGradientOn()) {
		quad_vertices.clear();
		sf::Color gradient_color;
		for (auto fn : flat_nodes) {
			scaled_col = fn->col * scale;
			scaled_row = fn->row * scale;
			//	rect.setPosition(petri_pos + sf::Vector2f(fn->col * scale, fn->row * scale));

			for (size_t j = 0; j < fn->old_gradients.size(); j++) {
				if (fn->old_gradients[j] == 0) { continue; }


				switch (j) {
				case 0:
					gradient_color = sf::Color(255, 0, 0, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 1:
					gradient_color = sf::Color(0, 255, 0, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 2:
					gradient_color = sf::Color(0, 0, 255, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 3:
					gradient_color = sf::Color(255, 255, 0, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 4:
					gradient_color = sf::Color(255, 0, 255, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 5:
					gradient_color = sf::Color(0, 255, 255, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 6:
					gradient_color = sf::Color(120, 255, 120, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				case 7:
					gradient_color = sf::Color(255, 120, 120, (uint8_t)(fn->old_gradients[j] * 150));
					break;
				default:
					gradient_color = sf::Color(255, 255, 255, (uint8_t)(fn->old_gradients[j] * 150));
				}


				vert_holder.color = gradient_color;
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f(scaled_col, scaled_row));
				quad_vertices.push_back(vert_holder);
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((1 + fn->col) * scale, scaled_row));
				quad_vertices.push_back(vert_holder);
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f((1 + fn->col) * scale, (1 + fn->row) * scale));
				quad_vertices.push_back(vert_holder);
				vert_holder.position = sf::Vector2f(petri_pos + sf::Vector2f(scaled_col, (1 + fn->row) * scale));
				quad_vertices.push_back(vert_holder);

			}
		}
		if (take_picture) { window_manager->get_r_texture().draw(&quad_vertices[0], quad_vertices.size(), sf::Quads); }
		window_manager->getWindow().draw(&quad_vertices[0], quad_vertices.size(), sf::Quads);

	}


	if (parent_lab->isDrawOrientOn()) {
		////debug draw oreitnation_vector
		sf::CircleShape circ;
		circ.setRadius(1 * scale);
		circ.setOrigin(1, 1);
		circ.setFillColor(sf::Color::Red);
		for (auto c : cells) {
			if (c->local_memory[Field_Active_Flag_Index] == 0) {
				circ.setFillColor(sf::Color(255,0,0,100));
			}
			else {
				circ.setFillColor(sf::Color::Red);
			}
			sf::Vector2f orient_vector = vector_from_degrees(-c->ppv.orientation);
		
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(petri_pos + c->cell_center_cartesian * scale)),
				sf::Vertex(sf::Vector2f(petri_pos + c->cell_center_cartesian * scale) + scale * (orient_vector * 10.0f))
			};

			window_manager->getWindow().draw(line, 2, sf::Lines);
			if (take_picture) {
				window_manager->get_r_texture().draw(line, 2, sf::Lines);
			}
			circ.setPosition((petri_pos + c->cell_center_cartesian * scale) + scale * (orient_vector * 10.0f));

			window_manager->getWindow().draw(circ);
			if (take_picture) {
				window_manager->get_r_texture().draw(circ);
			}
		}
	}

	if (parent_lab->isDrawVectorOn()) {
		////debug draw desired_migation_vectors
		sf::CircleShape circ;
		circ.setRadius(1 * scale);
		circ.setOrigin(1, 1);
		circ.setFillColor(sf::Color::Blue);
		for (auto c : cells) {
			sf::Vector2f norm_vector = c->desired_migration_vector;
			if (norm_vector == sf::Vector2f(0, 0)) { continue; }
			normalize_vector(norm_vector);

			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(petri_pos + c->cell_center_cartesian * scale)),
				sf::Vertex(sf::Vector2f(petri_pos + c->cell_center_cartesian * scale) + scale * (norm_vector * 10.0f))
			};

			window_manager->getWindow().draw(line, 2, sf::Lines);
			if (take_picture) {
				window_manager->get_r_texture().draw(line, 2, sf::Lines);
			}
			circ.setPosition((petri_pos + c->cell_center_cartesian * scale) + scale * (norm_vector * 10.0f));

			window_manager->getWindow().draw(circ);
			if (take_picture) {
				window_manager->get_r_texture().draw(circ);
			}
		}
	}

	else {
		if (monitored_cell != NULL) {
			////debug draw desired_migation_vectors
			sf::CircleShape circ;
			circ.setRadius(1 * scale);
			circ.setOrigin(1, 1);
			circ.setFillColor(sf::Color::Blue);
		
				sf::Vector2f norm_vector = monitored_cell->desired_migration_vector;
				if (norm_vector != sf::Vector2f(0, 0)) {
					normalize_vector(norm_vector);

					sf::Vertex line[] =
					{
						sf::Vertex(sf::Vector2f(petri_pos + monitored_cell->cell_center_cartesian * scale)),
						sf::Vertex(sf::Vector2f(petri_pos + monitored_cell->cell_center_cartesian * scale) + scale * (norm_vector * 10.0f))
					};

					window_manager->getWindow().draw(line, 2, sf::Lines);
					if (take_picture) {
						window_manager->get_r_texture().draw(line, 2, sf::Lines);
					}
					circ.setPosition((petri_pos + monitored_cell->cell_center_cartesian * scale) + scale * (norm_vector * 10.0f));

					window_manager->getWindow().draw(circ);
					if (take_picture) {
						window_manager->get_r_texture().draw(circ);
					}
				}

		}
	}

	if (parent_lab->lab_state == Manipulate_State) {
		if (parent_lab->cells_being_moved.size() > 0) {

			sf::RectangleShape rect;
			if (parent_lab->areMovingCellsObstructed()) {
				rect.setFillColor(sf::Color(250, 100, 100, 100));
			}
			else {
				rect.setFillColor(sf::Color(100, 250, 100, 100));
			}
			rect.setSize(sf::Vector2f(scale, scale));
			for (auto cm : parent_lab->cells_being_moved) {
				rect.setPosition(sf::Vector2f(petri_pos + sf::Vector2f((cm.second.x) * scale, (cm.second.y) * scale)));
				window_manager->getWindow().draw(rect);
				if (take_picture) {
					window_manager->get_r_texture().draw(circ);
				}
			}
		}
	}

	//line_vertices.clear();


	//sf::Vector2f input_vector;
	//float angle;
	//for (auto c : cells) {

	//	input_vector = vector_from_degrees(c->ppv.orientation);

	//	sf::Vertex line[] =
	//	{
	//		sf::Vertex(sf::Vector2f(petri_pos + c->cell_center_cartesian * scale)),
	//		sf::Vertex(sf::Vector2f(petri_pos + (c->cell_center_cartesian+10.0f*input_vector) * scale))
	//	};

	//	window_manager->getWindow().draw(line, 2, sf::Lines);

	//}

};




