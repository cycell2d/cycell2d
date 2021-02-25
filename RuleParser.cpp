#include "RuleParser.h"
#include "Cell.h"
using namespace std;
void RuleParser::loadRulesText(string s)
{


	size_t pos = s.find("\r");

	// Repeat till end is reached
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		s.replace(pos, 1, "");
		// Get the next occurrence from the current position
		pos = s.find("\r", pos);
	}


	rawRules.clear();

	stringstream s_stream;
	s_stream << s;
	string line;
	while (getline(s_stream, line)) {
		rawRules.push_back(line);
	}
}
bool RuleParser::poisson_test(float a) {
	
	
	//The following is the real poisson test. Used at one point when dealing with real time.
	//As everything is just iteration steps now, a cheaper function is used.

	return cheapo_Poisson(a);
	/*
	float fx = 1.0f - pow(2.71828182845f, -(((1.0f / a))));

	float r = random_float();


	if (random_float() < fx) {
		return true;
		mh();
	}
	return false;*/
}
bool RuleParser::comparison(float test_value, float param_a, float param_b, int comp_flag)
{
	//FLAG: 0 = [,], 1 = [,), 2 = (,], 3 = (,), 4 = []with no comma
	switch (comp_flag) {
	case 0:
		return ((test_value >= param_a) && (test_value <= param_b));
	case 1:
		return ((test_value >= param_a) && (test_value < param_b));
	case 2:
		return ((test_value > param_a) && (test_value <= param_b));
	case 3:
		return ((test_value > param_a) && (test_value < param_b));
	case 4:
		return (test_value == param_a);
	}
	return false;
}


void RuleParser::compile(stringstream& message, int& line_number)
{


	vector<string> raw_rules_without_default = rawRules;

	rawRules.push_back("STATE Default_Blue");
	rawRules.push_back("ENTERING_STATE; :: RGB(131,196,242); GOAL_AREA=60;");
	rawRules.push_back("STATE Default_Yellow");
	rawRules.push_back("ENTERING_STATE; :: RGB(227,226,98); GOAL_AREA=60;");
	rawRules.push_back("STATE OBSTRUCTION");
	rawRules.push_back("RGB(227,226,255);");




	//**************************
	//Quick Scan through Rules to
	//Pick up all the State Names
	//**************************
	StateNames = extractStateNamesFromRawRules(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************

	//cout << "States:\n";
	///cout << "Size: " << completeStateNames.size() << "\n";
	//cout << debug_print_vector(completeStateNames) << "\n\n";

//	bool need_to_add_defaults = 0;
	//**************************
	//Fill up double-map State_Name
	//Container. "MEDIUM" will be
	//inserted as first member
	//**************************
	StateNames.insert(StateNames.begin(), MEDIUM);

	//if (!isStringInStringVector("Default_Blue", StateNames)) {
	//	need_to_add_defaults = 1;
//	}
	//**************************
	//**************************

	SNIM.clear();
	for (size_t j = 0; j < StateNames.size(); j++) {
		SNIM[StateNames[j]] = j;
	}


	//**************************
	//Quick Scan through Rules to
	//Pick up all the User Variables
	//**************************
	completeUserVariableNames = extractUserVariablesFromRawRules(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************



	//**************************
	//Quick Scan through Rules to
	//Pick up all the Gradient Variables
	//**************************
	GradientVariableNames = extractGradientVariablesFromRawRules(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************
	GVIM.clear();
	for (size_t j = 0; j < GradientVariableNames.size(); j++) {
		GVIM[GradientVariableNames[j]] = j;
	}


	//**************************
//Quick Scan through Rules to
//Pick up all the Vector Variables
//**************************
	completeVectorVariableNames = extractVectorVariablesFromRawRules(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************


	//**************************
	//Set up the indexes for all 
	//variables in the cell_local_memory_index_map
	//and create a cell_local_memory of the correct
	//size.
	//**************************
	LocalMemoryNames.clear();
	for (auto p : reserved_Cell_Memory_Flag_variables) {
		LocalMemoryNames.push_back(p);
	}
	for (auto p : reserved_ROM_variables) {
		LocalMemoryNames.push_back(p);
	}
	for (auto p : reserved_RAM_variables) {
		LocalMemoryNames.push_back(p);
	}
	for (auto p : completeUserVariableNames) {
		LocalMemoryNames.push_back(p);
	}

	LMIM.clear();
	for (size_t j = 0; j < LocalMemoryNames.size(); j++) {
		LMIM[LocalMemoryNames[j]] = j;
	}


	VectorVariableNames.clear();
	for (auto p : completeVectorVariableNames) {
		VectorVariableNames.push_back(p);
	}
	VVIM.clear();
	for (size_t j = 0; j < VectorVariableNames.size(); j++) {
		VVIM[VectorVariableNames[j]] = j;
	}

	//**************************
	//Quick Check to ensure that 
	//the first non-comment line
	//is the start of either a 
	//STATE, GAP_JUNCTION or ADHESION section.
	//**************************
	isFirstLineStateOrAdhesionOrGJ(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************




	//**************************
	//Get the Line Number Range
	//For the State Section
	//**************************
	state_section_line_number_range = getStateRulesSection(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************


	//**************************
	//Get the Line Number Range
	//For the Adhesion Section
	//**************************
	adhesion_section_line_number_range = getAdhesionsSection(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************


	//**************************
	//Set flag if there is no 
	//Adhesion section.
	//**************************
	if (adhesion_section_line_number_range.first != -1) {
		isAdhesionSectionPresent = true;
	}
	else {
		isAdhesionSectionPresent = false;
	}
	//**************************
	//**************************


	//**************************
	//Get the Line Number Range
	//For the GJ
	//**************************
	GJ_section_line_number_range = getGJSection(message, line_number);
	if (message.str() != "") { return; }
	//**************************
	//**************************


	//**************************
	//Set flag if there is no 
	//GJ section.
	//**************************
	if (GJ_section_line_number_range.first != -1) {
		isGapJunctionPresent = true;
	}
	else {
		isGapJunctionPresent = false;
	}
	//**************************
	//**************************





	//**************************
	//Break State Section into 
	//Paragraphs
	//**************************
	paragraph_line_number_ranges = splitStateRulesSectionIntoParagraphs(message, line_number);
	//**************************
	//**************************




	//****************************************************
	//****************************************************
	//****************************************************
	//****************************************************
	//COMPILE STATE PARAGRAPHS
	//****************************************************
	//****************************************************
	//****************************************************
	//****************************************************
	state_paragraph_vector.clear();
	Paragraph paragraph_holder;
	for (size_t x = 0; x < StateNames.size(); x++) {
		state_paragraph_vector.push_back(paragraph_holder);
	}
	for (size_t x = 1; x < state_paragraph_vector.size(); x++) {

		packageParagraphs(state_paragraph_vector[x], paragraph_line_number_ranges[x - 1], message, line_number);
		if (message.str() != "") { return; }
	}
	//****************************************************
	//****************************************************




	//**************************
	//Every Rule set must have a Default_Blue and Default_Yellow
	//When Lab sends the default rules to be compiled, these will be there.
	//But, with User Rules, these 'hidden' states need to be manually added
	//at this point.
	//****************************


	//if (need_to_add_defaults) {
	//
	//	Paragraph default_blue_paragraph;
	//	Sentence default_blue_sentence;
	//	ConditionsAndCommands default_blue_condition;
	//	default_blue_condition.rawToken = ENTERING_STATE;
	//	default_blue_condition.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_blue_condition.components[TAG][0] = ENTERING_STATE;
	//	default_blue_sentence.conditions.push_back(default_blue_condition);
	//	ConditionsAndCommands default_blue_command;
	//	default_blue_command.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_blue_command.components[TAG][0] = RGB;
	//	default_blue_command.literal_values[PARAMETER_a] = 131;
	//	default_blue_command.isLiteralValue[PARAMETER_a] = 1;
	//	default_blue_command.literal_values[PARAMETER_b] = 196;
	//	default_blue_command.isLiteralValue[PARAMETER_b] = 1;
	//	default_blue_command.literal_values[PARAMETER_c] = 242;
	//	default_blue_command.isLiteralValue[PARAMETER_c] = 1;
	//	default_blue_sentence.commands.push_back(default_blue_command);
	//	default_blue_command.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_blue_command.components[TAG][0] = SET;
	//	default_blue_command.components[CURLY_BRACKET_PARAMETER][0] = GOAL_AREA;
	//	default_blue_command.indexes[CURLY_BRACKET_PARAMETER] = LMIM[GOAL_AREA];
	//	default_blue_command.literal_values[PARAMETER_a] = 60;
	//	default_blue_command.isLiteralValue[PARAMETER_a] = 1;
	//	default_blue_command.literal_values[PARAMETER_b] = 0;
	//	default_blue_command.isLiteralValue[PARAMETER_b] = 1;
	//	default_blue_command.literal_values[PARAMETER_c] = 0;
	//	default_blue_command.isLiteralValue[PARAMETER_c] = 1;
	//	default_blue_sentence.commands.push_back(default_blue_command);


	//	default_blue_paragraph.state_name = "Default_Blue";
	//	default_blue_paragraph.sentences.push_back(default_blue_sentence);
	//	state_paragraph_vector.push_back(default_blue_paragraph);



	//	Paragraph default_Yellow_paragraph;
	//	Sentence default_Yellow_sentence;
	//	ConditionsAndCommands default_Yellow_condition;
	//	default_Yellow_condition.rawToken = ENTERING_STATE;
	//	default_Yellow_condition.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_Yellow_condition.components[TAG][0] = ENTERING_STATE;
	//	default_Yellow_sentence.conditions.push_back(default_Yellow_condition);
	//	ConditionsAndCommands default_Yellow_command;
	//	default_Yellow_command.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_Yellow_command.components[TAG][0] = RGB;
	//	default_Yellow_command.literal_values[PARAMETER_a] = 227;
	//	default_Yellow_command.isLiteralValue[PARAMETER_a] = 1;
	//	default_Yellow_command.literal_values[PARAMETER_b] = 226;
	//	default_Yellow_command.isLiteralValue[PARAMETER_b] = 1;
	//	default_Yellow_command.literal_values[PARAMETER_c] = 98;
	//	default_Yellow_command.isLiteralValue[PARAMETER_c] = 1;
	//	default_Yellow_sentence.commands.push_back(default_Yellow_command);
	//	default_Yellow_command.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_Yellow_command.components[TAG][0] = SET;
	//	default_Yellow_command.components[CURLY_BRACKET_PARAMETER][0] = GOAL_AREA;
	//	default_Yellow_command.indexes[CURLY_BRACKET_PARAMETER] = LMIM[GOAL_AREA];
	//	default_Yellow_command.literal_values[PARAMETER_a] = 60;
	//	default_Yellow_command.isLiteralValue[PARAMETER_a] = 1;
	//	default_Yellow_command.literal_values[PARAMETER_b] = 0;
	//	default_Yellow_command.isLiteralValue[PARAMETER_b] = 1;
	//	default_Yellow_command.literal_values[PARAMETER_c] = 0;
	//	default_Yellow_command.isLiteralValue[PARAMETER_c] = 1;
	//	default_Yellow_sentence.commands.push_back(default_Yellow_command);


	//	default_Yellow_paragraph.state_name = "Default_Yellow";
	//	default_Yellow_paragraph.sentences.push_back(default_Yellow_sentence);
	//	state_paragraph_vector.push_back(default_Yellow_paragraph);

	//	Paragraph default_Obstruction_paragraph;
	//	Sentence default_Obstruction_sentence;
	//	ConditionsAndCommands default_Obstruction_command;
	//	default_Obstruction_command.components = vector < vector <string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));
	//	default_Obstruction_command.components[TAG][0] = RGB;
	//	default_Obstruction_command.literal_values[PARAMETER_a] = 227;
	//	default_Obstruction_command.isLiteralValue[PARAMETER_a] = 1;
	//	default_Obstruction_command.literal_values[PARAMETER_b] = 226;
	//	default_Obstruction_command.isLiteralValue[PARAMETER_b] = 1;
	//	default_Obstruction_command.literal_values[PARAMETER_c] = 255;
	//	default_Obstruction_command.isLiteralValue[PARAMETER_c] = 1;
	//	default_Obstruction_sentence.commands.push_back(default_Obstruction_command);
	//	;


	//	default_Obstruction_paragraph.state_name = "OBSTRUCTION";
	//	default_Obstruction_paragraph.sentences.push_back(default_Obstruction_sentence);
	//	state_paragraph_vector.push_back(default_Obstruction_paragraph);


	//	StateNames.push_back("Default_Blue");
	//	SNIM["Default_Blue"] = StateNames.size() - 1;
	//	StateNames.push_back("Default_Yellow");
	//	SNIM["Default_Yellow"] = StateNames.size() - 1;

	//	StateNames.push_back("OBSTRUCTION");
	//	SNIM["OBSTRUCTION"] = StateNames.size() - 1;

	//}


	//**************************
	//Initialize AdhesionValues With Defaults
	//**************************
	AdhesionValues = vector < vector <float> >(StateNames.size(), vector < float>(StateNames.size(), 4));
	AdhesionSelfValues = vector <float>(StateNames.size(), -20);
	//default adhesion values;
	// Cell_a - MEDIUM   = 2.4
	// Cell_a - Cell_a   =-6.5
	// Cell_a[self]      =-20
	// Cell_a - Cell_n   = 4
	for (size_t x = 0; x < AdhesionValues.size(); x++) {
		for (size_t y = 0; y < AdhesionValues[x].size(); y++) {
			if ((x == SNIM[MEDIUM]) || (y == SNIM[MEDIUM])) { AdhesionValues[x][y] = 2.4f; continue; }
			if (x == y) { AdhesionValues[x][y] = -6.5; continue; }
			AdhesionValues[x][y] = 4;
		}
	}
	//********************************************
	//********************************************



	//****************************************************
	//****************************************************
	//****************************************************
	//****************************************************
	//COMPILE ADHESIONS
	//****************************************************
	//****************************************************
	//****************************************************
	//****************************************************
	if (isAdhesionSectionPresent) {
		parseAdhesionValues(message, line_number);
		if (message.str() != "") { return; }
	}
	//****************************************************
	//****************************************************


	//**************************
	//Initialize GJ With Defaults
	//**************************
	GJValues = vector < vector < vector <float> >>(StateNames.size(), vector < vector <float> >(StateNames.size(), vector <float>(LocalMemoryNames.size(), 0)));
	//********************************************
	//********************************************

	//****************************************************
	//****************************************************
	//****************************************************
	//****************************************************
	//COMPILE GAP_JUNCTIONS
	//****************************************************
	//****************************************************
	//****************************************************
	//****************************************************
	if (isGapJunctionPresent) {
		parseGJ(message, line_number);
		if (message.str() != "") { return; }
	}


	rawRules = raw_rules_without_default;
	//****************************************************
	//****************************************************



	//cout << "\n\n\n\n********************************\nCOMPILED DEBUG\n*************************\n\n\n";
	//for (auto pp : state_paragraph_vector) {
	//	cout << "xxx" << pp.debugPrint() << "\n";
	//}
	//for (size_t a = 0; a < AdhesionValues.size(); a++) {
	//	for (size_t b = 0; b < AdhesionValues[a].size(); b++) {
	//		cout << "AdhesionValues[" << a << "][" << b << "] = " << AdhesionValues[a][b] << "\n";
	//	}
	//}
	//for (size_t a = 0; a < AdhesionSelfValues.size(); a++) {
	//	cout << "AdhesionSelfValues[" << a << "] = " << AdhesionSelfValues[a] << "\n";
	//}


	//for (int a = 0; a < GJValues.size(); a++) {
	//	for (int b = 0; b < GJValues[a].size(); b++) {
	//		for (int c = 0; c < GJValues[a][b].size(); c++) {
	//			cout << "GJValues[" << a << "]["<<b<<"]["<<c<<"]  = " << GJValues[a][b][c] << "\n";
	//		}
	//	}
	//}
	//sp();
}

void RuleParser::evaluate(Cell* _cell, stringstream& debugMessage, bool dbFlag)
{
	if (dbFlag) { debugMessage << "Evaluating Cell ID. No. " << _cell->ID << "\n"; }
	int cell_state = (int)_cell->local_memory[Current_State_Index];
	float test_value, a, b, c;
	int index_value;



	Paragraph* par_holder = &(state_paragraph_vector[cell_state]);


	for (auto sn : par_holder->sentences) {


		bool passed_condition = true;
		for (auto cn : sn.conditions) {
			if (dbFlag) {
				if (cn.isNot)
					debugMessage << "Condition: NOT " << cn.rawToken << ": ";
				else {
					debugMessage << "Condition: " << cn.rawToken << ": ";
				}
			}


			if (cn.components[TAG][0] == POISSON) {
				if (dbFlag) { debugMessage << " evaluating Poisson[" << cn.literal_values[PARAMETER_a] << "]: "; }
				passed_condition = poisson_test(cn.literal_values[PARAMETER_a]);
				if (cn.isNot) { passed_condition = !passed_condition; }
				if (passed_condition) { if (dbFlag) { debugMessage << " PASSED\n"; } continue; }
				else { if (dbFlag) { debugMessage << " FAILED\n"; } break; }
			}


			if (cn.components[TAG][0] == FIELD) {
				test_value = (float)_cell->getFieldValueAtPresumedPosition();
				if (cn.comparison_flag == 4) {
					a = cn.literal_values[PARAMETER_a];
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				else {
					a = cn.literal_values[PARAMETER_a];
					b = cn.literal_values[PARAMETER_b];
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ". b=" << b << ", comparison flag = " << cn.comparison_flag << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				if (passed_condition) { if (dbFlag) { debugMessage << " PASSED\n"; } continue; }
				else { if (dbFlag) { debugMessage << " FAILED\n"; } break; }
			}


			if (cn.components[TAG][0] == ENTERING_STATE) {
				passed_condition = (bool)_cell->local_memory[Entering_State_Index];
				if (cn.isNot) { passed_condition = !passed_condition; }
				if (passed_condition) { if (dbFlag) { debugMessage << " PASSED\n"; } continue; }
				else { if (dbFlag) { debugMessage << " FAILED\n"; } break; }
			}



			if (cn.components[TAG][0] == FIELD_EDGES) {
				test_value = (float)_cell->getNumberOfEdgeNodesWithThisFieldValue((int)cn.literal_values[CURLY_BRACKET_PARAMETER]);
				if (cn.comparison_flag == 4) {
					a = cn.literal_values[PARAMETER_a];
					if (dbFlag) {
			
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				else {
					a = cn.literal_values[PARAMETER_a];
					b = cn.literal_values[PARAMETER_b];
					if (dbFlag) {
						debugMessage << "db((int)cn.literal_values[CURLY_BRACKET_PARAMETER]) = " << (int)cn.literal_values[CURLY_BRACKET_PARAMETER] << "  ";
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ". b=" << b << ", comparison flag = " << cn.comparison_flag << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				if (passed_condition) { if (dbFlag) { debugMessage << " PASSED\n"; } continue; }
				else { if (dbFlag) { debugMessage << " FAILED\n"; } break; }
			}



			if (cn.components[TAG][0] == ADJ_CELLS) {
				if (cn.components[CURLY_BRACKET_PARAMETER][0] == "") { test_value = (float)_cell->get_adjacent_cell_count(); }
				else { test_value = (float)_cell->get_adjacent_cell_count(cn.indexes[CURLY_BRACKET_PARAMETER]); }
				if (cn.comparison_flag == 4) {
					if (cn.isLiteralValue[PARAMETER_a]) { a = cn.literal_values[PARAMETER_a]; }
					else { a = _cell->local_memory[cn.indexes[PARAMETER_a]]; }
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				else {
					if (cn.isLiteralValue[PARAMETER_a]) { a = cn.literal_values[PARAMETER_a]; }
					else { a = _cell->local_memory[cn.indexes[PARAMETER_a]]; }
					if (cn.isLiteralValue[PARAMETER_b]) { b = cn.literal_values[PARAMETER_b]; }
					else { b = _cell->local_memory[cn.indexes[PARAMETER_b]]; }
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ". b=" << b << ", comparison flag = " << cn.comparison_flag << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				if (passed_condition) { if (dbFlag) { debugMessage << " PASSED\n"; } continue; }
				else { if (dbFlag) { debugMessage << " FAILED\n"; } break; }
			}



			if (cn.components[TAG][0] == ADJ_MEMBRANES) {
				if (cn.components[CURLY_BRACKET_PARAMETER][0] == "") { test_value = _cell->get_adjacent_membrane_count(); }
				else { test_value = (float)_cell->get_adjacent_membrane_count(cn.indexes[CURLY_BRACKET_PARAMETER]); }
				if (cn.comparison_flag == 4) {
					if (cn.isLiteralValue[PARAMETER_a]) { a = cn.literal_values[PARAMETER_a]; }
					else { a = _cell->local_memory[cn.indexes[PARAMETER_a]]; }
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				else {
					if (cn.isLiteralValue[PARAMETER_a]) { a = cn.literal_values[PARAMETER_a]; }
					else { a = _cell->local_memory[cn.indexes[PARAMETER_a]]; }
					if (cn.isLiteralValue[PARAMETER_b]) { b = cn.literal_values[PARAMETER_b]; }
					else { b = _cell->local_memory[cn.indexes[PARAMETER_b]]; }
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ". b=" << b << ", comparison flag = " << cn.comparison_flag << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				if (passed_condition) {
					if (dbFlag) { debugMessage << "  PASSED\n"; } continue;
				}
				else {
					if (dbFlag) { debugMessage << "  FAILED\n"; } break;
				}
			}



			if (cn.components[TAG][0] == ADJ_MEMBRANES_PERCENT) {
				test_value = (float)_cell->get_adjacent_membranes_percent(cn.indexes[CURLY_BRACKET_PARAMETER]);
				if (cn.comparison_flag == 4) {
					if (cn.isLiteralValue[PARAMETER_a]) { a = cn.literal_values[PARAMETER_a]; }
					else { a = _cell->local_memory[cn.indexes[PARAMETER_a]]; }
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				else {
					if (cn.isLiteralValue[PARAMETER_a]) { a = cn.literal_values[PARAMETER_a]; }
					else { a = _cell->local_memory[cn.indexes[PARAMETER_a]]; }
					if (cn.isLiteralValue[PARAMETER_b]) { b = cn.literal_values[PARAMETER_b]; }
					else { b = _cell->local_memory[cn.indexes[PARAMETER_b]]; }
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ". b=" << b << ", comparison flag = " << cn.comparison_flag << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				if (passed_condition) {
					if (dbFlag) { debugMessage << "  PASSED\n"; } continue;
				}
				else {
					if (dbFlag) { debugMessage << "  FAILED\n"; } break;
				}
			}


			if ((cn.isLocalMemoryVariable[TAG]) || (cn.isLiteralValue[TAG]) || (cn.isGradient[TAG])) {

				if (cn.isLiteralValue[TAG]) {
					test_value = cn.literal_values[TAG];
				}
				if (cn.isGradient[TAG]) {
					test_value = _cell->get_G_VAR_value_at_center_node(cn.indexes[TAG]);
				}
				if (cn.isLocalMemoryVariable[TAG]) {
					test_value = _cell->local_memory[cn.indexes[TAG]];
				}


				if (cn.isLiteralValue[PARAMETER_a]) {
					a = cn.literal_values[PARAMETER_a];
				}
				if (cn.isGradient[PARAMETER_a]) {
					a = _cell->get_G_VAR_value_at_center_node(cn.indexes[PARAMETER_a]);
				}
				if (cn.isLocalMemoryVariable[PARAMETER_a]) {
					a = _cell->local_memory[cn.indexes[PARAMETER_a]];
				}


				if (cn.comparison_flag == 4) {
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }
				}
				else {


					if (cn.isLiteralValue[PARAMETER_b]) {
						b = cn.literal_values[PARAMETER_b];
					}
					if (cn.isGradient[PARAMETER_b]) {
						b = _cell->get_G_VAR_value_at_center_node(cn.indexes[PARAMETER_b]);
					}
					if (cn.isLocalMemoryVariable[PARAMETER_b]) {
						b = _cell->local_memory[cn.indexes[PARAMETER_b]];
					}
					if (dbFlag) {
						debugMessage << " to Comparison Function: test_value = " << test_value << ", a=" << a << ". b=" << b << ", comparison flag = " << cn.comparison_flag << ": ";
					}
					passed_condition = comparison(test_value, a, b, cn.comparison_flag);
					if (cn.isNot) { passed_condition = !passed_condition; }

				}

				if (passed_condition) {
					if (dbFlag) { debugMessage << "  PASSED\n"; } continue;
				}
				else {
					if (dbFlag) { debugMessage << "  FAILED\n"; } break;
				}
			}






		}

		if (passed_condition == false) { continue; }
		if (dbFlag) {
			debugMessage << "\nCommands:\n";
		}
		for (auto cm : sn.commands) {
			if (dbFlag) {
				debugMessage << "Command: " << cm.rawToken << ":  ";
			}


		


			if (cm.components[TAG][0] == EMIT) {
				index_value = cm.indexes[CURLY_BRACKET_PARAMETER];
				_cell->parent_petri->addToGradientsToSetToOne(pair<Cell*, int>(_cell, index_value));
				if (dbFlag) {
					debugMessage << "  Set G_VAR " << GradientVariableNames[index_value] << " to 1. \n";
				}
			}



			if (cm.components[TAG][0] == ADJUST_PP_SCALE) {
				_cell->grow_shrink_amount = cm.literal_values[PARAMETER_a];
				vector <Node*> medium_edges = _cell->getEdgesTouchingMediumOrObstruction();
			
		
				
				for (auto me: medium_edges) {
					if (_cell->parent_petri->getFieldValueAtNode(_cell, me) == 0) {
						_cell->grow_vector += _cell->parent_petri->getNodeLocalCartesian(_cell, me);
					}
					else {
						_cell->shrink_vector += _cell->parent_petri->getNodeLocalCartesian(_cell, me);
					}
				}

				if (dbFlag) {
					debugMessage << "  Adjusted Scale. \n";
				}
			}

			if (cm.components[TAG][0] == MOD_PP_LOCATION) {
				index_value = cm.indexes[PARAMETER_a];
				_cell->index_of_V_VAR_to_be_used_for_mm_PP_loc = index_value;
				if (dbFlag) {
					debugMessage << "  Assigning V_VAR " << VectorVariableNames[index_value] << " to mod_PP_loc. ";
				}
				if (cm.isLocalMemoryVariable[PARAMETER_c]) {
					_cell->local_vector_memory[index_value].offset_angle = _cell->local_memory[cm.indexes[PARAMETER_c]];
					if (dbFlag) {
						debugMessage << " Assigning " << LocalMemoryNames[index_value] << " (value = " << _cell->local_memory[cm.indexes[PARAMETER_c]] << ") to mm_PP_loc rotational offset_angle. ";
					}
				}
				if (cm.isLiteralValue[PARAMETER_c]) {
					_cell->local_vector_memory[index_value].offset_angle = cm.literal_values[PARAMETER_c];
					if (dbFlag) {
						debugMessage << " Assigning " << cm.literal_values[PARAMETER_c] << " to offset_angle. ";
					}
				}

				if (cm.isLocalMemoryVariable[PARAMETER_b]) {
					_cell->local_vector_memory[index_value].translation_amount = _cell->local_memory[cm.indexes[PARAMETER_b]];
					if (dbFlag) {
						debugMessage << " Assigning " << LocalMemoryNames[index_value] << " (value = " << _cell->local_memory[cm.indexes[PARAMETER_b]] << ") to mod_PP_loc translation amount. ";
					}
				}

				if (cm.isLiteralValue[PARAMETER_b]) {
					_cell->local_vector_memory[index_value].translation_amount = cm.literal_values[PARAMETER_b];
					if (dbFlag) {
						debugMessage << " Assigning " << cm.literal_values[PARAMETER_b] << " to mod_PP_loc translation amount. ";
					}
				}

			}


			if (cm.components[TAG][0] == MIGRATE) {

				index_value = cm.indexes[PARAMETER_a];
				if (dbFlag) {
					debugMessage << "  Assigning V_VAR " << VectorVariableNames[index_value] << " to MIGRATE. ";
				}
				if (cm.isLocalMemoryVariable[PARAMETER_c]) {
					_cell->local_vector_memory[index_value].offset_angle = _cell->local_memory[cm.indexes[PARAMETER_c]];
					if (dbFlag) {
						debugMessage << " Assigning " << LocalMemoryNames[index_value] << " (value = " << _cell->local_memory[cm.indexes[PARAMETER_c]] << ") to offset_angle. ";
					}
				}
				if (cm.isLiteralValue[PARAMETER_c]) {
					_cell->local_vector_memory[index_value].offset_angle = cm.literal_values[PARAMETER_c];
					if (dbFlag) {
						debugMessage << " Assigning " << cm.literal_values[PARAMETER_c] << " to offset_angle. ";
					}
				}

				if (cm.isLocalMemoryVariable[PARAMETER_b]) {
					_cell->local_vector_memory[index_value].force = _cell->local_memory[cm.indexes[PARAMETER_b]];
					if (dbFlag) {
						debugMessage << " Assigning " << LocalMemoryNames[index_value] << " (value = " << _cell->local_memory[cm.indexes[PARAMETER_b]] << ") to Force. ";
					}
				}

				if (cm.isLiteralValue[PARAMETER_b]) {
					_cell->local_vector_memory[index_value].force = cm.literal_values[PARAMETER_b];
					if (dbFlag) {
						debugMessage << " Assigning " << cm.literal_values[PARAMETER_b] << " to Force. ";
					}
				}

			}







			if (cm.components[TAG][0] == MATH) {

				if (cm.isVectorVariable[PARAMETER_a]) {
					if (cm.components[PARAMETER_b][0] == "+=") { // the initial syntax check ensures that PARAMETER_c is also a VectorVariable


						if (dbFlag) {
							debugMessage << "  Adding " << VectorVariableNames[cm.indexes[PARAMETER_c]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector.y << ") to " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").";
						}
						_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector += _cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector;

						if (dbFlag) {
							debugMessage << VectorVariableNames[cm.indexes[PARAMETER_a]] << " is now equal to (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n";
						}
					}

					if (cm.components[PARAMETER_b][0] == "-=") { // the initial syntax check ensures that PARAMETER_c is also a VectorVariable
						if (dbFlag) {
							debugMessage << "  Subtracting " << VectorVariableNames[cm.indexes[PARAMETER_c]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector.y << ") from " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").";
						}
						_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector -= _cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector;

						if (dbFlag) {
							debugMessage << VectorVariableNames[cm.indexes[PARAMETER_a]] << " is now equal to (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n";
						}
					}
					if (cm.components[PARAMETER_b][0] == "=") { // the initial syntax check ensures that PARAMETER_c is also a VectorVariable
						if (dbFlag) {
							debugMessage << "  Setting " << VectorVariableNames[cm.indexes[PARAMETER_c]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector.y << ") equal to " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n";
						}

						_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector = _cell->local_vector_memory[cm.indexes[PARAMETER_c]].dir_vector;
					}
					if (cm.components[PARAMETER_b][0] == "*=") { // the initial syntax check ensures that PARAMETER_c is NOT a VectorVariable
						c = 1;
						if (cm.isLocalMemoryVariable[PARAMETER_c]) {
							if (dbFlag) {
								debugMessage << "  Multiplying " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
									_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ") by " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_memory[cm.indexes[PARAMETER_a]] <<
									"). ";
							}
							c = _cell->local_memory[cm.indexes[PARAMETER_c]];
							_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector *= c;
							if (dbFlag) {
								debugMessage << VectorVariableNames[cm.indexes[PARAMETER_a]] << " now equal to  (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
									_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n ";
							}
						}
						if (cm.isLiteralValue[PARAMETER_c]) {

							c = cm.literal_values[PARAMETER_c];
							if (dbFlag) {
								debugMessage << "  Multiplying " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
									_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ") by " << c << ". ";
							}

							_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector *= c;
							if (dbFlag) {
								debugMessage << VectorVariableNames[cm.indexes[PARAMETER_a]] << " now equal to  (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
									_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n ";
							}
						}


					}


					if (cm.components[PARAMETER_b][0] == "/=") { // the initial syntax check ensures that PARAMETER_c is NOT a VectorVariable
						c = 1;
						if (cm.isLocalMemoryVariable[PARAMETER_c]) {

							c = _cell->local_memory[cm.indexes[PARAMETER_c]];

							if (c == 0) {
								if (dbFlag) {
									debugMessage << LocalMemoryNames[cm.indexes[PARAMETER_c]] << " equals 0, resulting in division by 0. Setting " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " to (0,0).\n ";
								}
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector = sf::Vector2f(0, 0);

							}
							else {

								if (dbFlag) {
									debugMessage << "  Dividing " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
										_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ") by " << LocalMemoryNames[cm.indexes[PARAMETER_c]] << " (" << c <<
										"). ";
								}

								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector /= c;
								if (dbFlag) {
									debugMessage << VectorVariableNames[cm.indexes[PARAMETER_a]] << " now equal to  (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
										_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n ";
								}
							}
						}
						if (cm.isLiteralValue[PARAMETER_c]) {
							c = cm.literal_values[PARAMETER_c];

							if (c == 0) {
								if (dbFlag) {
									debugMessage << "Division by 0. Setting " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " to (0,0).\n ";
								}
								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector = sf::Vector2f(0, 0);
							}
							else {
								if (dbFlag) {
									debugMessage << "  Dividing " << VectorVariableNames[cm.indexes[PARAMETER_a]] << " (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
										_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ") by " << c << ". ";
								}

								_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector /= c;
								if (dbFlag) {
									debugMessage << VectorVariableNames[cm.indexes[PARAMETER_a]] << " now equal to  (" << _cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.x << "," <<
										_cell->local_vector_memory[cm.indexes[PARAMETER_a]].dir_vector.y << ").\n ";
								}
							}

						}
					}

				}
				else {


				if (cm.indexes[PARAMETER_c] == -1) { c = cm.literal_values[PARAMETER_c]; }
				else { c = _cell->local_memory[cm.indexes[PARAMETER_c]]; }


		

					if (cm.components[PARAMETER_b][0] == "+=") {
						_cell->local_memory[cm.indexes[PARAMETER_a]] += c;
						if (dbFlag) {
							debugMessage << "  Adding " << c << " to " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << ", which is now equal to: " << _cell->local_memory[cm.indexes[PARAMETER_a]] << ".\n";
						}
					}
					if (cm.components[PARAMETER_b][0] == "-=") {
						_cell->local_memory[cm.indexes[PARAMETER_a]] -= c;
						if (dbFlag) {
							debugMessage << "  Subtracting " << c << " from " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << ", which is now equal to: " << _cell->local_memory[cm.indexes[PARAMETER_a]] << ".\n";
						}
					}
					if (cm.components[PARAMETER_b][0] == "*=") {
						_cell->local_memory[cm.indexes[PARAMETER_a]] *= c;
						if (dbFlag) {
							debugMessage << "  Multiplying " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " by " << c << ". Now equal to: " << _cell->local_memory[cm.indexes[PARAMETER_a]] << ".\n";
						}
					}
					if (cm.components[PARAMETER_b][0] == "/=") {
						if (c != 0) {
							_cell->local_memory[cm.indexes[PARAMETER_a]] /= c;
							if (dbFlag) {
								debugMessage << "  Dividing " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " by " << c << ". Now equal to: " << _cell->local_memory[cm.indexes[PARAMETER_a]] << ".\n";
							}
						}
						else {
							if (dbFlag) {
								debugMessage << "  Dividing " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " by " << c << ". Division by zero. Setting variable to 0.\n";
							}
							_cell->local_memory[cm.indexes[PARAMETER_a]] = 0;
						}
					}
					if (cm.components[PARAMETER_b][0] == "=") {
						_cell->local_memory[cm.indexes[PARAMETER_a]] = c;
						if (dbFlag) {
							debugMessage << "  Setting " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " to " << c << ".\n";
						}
					}
					if (_cell->local_memory[cm.indexes[PARAMETER_a]] < 0) {
						_cell->local_memory[cm.indexes[PARAMETER_a]] = 0;
						if (dbFlag) {
							debugMessage << "  *** " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " became negative and is now set to 0.\n";
						}
					}
				}
			
			//clip all the colors in case they've been changed.
			if (_cell->local_memory[Red_Index] < 0) {
				_cell->local_memory[Red_Index] = 0;
			}
			if (_cell->local_memory[Red_Index] > 255) {
				_cell->local_memory[Red_Index] = 255;
			}
			if (_cell->local_memory[Green_Index] < 0) {
				_cell->local_memory[Green_Index] = 0;
			}
			if (_cell->local_memory[Green_Index] > 255) {
				_cell->local_memory[Green_Index] = 255;
			}
			if (_cell->local_memory[Blue_Index] < 0) {
				_cell->local_memory[Blue_Index] = 0;
			}
			if (_cell->local_memory[Blue_Index] > 255) {
				_cell->local_memory[Blue_Index] = 255;
			}

		}




		if (cm.components[TAG][0] == SET_VEC) {
			index_value = cm.indexes[CURLY_BRACKET_PARAMETER];
			sf::Vector2f holder;

			if ((cm.isLiteralValue[PARAMETER_a]) && (cm.isLiteralValue[PARAMETER_b])) {
				holder.x = cm.literal_values[PARAMETER_a];
				holder.y = cm.literal_values[PARAMETER_b];
				if (dbFlag) {
					debugMessage << "   Assigning (" << holder.x << ", " << holder.y << ") (manual input) to V_VAR " << VectorVariableNames[index_value] << ".\n";
				}
			}

			else if ((cm.isFieldReference[PARAMETER_a])) {
				a = cm.literal_values[PARAMETER_a];
				holder = _cell->parent_petri->getVectorTowardsField(_cell, (int)a);
				if (dbFlag) {
					debugMessage << "   Assigning (" << holder.x << ", " << holder.y << ") (pointing towards Field Zone No. " << a << ") to V_VAR " << VectorVariableNames[index_value] << ".\n";
				}
			}

			else if (cm.isStateName[PARAMETER_a]) {

				holder = _cell->parent_petri->getVectorTowardsNeighborStates(_cell, cm.indexes[PARAMETER_a]);
				if (dbFlag) {
					debugMessage << "   Assigning (" << holder.x << ", " << holder.y << ") (pointing towards Cell State " << StateNames[cm.indexes[PARAMETER_a]] << ") to V_VAR " << VectorVariableNames[index_value] << ".\n";
				}
			}
			else if (cm.isGradient[PARAMETER_a]) {

				holder = _cell->parent_petri->getVectorTowardsGradient(_cell, cm.indexes[PARAMETER_a]);
				if (dbFlag) {
					debugMessage << "   Assigning (" << holder.x << ", " << holder.y << ") (pointing towards G_VAR " << GradientVariableNames[cm.indexes[PARAMETER_a]] << ") to V_VAR " << VectorVariableNames[index_value] << ".\n";
				}
			}
			else if (cm.isLocalMemoryVariable[PARAMETER_a]) {
				holder = _cell->parent_petri->getVectorTowardsUserVariables(_cell, cm.indexes[PARAMETER_a]);
				if (dbFlag) {
					debugMessage << "   Assigning (" << holder.x << ", " << holder.y << ") (pointing towards U_VAR  " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << ") to V_VAR " << VectorVariableNames[index_value] << ".\n";
				}
			}

			_cell->local_vector_memory[index_value].dir_vector = holder;

		}



		if (cm.components[TAG][0] == FIELD_ACTIVE) {
			if (cm.literal_values[PARAMETER_a] == 0) {
				_cell->local_memory[Field_Active_Flag_Index] = 0;

				if (dbFlag) {
					debugMessage << "  Setting FIELD to INACTIVE. " << "\n";
				}
			}
			else {
				_cell->local_memory[Field_Active_Flag_Index] = 1;

				if (dbFlag) {
					debugMessage << "  Setting FIELD to ACTIVE. " << "\n";
				}
			}
		}




		if (cm.components[TAG][0] == RGB) {
			_cell->set_haveRulesProcessedRGB(1);
			if (cm.isLiteralValue[PARAMETER_a]) { a = cm.literal_values[PARAMETER_a]; }
			else { a = _cell->local_memory[cm.indexes[PARAMETER_a]]; }
			if (cm.isLiteralValue[PARAMETER_b]) { b = cm.literal_values[PARAMETER_b]; }
			else { b = _cell->local_memory[cm.indexes[PARAMETER_b]]; }
			if (cm.isLiteralValue[PARAMETER_c]) { c = cm.literal_values[PARAMETER_c]; }
			else { c = _cell->local_memory[cm.indexes[PARAMETER_c]]; }

			if (a < 0) { a = 0; }
			if (a > 255) { a = 255; }
			if (b < 0) { b = 0; }
			if (b > 255) { b = 255; }
			if (c < 0) { c = 0; }
			if (c > 255) { c = 255; }

			_cell->local_memory[Red_Index] = a;
			_cell->local_memory[Green_Index] = b;
			_cell->local_memory[Blue_Index] = c;
			if (dbFlag) {
				debugMessage << "  set RGB to (" << a << ", " << b << ", " << c << ")\n";
			}
		}


		if (cm.components[TAG][0] == SET_RAND) {
			index_value = cm.indexes[CURLY_BRACKET_PARAMETER];
			if (cm.isLiteralValue[PARAMETER_a]) { a = cm.literal_values[PARAMETER_a]; }
			else { a = _cell->local_memory[cm.indexes[PARAMETER_a]]; }
			if (cm.isLiteralValue[PARAMETER_b]) { b = cm.literal_values[PARAMETER_b]; }
			else { b = _cell->local_memory[cm.indexes[PARAMETER_b]]; }
			float r_v = random_float_range(a, b);

			_cell->local_memory[index_value] = r_v;
			if (dbFlag) {
				debugMessage << "  set " << LocalMemoryNames[index_value] << " to random value between " << a << " and " << b << " (random value = " << r_v << ")\n";
			}
		}



		if (cm.components[TAG][0] == CHANGE_STATE) {
			index_value = cm.indexes[CURLY_BRACKET_PARAMETER];
			_cell->local_memory[State_to_Change_Into_Index] = index_value;
			if (dbFlag) {
				debugMessage << "  Changed State to " << StateNames[index_value] << ". RETURN\n";
			}
			return;
		}

		if (cm.components[TAG][0] == MITOSIS) {
			_cell->local_memory[Mitosis_Flag_Index] = true;
			if (dbFlag) {
				debugMessage << "Set Mitosis Flag. ";
			}

			if (cm.components[CURLY_BRACKET_PARAMETER][0] == EMPTY) {
				_cell->local_memory[Mitosis_State_to_Change_Into_Index] = _cell->local_memory[Current_State_Index];
				if (dbFlag) {
					debugMessage << "By default, Daughter will be same state as Mother. ";
				}
			}
			else {
				_cell->local_memory[Mitosis_State_to_Change_Into_Index] = (float)cm.indexes[CURLY_BRACKET_PARAMETER];
				if (dbFlag) {
					debugMessage << "Daughter will be State " << StateNames[cm.indexes[CURLY_BRACKET_PARAMETER]] << ". ";
				}
			}

			if (cm.components[PARAMETER_a][0] == EMPTY) {
				_cell->index_of_V_VAR_to_be_used_for_mitosis = -1;
				if (dbFlag) {
					debugMessage << "A random cleavage vector will be used. ";
				}
			}

			else {
				_cell->index_of_V_VAR_to_be_used_for_mitosis = cm.indexes[PARAMETER_a];
				if (dbFlag) {
					debugMessage << "Setting cleavage vector to V_VAR " << VectorVariableNames[cm.indexes[PARAMETER_a]] << ".\n";
				}


				if (cm.components[PARAMETER_b][0] == EMPTY) {
					_cell->local_vector_memory[cm.indexes[PARAMETER_a]].offset_angle = 0;
					if (dbFlag) {
						debugMessage << "No offset angle. ";
					}
				}

				if (cm.isLiteralValue[PARAMETER_b]) {
					_cell->local_vector_memory[cm.indexes[PARAMETER_a]].offset_angle = cm.literal_values[PARAMETER_b];
					if (dbFlag) {
						debugMessage << "Cleavage offset set to literal value of " << cm.literal_values[PARAMETER_b] << ". ";
					}
				}
				if (cm.isLocalMemoryVariable[PARAMETER_b]) {
					_cell->local_vector_memory[cm.indexes[PARAMETER_a]].offset_angle = _cell->local_memory[cm.indexes[PARAMETER_b]];
					if (dbFlag) {
						debugMessage << "Cleavage offset set to " << LocalMemoryNames[cm.indexes[PARAMETER_a]] << " (value = " << _cell->local_memory[cm.indexes[PARAMETER_b]] << "). ";
					}
				}


				if (cm.components[PARAMETER_c][0] == EMPTY) {
					if (dbFlag) {
						debugMessage << "Daughter position set randomly. ";
					}
					_cell->daughter_mother_position_determinor = rand() % 2;
				}
				else {


					if (cm.components[PARAMETER_c][0] == "CW") {
						_cell->daughter_mother_position_determinor = 0;
						if (dbFlag) {
							debugMessage << "Daughter cell will be clockwise position. ";
						}
					}
					if (cm.components[PARAMETER_c][0] == "CCW") {
						_cell->daughter_mother_position_determinor = 1;
						if (dbFlag) {
							debugMessage << "Daughter cell will be counter-clockwise position. ";
						}
					}
				}
			}
			if (dbFlag) {
				debugMessage << " END. RETURN\n";
			}
			return;
		}



		if (cm.components[TAG][0] == DIE) {
			_cell->local_memory[Die_Flag_Index] = true;
			if (dbFlag) {
				debugMessage << "  Set DIE flag. END.  RETURN\n";
			}
			return;
		}

		if (cm.components[TAG][0] == END) {
			if (dbFlag) {
				debugMessage << "  END. RETURN\n";
			}
			return;
		}


	}

}
}
vector<string> RuleParser::extractVectorVariablesFromRawRules(stringstream& message, int& line_number)
{
	vector <string> result;
	result.clear();


	//Instead of hardcoding in 8 as the size of "VARIABLE", we make it variable so the signified "VARIABLE"
	//may be easily changed globaly if the need arises.
	int variable_word_size = string(VECTOR_VARIABLE).size();
	//*********************************************


	string holder = "";
	string holder2;
	string line_holder;
	for (size_t t = 0; t < rawRules.size(); t++) {
		//if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (line_holder.substr(0, variable_word_size) == VECTOR_VARIABLE) {
			holder = removeLeadingAndTrailingWhiteSpaceFromString(line_holder.substr(variable_word_size));
			if (isStringInStringVector(holder, forbidden_user_variable_names)) {
				message << "Vector Variables may not be named: " << holder << "\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, StateNames)) {
				message << "The Vector Variable name, " << holder << ", is already declared as a State name.\n"; line_number = t; return result;
			}
			holder2 = removeWhiteSpaceFromString(holder);
			if (holder2.size() != holder.size()) {
				message << "Vector Variable names may not contain whitepsace.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, result)) {
				message << "The Vector Variable name, " << holder << ", is already declared.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, GradientVariableNames)) {
				message << "The Vector Variable name, " << holder << ", is already declared as a GRADIENT_VARIABLE.\n"; line_number = t; return result;
			}
			for (size_t x = 0; x < holder.size(); x++) {
				if (isStringInStringVector(holder.substr(x, 1), forbidden_label_characters)) {
					message << "Vector Variable names may not contain the character '" << holder.substr(x, 1) << "'\n"; line_number = t; return result;
				}
			}
			result.push_back(holder);
		}
	}

	return result;
}
vector<string> RuleParser::extractUserVariablesFromRawRules(stringstream& message, int& line_number)
{
	vector <string> result;
	result.clear();


	//Instead of hardcoding in 8 as the size of "VARIABLE", we make it variable so the signified "VARIABLE"
	//may be easily changed globaly if the need arises.
	int variable_word_size = string(VARIABLE).size();
	//*********************************************


	string holder = "";
	string holder2;
	string line_holder;
	for (size_t t = 0; t < rawRules.size(); t++) {
		//if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (line_holder.substr(0, variable_word_size) == VARIABLE) {
			holder = removeLeadingAndTrailingWhiteSpaceFromString(line_holder.substr(variable_word_size));
			if (isStringInStringVector(holder, forbidden_user_variable_names)) {
				message << "Variables may not be named: " << holder << "\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, StateNames)) {
				message << "The Variable name, " << holder << ", is already declared as a State name.\n"; line_number = t; return result;
			}
			holder2 = removeWhiteSpaceFromString(holder);
			if (holder2.size() != holder.size()) {
				message << "Variable names may not contain whitepsace.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, result)) {
				message << "The Variable name, " << holder << ", is already declared.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, GradientVariableNames)) {
				message << "The Variable name, " << holder << ", is already declared as a GRADIENT_VARIABLE.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, completeVectorVariableNames)) {
				message << "The Variable name, " << holder << ", is already declared as a Vector_VARIABLE.\n"; line_number = t; return result;
			}
			for (size_t x = 0; x < holder.size(); x++) {
				if (isStringInStringVector(holder.substr(x, 1), forbidden_label_characters)) {
					message << "Variable names may not contain the character '" << holder.substr(x, 1) << "'\n"; line_number = t; return result;
				}
			}
			result.push_back(holder);
		}
	}

	return result;
}

vector<string> RuleParser::extractGradientVariablesFromRawRules(stringstream& message, int& line_number)
{
	vector <string> result;
	result.clear();


	//Instead of hardcoding size of "GRADIENT_VARIABLE", we make it variable so the signified "GRADIENT_VARIABLE"
	//may be easily changed globaly if the need arises.
	int variable_word_size = string(GRADIENT_VARIABLE).size();
	//*********************************************


	string holder = "";
	string holder2;
	string line_holder;
	for (size_t t = 0; t < rawRules.size(); t++) {
		//if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (line_holder.substr(0, variable_word_size) == GRADIENT_VARIABLE) {
			holder = removeLeadingAndTrailingWhiteSpaceFromString(line_holder.substr(variable_word_size));
			if (isStringInStringVector(holder, forbidden_user_variable_names)) {
				message << "GRADIENT_VARIABLE may not be named: " << holder << "\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, StateNames)) {
				message << "The GRADIENT_VARIABLE name, " << holder << ", is already declared as a State name.\n"; line_number = t; return result;
			}
			holder2 = removeWhiteSpaceFromString(holder);
			if (holder2.size() != holder.size()) {
				message << "GRADIENT_VARIABLE names may not contain whitepsace.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, result)) {
				message << "The GRADIENT_VARIABLE name, " << holder << ", is already declared.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, completeUserVariableNames)) {
				message << "The GRADIENT_VARIABLE name, " << holder << ", is already declared as a Variable.\n"; line_number = t; return result;
			}
			for (size_t x = 0; x < holder.size(); x++) {
				if (isStringInStringVector(holder.substr(x, 1), forbidden_label_characters)) {
					message << "GRADIENT_VARIABLE names may not contain the character '" << holder.substr(x, 1) << "'\n"; line_number = t; return result;
				}
			}
			result.push_back(holder);
		}
	}

	return result;
}

vector<string> RuleParser::extractStateNamesFromRawRules(stringstream& message, int& line_number)
{
	vector <string> result;
	result.clear();


	//Instead of hardcoding in 5 as the size of "STATE", we make it variable so the signified "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************


	string holder = "";
	string holder2;
	string line_holder;
	for (size_t t = 0; t < rawRules.size(); t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (line_holder.substr(0, state_word_size) == STATE) {
			holder = removeLeadingAndTrailingWhiteSpaceFromString(line_holder.substr(state_word_size));
			if (isStringInStringVector(holder, forbidden_state_names)) {
				message << "States may not be named " << holder << "\n"; line_number = t; return result;
			}
			holder2 = removeWhiteSpaceFromString(holder);
			if (holder2.size() != holder.size()) {
				message << "State names may not contain whitepsace.\n"; line_number = t; return result;
			}
			if (isStringInStringVector(holder, result)) {
				message << "The State name, " << holder << ", is already declared.\n"; line_number = t; return result;
			}
			for (size_t x = 0; x < holder.size(); x++) {
				if (isStringInStringVector(holder.substr(x, 1), forbidden_label_characters)) {
					message << "State names may not contain the character '" << holder.substr(x, 1) << "'\n"; line_number = t; return result;
				}
			}


			result.push_back(holder);
		}
	}
	if (result.size() == 0) {
		message << "No States have been declared.\n"; line_number = -1; return result;
	}
	return result;
}

void RuleParser::isFirstLineStateOrAdhesionOrGJ(stringstream& message, int& line_number) {
	//Instead of hardcoding in 5 as the size of "STATE", we make it variable so the signifier "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************

	//Instead of hardcoding in 9 as the size of "ADHESIONS", we make it variable so the signified "ADHESIONS"
	//may be easily changed globaly if the need arises.
	int adhesions_word_size = string(ADHESIONS).size();
	//*********************************************
	//Instead of hardcoding in 9 as the size of "GAP_JUNCTIONS", we make it variable so the signified "GAP_JUNCTIONS"
	//may be easily changed globaly if the need arises.
	int gj_word_size = string(GAP_JUNCTIONS).size();
	//*********************************************

	string line_holder;
	for (size_t t = 0; t < rawRules.size(); t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if ((line_holder.substr(0, gj_word_size) != GAP_JUNCTIONS) && (line_holder.substr(0, state_word_size) != STATE) && (line_holder.substr(0, adhesions_word_size) != ADHESIONS)) {
			message << "The File must begin with a STATE or ADHESIONS or GAP_JUNCTIONS section.\n"; line_number = t; return;
		}
		else {
			return;
		}
	}
	message << "The File must begin with a STATE or ADHESIONS or GAP_JUNCTIONS section.\n"; line_number = -1; return;
}

pair<int, int> RuleParser::getStateRulesSection(stringstream& message, int& line_number)
{
	pair<int, int> result;


	//Instead of hardcoding in 5 as the size of "STATE", we make it variable so the signifier "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************

	//Instead of hardcoding in 9 as the size of "ADHESIONS", we make it variable so the signified "ADHESIONS"
	//may be easily changed globaly if the need arises.
	int adhesions_word_size;
	string adhesions_word = ADHESIONS;
	adhesions_word_size = adhesions_word.size();
	//*********************************************

	//Instead of hardcoding in 9 as the size of "GAP_JUNCTIONS", we make it variable so the signified "GAP_JUNCTIONS"
	//may be easily changed globaly if the need arises.
	int gj_word_size = string(GAP_JUNCTIONS).size();
	//*********************************************

	string line_holder;
	bool found_start_of_section = 0;
	for (size_t t = 0; t < rawRules.size(); t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (!found_start_of_section) {
			if (line_holder.substr(0, state_word_size) == STATE) {
				result.first = t;
				found_start_of_section = 1;
				continue;
			}
		}
		if (found_start_of_section) {
			if (line_holder.substr(0, adhesions_word_size) == ADHESIONS) {
				result.second = t - 1;
				break;
			}
		}

		if (found_start_of_section) {
			if (line_holder.substr(0, gj_word_size) == GAP_JUNCTIONS) {
				result.second = t - 1;
				break;
			}
		}
		result.second = t;
	}

	if (!found_start_of_section) {
		message << "No STATE section was found.\n"; line_number = -1; return result;
	}

	//quick check to make sure there isn't ANOTHER state section after the one 
	//picked up. i.e. a 'State' section, then an 'Adhesion' section, and then
	//ANOTHER 'State' section.

	if (result.second < (int)(rawRules.size() - 1)) {

		for (size_t t = result.second; t < rawRules.size(); t++) {
			if (isSkipLine(rawRules[t])) { continue; }
			line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
			if (line_holder.substr(0, state_word_size) == STATE) {
				message << "There can be only one STATE section.\n"; line_number = t; return result;
			}
		}
	}

	return result;


}

pair<int, int> RuleParser::getAdhesionsSection(stringstream& message, int& line_number)
{
	pair<int, int> result;


	//Instead of hardcoding in 5 as the size of "STATE", we make it variable so the signifier "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************

	//Instead of hardcoding in 9 as the size of "ADHESIONS", we make it variable so the signified "ADHESIONS"
	//may be easily changed globaly if the need arises.
	int adhesions_word_size = string(ADHESIONS).size();
	//*********************************************


	//Instead of hardcoding in 9 as the size of "GAP_JUNCTIONS", we make it variable so the signified "GAP_JUNCTIONS"
	//may be easily changed globaly if the need arises.
	int gj_word_size = string(GAP_JUNCTIONS).size();
	//*********************************************


	string line_holder;
	bool found_start_of_section = 0;
	for (size_t t = 0; t < rawRules.size(); t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (!found_start_of_section) {
			if (line_holder.substr(0, adhesions_word_size) == ADHESIONS) {
				result.first = t;
				found_start_of_section = 1;
				continue;
			}
		}
		if (found_start_of_section) {
			if (line_holder.substr(0, state_word_size) == STATE) {
				result.second = t - 1;
				break;
			}
			if (line_holder.substr(0, gj_word_size) == GAP_JUNCTIONS) {
				result.second = t - 1;
				break;
			}
		}
		result.second = t;
	}

	if (!found_start_of_section) {
		result.first = -1;
		result.second = -1;
		return result;
	}

	//quick check to make sure there isn't ANOTHER adhesion section after the one 
	//picked up. i.e. a 'Adhesion' section, then a 'State' section, and then
	//ANOTHER 'Adhesion' section.

	if (result.second < (int)(rawRules.size() - 1)) {

		for (size_t t = result.second; t < rawRules.size(); t++) {
			if (isSkipLine(rawRules[t])) { continue; }
			line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
			if (line_holder.substr(0, adhesions_word_size) == ADHESIONS) {
				message << "There can be only one ADHESIONS Section.\n"; line_number = t; return result;
			}
		}
	}

	return result;


}
pair<int, int> RuleParser::getGJSection(stringstream& message, int& line_number)
{
	pair<int, int> result;


	//Instead of hardcoding in 5 as the size of "STATE", we make it variable so the signifier "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************

	//Instead of hardcoding in 9 as the size of "ADHESIONS", we make it variable so the signified "ADHESIONS"
	//may be easily changed globaly if the need arises.
	int adhesions_word_size = string(ADHESIONS).size();
	//*********************************************

	//Instead of hardcoding in 9 as the size of "GAP_JUNCTIONS", we make it variable so the signified "GAP_JUNCTIONS"
	//may be easily changed globaly if the need arises.
	int gj_word_size = string(GAP_JUNCTIONS).size();
	//*********************************************

	string line_holder;
	bool found_start_of_section = 0;
	for (size_t t = 0; t < rawRules.size(); t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		if (!found_start_of_section) {
			if (line_holder.substr(0, gj_word_size) == GAP_JUNCTIONS) {
				result.first = t;
				found_start_of_section = 1;
				continue;
			}
		}
		if (found_start_of_section) {
			if (line_holder.substr(0, state_word_size) == STATE) {
				result.second = t - 1;
				break;
			}
		}
		if (found_start_of_section) {
			if (line_holder.substr(0, adhesions_word_size) == ADHESIONS) {
				result.second = t - 1;
				break;
			}
		}

		result.second = t;
	}

	if (!found_start_of_section) {
		result.first = -1;
		result.second = -1;
		return result;
	}

	//quick check to make sure there isn't ANOTHER adhesion section after the one 
	//picked up. i.e. a 'GAP_JUNCTIONS' section, then a 'State' section, and then
	//ANOTHER 'GAP_JUNCTIONS' section.

	if (result.second < (int)(rawRules.size() - 1)) {

		for (size_t t = result.second; t < rawRules.size(); t++) {
			if (isSkipLine(rawRules[t])) { continue; }
			line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
			if (line_holder.substr(0, gj_word_size) == GAP_JUNCTIONS) {
				message << "There can be only one GAP_JUNCTIONS Section.\n"; line_number = t; return result;
			}
		}
	}

	return result;

}

void RuleParser::parseGJ(stringstream& message, int& line_number)
{
	//format: <AA,BB,CC>(1234) where AA and BB are state names, CC is a user variable name, and parameter is float
	string s;
	for (int t = GJ_section_line_number_range.first + 1; t < GJ_section_line_number_range.second + 1; t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		s = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		s = removeWhiteSpaceFromString(s);



		// preliminary error check

		//empty token
		if (s.size() == 0) {
			message << s << ": Defective Gap Junction command.\n"; line_number = t; return;
		}

		//too many brackets/parenthesis
		if (
			(findNumberOfOccurencesOfCharacterInString(s, "<") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, ">") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, ",") != 2) ||
			(findNumberOfOccurencesOfCharacterInString(s, "(") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, ")") != 1)) {
			message << s << ": Improper number of brackets/parenthesis/commas.\n"; line_number = t; return;
		}



		//out of order punctuation
		std::size_t found_1 = s.find("<");
		std::size_t found_2 = s.find(",");
		std::size_t found_3 = s.find(">");
		std::size_t found_4 = s.find("(");
		std::size_t found_5 = s.find(")");

		if (!(
			(found_1 < found_2) &&
			(found_2 < found_3) &&
			(found_3 < found_4) &&
			(found_4 < found_5)
			)) {
			message << s << ": Out of order brackets/parenthesis/commas.\n"; line_number = t; return;
		}

		string interior = s.substr(found_1 + 1, found_3 - 1);

		vector <string> interior_pair = tokenizeByCommas(interior);
		if (interior_pair.size() != 3) {
			message << s << ": Syntax error with '<...,...,....>' section.\n"; line_number = t; return;
		}

		if (
			(!isStringInStringVector(interior_pair[0], StateNames)) ||
			(!isStringInStringVector(interior_pair[1], StateNames)) ||
			(!isStringInStringVector(interior_pair[2], completeUserVariableNames))
			)
		{
			message << s << ": Gap Junction parameters must be STATE name, STATE name, UserVariable name.\n"; line_number = t; return;
		}

		string last_parameter = s.substr(found_4 + 1, found_5 - found_4 - 1);

		if (last_parameter.size() == 0) {
			message << s << ": Gap Junction value parameter is empty.\n"; line_number = t; return;
		}
		if (!isFloat(last_parameter)) {
			message << s << ": Gap Junction value parameter must be numeric.\n"; line_number = t; return;
		}
		float last_parameter_value = (float)atof(last_parameter.c_str());



		GJValues[SNIM[interior_pair[0]]][SNIM[interior_pair[1]]][LMIM[interior_pair[2]]] = last_parameter_value;
		GJValues[SNIM[interior_pair[1]]][SNIM[interior_pair[0]]][LMIM[interior_pair[2]]] = last_parameter_value;

	}
}





void RuleParser::parseAdhesionValues(stringstream& message, int& line_number)
{
	//format: <AA,BB>(1234) where AA and BB are state names and parameters are floats
	string s;
	for (int t = adhesion_section_line_number_range.first + 1; t < adhesion_section_line_number_range.second + 1; t++) {
		if (isSkipLine(rawRules[t])) { continue; }
		s = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[t]));
		s = removeWhiteSpaceFromString(s);



		// preliminary error check

		//empty token
		if (s.size() == 0) {
			message << s << ": Defective Adhesion command.\n"; line_number = t; return;
		}

		//too many brackets/parenthesis
		if (
			(findNumberOfOccurencesOfCharacterInString(s, "<") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, ">") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, ",") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, "(") != 1) ||
			(findNumberOfOccurencesOfCharacterInString(s, ")") != 1)) {
			message << s << ": Improper number of brackets/parenthesis/commas.\n"; line_number = t; return;
		}



		//out of order punctuation
		std::size_t found_1 = s.find("<");
		std::size_t found_2 = s.find(",");
		std::size_t found_3 = s.find(">");
		std::size_t found_4 = s.find("(");
		std::size_t found_5 = s.find(")");

		if (!(
			(found_1 < found_2) &&
			(found_2 < found_3) &&
			(found_3 < found_4) &&
			(found_4 < found_5)
			)) {
			message << s << ": Out of order brackets/parenthesis/commas.\n"; line_number = t; return;
		}

		string interior = s.substr(found_1 + 1, found_3 - 1);

		vector <string> interior_pair = tokenizeByCommas(interior);
		if (interior_pair.size() != 2) {
			message << s << ": Syntax error with '<...,...>' section.\n"; line_number = t; return;
		}

		if (
			((interior_pair[0] == SELF) && (!isStringInStringVector(interior_pair[1], StateNames))) ||
			((interior_pair[1] == SELF) && (!isStringInStringVector(interior_pair[0], StateNames))) ||
			((!isStringInStringVector(interior_pair[0], StateNames)) && (interior_pair[0] != SELF)) ||
			((!isStringInStringVector(interior_pair[1], StateNames)) && (interior_pair[1] != SELF)) ||
			((interior_pair[0] == SELF) && (interior_pair[1] == SELF))
			)
		{
			message << s << ": Adhesion parameters must both be STATE names, or one STATE name and the term 'SELF'.\n"; line_number = t; return;
		}

		string last_parameter = s.substr(found_4 + 1, found_5 - found_4 - 1);

		if (last_parameter.size() == 0) {
			message << s << ": Adhesion value parameter is empty.\n"; line_number = t; return;
		}
		if (!isFloat(last_parameter)) {
			message << s << ": Adhesion value parameter must be numeric.\n"; line_number = t; return;
		}
		float last_parameter_value = (float)atof(last_parameter.c_str());

		if (interior_pair[0] == SELF) {
			AdhesionSelfValues[SNIM[interior_pair[1]]] = last_parameter_value; continue;
		}
		if (interior_pair[1] == SELF) {
			AdhesionSelfValues[SNIM[interior_pair[0]]] = last_parameter_value; continue;
		}

		if ((interior_pair[0] != SELF) && (interior_pair[1] != SELF)) {
			AdhesionValues[SNIM[interior_pair[0]]][SNIM[interior_pair[1]]] = last_parameter_value;
			AdhesionValues[SNIM[interior_pair[1]]][SNIM[interior_pair[0]]] = last_parameter_value;
			continue;
		}
		message << s << ": Syntax error with Adhesion command.\n"; line_number = t; return;
	}


}



vector<pair<int, int>> RuleParser::splitStateRulesSectionIntoParagraphs(stringstream& message, int& line_number)
{
	vector <pair<int, int>> result;
	result.clear();


	//Instead of hardcoding in 5 as the size of "STATE", we make it variable so the signified "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************

	pair<int, int> pair_holder;
	string line_holder;
	bool started_first_paragraph = false;
	for (int x = state_section_line_number_range.first; x < state_section_line_number_range.second + 1; x++) {
		line_holder = rawRules[x];
		if (isSkipLine(rawRules[x])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[x]));
		if ((line_holder.substr(0, state_word_size) == STATE)) {
			if (started_first_paragraph == false) {
				pair_holder.first = x;
				started_first_paragraph = true;
				if (x == state_section_line_number_range.second) {
					pair_holder.second = x;
					result.push_back(pair_holder);
					break;
				}
				continue;
			}
			if (started_first_paragraph == true) {
				pair_holder.second = x - 1;
				result.push_back(pair_holder);
				pair_holder.first = x;
			}
		}
		if (x == state_section_line_number_range.second) {
			pair_holder.second = x;
			result.push_back(pair_holder);
		}
	}
	return result;
}

void RuleParser::packageParagraphs(Paragraph& state_par, pair<int, int> single_paragraph_line_number_range, stringstream& message, int& line_number)

{
	string line_holder;
	string state_name_holder;
	Sentence sentence_holder;


	//Instead of hardcoding in 5 as the size of "STATE",
	//we make it variable so the signifier "STATE"
	//may be easily changed globaly if the need arises.
	int state_word_size = string(STATE).size();
	//*********************************************


	//Get State Name
	line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[single_paragraph_line_number_range.first]));


	if ((line_holder.substr(0, state_word_size) != STATE)) {
		message << "Program expected the declaration of a STATE at this line.\n"; line_number = single_paragraph_line_number_range.first; return;
	}
	state_name_holder = removeLeadingAndTrailingWhiteSpaceFromString(line_holder.substr(state_word_size));

	if (!isStringInStringVector(state_name_holder, StateNames)) {
		message << "Program expected to find '" << state_name_holder << "' in stateNameMap.\n"; line_number = single_paragraph_line_number_range.first; return;
	}

	state_par.state_name = state_name_holder;
	state_par.sentences.clear();

	packageSentences(state_par, single_paragraph_line_number_range, message, line_number);
	if (message.str() != "") { return; }


}

void RuleParser::packageSentences(Paragraph& par, pair<int, int> par_line_number_range, stringstream& message, int& line_number)
{
	Sentence sentence_holder;
	string line_holder;
	pair<string, string> split_sentence;
	vector <ConditionsAndCommands> conditions;
	vector <ConditionsAndCommands> commands;
	for (int x = par_line_number_range.first + 1; x < par_line_number_range.second + 1; x++) {
		line_holder = rawRules[x];
		if (isSkipLine(rawRules[x])) { continue; }
		line_holder = removeLeadingAndTrailingWhiteSpaceFromString(removeComments(rawRules[x]));
		split_sentence = splitSentenceByDoubleColon(line_holder);
		sentence_holder.commands.clear();
		sentence_holder.conditions.clear();

		split_sentence.first = removeLeadingAndTrailingWhiteSpaceFromString(split_sentence.first);
		split_sentence.second = removeLeadingAndTrailingWhiteSpaceFromString(split_sentence.second);

		if (split_sentence.first != "") {
			conditions.clear();
			packageTokens(split_sentence.first, conditions, message, x);
			if (message.str() != "") { line_number = x; return; }

			for (auto cc : conditions) {

				finalSyntaxCheckAndAssignmentOfIndexes(cc, conditionSyntaxTemplates, message, x);
				if (message.str() != "") { line_number = x;  return; }
				sentence_holder.conditions.push_back(cc);

			}

		}

		if (split_sentence.second != "") {

			commands.clear();
			packageTokens(split_sentence.second, commands, message, x);
			if (message.str() != "") { line_number = x; return; }


			for (auto cc : commands) {
				finalSyntaxCheckAndAssignmentOfIndexes(cc, commandSyntaxTemplates, message, x);
				if (message.str() != "") { line_number = x;  return; }
				sentence_holder.commands.push_back(cc);
			}

		}
		par.sentences.push_back(sentence_holder);
	}

}


void RuleParser::packageTokens(string tokens, vector <ConditionsAndCommands>& cc, stringstream& message, int& line_number)
{

	//	vector<string> token_holder = tokenizeByWhiteSpace(tokens);
	vector<string> token_holder = tokenizeBySemicolon(tokens);

	ConditionsAndCommands cc_holder;
	for (auto th : token_holder) {

		cc_holder.rawToken = th;
		cc_holder.components = splitTokenIntoComponents(th, message, line_number);
		if (message.str() != "") { return; }
		cc.push_back(cc_holder);
	}


}

vector< vector <string> > RuleParser::splitTokenIntoComponents(string s, stringstream& message, int& line_number)
{
	//create the appropriate size component holder
	vector < vector <string> > component_holder(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(1, ""));

	//probably redundant, but remove white space, comments, etc.
	s = removeLeadingAndTrailingWhiteSpaceFromString(removeWhiteSpaceFromString(removeComments(s)));

	if (s.substr(0, 1) == NOT) {
		component_holder[NOT_SIGNIFIER][0] = NOT;
		s = s.substr(1);
	}

	std::size_t found_1;
	std::size_t found_2;
	std::size_t found_3;
	std::size_t found_4;
	// preliminary error check

	//empty token
	if (s.size() == 0) {
		message << s << ": Defective token.\n"; return component_holder;
	}


	//quick detour to handle math operations
	found_1 = s.find("+=");
	if (found_1 != std::string::npos) {
		component_holder[TAG][0] = MATH;
		component_holder[LEFT_BRACKET][0] = "[";
		component_holder[RIGHT_BRACKET][0] = "]";
		component_holder[PARAMETER_a][0] = s.substr(0, found_1);
		component_holder[PARAMETER_b][0] = "+=";
		component_holder[PARAMETER_c][0] = s.substr(found_1 + 2);
		return component_holder;
	}
	found_1 = s.find("-=");
	if (found_1 != std::string::npos) {
		component_holder[TAG][0] = MATH;
		component_holder[LEFT_BRACKET][0] = "[";
		component_holder[RIGHT_BRACKET][0] = "]";
		component_holder[PARAMETER_a][0] = s.substr(0, found_1);
		component_holder[PARAMETER_b][0] = "-=";
		component_holder[PARAMETER_c][0] = s.substr(found_1 + 2);
		return component_holder;
	}
	found_1 = s.find("/=");
	if (found_1 != std::string::npos) {
		component_holder[TAG][0] = MATH;
		component_holder[LEFT_BRACKET][0] = "[";
		component_holder[RIGHT_BRACKET][0] = "]";
		component_holder[PARAMETER_a][0] = s.substr(0, found_1);
		component_holder[PARAMETER_b][0] = "/=";
		component_holder[PARAMETER_c][0] = s.substr(found_1 + 2);
		return component_holder;
	}
	found_1 = s.find("*=");
	if (found_1 != std::string::npos) {
		component_holder[TAG][0] = MATH;
		component_holder[LEFT_BRACKET][0] = "[";
		component_holder[RIGHT_BRACKET][0] = "]";
		component_holder[PARAMETER_a][0] = s.substr(0, found_1);
		component_holder[PARAMETER_b][0] = "*=";
		component_holder[PARAMETER_c][0] = s.substr(found_1 + 2);
		return component_holder;
	}
	found_1 = s.find("=");
	if (found_1 != std::string::npos) {
		component_holder[TAG][0] = MATH;
		component_holder[LEFT_BRACKET][0] = "[";
		component_holder[RIGHT_BRACKET][0] = "]";
		component_holder[PARAMETER_a][0] = s.substr(0, found_1);
		component_holder[PARAMETER_b][0] = "=";
		component_holder[PARAMETER_c][0] = s.substr(found_1 + 1);
		return component_holder;
	}

	//too many brackets/parenthesis
	if (
		(findNumberOfOccurencesOfCharacterInString(s, "{") > 1) ||
		(findNumberOfOccurencesOfCharacterInString(s, "}") > 1) ||
		(findNumberOfOccurencesOfCharacterInString(s, "[") > 1) ||
		(findNumberOfOccurencesOfCharacterInString(s, "]") > 1) ||
		(findNumberOfOccurencesOfCharacterInString(s, "(") > 1) ||
		(findNumberOfOccurencesOfCharacterInString(s, ")") > 1)) {
		message << s << ": Improper number of brackets/parenthesis.\n"; return component_holder;
	}

	//unmatched curlies
	found_1 = s.find("{");
	found_2 = s.find("}");
	if (((found_1 == std::string::npos) && (found_2 != std::string::npos)) ||
		(found_2 == std::string::npos) && (found_1 != std::string::npos)) {
		message << s << ": Extraneous '{' or '}' in token.\n"; return component_holder;
	}
	if ((found_1 != std::string::npos) && (found_2 != std::string::npos) && (found_1 > found_2)) {
		message << s << ": Mismatched '{' in token.\n"; return component_holder;
	}
	if ((found_1 != std::string::npos) && (found_2 != std::string::npos) && (found_2 - found_1 < 2)) {
		message << s << ": Empty '{' parameter.\n"; return component_holder;
	}

	//multiple left_bracket/parens
	found_1 = s.find("[");
	found_2 = s.find("(");
	if ((found_1 != std::string::npos) && (found_2 != std::string::npos)) {
		message << s << ": Extraneous '[' or '(' in token.\n"; return component_holder;
	}
	//multiple right_bracket/parens
	found_1 = s.find("]");
	found_2 = s.find(")");
	if ((found_1 != std::string::npos) && (found_2 != std::string::npos)) {
		message << s << ": Extraneous ']' or ')' in token.\n"; return component_holder;
	}

	//missing left
	found_1 = s.find("]");
	found_2 = s.find(")");
	found_3 = s.find("(");
	found_4 = s.find("[");
	if (((found_1 != std::string::npos) || (found_2 != std::string::npos)) &&
		((found_3 == std::string::npos) && (found_4 == std::string::npos)))
	{
		message << s << ": Missing '[' or '(' in token.\n"; return component_holder;
	}
	if (((found_3 != std::string::npos) || (found_4 != std::string::npos)) &&
		((found_1 == std::string::npos) && (found_2 == std::string::npos)))
	{
		message << s << ": Missing ']' or ')' in token.\n"; return component_holder;
	}

	//out of order curlies
	if ((s.find("{") != std::string::npos) && (s.find("}") != std::string::npos)) {

		if ((s.find("{") > s.find("(")) ||
			(s.find("{") > s.find("[")) ||
			(s.find("{") > s.find(")")) ||
			(s.find("{") > s.find("]")) ||
			(s.find("}") > s.find("(")) ||
			(s.find("}") > s.find("[")) ||
			(s.find("}") > s.find(")")) ||
			(s.find("}") > s.find("]"))) {
			message << s << ": Out of place brackets in token.\n"; return component_holder;
		}
	}

	//obtain tag
	found_1 = s.find("{");
	found_2 = s.find("[");
	found_3 = s.find("(");

	if ((found_1 == std::string::npos) &&
		(found_2 == std::string::npos) &&
		(found_3 == std::string::npos)) {


		component_holder[TAG][0] = s;
		return component_holder;
	}

	if (found_1 != std::string::npos) {
		component_holder[TAG][0] = s.substr(0, found_1);
	}
	else if (found_2 != std::string::npos) {
		component_holder[TAG][0] = s.substr(0, found_2);
	}
	else if (found_3 != std::string::npos) {
		component_holder[TAG][0] = s.substr(0, found_3);
	}


	//obtain bracket info
	found_1 = s.find("[");
	if (found_1 != std::string::npos) {
		component_holder[LEFT_BRACKET][0] = s.substr(found_1, 1);
	}
	found_1 = s.find("]");
	if (found_1 != std::string::npos) {
		component_holder[RIGHT_BRACKET][0] = s.substr(found_1, 1);
	}
	found_1 = s.find("(");
	if (found_1 != std::string::npos) {
		component_holder[LEFT_BRACKET][0] = s.substr(found_1, 1);
	}
	found_1 = s.find(")");
	if (found_1 != std::string::npos) {
		component_holder[RIGHT_BRACKET][0] = s.substr(found_1, 1);
	}


	//get curly_bracket
	found_1 = s.find("{");
	if (found_1 != std::string::npos) {
		found_2 = s.find("}");
		component_holder[CURLY_BRACKET_PARAMETER][0] = s.substr(found_1 + 1, found_2 - found_1 - 1);
	}

	//get bracket paramters
	string inside_brackets_temp = "";
	found_1 = s.find("(");
	if (found_1 != std::string::npos) {
		found_2 = s.find(")");
		if (found_2 != std::string::npos) {
			inside_brackets_temp = s.substr(found_1 + 1, found_2 - found_1 - 1);
		}
		else {
			found_2 = s.find("]");
			inside_brackets_temp = s.substr(found_1 + 1, found_2 - found_1 - 1);
		}
	}
	found_1 = s.find("[");
	if (found_1 != std::string::npos) {
		found_2 = s.find(")");
		if (found_2 != std::string::npos) {
			inside_brackets_temp = s.substr(found_1 + 1, found_2 - found_1 - 1);
		}
		else {
			found_2 = s.find("]");
			inside_brackets_temp = s.substr(found_1 + 1, found_2 - found_1 - 1);
		}
	}

	if (findNumberOfOccurencesOfCharacterInString(inside_brackets_temp, ",") > 2) {
		message << s << ": Too many commas inside brackets.\n"; return component_holder;
	}
	vector <string> inside_params = tokenizeByCommas(inside_brackets_temp);

	if (inside_params.size() > 0) {
		if (inside_params[0] == "") {
			message << s << ": Missing Value or Variable inside brackets.\n"; return component_holder;
		}
		component_holder[PARAMETER_a][0] = inside_params[0];
		if (inside_params.size() > 1) {
			if (inside_params[1] == "") {
				message << s << ": Missing Value or Variable inside brackets.\n"; return component_holder;
			}
			component_holder[PARAMETER_b][0] = inside_params[1];
			if (inside_params.size() > 2) {
				if (inside_params[2] == "") {
					message << s << ": Missing Value or Variable inside brackets.\n"; return component_holder;
				}
				component_holder[PARAMETER_c][0] = inside_params[2];
				if (inside_params.size() > 3) {
					message << s << ": Too many commas in token.\n"; return component_holder;
				}
			}
		}
	}

	return component_holder;
}

void RuleParser::finalSyntaxCheckAndAssignmentOfIndexes(ConditionsAndCommands& subject, vector <ConditionsAndCommands> syntax_templates, stringstream& message, int& line_number)
{

	//cout << subject.debugPrint() << "\n";

	bool passed = false;
	if (subject.components[NOT_SIGNIFIER][0] == NOT) {
		subject.isNot = true;
	}

	if ((subject.components[LEFT_BRACKET][0] == "[") && (subject.components[RIGHT_BRACKET][0] == "]")) {
		if (subject.components[PARAMETER_b][0] == "") {
			subject.comparison_flag = 4;
		}
		else {
			subject.comparison_flag = 0;
		}
	}
	if ((subject.components[LEFT_BRACKET][0] == "[") && (subject.components[RIGHT_BRACKET][0] == ")")) {
		subject.comparison_flag = 1;
	}
	if ((subject.components[LEFT_BRACKET][0] == "(") && (subject.components[RIGHT_BRACKET][0] == "]")) {
		subject.comparison_flag = 2;
	}
	if ((subject.components[LEFT_BRACKET][0] == "(") && (subject.components[RIGHT_BRACKET][0] == ")")) {
		subject.comparison_flag = 3;
	}

	//Tag Check
	passed = false;
	for (auto st : syntax_templates) {


		for (auto t : st.components[TAG]) {


			if (subject.components[TAG][0] == t) { passed = true;  break; }
			if (t == USER_DEFINED_VARIABLES) {
				if (isStringInStringVector(subject.components[TAG][0], completeUserVariableNames)) {
					subject.indexes[TAG] = LMIM[subject.components[TAG][0]]; subject.isLocalMemoryVariable[TAG] = true; passed = true; break;
				}
			}

			if (t == USER_DEFINED_GRADIENT_VARIABLES) {
				if (isStringInStringVector(subject.components[TAG][0], GradientVariableNames)) {
					subject.indexes[TAG] = GVIM[subject.components[TAG][0]]; subject.isGradient[TAG] = true; passed = true; break;
				}
			}
			if (t == RAM_VARIABLES) {
				if (isStringInStringVector(subject.components[TAG][0], reserved_RAM_variables)) {
					subject.indexes[TAG] = LMIM[subject.components[TAG][0]]; subject.isLocalMemoryVariable[TAG] = true;	passed = true; break;
				}
			}
			if (t == ROM_VARIABLES) {
				if (isStringInStringVector(subject.components[TAG][0], reserved_ROM_variables)) {
					subject.indexes[TAG] = LMIM[subject.components[TAG][0]]; subject.isLocalMemoryVariable[TAG] = true;	passed = true; break;
				}
			}
			if (t == STATE_NAMES) {
				if (isStringInStringVector(subject.components[TAG][0], StateNames)) {
					subject.indexes[TAG] = SNIM[subject.components[TAG][0]]; subject.isStateName[TAG];	passed = true; break;
				}
			}
			if (t == STATE_NAMES_PLUS_MEDIUM) {
				if ((isStringInStringVector(subject.components[TAG][0], StateNames)) || (subject.components[TAG][0] == MEDIUM)) {
					subject.indexes[TAG] = SNIM[subject.components[TAG][0]]; subject.isStateName[TAG];	passed = true; break;
				}
			}
			if (t == EMPTY) {
				if (subject.components[TAG][0] == "") { passed = true; break; }
			}


			if (t == ACTUAL_VALUE) {
				if (subject.components[TAG][0] == INF) {
					subject.literal_values[TAG] = std::numeric_limits<float>::max();
					subject.isLiteralValue[TAG] = 1;
					passed = true; break;
				}

				if (isFloat(subject.components[TAG][0])) {
					subject.literal_values[TAG] = std::stof(subject.components[TAG][0].c_str());
					subject.isLiteralValue[TAG] = 1;
					passed = true; break;
				}
			}
		}
		if (!passed) { continue; }


		//Curly Check
		passed = false;
		for (auto t : st.components[CURLY_BRACKET_PARAMETER]) {
			if (subject.components[CURLY_BRACKET_PARAMETER][0] == t) { passed = true;  break; }
			if (t == USER_DEFINED_VARIABLES) {
				if (isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], completeUserVariableNames)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = LMIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isLocalMemoryVariable[CURLY_BRACKET_PARAMETER] = 1; passed = true; break;
				}
			}

			if (t == USER_DEFINED_VECTOR_VARIABLES) {
				if (isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], VectorVariableNames)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = VVIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isVectorVariable[CURLY_BRACKET_PARAMETER] = 1; passed = true; break;
				}
			}

			if (t == USER_DEFINED_GRADIENT_VARIABLES) {
				if (isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], GradientVariableNames)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = GVIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isGradient[CURLY_BRACKET_PARAMETER] = true; passed = true; break;
				}
			}
			if (t == RAM_VARIABLES) {
				if (isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], reserved_RAM_variables)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = LMIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isLocalMemoryVariable[CURLY_BRACKET_PARAMETER] = true;	passed = true; break;
				}
			}
			if (t == ROM_VARIABLES) {
				if (isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], reserved_ROM_variables)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = LMIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isLocalMemoryVariable[CURLY_BRACKET_PARAMETER] = true;	passed = true; break;
				}
			}
			if (t == STATE_NAMES) {
				if (isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], StateNames)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = SNIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isStateName[CURLY_BRACKET_PARAMETER] = 1;	passed = true; break;
				}
			}
			if (t == STATE_NAMES_PLUS_MEDIUM) {
				if ((isStringInStringVector(subject.components[CURLY_BRACKET_PARAMETER][0], StateNames)) || (subject.components[CURLY_BRACKET_PARAMETER][0] == MEDIUM)) {
					subject.indexes[CURLY_BRACKET_PARAMETER] = SNIM[subject.components[CURLY_BRACKET_PARAMETER][0]]; subject.isStateName[CURLY_BRACKET_PARAMETER] = 1;	passed = true; break;
				}
			}
			if (t == EMPTY) {
				if (subject.components[CURLY_BRACKET_PARAMETER][0] == "") { passed = true; break; }
			}
			if (t == ACTUAL_VALUE) {
				if (subject.components[CURLY_BRACKET_PARAMETER][0] == INF) {
					subject.literal_values[CURLY_BRACKET_PARAMETER] = std::numeric_limits<float>::max();
					subject.isLiteralValue[CURLY_BRACKET_PARAMETER] = 1;
					passed = true; break;
				}

				if (isFloat(subject.components[CURLY_BRACKET_PARAMETER][0])) {
					subject.literal_values[CURLY_BRACKET_PARAMETER] = std::stof(subject.components[CURLY_BRACKET_PARAMETER][0].c_str());
					subject.isLiteralValue[CURLY_BRACKET_PARAMETER] = 1;
					passed = true; break;
				}
			}
		}
		if (!passed) { message << subject.rawToken << ": Syntax error with curly bracket parameter.\n";  return; }

		//NOT_SIGNIFIER check
		passed = false;
		for (auto t : st.components[NOT_SIGNIFIER]) {
			if (subject.components[NOT_SIGNIFIER][0] == t) { passed = true;  break; }
			if (t == EMPTY) {
				if (subject.components[NOT_SIGNIFIER][0] == "") { passed = true; break; }
			}
		}
		if (!passed) { message << subject.rawToken << ": Syntax error with Not Signifier.\n";  return; }

		//Left Bracket Check
		passed = false;
		for (auto t : st.components[LEFT_BRACKET]) {
			if (subject.components[LEFT_BRACKET][0] == t) { passed = true;  break; }
			if (t == EMPTY) {
				if (subject.components[LEFT_BRACKET][0] == "") { passed = true; break; }
			}
		}
		if (!passed) { message << subject.rawToken << ": Syntax error with left bracket.\n";  return; }

		//Right Bracket Check
		passed = false;
		for (auto t : st.components[RIGHT_BRACKET]) {
			if (subject.components[RIGHT_BRACKET][0] == t) { passed = true;  break; }
			if (t == EMPTY) {
				if (subject.components[RIGHT_BRACKET][0] == "") { passed = true; break; }
			}
		}
		if (!passed) { message << subject.rawToken << ": Syntax error with right bracket.\n";  return; }

		//Parameter_a Check
		passed = false;
		for (auto t : st.components[PARAMETER_a]) {
			if (subject.components[PARAMETER_a][0] == t) { passed = true;  break; }
			if (t == USER_DEFINED_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_a][0], completeUserVariableNames)) {
					subject.indexes[PARAMETER_a] = LMIM[subject.components[PARAMETER_a][0]];
					subject.isLocalMemoryVariable[PARAMETER_a] = 1;

					passed = true;

					break;
				}
			}

			if (t == USER_DEFINED_GRADIENT_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_a][0], GradientVariableNames)) {
					subject.indexes[PARAMETER_a] = GVIM[subject.components[PARAMETER_a][0]]; passed = true;
					subject.isGradient[PARAMETER_a] = 1;
					break;
				}
			}

			if (t == USER_DEFINED_VECTOR_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_a][0], VectorVariableNames)) {
					subject.indexes[PARAMETER_a] = VVIM[subject.components[PARAMETER_a][0]]; subject.isVectorVariable[PARAMETER_a] = 1; passed = true; break;
				}
			}

			if (t == RAM_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_a][0], reserved_RAM_variables)) {
					subject.indexes[PARAMETER_a] = LMIM[subject.components[PARAMETER_a][0]];	subject.isLocalMemoryVariable[PARAMETER_a] = true; passed = true; break;
				}
			}
			if (t == ROM_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_a][0], reserved_ROM_variables)) {
					subject.indexes[PARAMETER_a] = LMIM[subject.components[PARAMETER_a][0]]; subject.isLocalMemoryVariable[PARAMETER_a] = true;	passed = true; break;
				}
			}
			if (t == STATE_NAMES) {
				if (isStringInStringVector(subject.components[PARAMETER_a][0], StateNames)) {
					subject.indexes[PARAMETER_a] = SNIM[subject.components[PARAMETER_a][0]]; subject.isStateName[PARAMETER_a] = 1;	passed = true; break;
				}
			}
			if (t == STATE_NAMES_PLUS_MEDIUM) {
				if ((isStringInStringVector(subject.components[PARAMETER_a][0], StateNames)) || (subject.components[PARAMETER_a][0] == MEDIUM)) {
					subject.indexes[PARAMETER_a] = SNIM[subject.components[PARAMETER_a][0]];	passed = true;


					subject.isStateName[PARAMETER_a] = 1;


					break;
				}
			}


			if (t == EMPTY) {
				if (subject.components[PARAMETER_a][0] == "") { passed = true; break; }
			}
			if (t == ACTUAL_VALUE) {
				if (subject.components[PARAMETER_a][0] == INF) {
					subject.literal_values[PARAMETER_a] = std::numeric_limits<float>::max();
					subject.isLiteralValue[PARAMETER_a] = 1;
					passed = true; break;
				}
				if (isFloat(subject.components[PARAMETER_a][0])) {
					subject.literal_values[PARAMETER_a] = std::stof(subject.components[PARAMETER_a][0].c_str());
					subject.isLiteralValue[PARAMETER_a] = 1;
					passed = true; break;
				}
			}
		}
		if (!passed) { message << subject.rawToken << ": Syntax error with first parameter in brackets.\n";  return; }

		//Parameter_b Check
		passed = false;
		for (auto t : st.components[PARAMETER_b]) {
			if (subject.components[PARAMETER_b][0] == t) { passed = true;  break; }
			if (t == USER_DEFINED_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_b][0], completeUserVariableNames)) {
					subject.indexes[PARAMETER_b] = LMIM[subject.components[PARAMETER_b][0]]; subject.isLocalMemoryVariable[PARAMETER_b] = 1;
					passed = true; break;
				}
			}

			if (t == USER_DEFINED_VECTOR_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_b][0], VectorVariableNames)) {
					subject.indexes[PARAMETER_b] = VVIM[subject.components[PARAMETER_b][0]]; subject.isVectorVariable[PARAMETER_b] = 1; passed = true; break;
				}
			}

			if (t == RAM_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_b][0], reserved_RAM_variables)) {
					subject.indexes[PARAMETER_b] = LMIM[subject.components[PARAMETER_b][0]];	subject.isLocalMemoryVariable[PARAMETER_b] = true; passed = true; break;
				}
			}
			if (t == ROM_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_b][0], reserved_ROM_variables)) {
					subject.indexes[PARAMETER_b] = LMIM[subject.components[PARAMETER_b][0]];	subject.isLocalMemoryVariable[PARAMETER_b] = true; passed = true; break;
				}
			}
			if (t == STATE_NAMES) {
				if (isStringInStringVector(subject.components[PARAMETER_b][0], StateNames)) {
					subject.indexes[PARAMETER_b] = SNIM[subject.components[PARAMETER_b][0]]; subject.isStateName[PARAMETER_b] = 1;	passed = true; break;
				}
			}
			if (t == STATE_NAMES_PLUS_MEDIUM) {
				if ((isStringInStringVector(subject.components[PARAMETER_b][0], StateNames)) || (subject.components[PARAMETER_b][0] == MEDIUM)) {
					subject.indexes[PARAMETER_b] = SNIM[subject.components[PARAMETER_b][0]];	subject.isStateName[PARAMETER_b] = 1; passed = true; break;
				}
			}
			if (t == MITOSIS_CLEAVAGE) {
				if ((subject.components[PARAMETER_b][0] == "+") || ((subject.components[PARAMETER_b][0] == "-"))) { passed = true; break; }
			}

			if (t == EMPTY) {
				if (subject.components[PARAMETER_b][0] == "") {
					passed = true; break;
				}
			}
			if (t == ACTUAL_VALUE) {
				if (subject.components[PARAMETER_b][0] == INF) {
					subject.literal_values[PARAMETER_b] = std::numeric_limits<float>::max();
					subject.isLiteralValue[PARAMETER_b] = 1;
					if ((subject.components[TAG][0] == SET_VEC) && (!subject.isLiteralValue[PARAMETER_a])) {
						passed = false;
						break;
					}
					passed = true; break;
				}

				if (isFloat(subject.components[PARAMETER_b][0])) {
					subject.literal_values[PARAMETER_b] = std::stof(subject.components[PARAMETER_b][0].c_str());
					subject.isLiteralValue[PARAMETER_b] = 1;
					if ((subject.components[TAG][0] == SET_VEC) && (!subject.isLiteralValue[PARAMETER_a])) {
						passed = false;
						break;
					}
					passed = true; break;
				}
			}
		}
		if (!passed) { message << subject.rawToken << ": Syntax error with second parameter in brackets.\n";  return; }

		//Parameter_c Check
		passed = false;
		for (auto t : st.components[PARAMETER_c]) {
			if (subject.components[PARAMETER_c][0] == t) { passed = true;  break; }
			if (t == USER_DEFINED_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_c][0], completeUserVariableNames)) {
					subject.indexes[PARAMETER_c] = LMIM[subject.components[PARAMETER_c][0]]; subject.isLocalMemoryVariable[PARAMETER_c] = 1; passed = true; break;
				}
			}
			if (t == USER_DEFINED_VECTOR_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_c][0], VectorVariableNames)) {
					subject.indexes[PARAMETER_c] = VVIM[subject.components[PARAMETER_c][0]]; subject.isVectorVariable[PARAMETER_c] = 1; passed = true; break;
				}
			}

			if (t == RAM_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_c][0], reserved_RAM_variables)) {
					subject.indexes[PARAMETER_c] = LMIM[subject.components[PARAMETER_c][0]]; subject.isLocalMemoryVariable[PARAMETER_c] = true;	passed = true; break;
				}
			}
			if (t == ROM_VARIABLES) {
				if (isStringInStringVector(subject.components[PARAMETER_c][0], reserved_ROM_variables)) {
					subject.indexes[PARAMETER_c] = LMIM[subject.components[PARAMETER_c][0]]; subject.isLocalMemoryVariable[PARAMETER_c] = true;	passed = true; break;
				}
			}
			if (t == STATE_NAMES) {
				if (isStringInStringVector(subject.components[PARAMETER_c][0], StateNames)) {
					subject.indexes[PARAMETER_c] = SNIM[subject.components[PARAMETER_c][0]]; subject.isStateName[PARAMETER_c] = 1;	passed = true; break;
				}
			}
			if (t == STATE_NAMES_PLUS_MEDIUM) {
				if ((isStringInStringVector(subject.components[PARAMETER_c][0], StateNames)) || (subject.components[PARAMETER_c][0] == MEDIUM)) {
					subject.indexes[PARAMETER_c] = SNIM[subject.components[PARAMETER_c][0]]; subject.isStateName[PARAMETER_c] = 1;		passed = true; break;
				}
			}
			if (t == MITOSIS_CW_0_CCW_1) {
				if ((subject.components[PARAMETER_c][0] == "CW") || (subject.components[PARAMETER_c][0] == "CCW")) {

					passed = true; break;
				}
			}


			if (t == EMPTY) {
				if (subject.components[PARAMETER_c][0] == "") { passed = true; break; }
			}
			if (t == ACTUAL_VALUE) {
				if (subject.components[PARAMETER_c][0] == INF) {
					subject.literal_values[PARAMETER_c] = std::numeric_limits<float>::max();
					subject.isLiteralValue[PARAMETER_c] = 1;
					passed = true; break;
				}

				if (isFloat(subject.components[PARAMETER_c][0])) {
					subject.literal_values[PARAMETER_c] = std::stof(subject.components[PARAMETER_c][0].c_str());
					subject.isLiteralValue[PARAMETER_c] = 1;
					passed = true; break;
				}
			}
		}

		if (!passed) { message << subject.rawToken << ": Syntax error with third parameter in brackets.\n";  return; }
		break;
	}
	if (!passed) { message << subject.rawToken << ": Syntax error with tag.\n";  return; }

	//A few unique syntax checks
	if (subject.components[TAG][0] == SET_VEC) {
		if (subject.isLiteralValue[PARAMETER_a] && !subject.isLiteralValue[PARAMETER_b]) {
			if ((subject.literal_values[PARAMETER_a] < 0) || (subject.literal_values[PARAMETER_a] > MAX_NUMBER_OF_FIELDS) || (subject.literal_values[PARAMETER_a] != (int)subject.literal_values[PARAMETER_a])) {
				message << subject.rawToken << ": As parameter_b is empty, parameter_a will refer to a Field. All Field references must be integers from 0 to " << MAX_NUMBER_OF_FIELDS << ".\n";  return;
			}
			subject.isFieldReference[PARAMETER_a] = 1;
		}
	}

	//A few unique syntax checks
	if (subject.components[TAG][0] == FIELD_EDGES) {
		if ((subject.literal_values[CURLY_BRACKET_PARAMETER] < 0) || (subject.literal_values[CURLY_BRACKET_PARAMETER] > MAX_NUMBER_OF_FIELDS) || (subject.literal_values[CURLY_BRACKET_PARAMETER] != (int)subject.literal_values[CURLY_BRACKET_PARAMETER])) {
			message << subject.rawToken << ": The curly_bracket parameter must be a Field reference; i.e. an integer from 0 to " << MAX_NUMBER_OF_FIELDS << ".\n";  return;
		}
		subject.isFieldReference[CURLY_BRACKET_PARAMETER] = 1;
	}

	//A few unique syntax checks
	if (subject.components[TAG][0] == FIELD) {
		if ((subject.literal_values[PARAMETER_a] < 0) || (subject.literal_values[PARAMETER_a] > MAX_NUMBER_OF_FIELDS) || (subject.literal_values[PARAMETER_a] != (int)subject.literal_values[PARAMETER_a])) {
			message << subject.rawToken << ": Parameters must be Field references; i.e. integers from 0 to " << MAX_NUMBER_OF_FIELDS << ".\n";  return;
		}
		subject.isFieldReference[PARAMETER_a] = 1;
		if ((subject.components[PARAMETER_b][0] != EMPTY)) {
			if ((subject.literal_values[PARAMETER_b] < 0) || (subject.literal_values[PARAMETER_b] > MAX_NUMBER_OF_FIELDS) || (subject.literal_values[PARAMETER_b] != (int)subject.literal_values[PARAMETER_b])) {
				message << subject.rawToken << ": Parameters must be Field references; i.e. integers from 0 to " << MAX_NUMBER_OF_FIELDS << ".\n";  return;
			}
			subject.isFieldReference[PARAMETER_b] = 1;
		}
	}


	//A few unique syntax checks
	if (subject.components[TAG][0] == MATH) {

		if (
			(
			(subject.components[PARAMETER_b][0] == "=") ||
				(subject.components[PARAMETER_b][0] == "+=") ||
				(subject.components[PARAMETER_b][0] == "-=")
				)
			&&
			(
			((subject.isVectorVariable[PARAMETER_a]) && (!subject.isVectorVariable[PARAMETER_c])) ||
				((!subject.isVectorVariable[PARAMETER_a]) && (subject.isVectorVariable[PARAMETER_c]))
				)
			) {
			message << subject.rawToken << ": Cannot add, subtract, or make equal a V_VAR and a non-V_VAR.\n";  return;
		}

		if (
			(
			(subject.components[PARAMETER_b][0] == "/=") ||
				(subject.components[PARAMETER_b][0] == "*=")

				)
			&&
			(
			(subject.isVectorVariable[PARAMETER_c])
				)
			) {
			message << subject.rawToken << ": Cannot multiply or divide with a V_VAR in this manner. Permissible syntax is V_VAR*=non_V_VAR or V_VAR/=non_V_VAR.\n";  return;
		}


	}



	//cout << "AFTER\n"<< subject.debugPrint() << "\n";
}

vector<string> RuleParser::tokenizeByWhiteSpace(string s)
{
	//change all white space to spaces
	s = removeLeadingAndTrailingWhiteSpaceFromString(s);
	string holder = "";
	for (size_t x = 0; x < s.size(); x++) {

		if ((s.substr(x, 1) == "\n") || (s.substr(x, 1) == "\t")) {
			holder += " ";
			continue;
		}
		holder += s.substr(x, 1);
	}


	vector <string> result;
	result.clear();
	if (holder.size() == 0) { return result; }
	if (holder.find(" ") == std::string::npos) { result.push_back(holder); return result; }
	while (holder.find(" ") != std::string::npos) {
		result.push_back(holder.substr(0, holder.find(" ")));
		holder = holder.substr(holder.find(" ") + 1);
		holder = removeLeadingAndTrailingWhiteSpaceFromString(holder);

	}
	if (holder.size() > 0) { result.push_back(holder); }
	return result;
}

vector<string> RuleParser::tokenizeBySemicolon(string s)
{
	vector <string> result;
	result.clear();
	string holder = s;
	if (holder.size() == 0) { return result; }
	if (holder.find(";") == std::string::npos) { result.push_back(holder); return result; }
	while (holder.find(";") != std::string::npos) {
		result.push_back(holder.substr(0, holder.find(";")));
		holder = holder.substr(holder.find(";") + 1);
		holder = removeLeadingAndTrailingWhiteSpaceFromString(holder);

	}
	if (holder.size() > 0) { result.push_back(holder); }
	return result;
}

vector<string> RuleParser::tokenizeByCommas(string s)
{



	vector <string> result;
	result.clear();
	if (s.size() == 0) { return result; }
	if (s.find(",") == std::string::npos) { result.push_back(s); return result; }
	while (s.find(",") != std::string::npos) {
		result.push_back(s.substr(0, s.find(",")));
		s = s.substr(s.find(",") + 1);
		s = removeLeadingAndTrailingWhiteSpaceFromString(s);

	}
	if (s.size() > 0) { result.push_back(s); }
	return result;
}

int RuleParser::findNumberOfOccurencesOfCharacterInString(string s, string searched)
{
	int result = 0;
	for (size_t x = 0; x < s.size(); x++) {
		if (s.substr(x, 1) == searched) {
			result++;
		}
	}
	return result;
}

pair<string, string> RuleParser::splitSentenceByDoubleColon(string s)
{
	pair<string, string> result;
	result.first = "";
	result.second = "";

	std::size_t found = s.find("::");
	if (found == std::string::npos) {
		result.first = "";
		result.second = s;
		return result;
	}
	if (s.substr(0, 2) == "::") {
		result.first = "";
		result.second = s.substr(2);;
		return result;
	}

	result.first = s.substr(0, found);
	result.second = s.substr(found + 2);
	return result;

}




string RuleParser::removeComments(string s)
{
	std::size_t found = s.find("//");
	if (found != std::string::npos) {
		return s.substr(0, found);
	}
	return s;
}
string RuleParser::removeWhiteSpaceFromString(string s)
{
	string result = "";
	for (size_t x = 0; x < s.size(); x++) {
		if ((s[x] != ' ') && (s[x] != '\t')) { result = result + s[x]; }
	}
	return result;
}
bool RuleParser::isFloat(string myString)
{
	myString = removeWhiteSpaceFromString(myString);
	std::istringstream iss(myString);
	float f;
	iss >> f; // noskipws considers leading whitespace invalid
			  // Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}
inline bool RuleParser::isInteger(const std::string& s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

	char* p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}
string RuleParser::removeLeadingAndTrailingWhiteSpaceFromString(string s)
{
	string result = "";
	for (size_t x = 0; x < s.size(); x++) {

		if ((s[x] == ' ') || (s[x] == '\t')) { continue; }
		else {

			result = s.substr(int(x));
			break;
		}
	}

	while (result.size() > 0) {
		if ((result.substr(result.size() - 1) == " ") ||

			(result.substr(result.size() - 1) == "\t")
			) {
			result = result.substr(0, result.size() - 1);

		}
		else {
			break;
		}

	}
	return result;
}
bool RuleParser::isSkipLine(string s)
{
	if (removeWhiteSpaceFromString(removeComments(s)) == "") { return true; }
	if (removeWhiteSpaceFromString(s).substr(0, string(VARIABLE).size()) == VARIABLE) { return true; }
	if (removeWhiteSpaceFromString(s).substr(0, string(GRADIENT_VARIABLE).size()) == GRADIENT_VARIABLE) { return true; }
	if (removeWhiteSpaceFromString(s).substr(0, string(VECTOR_VARIABLE).size()) == VECTOR_VARIABLE) { return true; }
	return false;
}
bool RuleParser::isStringInStringVector(string s, vector<string> v)
{
	if ((s == "") && (v.size() == 0)) { return true; }
	if ((s != "") && (v.size() == 0)) { return false; }


	if (find(v.begin(), v.end(), s) != v.end()) { return true; } return false;
}







RuleParser::RuleParser()
{



	ConditionsAndCommands syntax_temp_holder;

	//************
	//Conditions
	//************
	conditionSyntaxTemplates.clear();

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { FIELD };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[", "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]", ")" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { FIELD_EDGES };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { ACTUAL_VALUE };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[", "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]", ")" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);


	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { POISSON };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { ENTERING_STATE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_a] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { EMPTY };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { ADJ_CELLS };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY, STATE_NAMES };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[", "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]", ")" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { ADJ_MEMBRANES };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY, STATE_NAMES_PLUS_MEDIUM };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[", "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]", ")" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { ADJ_MEMBRANES_PERCENT };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { STATE_NAMES_PLUS_MEDIUM };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[", "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE,USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]", ")" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[NOT_SIGNIFIER] = { EMPTY, NOT };
	syntax_temp_holder.components[TAG] = { USER_DEFINED_GRADIENT_VARIABLES, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES, ACTUAL_VALUE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[", "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE,USER_DEFINED_GRADIENT_VARIABLES, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY,ACTUAL_VALUE,USER_DEFINED_GRADIENT_VARIABLES, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]", ")" };
	conditionSyntaxTemplates.push_back(syntax_temp_holder);



	//************
	//Commands
	//************
	commandSyntaxTemplates.clear();






	syntax_temp_holder.components[TAG] = { EMIT };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { USER_DEFINED_GRADIENT_VARIABLES };
	syntax_temp_holder.components[LEFT_BRACKET] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_a] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { EMPTY };
	commandSyntaxTemplates.push_back(syntax_temp_holder);



	syntax_temp_holder.components[TAG] = { MIGRATE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = { USER_DEFINED_VECTOR_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY ,USER_DEFINED_VARIABLES, ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY, ACTUAL_VALUE, USER_DEFINED_VARIABLES };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);


	syntax_temp_holder.components[TAG] = { SET_VEC };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { USER_DEFINED_VECTOR_VARIABLES };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = { USER_DEFINED_VARIABLES, USER_DEFINED_GRADIENT_VARIABLES, STATE_NAMES_PLUS_MEDIUM, ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = { ADJUST_PP_SCALE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = {  ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = {MOD_PP_LOCATION };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = { USER_DEFINED_VECTOR_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY, ACTUAL_VALUE };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = { RGB };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { ACTUAL_VALUE, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { ACTUAL_VALUE, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[RIGHT_BRACKET] = { ")" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);


	syntax_temp_holder.components[TAG] = { SET_RAND };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { USER_DEFINED_VARIABLES, RAM_VARIABLES };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[","(" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { ACTUAL_VALUE, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]",")" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);


	syntax_temp_holder.components[TAG] = { CHANGE_STATE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { STATE_NAMES };
	syntax_temp_holder.components[LEFT_BRACKET] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_a] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { EMPTY };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = { MITOSIS };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY, STATE_NAMES };
	syntax_temp_holder.components[LEFT_BRACKET] = { EMPTY, "[" };
	syntax_temp_holder.components[PARAMETER_a] = { EMPTY, USER_DEFINED_VECTOR_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY, ACTUAL_VALUE,USER_DEFINED_VARIABLES };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY, MITOSIS_CW_0_CCW_1 };
	syntax_temp_holder.components[RIGHT_BRACKET] = { EMPTY ,"]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = { FIELD_ACTIVE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = { ACTUAL_VALUE };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = { END };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_a] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { EMPTY };
	commandSyntaxTemplates.push_back(syntax_temp_holder);

	syntax_temp_holder.components[TAG] = { DIE };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_a] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_b] = { EMPTY };
	syntax_temp_holder.components[PARAMETER_c] = { EMPTY };
	syntax_temp_holder.components[RIGHT_BRACKET] = { EMPTY };
	commandSyntaxTemplates.push_back(syntax_temp_holder);


	//This command has an alternate syntax that can be input.
	//Specifically, the user can type in "variable+=3" for instance and the compiler
	//will package it into the following.
	syntax_temp_holder.components[TAG] = { MATH };
	syntax_temp_holder.components[CURLY_BRACKET_PARAMETER] = { EMPTY };
	syntax_temp_holder.components[LEFT_BRACKET] = { "[" };
	syntax_temp_holder.components[PARAMETER_a] = { USER_DEFINED_VECTOR_VARIABLES, USER_DEFINED_VARIABLES, RAM_VARIABLES };
	syntax_temp_holder.components[PARAMETER_b] = { "=", "+=", "-=", "*=", "/=" };
	syntax_temp_holder.components[PARAMETER_c] = {  USER_DEFINED_VECTOR_VARIABLES, ACTUAL_VALUE, USER_DEFINED_VARIABLES, RAM_VARIABLES, ROM_VARIABLES };
	syntax_temp_holder.components[RIGHT_BRACKET] = { "]" };
	commandSyntaxTemplates.push_back(syntax_temp_holder);


}


RuleParser::~RuleParser()
{
}




