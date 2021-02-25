#include "Lab.h"
#include "PetriDish.h"
#include "Cell.h"
#include "Window_Manager.h"
#include "User_Input.h"
#include "Field.h"
using namespace std;
Lab::Lab(int rows, Window_Manager* w_m, User_Input* u_i)
{

	one_shot_switch = vector <bool>(gui_LAST_ENTRY + 1, 0);
	momentary_switch = vector <bool>(gui_LAST_ENTRY + 1, 0);
	toggle_switch = vector <bool>(gui_LAST_ENTRY + 1, 0);
	values_switch = vector <float>(gui_LAST_ENTRY + 1, 0);

	petri_time_average = vector <float>(50, 0);
	lab_time_average = vector <float>(50, 0);

	window_manager = w_m;
	ui = u_i;
	petri_ratio = sf::Vector2f(1, 1);
	window_resize_update();
	int rr, cc;
	rr = rows;
	cc = int(rows * petri_ratio.y / petri_ratio.x);

	FieldObject = new Field(rr, cc);

	petri = new PetriDish(rr, cc, FieldObject);
	petri->flux = 1;
	petri->parent_lab = this;

	petri->loadDefaultRules();
	ui->setIteratePanelRuleLoadedLabel("DEFAULT");
	ui->setStateNamestoComboBox(petri->rule_parser->StateNames);

	ui->poll(w_m->getWindow());

	blue_goal_area = values_switch[gui_SLIDER_BLUE_SIZE];
	yellow_goal_area = values_switch[gui_SLIDER_YELLOW_SIZE];
	petri->rule_parser->set_Adhesion_Self_Value(petri->default_Blue_index, values_switch[gui_SLIDER_BLUE_SELF]);
	petri->rule_parser->set_Adhesion_Self_Value(petri->default_Yellow_index, values_switch[gui_SLIDER_YELLOW_SELF]);

	petri->rule_parser->set_Adhesion_Value(petri->default_Blue_index, petri->default_Yellow_index, values_switch[gui_SLIDER_YELLOW_BLUE]);

	petri->rule_parser->set_Adhesion_Value(petri->default_Blue_index, 0, values_switch[gui_SLIDER_BLUE_MEDIUM]);
	petri->rule_parser->set_Adhesion_Value(petri->default_Yellow_index, 0, values_switch[gui_SLIDER_YELLOW_MEDIUM]);

	petri->rule_parser->set_Adhesion_Value(petri->default_Blue_index, petri->default_Blue_index, values_switch[gui_SLIDER_BLUE_BLUE]);
	petri->rule_parser->set_Adhesion_Value(petri->default_Yellow_index, petri->default_Yellow_index, values_switch[gui_SLIDER_YELLOW_YELLOW]);



}

void Lab::setTemp(float x)
{

	petri->flux = x;
}



Node* Lab::node_mouse_is_on()
{
	if (ui->isMouseOnLargerWidgets()) { return NULL; }
	if (ui->getMouseRawPosition(window_manager->getWindow()).x < status_bar_width) { return NULL; }
	sf::Vector2f mouse_local_position = ui->getMouseWorldPosition(window_manager->getWindow(), window_manager->get_zoomable_view());

	sf::Vector2f v = mouse_local_position;


	float scale = float(petri_width) / float(petri->cols);

	v = v - petri_pos;
	v = v / scale;

	petri->mouse_position = v;
	if ((v.x >= 0) && (v.x < petri->cols) && (v.y >= 0) && (v.y < petri->rows)) {
		//	return petri->nodes[v.y][v.x];

		return petri->flat_nodes[int(v.y) * petri->cols + int(v.x)];
	}
	else { return NULL; }
}

string Lab::load_file(string fn)
{
	//**************************
	//Load File from Hard Drive
	//**************************
	stringstream result;
	string line;
	ifstream myfile(fn);
	if (myfile.is_open()) { while (myfile.good()) { getline(myfile, line); result << line << "\n"; } }
	else { return ""; }
	myfile.close();
	rule_text = result.str();

	//remove trailing blank lines
	if (rule_text.size() > 0) {
		while (rule_text.substr(rule_text.size() - 1, 1) == "\n") {
			rule_text = rule_text.substr(0, rule_text.size() - 1);
		}
	}
	return rule_text;
	//**************************
	//**************************
}

bool Lab::save_file(string name)
{

	if (file_type_flag == 0) {
		if (name == "*.cyr") { return false; }
		if (name.size() < 5) { return false; }
		if ((name.substr(name.size() - 4) != ".cyr")) { return false; }
	}

	if (file_type_flag == 1) {
		if (name == "*.png") { return false; }
		if (name.size() < 5) { return false; }

		FieldObject->update_field_data_from_rendertexture();
		FieldObject->getImageFromFieldData().saveToFile(name);
		return true;
	}

	//**************************
	//Save File to Hard Drive
	//**************************
	ofstream myfile(name);
	if (myfile.is_open())
	{
		if (file_type_flag == 0) {
			myfile << user_rules.text;
		}

		myfile.close();
		return true;
	}
	return false;
}

void Lab::place_cells_automatically()
{

	int init_rows = (int)values_switch[gui_SLIDER_ROWS];
	int init_cols = (int)values_switch[gui_SLIDER_COLS];
	//default Area is 64.
	int side = 8;
	int init_width = init_cols * side;
	int init_heigth = init_rows * side;
	if (init_heigth > petri->rows) { return; }
	if (init_width > petri->cols) { return; }

	int col_offset = (petri->cols / 2) - (init_width / 2);
	int row_offset = (petri->rows / 2) - (init_heigth / 2);

	int node_coorid_col;
	int node_coorid_row;
	cell_placement_coordinates.clear();
	Init_Placement holder;


	for (int r = 0; r < init_rows; r++) {
		node_coorid_row = r * side + row_offset;
		for (int c = 0; c < init_cols; c++) {
			node_coorid_col = c * side + col_offset;

			int rr = rand() % 2;
			if (rr == 0) {
				holder.cell_type = petri->getDefault_Blue_Index();
			}
			else {
				holder.cell_type = petri->getDefault_Yellow_Index();
			}
			holder.size = side * side;
			holder.row = node_coorid_row;
			holder.col = node_coorid_col;
			cell_placement_coordinates.push_back(holder);
		}
	}

	for (auto cpc : cell_placement_coordinates) {
		petri->auto_place_cell(cpc);
	}
}


bool Lab::load_field_file(sf::Image& im, string n)
{

	return im.loadFromFile(n);

}


void Lab::update() {

	lab_clock.restart();
	ui->loadSwitchData(one_shot_switch, momentary_switch, toggle_switch, values_switch);

	blue_goal_area = values_switch[gui_SLIDER_BLUE_SIZE];
	yellow_goal_area = values_switch[gui_SLIDER_YELLOW_SIZE];
	petri->rule_parser->set_Adhesion_Self_Value(petri->default_Blue_index, values_switch[gui_SLIDER_BLUE_SELF]);
	petri->rule_parser->set_Adhesion_Self_Value(petri->default_Yellow_index, values_switch[gui_SLIDER_YELLOW_SELF]);

	petri->rule_parser->set_Adhesion_Value(petri->default_Blue_index, petri->default_Yellow_index, values_switch[gui_SLIDER_YELLOW_BLUE]);

	petri->rule_parser->set_Adhesion_Value(petri->default_Blue_index, 0, values_switch[gui_SLIDER_BLUE_MEDIUM]);
	petri->rule_parser->set_Adhesion_Value(petri->default_Yellow_index, 0, values_switch[gui_SLIDER_YELLOW_MEDIUM]);

	petri->rule_parser->set_Adhesion_Value(petri->default_Blue_index, petri->default_Blue_index, values_switch[gui_SLIDER_BLUE_BLUE]);
	petri->rule_parser->set_Adhesion_Value(petri->default_Yellow_index, petri->default_Yellow_index, values_switch[gui_SLIDER_YELLOW_YELLOW]);
	speed = values_switch[gui_SLIDER_SPEED];
	setTemp(values_switch[gui_SLIDER_TEMP]);


	newNodeMouseIsOn = node_mouse_is_on();

	petri->fancy_graphics = toggle_switch[gui_IT_FANCY];

	petri->take_picture = 0;
	//petri->take_picture =	ui->getKeyboard_Toggle_switch(sf::Keyboard::P);

//	petri->levin_juice = ui->getKeyboard_Toggle_switch(sf::Keyboard::O);
//	petri->pause_mitosis = ui->getKeyboard_Toggle_switch(sf::Keyboard::M);
	//petri->randomize_orientation_one_shot =ui->getKeyboard_One_shot_switch(sf::Keyboard::O);
//	if (petri->levin_juice == 0) {
	//	petri->randomize_orientation_one_shot = 0;
	//}

	switch (lab_state) {
	case Start_State:
		start_state();
		break;
	case Sorter_State:
		sorter_state();
		break;
	case Monitor_State:
		monitor_state();
		break;
	case Field_State:
		field_state();
		break;
	case Manipulate_State:
		manipulate_state();
		break;
	case Editor_State:
		editor_state();
		break;
	case Wish_To_Save_Text_State:
		wish_to_save_text_state();
		break;
	case Save_Text_State:
		save_text_state();
		break;
	case Load_Text_State:
		load_text_state();
		break;
	case Save_Field_State:
		save_field_state();
		break;
	case Load_Field_State:
		load_field_state();
		break;
	}

	oldNodeMouseWasOn = newNodeMouseIsOn;

	//draw();
	/*lab_time = lab_clock.getElapsedTime();
		lab_time_average.insert(lab_time_average.begin(),lab_time.asMilliseconds());
		lab_time_average.pop_back();
		float labsum = 0;
		for (auto t : lab_time_average) {
			labsum += t;
		}
		labsum = labsum / 50.0f;
		cout << labsum << "\n";*/
	return;
}

void Lab::start_state()
{

	if (entering_lab_state) {
		entering_lab_state = 0;
	}


	if (toggle_switch[gui_TAB_SORTER]) {
		state_change_New_Former(Sorter_State, Start_State);
		return;
	}
	if (toggle_switch[gui_TAB_MONITOR]) {
		state_change_New_Former(Monitor_State, Start_State);
		return;
	}
	if (toggle_switch[gui_TAB_MANIPULATE]) {
		state_change_New_Former(Manipulate_State, Start_State);
		return;
	}
	if (toggle_switch[gui_TAB_FIELD]) {
		state_change_New_Former(Field_State, Start_State);
		return;
	}
	if (toggle_switch[gui_TAB_EDITOR]) {
		state_change_New_Former(Editor_State, Start_State);
		return;
	}

}

void Lab::sorter_state()
{

	if (entering_lab_state) {

		ui->set_iterate_panel_visibility(1);
		ui->set_sorter_panel_visibility(1);
		ui->set_monitor_panel_visibility(0);
		ui->set_field_panel_visibility(0);
		ui->set_manipulate_panel_visibility(0);
		ui->set_editor_panel_visibility(0);
		ui->uncheckManualPlacementCheckbox();
		ui->set_debug_textbox_visibility(0);
		entering_lab_state = 0;
	}



	if (toggle_switch[gui_TAB_MONITOR]) {
		state_change_New_Former(Monitor_State, Sorter_State);
		return;
	}
	if (toggle_switch[gui_TAB_MANIPULATE]) {
		state_change_New_Former(Manipulate_State, Sorter_State);
		return;
	}
	if (toggle_switch[gui_TAB_FIELD]) {
		state_change_New_Former(Field_State, Sorter_State);
		return;
	}
	if (toggle_switch[gui_TAB_EDITOR]) {
		state_change_New_Former(Editor_State, Sorter_State);
		return;
	}

	//clear check
	if (one_shot_switch[gui_IT_CLEAR]) {
		petri->clear_petri_dish();
		return;
	}


	//place grid
	if (one_shot_switch[gui_CS_CLEAR_AND_PLACE]) {
		petri->clear_petri_dish();
		place_cells_automatically();
	}

	updatePetri();



}

void Lab::monitor_state()
{
	if (entering_lab_state) {

		ui->set_iterate_panel_visibility(1);
		ui->set_sorter_panel_visibility(0);
		ui->set_monitor_panel_visibility(1);
		ui->set_field_panel_visibility(0);
		ui->set_manipulate_panel_visibility(0);
		ui->set_editor_panel_visibility(0);
		entering_lab_state = 0;
	}


	if (toggle_switch[gui_TAB_SORTER]) {
		state_change_New_Former(Sorter_State, Monitor_State);
		ui->set_debug_textbox_visibility(0);
		return;
	}
	if (toggle_switch[gui_TAB_MANIPULATE]) {
		state_change_New_Former(Manipulate_State, Monitor_State);
		ui->set_debug_textbox_visibility(0);
		return;
	}
	if (toggle_switch[gui_TAB_FIELD]) {
		state_change_New_Former(Field_State, Monitor_State);
		ui->set_debug_textbox_visibility(0);
		return;
	}
	if (toggle_switch[gui_TAB_EDITOR]) {
		state_change_New_Former(Editor_State, Monitor_State);
		ui->set_debug_textbox_visibility(0);
		return;
	}

	//clear check
	if (one_shot_switch[gui_IT_CLEAR]) {
		ui->setTextToMonitorTextBox("");
		petri->clear_petri_dish();
		return;
	}

	//placment of cells
	if (toggle_switch[gui_IT_MANUAL_PLACEMENT_CHECKBOX]) {
		if (momentary_switch[gui_MOUSE_LEFT_BUTTON]) {


			if (newNodeMouseIsOn != NULL) {
				if (ui->get_selected_state_name_from_combo_box() != "") {
					petri->manually_place_cell(newNodeMouseIsOn, petri->rule_parser->SNIM[ui->get_selected_state_name_from_combo_box()]);
				}
			}
		}
		else {
			if (ui->getKeyboard_One_shot_switch(sf::Keyboard::Enter)) {
				petri->selected_cell = NULL;
			}

		}
	}
	else {
		petri->selected_cell = NULL;
	}



	//Set Monitored Cell
	if (ui->getKeyboard_One_shot_switch(sf::Keyboard::Space)) {
		if (petri->monitored_cell != NULL) {

			petri->monitored_cell->monitored = false;
			petri->monitored_cell = NULL;
			ui->setTextToMonitorTextBox("");
		}
		else {
			Node* node_holder;
			node_holder = node_mouse_is_on();
			if (node_holder != NULL) {
				if (node_holder->cell != petri->SUBSTRATE) {
					petri->monitored_cell = node_holder->cell;
					petri->monitored_cell->monitored = true;

				}
			}
		}
	}


	//debug
	if ((petri->monitored_cell != NULL) && (toggle_switch[gui_CR_DEBUGCHECKBOX])) {
		ui->set_debug_textbox_visibility(1);
		if (!isDebugOn) {
			string s = "";
			vector <string> cpu_rules = petri->rule_parser->getRawRules();
			for (auto ss : cpu_rules) {
				s = s + ss + "\n";
			}
			ui->set_text_to_mini_rule_viewer(s);}
			ui->set_Iterate_To_Step();

			speed = 1;
		
		isDebugOn = true;
	}
	else {
		isDebugOn = false;

		if (ui->isDebugTextBoxVisible()) {
			ui->clearTextFromDebugTextBox();
			ui->set_text_to_mini_rule_viewer("");
			ui->setDebugCheckBox(0);
			ui->set_debug_textbox_visibility(0);

		}
	}

	if (petri->monitored_cell == NULL) {
		ui->setDebugCheckBox(0);
	}

	//Update Petri
	updatePetri();


	if (petri->monitored_cell != NULL) {
		ui->setTextToMonitorTextBox(petri->monitored_cell->info());
	}

}

void Lab::field_state()
{
	if (entering_lab_state) {

		ui->set_iterate_panel_visibility(0);
		ui->set_sorter_panel_visibility(0);
		ui->set_monitor_panel_visibility(0);
		ui->set_field_panel_visibility(1);
		ui->set_manipulate_panel_visibility(0);
		ui->set_editor_panel_visibility(0);
		ui->set_debug_textbox_visibility(0);
		entering_lab_state = 0;
	}

	if (toggle_switch[gui_TAB_SORTER]) {
		state_change_New_Former(Sorter_State, Field_State);
		return;
	}
	if (toggle_switch[gui_TAB_MONITOR]) {
		state_change_New_Former(Monitor_State, Field_State);
		return;
	}
	if (toggle_switch[gui_TAB_MANIPULATE]) {
		state_change_New_Former(Manipulate_State, Field_State);
		return;
	}
	if (toggle_switch[gui_TAB_EDITOR]) {
		state_change_New_Former(Editor_State, Field_State);
		return;
	}




	if (one_shot_switch[gui_FIELD_CLEAR]) {
		FieldObject->clear();
		ui->setIteratePanelFieldLoadedLabel(FieldObject->filename);
	}



	if (one_shot_switch[gui_FIELD_SAVE]) {
		ui->adjustSaveLoadLabelsForFileType(1);
		state_change_New_Former(Save_Field_State, Field_State);
		return;
	}


	if (one_shot_switch[gui_FIELD_LOAD]) {
		ui->adjustSaveLoadLabelsForFileType(1);
		state_change_New_Former(Load_Field_State, Field_State);
		return;
	}

	if (values_switch[gui_KNOB_ORIENTATION]!= old_orientation) {
		old_orientation = values_switch[gui_KNOB_ORIENTATION];
		FieldObject->default_orientation = values_switch[gui_KNOB_ORIENTATION];
		FieldObject->isFieldDataInSynchWithTexture = 0;
	}



	if (ui->shouldMouseBeAbleToDrawOrPlaceCells()) {

		if (momentary_switch[gui_MOUSE_LEFT_BUTTON]) {


			if (newNodeMouseIsOn != NULL) {
				FieldObject->isFieldDataInSynchWithTexture = 0;
				if (toggle_switch[gui_FIELD_ADJUST_PL_CHECKBOX]) {
					FieldObject->default_location = sf::Vector2f((float)newNodeMouseIsOn->col, (float)newNodeMouseIsOn->row);
				}
				else {
					sf::Color swatch_color;
					int swatch = ui->getGUI_FIELD_VALUE_SWATCH();
					if (swatch >= 0) {
						switch (swatch) {
						case 0:
							swatch_color = sf::Color(255, 255, 255, 255);
							break;
						case 1:
							swatch_color = sf::Color(255, 0, 0, 255);
							break;
						case 2:
							swatch_color = sf::Color(0, 255, 0, 255);
							break;
						case 3:
							swatch_color = sf::Color(0, 0, 255, 255);
							break;
						case 4:
							swatch_color = sf::Color(255, 255, 0, 255);
							break;

						}
						sf::CircleShape brush;
						brush.setRadius(values_switch[gui_SLIDER_BRUSH_SIZE]);
						brush.setFillColor(swatch_color);
						brush.setOrigin(brush.getRadius() / 2, brush.getRadius() / 2);

						brush.setPosition((float)newNodeMouseIsOn->col, (float)newNodeMouseIsOn->row);
						FieldObject->field_RenderTexture.draw(brush);
						FieldObject->field_RenderTexture.display();
						ui->setIteratePanelFieldLoadedLabel(FieldObject->filename + " [unsaved]");
					}
				}

			}
		}

	}

	if (!FieldObject->isFieldDataInSynchWithTexture) {
		FieldObject->update_field_data_from_rendertexture();
		//petri->clear_petri_dish();
	}


}

void Lab::manipulate_state()
{
	if (entering_lab_state) {

		ui->set_iterate_panel_visibility(0);
		ui->set_sorter_panel_visibility(0);
		ui->set_monitor_panel_visibility(0);
		ui->set_field_panel_visibility(0);
		ui->set_manipulate_panel_visibility(1);
		ui->set_editor_panel_visibility(0);
		ui->set_debug_textbox_visibility(0);
		entering_lab_state = 0;
		cells_selected.clear();
	}


	if (toggle_switch[gui_TAB_SORTER]) {
		state_change_New_Former(Sorter_State, Manipulate_State);
		cells_selected.clear();
		cells_being_moved.clear();
		return;
	}
	if (toggle_switch[gui_TAB_MONITOR]) {
		state_change_New_Former(Monitor_State, Manipulate_State);
		cells_selected.clear();
		cells_being_moved.clear();
		return;
	}

	if (toggle_switch[gui_TAB_FIELD]) {
		state_change_New_Former(Field_State, Manipulate_State);
		cells_selected.clear();
		cells_being_moved.clear();
		return;
	}
	if (toggle_switch[gui_TAB_EDITOR]) {
		state_change_New_Former(Editor_State, Manipulate_State);
		cells_selected.clear();
		cells_being_moved.clear();
		return;
	}



	if (toggle_switch[gui_MANIP_SELECT_RADIO]) {
		cells_being_moved.clear();
		if (newNodeMouseIsOn != NULL) {
			if (momentary_switch[gui_MOUSE_LEFT_BUTTON]) {
				if (newNodeMouseIsOn->cell != petri->SUBSTRATE) {
					cells_selected.insert(newNodeMouseIsOn->cell);
				}
			}
		}
	}
	if (toggle_switch[gui_MANIP_DESELECT_RADIO]) {
		cells_being_moved.clear();
		if (newNodeMouseIsOn != NULL) {
			if (momentary_switch[gui_MOUSE_LEFT_BUTTON]) {
				if (cells_selected.isInSpecialVector(newNodeMouseIsOn->cell)) {
					cells_selected.erase(newNodeMouseIsOn->cell);
				}
			}
		}
	}
	if (toggle_switch[gui_MANIP_MOVE_RADIO]) {
		if (cells_being_moved.size() == 0) {
			if (cells_selected.size() > 0) {
				vector <Cell *> cell_holder2 = cells_selected.getData();
				for (auto fn : petri->flat_nodes) {
					if (fn->cell == petri->SUBSTRATE) { continue; }
					if (find(cell_holder2.begin(), cell_holder2.end(), fn->cell) == cell_holder2.end()) { continue; }
					cells_being_moved.push_back(pair<Cell *, sf::Vector2f>(fn->cell, sf::Vector2f((float)fn->col, (float)fn->row)));
				}
			}
		}
		else {
			if (momentary_switch[gui_MOUSE_LEFT_BUTTON]) {
				moving_cells_mouse_down_toggle = 1;
				if (  (newNodeMouseIsOn != NULL) && (oldNodeMouseWasOn != NULL)  && (newNodeMouseIsOn!=oldNodeMouseWasOn) ) {
					sf::Vector2f mod_offset((float)petri->cols, (float)petri->rows);
					sf::Vector2f delta = sf::Vector2f((float)(newNodeMouseIsOn->col - oldNodeMouseWasOn->col), (float)(newNodeMouseIsOn->row - oldNodeMouseWasOn->row));
					for (size_t j = 0; j < cells_being_moved.size();j++) {
						cells_being_moved[j].second += delta;
						wrap_around_sfvector_fmod(cells_being_moved[j].second,mod_offset);
					}
					Node * n;
					moving_cells_obstructed = 0;
					for (size_t j = 0; j < cells_being_moved.size(); j++) {
						n =  petri->flat_nodes[int(cells_being_moved[j].second.y) * petri->cols + int(cells_being_moved[j].second.x)];
						if ( (n->cell!=petri->SUBSTRATE) &&(!cells_selected.isInSpecialVector(n->cell))) {
							moving_cells_obstructed = true;
							break;
						}
					}
				}
			}
			else {
				if (moving_cells_mouse_down_toggle == 1) {  //mouse button released
					if (moving_cells_obstructed) {
						cells_being_moved.clear();
						moving_cells_mouse_down_toggle = 0;
					}
					else { //the big switcheroo
						vector <Cell *> cell_holder3 = cells_selected.getData();
						for (size_t j = 0; j < cell_holder3.size(); j++) {
							vector <Node *> all_nodes_of_cell;
							all_nodes_of_cell.clear();
							petri->get_all_nodes_of_a_cell(cell_holder3[j], all_nodes_of_cell);
							for (size_t k = 0; k < all_nodes_of_cell.size(); k++) {
								petri->change_n1_to_c(all_nodes_of_cell[k], petri->SUBSTRATE);
							}						
						}
						for (size_t j = 0; j < cells_being_moved.size(); j++) {
							petri->change_n1_to_c(petri->flat_nodes[int(cells_being_moved[j].second.y) * petri->cols + int(cells_being_moved[j].second.x)], cells_being_moved[j].first);
						}
						cells_being_moved.clear();
						moving_cells_mouse_down_toggle = 0;
						petri->updateCellEdgesAndMembranes();
					}
				}

			}
		}
	}


	if (one_shot_switch[gui_MANIP_KILL_BUTTON]) {
		cells_being_moved.clear();
		vector <Cell *> cell_holder = cells_selected.getData();
		for (auto kc : cell_holder) {
			if (kc->monitored) {
				ui->setTextToMonitorTextBox("");
			}
			petri->kill_cell(kc);
		}
		cells_selected.clear();
	}






}

void Lab::editor_state()
{
	if (entering_lab_state) {

		ui->set_iterate_panel_visibility(0);
		ui->set_sorter_panel_visibility(0);
		ui->set_monitor_panel_visibility(0);
		ui->set_field_panel_visibility(0);
		ui->set_manipulate_panel_visibility(0);
		ui->set_editor_panel_visibility(1);
		entering_lab_state = 0;
	}


	if (toggle_switch[gui_TAB_SORTER]) {
		state_change_New_Former(Sorter_State, Editor_State);
		return;
	}
	if (toggle_switch[gui_TAB_MONITOR]) {
		state_change_New_Former(Monitor_State, Editor_State);
		return;
	}
	if (toggle_switch[gui_TAB_MANIPULATE]) {
		state_change_New_Former(Manipulate_State, Editor_State);
		return;
	}
	if (toggle_switch[gui_TAB_FIELD]) {
		state_change_New_Former(Field_State, Editor_State);
		return;
	}


	//has text changed
	if (one_shot_switch[gui_ED_TEXT_CHANGED]) {
		user_rules.text = ui->getRuleText();
		user_rules.isSaved = 0;
	}


	if (one_shot_switch[gui_ED_LOAD_BUTTON]) {
		if (!user_rules.getIsSaved()) {
			state_change_New_Former(Wish_To_Save_Text_State, Editor_State);
			ui->set_wts_filename_label(user_rules.filename);
			return;
		}
		state_change_New_Former(Load_Text_State, Editor_State);
		return;
	}


	if (one_shot_switch[gui_ED_SAVE_BUTTON]) {
		state_change_New_Former(Save_Text_State, Editor_State);
		return;
	}

	if ((one_shot_switch[gui_SYNTAX_GUIDE_BUTTON]) && (toggle_switch[gui_SYNTAX_GUIDE_BUTTON])) {
		ui->set_syntax_guide_visibility(1);
		ui->adjustVariableSizeIcons(window_manager->getWindow());
		return;
	}
	if ((one_shot_switch[gui_SYNTAX_GUIDE_BUTTON]) && (!toggle_switch[gui_SYNTAX_GUIDE_BUTTON])) {
		ui->set_syntax_guide_visibility(0);
		ui->adjustVariableSizeIcons(window_manager->getWindow());
		return;
	}

	if (one_shot_switch[gui_ED_COMPILE_BUTTON]) {

		file_type_flag = 0;
		ui->adjustSaveLoadLabelsForFileType(file_type_flag);
		if (save_file(user_rules.filename)) {
			user_rules.isSaved = 1;
			ui->setUnsavedEditorLabel(0);
			ui->setEditorFileNameLabel(user_rules.filename);
		}
		else {
			ui->clearAndLoadEditBoxErrorText("** Error Saving File");
		}



		petri->assignTextToRuleParser(user_rules.text);

		stringstream message;
		int line_number = -1;
		petri->compileRuleParser(message, line_number);
		if (message.str() != "") {
			vector <string> rule_text_vector = petri->getRawRules();
			stringstream error_text;
			if (line_number > -1) {
				for (int x = 0; x < line_number + 1; x++) {
					if (x != line_number) {
						error_text << "COMPILED:   ";
					}
					if (x == line_number) {
						error_text << "ERROR   :   ";
					}
					error_text << rule_text_vector[x] << "\n";
				}
			}
			error_text << "****    :   " << message.str() << "\n";
			ui->clearAndLoadEditBoxErrorText(error_text.str());

			petri->loadDefaultRules();
			ui->setIteratePanelRuleLoadedLabel("DEFAULT");
			ui->setStateNamestoComboBox(petri->rule_parser->StateNames);

		}
		else {
			ui->clearAndLoadEditBoxErrorText("Compiled Succesfully");
			ui->setStateNamestoComboBox(petri->rule_parser->StateNames);
			user_rules.isCompiled = 1;
			ui->setIteratePanelRuleLoadedLabel(user_rules.filename);
		}


	}
}
void Lab::load_text_state()
{
	if (entering_lab_state) {
		if (coming_from_lab_state == Editor_State) {
			ui->adjustSaveLoadLabelsForFileType(0);  //rules
			ui->turn_on_load_dialog2(window_manager->getWindow(), 0);
		}
		else {
			ui->adjustSaveLoadLabelsForFileType(1);  //field
			ui->turn_on_load_dialog2(window_manager->getWindow(), 1);
		}
		entering_lab_state = 0;
	}

	if (!ui->is_load_childwindow_visible()) {
		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		ui->turn_off_load_dialog();
		return;
	}

	if (one_shot_switch[gui_LD_LOAD_BUTTON]) {
		string file_name_from_dialog;
		string file_text;
		file_name_from_dialog = ui->getLoadDialogFileName();
		file_text = load_file(file_name_from_dialog);

		if (file_text != "") {
			user_rules.filename = file_name_from_dialog;
			user_rules.text = file_text;
			user_rules.isSaved = 1;
			ui->setUnsavedEditorLabel(0);
			user_rules.isCompiled = 0;
			ui->turn_off_load_dialog();

			ui->setEditorTextAndLabels(file_text, file_name_from_dialog);



			state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		}
		else {
			ui->displayLoadDialogErrorLabel();
		}

	}



}

void Lab::save_text_state()
{
	if (entering_lab_state) {
	
		if (coming_from_lab_state == Editor_State) {
		
			ui->adjustSaveLoadLabelsForFileType(0);  //rules
		}
		else {
			ui->adjustSaveLoadLabelsForFileType(1);  //field
		}

		ui->turn_on_save_dialog2(window_manager->getWindow(), user_rules.filename);
		entering_lab_state = 0;
		file_type_flag = 0;
	}

	if (!ui->is_save_childwindow_visible()) {

		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		ui->turn_off_save_dialog();
		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		return;
	}

	if (one_shot_switch[gui_SD_SAVE_BUTTON]) {

		string proposed_saved_name;
		proposed_saved_name = ui->getSaveDialogFileName();
		if (save_file(proposed_saved_name)) {
			user_rules.filename = proposed_saved_name;
			user_rules.isSaved = 1;
			ui->setUnsavedEditorLabel(0);
			ui->setEditorFileNameLabel(user_rules.filename);
			ui->turn_off_save_dialog();
			state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		}
		else {
			ui->displaySaveDialogErrorLabel();
		}
	}



}

void Lab::load_field_state()
{
	if (entering_lab_state) {
		if (coming_from_lab_state == Editor_State) {
			ui->adjustSaveLoadLabelsForFileType(0);  //rules
			ui->turn_on_load_dialog2(window_manager->getWindow(), 0);
		}
		else {
			ui->adjustSaveLoadLabelsForFileType(1);  //field
			ui->turn_on_load_dialog2(window_manager->getWindow(), 1);
		}
		entering_lab_state = 0;
	}

	if (!ui->is_load_childwindow_visible()) {
		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		ui->turn_off_load_dialog();
		return;
	}
	if (one_shot_switch[gui_LD_LOAD_BUTTON]) {
		string file_name_from_dialog;
		string file_text;

	
			file_name_from_dialog = ui->getLoadDialogFileName();
			sf::Image loaded_image;
			if (load_field_file(loaded_image, file_name_from_dialog)) {
				FieldObject->acceptImageAndInitializeEverything(loaded_image, file_name_from_dialog);
				ui->setIteratePanelFieldLoadedLabel(FieldObject->filename);
				ui->turn_off_load_dialog();
				state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
			}
			else {
				ui->displayLoadDialogErrorLabel();
			}
		
	}

}

void Lab::save_field_state()
{
	if (entering_lab_state) {
		if (coming_from_lab_state == Editor_State) {
			ui->adjustSaveLoadLabelsForFileType(0);  //rules
		}
		else {
			ui->adjustSaveLoadLabelsForFileType(1);  //field
		}
		ui->turn_on_save_dialog2(window_manager->getWindow(), FieldObject->filename);
		entering_lab_state = 0;
		file_type_flag = 1;
	}

	if (!ui->is_save_childwindow_visible()) {
		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		ui->turn_off_save_dialog();
		return;
	}


	if (one_shot_switch[gui_SD_SAVE_BUTTON]) {
		string proposed_saved_name;

		proposed_saved_name = ui->getSaveDialogFileName();
		if (save_file(proposed_saved_name)) {
			FieldObject->filename = proposed_saved_name;
			ui->turn_off_save_dialog();
			ui->setIteratePanelFieldLoadedLabel(FieldObject->filename);
			state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		}
	}



}


void Lab::wish_to_save_text_state()
{

	if (entering_lab_state) {
		ui->turn_on_wts_dialog2(window_manager->getWindow());

		entering_lab_state = 0;
	}

	if (one_shot_switch[gui_WTS_CANCEL]) {
		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		ui->turn_off_wts_dialog();
		return;
	}

	if (one_shot_switch[gui_WTS_DISCARD]) {
		state_change_New_Former(coming_from_lab_state, coming_from_lab_state);
		user_rules.text = "";
		user_rules.filename = "*.cyr";
		ui->turn_off_wts_dialog();
		return;
	}

	if (one_shot_switch[gui_WTS_SAVE]) {
		state_change_New_Former(Save_Text_State, coming_from_lab_state);
		ui->turn_off_wts_dialog();
		return;
	}
}

















void Lab::updatePetri()
{



	if (petri->cells.size() > 0) {

		if (ui->getGUI_ITERATE_BUTTON()) {

			if (momentary_switch[gui_MOUSE_LEFT_BUTTON]) {

				if (newNodeMouseIsOn != NULL) {
					if (newNodeMouseIsOn->cell->getState() != 0) {
						if (cell_being_dragged == NULL) {
							cell_being_dragged = newNodeMouseIsOn->cell;
							cell_being_dragged->selected = true;
						}
					}
				}
			}
			else {
				if (cell_being_dragged != NULL) {

					cell_being_dragged->selected = false;
					cell_being_dragged = NULL;
				}
			}



			ui->uncheckManualPlacementCheckbox();
			stringstream debug_message;
			for (int x = 0; x < speed; x++) {
				markov_count++;
				if ((toggle_switch[gui_CR_DEBUGCHECKBOX])) {
					debug_message << "\n\nIteration No. " << markov_count << "\n";
				}
				petri->update(debug_message);
				petri->diffuse();

				if (lab_state == Monitor_State) {
					if (petri->monitored_cell != NULL) {
						if ((toggle_switch[gui_CR_DEBUGCHECKBOX])) {
							ui->addTextToDebugTextBox(debug_message.str());
						}
					}
				}

			}

		}
	}


}



void Lab::window_resize_update()
{
	border = 50;
	status_bar_width = 300;
	status_bar_height = (float)window_manager->getScreenHeight();
	sf::Vector2f temp_petri_dims;

	temp_petri_dims.x = (float)(window_manager->getScreenWidth() - status_bar_width);
	temp_petri_dims.y = (float)window_manager->getScreenHeight();

	if (temp_petri_dims.x / temp_petri_dims.y <= petri_ratio.x / petri_ratio.y) {

		petri_width = temp_petri_dims.x - (2 * border);
		petri_height = ((petri_width * petri_ratio.y / petri_ratio.x));
		petri_pos.x = status_bar_width + ((temp_petri_dims.x - petri_width) / 2);
		petri_pos.y = (temp_petri_dims.y - petri_height) / 2;
	}
	else {

		petri_height = (temp_petri_dims.y - (2 * border));
		petri_width = ((petri_height * petri_ratio.x / petri_ratio.y));
		petri_pos.x = status_bar_width + ((temp_petri_dims.x - petri_width) / 2);
		petri_pos.y = (temp_petri_dims.y - petri_height) / 2;
	}

	ui->adjustVariableSizeIcons(window_manager->getWindow());
};


void Lab::draw()
{
	if (!petri->levin_juice) {
		window_manager->getWindow().clear(sf::Color(64, 64, 64));
	}
	else {
		window_manager->getWindow().clear(sf::Color(114, 64, 64));
	}
	if (petri->take_picture) {
		window_manager->get_r_texture().clear(sf::Color(0,0,0,0));
	}

	sf::CircleShape cursor;
	cursor.setRadius(10);
	cursor.setFillColor(sf::Color(255, 128, 128, 50));
	cursor.setOutlineColor(sf::Color::Blue);
	cursor.setOutlineThickness(2);
	cursor.setOrigin(cursor.getRadius() / 2, cursor.getRadius() / 2);
	sf::Vector2i mouse_pos = ui->getMouseRawPosition(window_manager->getWindow());
	cursor.setPosition((float)mouse_pos.x, (float)mouse_pos.y);
	window_manager->change_to_Zoomable_View();
	draw_grid(window_manager);


	if (lab_state == Field_State) {  //Field
		float scale = petri_width / petri->cols;
		sf::Sprite f_sprite = FieldObject->getFieldSprite();
		f_sprite.setTexture(FieldObject->field_RenderTexture.getTexture());
		f_sprite.setOrigin(0, 0);
		f_sprite.setScale(scale, scale);
		f_sprite.setColor(sf::Color(255, 255, 255, 100));

		f_sprite.setPosition(petri_pos.x, petri_pos.y);
		sf::CircleShape default_marker;
		default_marker.setRadius(2 * scale);
		default_marker.setFillColor(sf::Color(200, 200, 200));
		default_marker.setPosition(petri_pos.x + FieldObject->default_location.x * scale, petri_pos.y + FieldObject->default_location.y * scale);
		window_manager->getWindow().draw(f_sprite);
		window_manager->getWindow().draw(default_marker);
		cursor.setRadius(values_switch[gui_SLIDER_BRUSH_SIZE]);
	}

	else {
		if ((lab_state == Sorter_State) || (lab_state == Monitor_State) || (lab_state == Manipulate_State)) {
			petri->draw(window_manager);


			if (petri->take_picture) {

				stringstream pic_name;
				if (picture_count < 10) {
					pic_name << "cycmovie_0000" << picture_count << ".png";
				}
				else
				{
					if (picture_count < 100) {
						pic_name << "cycmovie_000" << picture_count << ".png";
					}

					else
					{
						if (picture_count < 1000) {
							pic_name << "cycmovie_00" << picture_count << ".png";
						}
						else {
							if (picture_count < 10000) {
								pic_name << "cycmovie_0" << picture_count << ".png";
							}
						}
					}
				}
				
				window_manager->get_r_texture().display();
				window_manager->get_r_texture().getTexture().copyToImage().saveToFile(pic_name.str());
				cout << "Took Picture: " << pic_name.str() << "\n";;
				picture_count++;

			}
		}
	}

	window_manager->change_to_Fixed_View();
	sf::RectangleShape status_bar;
	status_bar.setFillColor(sf::Color(96, 96, 96));
	status_bar.setOutlineColor(sf::Color(128, 128, 128));
	status_bar.setOutlineThickness(-4);
	status_bar.setSize(sf::Vector2f(status_bar_width, status_bar_height));
	status_bar.setPosition(0, 0);
	window_manager->getWindow().draw(status_bar);
	ui->drawGUI(window_manager->get_fixed_view());


	cursor.setOrigin(cursor.getRadius() / 2, cursor.getRadius() / 2);
	if ((mouse_pos.x > status_bar_width) && (!ui->shouldUseNormalCursor())) {

		window_manager->getWindow().setMouseCursorVisible(0);
		window_manager->getWindow().draw(cursor);
	}
	else {
		window_manager->getWindow().setMouseCursorVisible(1);
	}

}


void Lab::draw_grid(Window_Manager * window_manager) {

	window_manager->change_to_Zoomable_View();
	float scale = petri_width / petri->cols;
	sf::RectangleShape rect;

	vector <sf::Vertex> grid_line_vertices;
	grid_line_vertices.clear();
	sf::Vertex vertice;

	vertice.color = sf::Color(96, 96, 96);

	for (int j = 0; j < petri->rows + 1; j = j + 5) {
		vertice.position.x = petri_pos.x + 0.0f * scale;
		vertice.position.y = petri_pos.y + float(j) * scale;
		grid_line_vertices.push_back(vertice);
		vertice.position.x = petri_pos.x + float(petri->cols) * scale;
		vertice.position.y = petri_pos.y + float(j) * scale;
		grid_line_vertices.push_back(vertice);
	}
	for (int j = 0; j < petri->cols + 1; j = j + 5) {
		vertice.position.y = petri_pos.y + 0.0f * scale;
		vertice.position.x = petri_pos.x + float(j) * scale;
		grid_line_vertices.push_back(vertice);
		vertice.position.y = petri_pos.y + float(petri->rows) * scale;
		vertice.position.x = petri_pos.x + float(j) * scale;
		grid_line_vertices.push_back(vertice);

	}

	window_manager->getWindow().draw(&grid_line_vertices[0], grid_line_vertices.size(), sf::Lines);
}