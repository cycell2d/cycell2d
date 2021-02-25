#include "User_Input.h"
#include "parameters.h"
#include "dirent.h"
#include <algorithm>
using namespace std;


sf::Vector2i User_Input::getMouseRawPosition(sf::RenderWindow& window)
{
	return sf::Mouse::getPosition(window);
}

sf::Vector2f User_Input::getMouseWorldPosition(sf::RenderWindow& window, const sf::View& view)
{

	sf::Vector2f result;
	sf::View original_view = window.getView();
	window.setView(view);
	result = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	return result;
	window.setView(original_view);
}

void User_Input::poll(sf::RenderWindow& window)
{

	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setPosition(status_bar_width, 0);

	memset(one_shot_switch_key, 0, sizeof(one_shot_switch_key));
	one_shot_switch = vector<bool>(gui_LAST_ENTRY + 1, 0);
	gui.get<tgui::Label>("knob_value_label")->setText(std::to_string(getOrientationKnob()) + "  Degrees");

	if (getGUI_ITERATE_BUTTON()) {
		gui.get<tgui::Button>("_it_iterateButton")->getRenderer()->setTextColor(tgui::Color(30, 255, 20));
		gui.get<tgui::Button>("_it_iterateButton")->getRenderer()->setTextColorHover(tgui::Color(30, 255, 20));
	}
	else {
		gui.get<tgui::Button>("_it_iterateButton")->getRenderer()->setTextColor(tgui::Color(190,190,190));
		gui.get<tgui::Button>("_it_iterateButton")->getRenderer()->setTextColorHover(tgui::Color(250, 250, 250));
	}

	//********************
	//SIMPLE GUI EVENTS
	//********************

	pollLinkedTabs();



	if (gui.get<tgui::RadioButton>("manipu_rb_select")->isChecked()) {
		toggle_switch[gui_MANIP_SELECT_RADIO] = true;
	}
	else {
		toggle_switch[gui_MANIP_SELECT_RADIO] = false;
	}
	if (gui.get<tgui::RadioButton>("manipu_rb_deselect")->isChecked()) {
		toggle_switch[gui_MANIP_DESELECT_RADIO] = true;
	}
	else {
		toggle_switch[gui_MANIP_DESELECT_RADIO] = false;
	}
	if (gui.get<tgui::RadioButton>("manipu_rb_move")->isChecked()) {
		toggle_switch[gui_MANIP_MOVE_RADIO] = true;
	}
	else {
		toggle_switch[gui_MANIP_MOVE_RADIO] = false;
	}

	if (gui.get<tgui::CheckBox>("_it_useField")->isChecked()) {
		toggle_switch[gui_IT_USE_FIELD] = true;
		gui.get<tgui::CheckBox>("_it_drawField")->setVisible(1);
		gui.get<tgui::CheckBox>("_it_drawOrientations")->setVisible(1);
	}
	else {
		toggle_switch[gui_IT_USE_FIELD] = false;
		gui.get<tgui::CheckBox>("_it_drawField")->setVisible(0);
		gui.get<tgui::CheckBox>("_it_drawField")->setChecked(0);
		gui.get<tgui::CheckBox>("_it_drawOrientations")->setVisible(0);
		gui.get<tgui::CheckBox>("_it_drawOrientations")->setChecked(0);
	}

	if (gui.get<tgui::CheckBox>("_it_drawField")->isChecked()) {
		toggle_switch[gui_IT_DRAW_FIELD] = true;
	}
	else {
		toggle_switch[gui_IT_DRAW_FIELD] = false;
	}
	if (gui.get<tgui::CheckBox>("_it_drawOrientations")->isChecked()) {
		toggle_switch[gui_IT_DRAW_ORIENTS] = true;
	}
	else {
		toggle_switch[gui_IT_DRAW_ORIENTS] = false;
	}


	if (gui.get<tgui::CheckBox>("IT_fancy_checkbox")->isChecked()) {
		toggle_switch[gui_IT_FANCY] = true;
	}
	else {
		toggle_switch[gui_IT_FANCY] = false;
	}

	if (gui.get<tgui::CheckBox>("cr_manualPlacement_Checkbox")->isChecked()) {
		toggle_switch[gui_IT_MANUAL_PLACEMENT_CHECKBOX] = true;
	}
	else {
		toggle_switch[gui_IT_MANUAL_PLACEMENT_CHECKBOX] = false;
	}
	if (gui.get<tgui::CheckBox>("_it_drawGradient")->isChecked()) {
		toggle_switch[gui_IT_DRAW_GRADIENT_CHECKBOX] = true;
	}
	else {
		toggle_switch[gui_IT_DRAW_GRADIENT_CHECKBOX] = false;
	}

	if (gui.get<tgui::CheckBox>("_ft_Adjust_Checkbox")->isChecked()) {
		toggle_switch[gui_FIELD_ADJUST_PL_CHECKBOX] = true;
	}
	else {
		toggle_switch[gui_FIELD_ADJUST_PL_CHECKBOX] = false;
	}

	if (gui.get<tgui::CheckBox>("_cr_debugcheckbox")->isChecked()) {
		toggle_switch[gui_CR_DEBUGCHECKBOX] = true;
	}
	else {
		toggle_switch[gui_CR_DEBUGCHECKBOX] = false;
	}
	if (gui.get<tgui::CheckBox>("draw_vector_checkbox")->isChecked()) {
		toggle_switch[gui_DRAW_VECTOR_CHECKBOX] = true;
	}
	else {
		toggle_switch[gui_DRAW_VECTOR_CHECKBOX] = false;
	}
	
	
	gui.get<tgui::Button>("cs_clear_Button")->setText("Place " + to_string_with_precision(gui.get<tgui::Slider>("cs_colSlider")->getValue(), 0) + " by " + to_string_with_precision(gui.get<tgui::Slider>("cs_rowSlider")->getValue(), 0) + " grid.");

	

	resize_needed = 0;
	close_window_needed = 0;
	mouse_wheel_delta = 0;
	//*********************
	//POLL EVENTS
	//*********************


	poll_events(window);

	if (resize_needed) {

		adjustVariableSizeIcons(window);
	}

	//*********************
	//MOUSE BUTTONS
	//*********************
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (!momentary_switch[gui_MOUSE_LEFT_BUTTON]) {
			one_shot_switch[gui_MOUSE_LEFT_BUTTON] = true;
		}
		momentary_switch[gui_MOUSE_LEFT_BUTTON] = true;
	}
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		momentary_switch[gui_MOUSE_LEFT_BUTTON] = false;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		if (!momentary_switch[gui_MOUSE_RIGHT_BUTTON]) {
			one_shot_switch[gui_MOUSE_RIGHT_BUTTON] = true;
		}
		momentary_switch[gui_MOUSE_RIGHT_BUTTON] = true;
	}
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		momentary_switch[gui_MOUSE_RIGHT_BUTTON] = false;
	}

}

void User_Input::poll_events(sf::RenderWindow & window) {

	sf::Event event;
	while (window.pollEvent(event)) {

		//*********************
		//GUI events
		//*********************
		gui.handleEvent(event);


		//*********************
		//KEYBOARD EVENTS
		//*********************
		if (event.type == sf::Event::KeyPressed)
		{
			for (int key_x = sf::Keyboard::A; key_x != sf::Keyboard::KeyCount; key_x++)
			{
				if (event.key.code == key_x) {
					if (momentary_switch_key[key_x] == false) {
						one_shot_switch_key[key_x] = true;
					}
					momentary_switch_key[key_x] = true;
					toggle_switch_key[key_x] = !toggle_switch_key[key_x];
				}
			}
		}
		if (event.type == sf::Event::KeyReleased)
		{
			for (int key_x = sf::Keyboard::A; key_x != sf::Keyboard::KeyCount; key_x++)
			{
				if (event.key.code == key_x) {
					momentary_switch_key[key_x] = false;
				}

			}
		}


		//*********************
		//MOUSE SCROLL EVENTS
		//*********************
		if (event.type == sf::Event::MouseWheelScrolled) {
			mouse_wheel_delta = (int)event.mouseWheelScroll.delta;
		}


		//*********************
		//WINDOW EVENTS
		//*********************

		if (event.type == sf::Event::Closed) {
			window.close();
		}
		if (event.type == sf::Event::Resized)
		{
			resize_needed = 1;
		}

	}


}
void User_Input::pollLinkedTabs()
{

	auto top_tab = gui.get<tgui::Tabs>("top_tab");
	auto bottom_tab = gui.get<tgui::Tabs>("bottom_tab");
	int top_selected = top_tab->getSelectedIndex();
	int bottom_selected = bottom_tab->getSelectedIndex();

	if (top_selected < 0) {
		toggle_switch[gui_TAB_SORTER] = 0;
		toggle_switch[gui_TAB_MONITOR] = 0;
		toggle_switch[gui_TAB_MANIPULATE] = 0;
	}
	if (bottom_selected < 0) {
		toggle_switch[gui_TAB_FIELD] = 0;
		toggle_switch[gui_TAB_EDITOR] = 0;
	}



	if ((top_selected >= 0) &&
		(toggle_switch[gui_TAB_SORTER] == 0) &&
		(toggle_switch[gui_TAB_MONITOR] == 0) &&
		(toggle_switch[gui_TAB_MANIPULATE] == 0)

		) {
		toggle_switch[gui_TAB_FIELD] = 0;
		toggle_switch[gui_TAB_EDITOR] = 0;
		bottom_tab->deselect();
		bottom_selected = -1;
	
	}

	if ((bottom_selected >= 0) &&
		(toggle_switch[gui_TAB_FIELD] == 0) &&
		(toggle_switch[gui_TAB_EDITOR] == 0))
		 {
		toggle_switch[gui_TAB_SORTER] = 0;
		toggle_switch[gui_TAB_MONITOR] = 0;
		toggle_switch[gui_TAB_MANIPULATE] = 0;
		top_tab->deselect();
		top_selected = -1;
		
	}


	if (top_selected >= 0){
	

		switch (top_selected) {
		case 0:
			toggle_switch[gui_TAB_SORTER] = 1;
			toggle_switch[gui_TAB_MONITOR] = 0;
			toggle_switch[gui_TAB_MANIPULATE] = 0;
			break;
		case 1:
			toggle_switch[gui_TAB_SORTER] = 0;
			toggle_switch[gui_TAB_MONITOR] =1;
			toggle_switch[gui_TAB_MANIPULATE] = 0;
			break;
		case 2:
			toggle_switch[gui_TAB_SORTER] = 0;
			toggle_switch[gui_TAB_MONITOR] = 0;
			toggle_switch[gui_TAB_MANIPULATE] = 1;
			break;
		}
	}

	if (bottom_selected >= 0){

		switch (bottom_selected) {
		case 0:
			toggle_switch[gui_TAB_FIELD] = 1;
			toggle_switch[gui_TAB_EDITOR] = 0;
			break;
		case 1:
			toggle_switch[gui_TAB_FIELD] = 0;
			toggle_switch[gui_TAB_EDITOR] = 1;
			break;
		}
	}




}
void User_Input::adjustVariableSizeIcons(sf::RenderWindow& wind)
{
	auto c_w = gui.get<tgui::ChildWindow>("_ed_ChildWindow");

	c_w->setSize(wind.getSize().x-status_bar_width, wind.getSize().y);
	c_w->setPosition(status_bar_width, 0);



	auto err_text_b = gui.get<tgui::TextBox>("_ed_errorTextBox");
	auto t_w = gui.get<tgui::TextBox>("_ed_TextBox");
	auto synt = gui.get<tgui::TextBox>("_ed_syntaxTextBox");

	if (!synt->isVisible()) {
		t_w->setSize(c_w->getSize().x - 20, c_w->getSize().y - 90.0f - 115);
		t_w->setPosition(10, 20);
		err_text_b->setSize(c_w->getSize().x - 20, 115);
		err_text_b->setPosition(10, c_w->getSize().y - (50.0f + 115));
	}
	else {
		t_w->setSize(c_w->getSize().x - 20, c_w->getSize().y - 90.0f - 115-240-20);
		t_w->setPosition(10, 20);
		err_text_b->setSize(c_w->getSize().x - 20, 115);
		err_text_b->setPosition(10, c_w->getSize().y - (50.0f + 115));
		synt->setSize(c_w->getSize().x - 20, 240);
		synt->setPosition(10, c_w->getSize().y - (50.0f + 115) - 240 - 20);
	}


	auto debug_text_b = gui.get<tgui::TextBox>("_debug_textbox");
	debug_text_b->setSize((int)wind.getSize().x-status_bar_width, 155);
	debug_text_b->setPosition(status_bar_width, wind.getSize().y-debug_text_b->getSize().y);
	auto mini_rules_debug = gui.get<tgui::TextBox>("rule_viewer");
	mini_rules_debug->setSize( (int)wind.getSize().x - status_bar_width, 215);
	mini_rules_debug->setPosition(status_bar_width, wind.getSize().y - mini_rules_debug->getSize().y- debug_text_b->getSize().y-5);
	auto unsaved_label = gui.get<tgui::Label>("_ed_unsaved");
	unsaved_label->setPosition(10, 0);
	unsaved_label->setSize(t_w->getSize().x, 18);

}
bool User_Input::getGUI_ITERATE_BUTTON()
{
	if (gui.get<tgui::RadioButton>("_it_step_RadioButton")->isChecked()) {
		toggle_switch[gui_IT_ITERATE_BUTTON] = 0;
		return one_shot_switch[gui_IT_ITERATE_BUTTON];
	}
	if (gui.get<tgui::RadioButton>("_it_momentary_RadioButton")->isChecked()) {
		toggle_switch[gui_IT_ITERATE_BUTTON] = 0;
		return momentary_switch[gui_IT_ITERATE_BUTTON];
	}
	if (gui.get<tgui::RadioButton>("_it_toggle_RadioButton")->isChecked()) {
		return toggle_switch[gui_IT_ITERATE_BUTTON];
	}
	return false;
}

int User_Input::getGUI_FIELD_VALUE_SWATCH()
{
	if (gui.get<tgui::RadioButton>("_field_rb_0")->isChecked()) {
		return 0;
	}
	if (gui.get<tgui::RadioButton>("_field_rb_1")->isChecked()) {
		return 1;
	}
	if (gui.get<tgui::RadioButton>("_field_rb_2")->isChecked()) {
		return 2;
	}
	if (gui.get<tgui::RadioButton>("_field_rb_3")->isChecked()) {
		return 3;
	}
	if (gui.get<tgui::RadioButton>("_field_rb_4")->isChecked()) {
		return 4;
	}
	return -1;
}

void User_Input::setStateNamestoComboBox(vector<string> sn)
{
	gui.get<tgui::ComboBox>("cr_ManualPlacementComboBox")->removeAllItems();

	for (auto s : sn) {
		if (s == "MEDIUM") { continue; }
		gui.get<tgui::ComboBox>("cr_ManualPlacementComboBox")->addItem(s);
	}
}

bool User_Input::shouldMouseBeAbleToDrawOrPlaceCells()
{

	if (gui.get<tgui::ChildWindow>("_ed_ChildWindow")->isVisible()) {  return 0; }
	if (gui.get<tgui::ChildWindow>("_sd_ChildWindow")->isVisible()) { return 0; }
	if (gui.get<tgui::ChildWindow>("_ld_ChildWindow")->isVisible()) { return 0; }
	if (gui.get<tgui::ChildWindow>("_wts_ChildWindow")->isVisible ()) { return 0; }

	return 1;
}

bool User_Input::isMouseOnLargerWidgets()
{
	if (gui.get<tgui::ChildWindow>("_sd_ChildWindow")->isFocused()) {
		return true;
	}
	if (gui.get<tgui::ChildWindow>("_ed_ChildWindow")->isFocused()) {
		return true;
	}
	if (gui.get<tgui::ChildWindow>("_ld_ChildWindow")->isFocused()) {
		return true;
	}
	if (gui.get<tgui::TextBox>("_debug_textbox")->isFocused()) {
		return true;
	}
	if (gui.get<tgui::TextBox>("rule_viewer")->isFocused()) {
		return true;
	}
	return false;
}

void User_Input::loadSwitchData(vector<bool>& one_shot, vector<bool>& momentary, vector<bool>& toggle, vector<float>& values)
{
	one_shot = one_shot_switch;
	momentary = momentary_switch;
	toggle = toggle_switch;
	
		values[gui_SLIDER_TEMP] = gui.get<tgui::Slider>("_it_flux_Slider")->getValue();
		values[gui_SLIDER_SPEED] = gui.get<tgui::Slider>("_it_speed_Slider")->getValue();
		values[gui_SLIDER_YELLOW_SELF] = gui.get<tgui::Slider>("cs_YellowSelf_Slider")->getValue();
		values[gui_SLIDER_YELLOW_YELLOW] = gui.get<tgui::Slider>("cs_YellowYellow_Slider")->getValue();
		values[gui_SLIDER_YELLOW_MEDIUM] = gui.get<tgui::Slider>("cs_YellowMedium_Slider")->getValue();
		values[gui_SLIDER_YELLOW_SIZE] = gui.get<tgui::Slider>("cs_yellowSize_slider")->getValue();
		values[gui_SLIDER_BLUE_SELF] = gui.get<tgui::Slider>("cs_BlueSelf_Slider")->getValue();
		values[gui_SLIDER_BLUE_BLUE] = gui.get<tgui::Slider>("cs_BlueBlue_Slider")->getValue();
		values[gui_SLIDER_BLUE_MEDIUM] = gui.get<tgui::Slider>("cs_BlueMedium_Slider")->getValue();
		values[gui_SLIDER_BLUE_SIZE] = gui.get<tgui::Slider>("cs_blueSize_slider")->getValue();
		values[gui_SLIDER_YELLOW_BLUE] = gui.get<tgui::Slider>("cs_BlueYellow_Slider")->getValue();
		values[gui_SLIDER_BRUSH_SIZE] = gui.get<tgui::Slider>("field_brushsize")->getValue();
		values[gui_KNOB_ORIENTATION] =  gui.get<tgui::Knob>("field_knob")->getValue();
		values[gui_SLIDER_ROWS] = gui.get<tgui::Slider>("cs_rowSlider")->getValue();
		values[gui_SLIDER_COLS] =  gui.get<tgui::Slider>("cs_colSlider")->getValue();
}



void User_Input::setTimeLabel(float a, float b)
{
	
	gui.get<tgui::Label>("time_label")->setText("Lab: "+to_string(a) + " ms\n" + "Petri: " + to_string(b) + " ms");
}
void User_Input::setIteratePanelFieldLoadedLabel(string s)
{
	gui.get<tgui::Label>("_it_Field_in_memory")->setText("Field in CPU: " + s);
}

void User_Input::setIteratePanelRuleLoadedLabel(string s)
{
		gui.get<tgui::Label>("_it_rule_loaded_label")->setText("Rules in CPU: " + s);
}

void User_Input::load_state_names_to_GUI(vector<string> s)
{
	tgui::ComboBox::Ptr cb_holder = gui.get<tgui::ComboBox>("cr_ManualPlacementComboBox");
	cb_holder->removeAllItems();

	for (auto sn : s) {
		cb_holder->addItem(sn);
	}

}
void User_Input::turn_on_wts_dialog2(sf::RenderWindow& wind) {
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(0);
	gui.get<tgui::Tabs>("top_tab")->setEnabled(0);
	gui.get<tgui::Tabs>("bottom_tab")->setEnabled(0);
	gui.get<tgui::Panel>("Editor_Panel")->setEnabled(0);
	gui.get<tgui::Panel>("Field_Panel")->setEnabled(0);
	tgui::ChildWindow::Ptr c_w;

	c_w = gui.get<tgui::ChildWindow>("_wts_ChildWindow");
	c_w->moveToFront();
	c_w->setEnabled(1);
	c_w->setPosition(((wind.getSize().x - status_bar_width - c_w->getSize().x) / 2.0f) + status_bar_width, 30);
	c_w->setVisible(1);
	gui.get<tgui::Label>("_wts_filename")->setVisible(1);
}
void User_Input::turn_on_load_dialog2(sf::RenderWindow& wind, int file_type_flag) {

	string extension = "";
	if (file_type_flag == 0) {
		extension = ".cyr";
	}
	if (file_type_flag == 1) {
		extension = ".png";
	}
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(0);
	gui.get<tgui::Tabs>("top_tab")->setEnabled(0);
	gui.get<tgui::Tabs>("bottom_tab")->setEnabled(0);
	gui.get<tgui::Panel>("Editor_Panel")->setEnabled(0);
	gui.get<tgui::Panel>("Field_Panel")->setEnabled(0);
	tgui::ChildWindow::Ptr c_w;
	c_w = gui.get<tgui::ChildWindow>("_ld_ChildWindow");
	c_w->setEnabled(1);
	c_w->moveToFront();
	c_w->setPosition(((wind.getSize().x - status_bar_width - c_w->getSize().x) / 2.0f) + status_bar_width, 30);
	c_w->setVisible(1);

	gui.get<tgui::TextBox>("_ld_TextBox")->setText("");
	gui.get<tgui::Label>("_ld_Position_File_Error")->setVisible(0);
	gui.get<tgui::Label>("_ld_Label")->setVisible(0);

	DIR* dir;
	struct dirent* ent;
	stringstream text_holder;
	string word_holder;
	if ((dir = opendir(".\\")) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			word_holder = ent->d_name;
			if (word_holder.size() > extension.size()) {

				if (word_holder.substr(word_holder.size() - extension.size()) == extension) {
					text_holder << word_holder << "\n";
				}
			}

		}
		closedir(dir);

	}
	else {
		text_holder << "*** Can't Read Directory ***\n";
	}


	gui.get<tgui::TextBox>("_ld_TextBox")->addText(text_holder.str());


}

void User_Input::turn_on_save_dialog2(sf::RenderWindow& wind, string proposed_file_name) {
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(0);
	gui.get<tgui::Tabs>("top_tab")->setEnabled(0);
	gui.get<tgui::Tabs>("bottom_tab")->setEnabled(0);
	gui.get<tgui::Panel>("Editor_Panel")->setEnabled(0);
	gui.get<tgui::Panel>("Field_Panel")->setEnabled(0);

	tgui::ChildWindow::Ptr c_w;
	gui.get<tgui::EditBox>("_sd_editBox")->setText(proposed_file_name);

	c_w = gui.get<tgui::ChildWindow>("_sd_ChildWindow");
	c_w->moveToFront();
	c_w->setEnabled(1);
	c_w->setPosition(((wind.getSize().x - status_bar_width - c_w->getSize().x) / 2.0f) + status_bar_width, 30);
	c_w->setVisible(1);
	gui.get<tgui::Label>("_sd_saveError")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(0);

}


void User_Input::setEditorTextAndLabels(string s, string file)
{
	gui.get<tgui::TextBox>("_ed_TextBox")->setText(s);
	gui.get<tgui::Label>("_ed_filenameLabel")->setText(file);
	gui.get<tgui::TextBox>("_ed_errorTextBox")->setText("");


}

void User_Input::turn_off_load_dialog()
{


	gui.get<tgui::ChildWindow>("_ld_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ld_ChildWindow")->setEnabled(0);
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(1);
	gui.get<tgui::Tabs>("top_tab")->setEnabled(1);
	gui.get<tgui::Tabs>("bottom_tab")->setEnabled(1);
	gui.get<tgui::Panel>("Editor_Panel")->setEnabled(1);
	gui.get<tgui::Panel>("Field_Panel")->setEnabled(1);

}
void User_Input::turn_off_save_dialog()
{
	gui.get<tgui::ChildWindow>("_sd_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_sd_ChildWindow")->setEnabled(0);
	gui.get<tgui::Label>("_sd_saveError")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(1);
	gui.get<tgui::Tabs>("top_tab")->setEnabled(1);
	gui.get<tgui::Tabs>("bottom_tab")->setEnabled(1);
	gui.get<tgui::Panel>("Editor_Panel")->setEnabled(1);
	gui.get<tgui::Panel>("Field_Panel")->setEnabled(1);
}
void User_Input::turn_off_edit_dialog()
{
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(0);
}
void User_Input::turn_off_wts_dialog()
{
	gui.get<tgui::ChildWindow>("_wts_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_wts_ChildWindow")->setEnabled(0);
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setEnabled(1);
	gui.get<tgui::Tabs>("top_tab")->setEnabled(1);
	gui.get<tgui::Tabs>("bottom_tab")->setEnabled(1);
	gui.get<tgui::Panel>("Editor_Panel")->setEnabled(1);
	gui.get<tgui::Panel>("Field_Panel")->setEnabled(1);
}



void User_Input::clearAndLoadEditBoxErrorText(string s)
{
	gui.get<tgui::TextBox>("_ed_errorTextBox")->setText(s);


}
string User_Input::getLoadDialogFileName()
{
	return gui.get<tgui::EditBox>("_ld_EditBox")->getText();
}
string User_Input::getSaveDialogFileName()
{
	string word_holder = gui.get<tgui::EditBox>("_sd_editBox")->getText();
	

	return word_holder;

}
void User_Input::displayLoadDialogErrorLabel()
{

	gui.get<tgui::Label>("_ld_Label")->setVisible(1);
}
void User_Input::displaySaveDialogErrorLabel()
{
	gui.get<tgui::Label>("_sd_saveError")->setVisible(1);
}

void User_Input::adjustSaveLoadLabelsForFileType(int x)
{

	if (x == 0) {
		gui.get<tgui::ChildWindow>("_sd_ChildWindow")->setTitle("Save Rules");
		gui.get<tgui::ChildWindow>("_ld_ChildWindow")->setTitle("Load Rules");
		gui.get<tgui::Label>("_sd_extension_label")->setText("[Filename must end with .cyr]");
		return;
	}

	if (x == 1) {
		gui.get<tgui::ChildWindow>("_sd_ChildWindow")->setTitle("Save Field Image");
		gui.get<tgui::ChildWindow>("_ld_ChildWindow")->setTitle("Load Field Image");
		gui.get<tgui::Label>("_sd_extension_label")->setText("[Filename must end with .png]");
		return;
	}
	return;
};

void User_Input::create_widgets(sf::RenderWindow& wind)
{



	gui.setTarget(wind);
	


	gui.loadWidgetsFromStream(form_file);

	gui.get<tgui::ChildWindow>("_sd_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ld_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ed_ChildWindow")->setVisible(0);
	
	gui.get<tgui::RadioButton>("_field_rb_0")->setChecked(1);

	auto c_w = gui.get<tgui::ChildWindow>("_ld_ChildWindow");
	c_w->connect("Closed", [=]() {	 c_w->setVisible(0);
	gui.get<tgui::TextBox>("_ld_TextBox")->setText(""); });

	c_w = gui.get<tgui::ChildWindow>("_ed_ChildWindow");
	c_w->connect("Closed", [=]() {c_w->setVisible(0); });

	c_w = gui.get<tgui::ChildWindow>("_sd_ChildWindow");
	c_w->connect("Closed", [=]() {	 c_w->setVisible(0); });

	auto t_b = gui.get<tgui::TextBox>("_ld_TextBox");
	t_b->connect("SelectionChanged", [=]() { if ((t_b->getSelectedText().find("\n") == std::string::npos) && (t_b->getSelectedText() != "")) { gui.get<tgui::EditBox>("_ld_EditBox")->setText(t_b->getSelectedText()); } });
	t_b = gui.get<tgui::TextBox>("_ed_TextBox");
	t_b->connect("TextChanged", [=]() {
		one_shot_switch[gui_ED_TEXT_CHANGED]=1;
		setUnsavedEditorLabel(1);
	});
	auto e_b = gui.get<tgui::EditBox>("_sd_editBox");
	e_b->connect("TextChanged", [=]() {
		gui.get<tgui::Label>("_sd_saveError")->setVisible(0); });


	

	create_button_widget_helper("_ld_loadButton", gui_LD_LOAD_BUTTON);
	create_button_widget_helper("manip_kill", gui_MANIP_KILL_BUTTON);
	create_button_widget_helper("_sd_saveButton", gui_SD_SAVE_BUTTON);
	create_button_widget_helper("_it_clear", gui_IT_CLEAR);

	create_button_widget_helper("_it_iterateButton", gui_IT_ITERATE_BUTTON);

	create_button_widget_helper("cs_clear_Button", gui_CS_CLEAR_AND_PLACE);
	create_button_widget_helper("_wts_save_button", gui_WTS_SAVE);
	create_button_widget_helper("_wts_discard", gui_WTS_DISCARD);
	create_button_widget_helper("_wts_cancel", gui_WTS_CANCEL);
	create_button_widget_helper("field_save", gui_FIELD_SAVE);
	create_button_widget_helper("field_load", gui_FIELD_LOAD);
	create_button_widget_helper("field_clear", gui_FIELD_CLEAR);
	create_button_widget_helper("load_rules", gui_ED_LOAD_BUTTON);
	create_button_widget_helper("compile_rules", gui_ED_COMPILE_BUTTON);
	create_button_widget_helper("save_rules", gui_ED_SAVE_BUTTON);
	create_button_widget_helper("syntax_guide_button", gui_SYNTAX_GUIDE_BUTTON);
	

	create_slider_widget_helper("_it_flux_Slider", "_it_flux_Label", "Flux: ", 2);
	create_slider_widget_helper("_it_speed_Slider", "_it_speed_Label", "Speed: ", 2);
	create_slider_widget_helper("cs_BlueSelf_Slider", "cs_BlueSelf_SliderLabel", "", 1);
	create_slider_widget_helper("cs_BlueMedium_Slider", "cs_BlueMedium_SliderLabel", "", 1);
	create_slider_widget_helper("cs_BlueBlue_Slider", "cs_BlueBlue_SliderLabel", "", 1);
	create_slider_widget_helper("cs_BlueYellow_Slider", "cs_BlueYellow_SliderLabel", "", 1);
	create_slider_widget_helper("cs_YellowYellow_Slider", "cs_YellowYellow_SliderLabel", "", 1);
	create_slider_widget_helper("cs_YellowSelf_Slider", "cs_YellowSelf_SliderLabel", "", 1);
	create_slider_widget_helper("cs_YellowMedium_Slider", "cs_YellowMedium_SliderLabel", "", 1);
	create_slider_widget_helper("cs_blueSize_slider", "cs_BlueSize_SliderLabel", "", 0);
	create_slider_widget_helper("cs_yellowSize_slider", "cs_YellowSize_SliderLabel", "", 0);



	gui.get<tgui::CheckBox>("IT_fancy_checkbox")->setFocusable(0);
	gui.get<tgui::CheckBox>("_it_drawGradient")->setFocusable(0);
	gui.get<tgui::CheckBox>("_it_useField")->setFocusable(0);
	gui.get<tgui::CheckBox>("_it_drawField")->setFocusable(0);
	gui.get<tgui::CheckBox>("_it_drawOrientations")->setFocusable(0);
	gui.get<tgui::CheckBox>("cr_manualPlacement_Checkbox")->setFocusable(0);
	gui.get<tgui::CheckBox>("_cr_debugcheckbox")->setFocusable(0);
	gui.get<tgui::CheckBox>("_ft_Adjust_Checkbox")->setFocusable(0);
	gui.get<tgui::CheckBox>("draw_vector_checkbox")->setFocusable(0);
	gui.get<tgui::RadioButton>("_it_step_RadioButton")->setChecked(true);


	gui.get<tgui::ChildWindow>("_sd_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_wts_ChildWindow")->setVisible(0);
	gui.get<tgui::ChildWindow>("_ld_ChildWindow")->setVisible(0);
	gui.get<tgui::TextBox>("_ed_syntaxTextBox")->setVisible(0);


	gui.get<tgui::Tabs>("top_tab")->setVisible(1);
	gui.get<tgui::Tabs>("bottom_tab")->setVisible(1);


	set_sorter_panel_visibility( 0);
	set_monitor_panel_visibility(0);
	set_iterate_panel_visibility(0);
	set_field_panel_visibility(0);
	set_manipulate_panel_visibility(0);
	set_editor_panel_visibility(0);
	set_debug_textbox_visibility(0);

	auto tb = gui.get<tgui::TextBox>("_debug_textbox" );
	tb->connect("MouseEntered", [=]() {
		tb->setFocused(1);
	}
	);
	tb = gui.get<tgui::TextBox>("rule_viewer");
	tb->connect("MouseEntered", [=]() {
		tb->setFocused(1);
	}
	);
	
	gui.get<tgui::TextBox>("_ed_syntaxTextBox")->setText(syntax);


}

void User_Input::create_button_widget_helper(string s, int n)
{
	tgui::Button::Ptr generic_button_holder = gui.get<tgui::Button>(s);
	generic_button_holder->connect("MousePressed", [=]() {

		if (!momentary_switch[n]) {
			one_shot_switch[n] = true;
		}
		momentary_switch[n] = true;
		toggle_switch[n] = !toggle_switch[n];

	}
	);
	generic_button_holder->connect("MouseReleased", [=]() {
		momentary_switch[n] = false;
	}
	);
	generic_button_holder->connect("MouseLeft", [=]() {
		momentary_switch[n] = false;
	}
	);

}

void User_Input::create_slider_widget_helper(string s, string label, string label_text, int precision)
{

	tgui::Slider::Ptr slider_holder = gui.get<tgui::Slider>(s);
	slider_holder->connect("ValueChanged", [=]() {
		gui.get<tgui::Label>(label)->setText(label_text + to_string_with_precision(slider_holder->getValue(), precision));
	}
	);
	gui.get<tgui::Label>(label)->setText(label_text + to_string_with_precision(gui.get<tgui::Slider>(s)->getValue(), precision));

}
