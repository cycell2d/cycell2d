
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
#include <limits>
#include <list>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <queue>
#include "parameters.h"
#include "Cell.h"

using namespace std;
#define  USER_LABEL "user_label"
#define  POISSON "POISSON"
#define  ADJ_CELLS "ADJ_CELLS"
#define  ADJ_MEMBRANES "ADJ_MEMBRANES"
#define  ADJ_MEMBRANES_PERCENT "ADJ_MEMBRANES_PERCENT"
#define  ENTERING_STATE "ENTERING_STATE"

#define MIGRATE "MIGRATE"
#define EMIT "EMIT"

#define  SET_RAND "SET_RAND"
#define  END "END"
#define DIE "DIE"
#define  CHANGE_STATE "CHANGE_STATE"
#define  MITOSIS "MITOSIS"
#define SET_VEC "SET_VEC"

#define MITOSIS_CHANGE_STATE "MITOSIS_CHANGE_STATE"
#define  ACTUAL_AREA "ACTUAL_AREA"
#define TYPICAL_AREA "TYPICAL_AREA"
#define  GOAL_AREA "GOAL_AREA"
#define  MEDIUM "MEDIUM"
#define  STATE "STATE"
#define  ADHESIONS "ADHESIONS"
#define  GAP_JUNCTIONS "GAP_JUNCTIONS"
#define  SELF "SELF"
#define BEGIN "BEGIN"
#define RED "RED"
#define GREEN "GREEN"
#define BLUE "BLUE"
#define BULK_MODULUS "BULK_MODULUS"
#define INF "INF"
#define RGB "RGB"
#define VARIABLE "U_VAR"
#define GRADIENT_VARIABLE "G_VAR"
#define VECTOR_VARIABLE "V_VAR"
#define EMPTY ""

#define USER_DEFINED_VARIABLES "UDV"
#define USER_DEFINED_VECTOR_VARIABLES "UDVV"
#define USER_DEFINED_GRADIENT_VARIABLES "UDGV"

#define RAM_VARIABLES "RAM_VARIABLES"
#define ROM_VARIABLES "ROM_VARIABLES"
#define ACTUAL_VALUE "ACTUAL_VALUE"
#define STATE_NAMES "STATE_NAMES"
#define STATE_NAMES_PLUS_MEDIUM "STATE_NAMES_PLUS_MEDIUM"
#define CURRENT_STATE "CURRENT_STATE"
#define STATE_TO_CHANGE_INTO "STATE_TO_CHANGE_INTO"
#define DIE_FLAG "DIE_FLAG"

#define ENTERING_STATE_FLAG "ENTERING_STATE_FLAG"
#define CS_BLUE_default "CS_BLUE_default"
#define CS_YELLOW_default "CS_YELLOW_default"
#define FIELD "FIELD"
#define FIELD_EDGES "FIELD_EDGES"

#define NOT "!"
#define FIELD_ACTIVE "FIELD_ACTIVE"
#define FIELD_ACTIVE_FLAG "FIELD_ACTIVE_FLAG"
#define ADJUST_PP_SCALE "ADJUST_PP_SCALE"
#define MOD_PP_LOCATION "MOD_PP_LOCATION"
#define MATH "MATH"
#define SCALE "SCALE"

#define PP_SCALED_WIDTH "PP_SCALED_WIDTH"
#define PP_SCALED_HEIGHT "PP_SCALED_HEIGHT"

#define MITOSIS_FLAG "MITOSIS_FLAG"
#define MITOSIS_CLEAVAGE_VECTOR_SIGNIFIER "MITOSIS_CLEAVAGE_VECTOR_SIGNIFIER"
#define MITOSIS_CLEAVAGE "MITOSIS_CLEAVAGE"
#define MITOSIS_STATE_TO_CHANGE_INTO "MITOSIS_STATE_TO_CHANGE_INTO"
#define MITOSIS_CLEAVAGE_OFFSET "MITOSIS_CLEAVAGE_OFFSET"
#define MITOSIS_CW_0_CCW_1 "MITOSIS_CW_0_CCW_1"

class RuleParser
{

private:
	

	vector <string> forbidden_label_characters{ "!", "@", "#","$","%","^","&","*","(",")","-","+","=","<",">",",",".","/","?",";",":","|","\\","{","}","[","]" };
	vector <string> reserved_conditional_words{ POISSON, ADJ_CELLS, ADJ_MEMBRANES, ADJ_MEMBRANES_PERCENT, ENTERING_STATE, FIELD, FIELD_EDGES };
	vector <string> reserved_command_words{  RGB, SET_RAND,END,CHANGE_STATE,MITOSIS,DIE, MIGRATE, SET_VEC };

	//**WARNING: The Order of these strings MUST match up with the corresponding index values
	//in parameters.h
	vector <string> reserved_Cell_Memory_Flag_variables{ CURRENT_STATE,ENTERING_STATE_FLAG,STATE_TO_CHANGE_INTO, DIE_FLAG,
		FIELD_ACTIVE_FLAG,MITOSIS_FLAG, MITOSIS_CLEAVAGE_VECTOR_SIGNIFIER,MITOSIS_CLEAVAGE,MITOSIS_STATE_TO_CHANGE_INTO,MITOSIS_CLEAVAGE_OFFSET,MITOSIS_CW_0_CCW_1};
	vector <string> reserved_ROM_variables{ ACTUAL_AREA };
	vector <string> reserved_RAM_variables{ RED, GREEN, BLUE,GOAL_AREA, BULK_MODULUS,TYPICAL_AREA,PP_SCALED_WIDTH ,PP_SCALED_HEIGHT };
	//************************


	vector <string> forbidden_state_names{ CS_BLUE_default, CS_YELLOW_default,
		POISSON, ADJ_CELLS, ADJ_MEMBRANES, ADJ_MEMBRANES_PERCENT,
		ENTERING_STATE, RGB, MEDIUM, STATE, FIELD, FIELD_EDGES,
		ADHESIONS, GAP_JUNCTIONS, SELF, SET_RAND,END,
		CHANGE_STATE,MITOSIS,DIE, ACTUAL_AREA ,INF,
		RED, GREEN, BLUE,GOAL_AREA, BULK_MODULUS,TYPICAL_AREA,VARIABLE,
		USER_DEFINED_VARIABLES,RAM_VARIABLES,ROM_VARIABLES,ACTUAL_VALUE, STATE_NAMES,
		CURRENT_STATE, STATE_TO_CHANGE_INTO, DIE_FLAG, MITOSIS_FLAG, MIGRATE };
	vector <string> forbidden_user_variable_names{ CS_BLUE_default, CS_YELLOW_default,
		POISSON, ADJ_CELLS, ADJ_MEMBRANES, ADJ_MEMBRANES_PERCENT,
		ENTERING_STATE, RGB, MEDIUM, STATE,FIELD, FIELD_EDGES,
		ADHESIONS, GAP_JUNCTIONS, SELF,SET_RAND,END,
		CHANGE_STATE,MITOSIS,DIE, ACTUAL_AREA ,INF,
		RED, GREEN, BLUE,GOAL_AREA, BULK_MODULUS,TYPICAL_AREA, VARIABLE,
		USER_DEFINED_VARIABLES,RAM_VARIABLES,ROM_VARIABLES,ACTUAL_VALUE, STATE_NAMES,
		CURRENT_STATE, STATE_TO_CHANGE_INTO, DIE_FLAG, MITOSIS_FLAG,  MIGRATE };

	enum syntactical_component_names {
		NOT_SIGNIFIER = 0,
		TAG,
		RIGHT_BRACKET,
		LEFT_BRACKET,
		CURLY_BRACKET_PARAMETER,
		PARAMETER_a,
		PARAMETER_b,
		PARAMETER_c,
		LAST
	};

#define SYNTACTICAL_COMPONENT_NAME_SIZE 8





	struct ConditionsAndCommands {
		string rawToken;
		vector < vector <string> > components;
		vector<int> indexes;
		vector <bool> isVectorVariable;
		vector <bool> isGradient;
		vector <bool> isLocalMemoryVariable;
		vector <bool> isStateName;
		vector <bool> isLiteralValue;
		vector <bool> isFieldReference;
		vector <float> literal_values;
		int comparison_flag; //FLAG: 0 = [,], 1 = [,), 2 = (,], 3 = (,), 4 = []with no comma
		bool isNot;
		string debugPrint() {
			stringstream result;
			result << "Raw Token = " << rawToken << "\n";
			result << "Not_Sign:     *" << components[NOT_SIGNIFIER][0] << "*, Value = " << isNot << "\n";

			result << "Tag:          *" << components[TAG][0] << "*, Value = " << literal_values[TAG] << ", Index = " << indexes[TAG] << "\n";
			result << "Curly:        *" << components[CURLY_BRACKET_PARAMETER][0] << "*, Value = " << literal_values[CURLY_BRACKET_PARAMETER] << ", Index = " << indexes[CURLY_BRACKET_PARAMETER] << "\n";
			result << "LeftBracket:  *" << components[LEFT_BRACKET][0] << "*, Value = " << literal_values[LEFT_BRACKET] << ", Index = " << indexes[LEFT_BRACKET] << "\n";
			result << "Param_a:      *" << components[PARAMETER_a][0] << "*, Value = " << literal_values[PARAMETER_a] << ", Index = " << indexes[PARAMETER_a] << "\n";
			result << "Param_b:      *" << components[PARAMETER_b][0] << "*, Value = " << literal_values[PARAMETER_b] << ", Index = " << indexes[PARAMETER_b] << "\n";
			result << "Param_c:      *" << components[PARAMETER_c][0] << "*, Value = " << literal_values[PARAMETER_c] << ", Index = " << indexes[PARAMETER_c] << "\n";
			result << "RightBracket: *" << components[RIGHT_BRACKET][0] << "*, Value = " << literal_values[RIGHT_BRACKET] << ", Index = " << indexes[RIGHT_BRACKET] << "\n";
			return result.str();

		}
		ConditionsAndCommands() {
			components = vector < vector<string> >(SYNTACTICAL_COMPONENT_NAME_SIZE, vector<string>(0, ""));
			indexes = vector < int >(SYNTACTICAL_COMPONENT_NAME_SIZE, -1);
			isGradient = vector < bool >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			isLocalMemoryVariable = vector < bool >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			isStateName = vector < bool >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			isVectorVariable = vector < bool >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			isLiteralValue = vector < bool >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			isFieldReference = vector < bool >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			literal_values = vector < float >(SYNTACTICAL_COMPONENT_NAME_SIZE, 0);
			comparison_flag = -1;
			isNot = 0;
		}

	};

	struct Sentence {
		vector<ConditionsAndCommands> conditions;
		vector <ConditionsAndCommands> commands;
		string debugPrint() {
			stringstream result;
			result << "Conditions: \n";
			for (auto cn : conditions) {
				result << cn.debugPrint() << "\n";
			}
			result << "Commands: \n";
			for (auto cm : commands) {
				result << cm.debugPrint() << "\n";
			}
			return result.str();
		}
		Sentence() { conditions.clear(); commands.clear(); }
	};
	struct Paragraph {
		string state_name; //for debugging;
		vector <Sentence> sentences;
		string debugPrint() {
			stringstream result;
			for (auto sn : sentences) {
				result << sn.debugPrint() << "\n";
			}
			return result.str();

		}
		Paragraph() { sentences.clear(); }
	};

	vector <Paragraph> state_paragraph_vector;

	vector <ConditionsAndCommands> conditionSyntaxTemplates;
	vector <ConditionsAndCommands> commandSyntaxTemplates;



	bool isAdhesionSectionPresent = false;
	bool isGapJunctionPresent = false;

	//Generic String Manipulation Functions
	string removeComments(string s);
	string removeWhiteSpaceFromString(string s);
	bool isFloat(string myString);
	inline bool isInteger(const std::string& s);
	bool isSkipLine(string s);
	bool isStringInStringVector(string s, vector <string> v);
	string removeLeadingAndTrailingWhiteSpaceFromString(string s);
	pair <string, string> splitSentenceByDoubleColon(string s);
	vector <string> tokenizeByWhiteSpace(string s);
	vector <string> tokenizeBySemicolon(string s);
	vector <string> tokenizeByCommas(string s);
	int findNumberOfOccurencesOfCharacterInString(string s, string searched);
	//****************************************

	//Parsing Functions for User-Defined Variables
	vector <string> extractUserVariablesFromRawRules(stringstream & message, int & line_number);
	//****************************************

	//Parsing Functions for Vector Variables
	vector <string> extractVectorVariablesFromRawRules(stringstream& message, int& line_number);
	//****************************************

	//Parsing Functions for Gradient Variables
	vector <string> extractGradientVariablesFromRawRules(stringstream & message, int & line_number);
	//****************************************


	//Parsing Functions for STATE section, generally in logical order of exec
	vector <string> extractStateNamesFromRawRules(stringstream & message, int & line_number);
	void isFirstLineStateOrAdhesionOrGJ(stringstream & message, int & line_number);
	pair <int, int> getStateRulesSection(stringstream & message, int & line_number);
	pair<int, int> state_section_line_number_range;
	vector <pair<int, int> > paragraph_line_number_ranges;
	vector < pair <int, int> > splitStateRulesSectionIntoParagraphs(stringstream & message, int & line_number);
	void packageParagraphs(Paragraph& state_par, pair<int, int> single_paragraph_line_number_range, stringstream & message, int & line_number);
	void packageSentences(Paragraph & par, pair<int, int> single_paragraph_line_number_range, stringstream & message, int & line_number);
	void packageTokens(string tokens, vector <ConditionsAndCommands> & cc, stringstream & message, int & line_number);
	vector< vector <string> > splitTokenIntoComponents(string s, stringstream & message, int & line_number);
	void finalSyntaxCheckAndAssignmentOfIndexes(ConditionsAndCommands & subject, vector <ConditionsAndCommands> syntax_templates, stringstream & message, int & line_number);
	//****************************************


	vector < vector <float> > AdhesionValues;
	vector < float> AdhesionSelfValues;
	//Parsing Functions for ADHESIONS section, generally in logical order of exec
	pair <int, int> getAdhesionsSection(stringstream & message, int & line_number);
	pair<int, int> adhesion_section_line_number_range;
	void parseAdhesionValues(stringstream & message, int & line_number);
	//****************************************


	vector < vector < vector <float> > > GJValues;
	//Parsing Functions for Gap Junctions
	pair <int, int> getGJSection(stringstream & message, int & line_number);
	pair<int, int> GJ_section_line_number_range;
	void parseGJ(stringstream & message, int & line_number);
	//***************************************
	


	vector <string> rawRules;
	bool comparison(float test_value, float param_a, float param_b, int comp_flag);
	bool poisson_test(float a);
public:


	float get_GJ_value(int a, int b, int c) { return GJValues[a][b][c]; }
	float get_Adhesion_Value(int a, int b) { return AdhesionValues[a][b]; }
	inline float get_Adhesion_Self_Value(int a) { return AdhesionSelfValues[a]; }
	void set_Adhesion_Value(int a, int b, float x) {
		AdhesionValues[a][b] = x; AdhesionValues[b][a] = x;
	}
	void set_Adhesion_Self_Value(int a, float x) { AdhesionSelfValues[a] = x; }
	
	
	vector <string> completeUserVariableNames;
	vector <string> completeVectorVariableNames;



	vector <string> StateNames;
	map <string, int> SNIM;

	vector <string> LocalMemoryNames;
	map <string, int> LMIM;

	vector <string> GradientVariableNames;
	map <string, int> GVIM;

	vector <string> VectorVariableNames;
	map <string, int> VVIM;

	vector <string> getRawRules() { return rawRules; }
	void loadRulesText(string s);
	void compile(stringstream& message, int& line_number);
	void evaluate(Cell* c, stringstream& debugMessage, bool debugflag);



	RuleParser();
	~RuleParser();
};

