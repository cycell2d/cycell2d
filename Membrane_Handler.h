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
#include "parameters.h"

using namespace std;
class Cell;

class Membrane_Handler
{
private:
	struct cell_and_membrane_date {
		Cell * c;
		int number_of_membranes;
		cell_and_membrane_date(Cell * cc, int xx) {
			c = cc;
			number_of_membranes = xx;
		}
	};
	vector <cell_and_membrane_date *> data;
	Cell * parent_cell;
public:
	void clear() {
		for (auto d : data) {
			delete d;
		}
		data.clear();
	}
	void erase_mention_of_this_cell(Cell * cc);
	void add_membrane_associated_with_cell(Cell * cc);
	void remove_membrane_associated_with_cell(Cell * cc);
	vector <Cell *> get_all_neighbor_cells();
	bool isCellNeighbor(Cell * cc);
	int total_membranes();
	int total_membranes_of_particular_cell(Cell * cc);
	int total_membranes_of_cells_with_particular_state(int xx);
	int total_cell_neighbors();
	int total_cell_neighbors_of_particular_state(int xx);
	Membrane_Handler(Cell * c);
	~Membrane_Handler();
};

