/**
 * @file Grid.cpp
 * @brief 
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author 	Anne C. van Rossum
 * @date	Jul 15, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */


// General files
#include <Grid.h>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <iomanip>

#include <boost/random/uniform_smallint.hpp>

using namespace std;
using namespace boost;

int Grid::neighbour_feed = 334340;

/**
 * Randomize neighbours over grid
 */
ptrdiff_t boost_random_neigh (ptrdiff_t size) {
	static boost::mt19937 randomGenerator(Grid::GetNeighbourFeed());
	uniform_smallint<size_t> distr(0, size-1);
	return distr(randomGenerator);
}

// pointer object to it:
ptrdiff_t (*p_boost_random_neigh)(ptrdiff_t) = boost_random_neigh;

/* **************************************************************************************
 * Implementation of Grid
 * **************************************************************************************/

std::ostream& operator<<( std::ostream& os, const BoundaryType& type ){
	switch(type) {
	case BT_PERIODIC: os << "periodic"; break;
	case BT_DISSIPATING: os << "dissipating"; break;
	case BT_WALL_DISSIPATING: os << "walls and dissipating"; break;
	case BT_CIRCULAR: os << "circular"; break;
	case BT_RANDOM_NEIGHBOURS: os << "random neighbours"; break;
	case BT_FULLY_CONNECTED: os << "fully connected"; break;
	case BT_UNDEFINED: os << "undefined"; break;
	}
	return os;
}

/**
 * Construct a grid with width*height cells and of a certain boundary type. There are
 * periodic and dissipating boundaries. The former makes the grid a kind of "Mobiüs"
 * strip, but then two-dimensional. And the latter connects all boundaries to a
 * reservoir.
 */
Grid::Grid(int width, int height, BoundaryType boundary_type) {
	cout << "Create cells " << width << "*" << height << " (total=" << width * height << ") and type " << boundary_type << endl;
	this->width = width;
	this->height = height;
	int size = width * height;
	cells = new Cell[size];
	for (int i = 0; i < size; ++i) cells[i].SetId(i);
	this->boundary_type = boundary_type;
	reservoir.SetId(-1-width); //=(width+1)*(height+1)) (an "impossible" id)

	random_indices = new int[size];
	for (int i = 0; i < size; ++i) random_indices[i] = i;
	std::random_shuffle(random_indices, random_indices+size, p_boost_random_neigh);
}

/**
 * Remove all the cells and set width and height to zero.
 */
Grid::~Grid() {
	delete [] cells;
	delete [] random_indices;
	cells = NULL;
	width = height = 0;
}

/**
 * Can be used to define a circle within a square grid. Everything outside of the circle
 * can be treated as the reservoir.
 */
bool Grid::WithinCircle(int i, int j) {
	// only accept even boundary sizes and square grid
	assert (width == height);
	assert (width % 2 == 0);
	// a circle with 1 cell as boundary at the vertical and horizontal axis
	// distance to origin
	float d_i = fabs((width/(float)2) - (i+0.5));
	float d_j = fabs((width/(float)2) - (j+0.5));
	float c = d_i * d_i + d_j * d_j;

	// instead of sqrt root we just compare the squares themselves
	return (c <= (((width-1)/2)*((width-1)/2)));
}

/**
 * Returns the four neighbours in a rectangular grid. In the periodic case, the
 * boundaries are connected to a "reservoir" cell (might be multiple times). The given
 * vector should already be allocated, naturally, and will be emptied in this function
 * before it is refilled.
 */
void Grid::GetNeighbours(int i, int j, vector<Cell*> & neighbours) {
	assert (width != 0);
	assert (height != 0);
	neighbours.clear();

	switch(boundary_type) {
	case BT_PERIODIC: {
		int t_i = i + width;
		int t_j = j + height;
		for (int n = -1; n <= 1; n += 2) {
			for (int xy_toggle = 0; xy_toggle <= 1; ++xy_toggle) {
				// n_i is x coordinate, n_j is y-coord.
				// first we go for NORTH and WEST (n = -1)
				// first we go for the y-coord, then x-coord (xy_toggle = 0)
				int n_i = (t_i+n*xy_toggle)%width;
				int n_j = (t_j+n*(1-xy_toggle))%height;
				Cell *cptr = &cells[n_j*width+n_i];
				neighbours.push_back(cptr);
			}
		}
		assert (neighbours.size() == 4);
		break;
	}
	case BT_DISSIPATING: {
		if (i == 0) neighbours.push_back(&reservoir);
		else neighbours.push_back(&cells[j*width+(i-1)]);

		if (i == width-1) neighbours.push_back(&reservoir);
		else neighbours.push_back(&cells[j*width+(i+1)]);

		if (j == 0) neighbours.push_back(&reservoir);
		else neighbours.push_back(&cells[(j-1)*width+i]);

		if (j == height-1) neighbours.push_back(&reservoir);
		else neighbours.push_back(&cells[(j+1)*width+i]);

		assert (neighbours.size() == 4);
		break;
	}
	case BT_WALL_DISSIPATING: {
		if (i != 0) neighbours.push_back(&cells[j*width+(i-1)]);

		if (i == width-1) neighbours.push_back(&reservoir);
		else neighbours.push_back(&cells[j*width+(i+1)]);

		if (j != 0) neighbours.push_back(&cells[(j-1)*width+i]);

		if (j == height-1) neighbours.push_back(&reservoir);
		else neighbours.push_back(&cells[(j+1)*width+i]);

		//		assert (neighbours.size() == 4); // not true anymore!
		// we can make it true again and be faithful to implementation by introducing
		// a stochastic element: add one (or two in the corner) random neighbour(s) twice
		break;
	}
	case BT_CIRCULAR: {
		for (int n = -1; n <= 1; n += 2) {
			for (int xy_toggle = 0; xy_toggle <= 1; ++xy_toggle) {
				int n_i = (i+n*xy_toggle);
				int n_j = (j+n*(1-xy_toggle));
				Cell *cptr = &GetCell(n_i, n_j);
				if (!WithinCircle(n_i, n_j))
					neighbours.push_back(&reservoir);
				else
					neighbours.push_back(cptr);
			}
		}
		assert (neighbours.size() == 4);
		break;
	}
	// not sure how the fully connected case should work out
	// on toppling I guess there will be 4 cells picked out to which to transfer a grain
	// so I interpret it as returning a different set of 4 cells each time
	// the BT_RANDOM_NEIGHBOURS case returns instead the same set of 4 cells for a cell

	case BT_FULLY_CONNECTED: {
		unsigned int no_n = 4;
		unsigned int cnt = 0;
		int this_i = j * width + i;
		do {
			int n = boost_random_neigh(width*height);
			if (n != this_i) {
				Cell *cptr = &GetCell(n);
				neighbours.push_back(cptr);
				cnt++;
			}
		} while (cnt != no_n);
		assert (neighbours.size() == no_n);
		break;
	}
	// random neighbours, but quenched (fixed) over the course of the simulation
	// "boundary" does not say so much in a random setting
	// so we can only test the bulk dissipating case without boundary dissipation
	// if we remove bulk dissipation, the system will go on forever

	// remark: too lazy to remove the case that cell is its own neighbour...
	// will not change dynamics so much anyway, only problem if there is a very small cycle
	// formed: 5 neighbours only connected to each other
	case BT_RANDOM_NEIGHBOURS: {
		int t_i = i + width;
		int t_j = j + height;
		for (int n = -1; n <= 1; n += 2) {
			for (int xy_toggle = 0; xy_toggle <= 1; ++xy_toggle) {
				int n_i = (t_i+n*xy_toggle)%width;
				int n_j = (t_j+n*(1-xy_toggle))%height;
				int index = random_indices[n_j*width+n_i];
				Cell *cptr = &GetCell(index);
				neighbours.push_back(cptr);
			}
		}
		assert (neighbours.size() == 4);
		break;
	}
	case BT_UNDEFINED: {
		cerr << "Undefined boundary type!" << endl;
		assert(false);
	}
	}

	// often the directions do not really matter, but in case they do
	// neighbours[Direction] should be really in the given direction
	//#define CHECK_DIRECTIONS
#ifdef CHECK_DIRECTIONS
	int id = neighbours[NORTH]->GetId();
	int n_i = id % width;
	int n_j = id / width;
	int modj_min = (j + height - 1) % height;
	assert ((i == n_i) && (n_j == modj_min));

	id = neighbours[SOUTH]->GetId();
	n_i = id % width;
	n_j = id / width;
	int modj_plus = (j + 1) % height;
	assert ((i == n_i) && (n_j == modj_plus));

	id = neighbours[WEST]->GetId();
	n_i = id % width;
	n_j = id / width;
	int modi_min = (i + width - 1) % width;
	assert ((j == n_j) && (n_i == modi_min) );

	id = neighbours[EAST]->GetId();
	n_i = id % width;
	n_j = id / width;
	int modi_plus = (i + 1) % width;
	assert ((j == n_j) && (n_i == modi_plus) );
#endif
}

/**
 * Counting total grains over all grid cells.
 */
GrainType Grid::CountGrains() {
	GrainType sum = 0;
	for (int i = 0; i < width*height; ++i) {
		sum += cells[i].GetHeight();
	}
	return sum;
}

/**
 * Returns a cell with the given coordinates. It is now excessively checking
 * on conditions. The first coordinate should not be less then the width, the
 * second should be below the height. This is the same as
 * GetCell(i+j*width).
 */
Cell & Grid::GetCell(int i, int j) {
	assert (j < height);
	assert (i < width);
	return cells[j*width+i];
}

/**
 * Just give the cell directly and assume the user knows how it is stored internally.
 * Do not mix the x and y coordinates of course. :-) This is the same as
 * GetCell(n % width, n / width).
 */
Cell & Grid::GetCell(int n) {
	assert (n < height * width);
	return cells[n];
}

/**
 * Print the total number of "grains" in the grid itself and go over all cells
 * and print the number of "grains" divided by spaces. The layout will be an
 * table with size width*height. It will become less useful when the grid
 * becomes larger than say 60*60.
 */
void Grid::Print() {
	cout << "Grid size = " << CountGrains() << endl;
	int p = cout.precision();
	cout.setf(ios::fixed);
	cout.precision(0);
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			int height = GetCell(i,j).GetHeight();
			cout << height << " ";
		}
		cout << endl;
	}
	cout << endl;

	// Set back to original printing format
	cout.unsetf(ios::fixed);
	cout.precision(p);
}

