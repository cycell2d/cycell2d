#include "Membrane_Handler.h"
#include "Cell.h"
#include "PetriDish.h"
using namespace std;

void Membrane_Handler::erase_mention_of_this_cell(Cell * cc)
{
	cell_and_membrane_date * holder = NULL;
	for (auto d : data) {
		if (d->c == cc) {
			holder = d;
			break;
		}
	}
	if (holder == NULL) {
		cout << "Error. There should have been a membrane to remove but the cell is not present.\n";
		sp();
	}
	if (holder != NULL) {
	
			data.erase(find(data.begin(), data.end(), holder));
			delete holder;
		}
		return;
	}



void Membrane_Handler::add_membrane_associated_with_cell(Cell * cc) {

	 if (parent_cell->parent_petri->SUBSTRATE == cc) {
		 parent_cell->is_touching_medium = 1;
	  }
	cell_and_membrane_date * holder = NULL;
	for (auto d : data) {
		if (d->c == cc) {
			holder = d;
			break;
		}
	}
	if (holder != NULL) {
		holder->number_of_membranes++;
		return;
	}
	else {
		holder = new cell_and_membrane_date(cc, 1);
		data.push_back(holder);
		return;
	}
}

 void Membrane_Handler::remove_membrane_associated_with_cell(Cell * cc) {
	cell_and_membrane_date * holder = NULL;
	for (auto d : data) {
		if (d->c == cc) {
			holder = d;
			break;
		}
	}
	if (holder == NULL) {
		cout << "Error. There should have been a membrane to remove but the cell is not present.\n";
		sp();
	}
	if (holder != NULL) {
		holder->number_of_membranes--;
		if (holder->number_of_membranes == 0) {
			if (parent_cell->parent_petri->SUBSTRATE == holder->c) {
				parent_cell->is_touching_medium = 0;
			}
			data.erase(find(data.begin(), data.end(), holder));
			delete holder;
		}
		return;
	}
}

 vector<Cell*> Membrane_Handler::get_all_neighbor_cells()
 {
	 vector<Cell*> result;
	 result.clear();
	 for (auto d : data) {
		 result.push_back(d->c);
	 }
	 return result;
 }

 bool Membrane_Handler::isCellNeighbor(Cell * cc)
 {
	 for (auto d : data) {
		 if (d->c == cc) {
			 return true;
		 }
	 }
	 return false;
 }

 int Membrane_Handler::total_membranes() {
	int count = 0;
	for (auto d : data) {
		count = count + d->number_of_membranes;
	}
	return count;
}

 int Membrane_Handler::total_membranes_of_particular_cell(Cell * cc)
 {
	
	 for (auto d : data) {
		
		 if (d->c== cc) {
			 return d->number_of_membranes;
		 }
	 }
	 return 0;
	
 }

 

 int Membrane_Handler::total_membranes_of_cells_with_particular_state(int xx) {
	int count=0;
	for (auto d : data) {
		if (d->c->getState()==xx) {
			count += d->number_of_membranes;
		}
	}
	return count;
}

 int Membrane_Handler::total_cell_neighbors() {
	return data.size();
}

 int Membrane_Handler::total_cell_neighbors_of_particular_state(int xx) {
	int count = 0;
	for (auto d : data) {
		if (d->c->getState() == xx) {
			count++;
		}
	}
	return count;
}

Membrane_Handler::Membrane_Handler(Cell * c)
{
	parent_cell = c;
	data.clear();
}


Membrane_Handler::~Membrane_Handler()
{
	for (auto d : data) {
		delete d;
	}
}
