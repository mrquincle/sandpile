/**
 * @file Toppling.cpp
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
#include <Toppling.h>
#include <assert.h>

#include <boost/random/uniform_smallint.hpp>
#include <boost/random/uniform_01.hpp>

using namespace std;
using namespace boost;

/* **************************************************************************************
 * Implementation of Toppling
 * **************************************************************************************/

// Static members

int Toppling::grid_feed = 230895;

int Toppling::toppling_feed = 9237593;

/**
 * Randomize cells over grid
 */
ptrdiff_t boost_random (ptrdiff_t size) {
	static boost::mt19937 randomGenerator(Toppling::GetGridFeed());
	uniform_smallint<size_t> distr(0, size-1);
	return distr(randomGenerator);
}

// pointer object to it:
ptrdiff_t (*p_boost_random)(ptrdiff_t) = boost_random;

/**
 * Stream operator to use TopplingMethod in stdout, to file, etc.
 */
std::ostream& operator<<( std::ostream& os, const TopplingMethod& method) {
	switch(method) {
	case Bak_Tang_Wiesenfeld1987: os << "BTW1987, deterministic"; break;
	case Manna_Lin2010: os << "Lin2010, stochastic"; break;
	case Lin_etal2006: os << "Lin2006, bulk-dissipation"; break;
	case Rossum2011: os << "Rossum2011, emergent dissipation"; break;
	case Rossum2011_diss: os << "Rossum2011_diss, emergent dissipation (second field)"; break;
	case TM_UNDEFINED: os << "undefined"; break;
	}
	return os;
}

/**
 * A toppling method is coupled to a sand_grid. The class understands a few different methods
 * named after authors of papers. They differ in toppling probability and the threshold
 * upon which a cell becomes critical.
 */
Toppling::Toppling(Grid *grid): sand_grid(grid),
		diss_grid(NULL),
		noDuringAvalanches(NULL),
		countDuringAvalanches(false),
		topple_threshold(4),
		dissipative_mode(false),
		diss_rate(0.1),
		diss_threshold(0),
		diss_amount(4),
		toppling_method(TM_UNDEFINED),
		toppling_iterator(FOLLOW_ACTIVITY),
		random_indices(NULL) {
}

/**
 * Set toppling method and the corresponding default threshold for toppling. This can
 * be overwritten by using SetTopplingThreshold.
 */
void Toppling::SetTopplingMethod(TopplingMethod toppling_method) {
	this->toppling_method = toppling_method;
	switch(toppling_method) {
	case Manna_Lin2010:
		topple_threshold = 2;
		break;
	case Lin_etal2006:
	case Rossum2011:
	case Bak_Tang_Wiesenfeld1987:
		topple_threshold = 4;
		break;
	case Rossum2011_diss:
		topple_threshold = 0;
		break;
	case TM_UNDEFINED:
		cerr << "Undefined toppling method" << endl;
		assert(false);
		break;
	}
}

/**
 * Overwrite toppling threshold. First set SetTopplingMethod. A warning is written to
 * stderr if the threshold is non-default. If the threshold is below zero the default
 * threshold is set again.
 */
void Toppling::SetTopplingThreshold(GrainType threshold) {
	assert (toppling_method != TM_UNDEFINED);
	if (threshold < 0) {
		SetTopplingMethod(toppling_method);
	} else if (threshold != topple_threshold) {
		cerr << "Non-standard toppling threshold: " << threshold << endl;
		topple_threshold = threshold;
	}
}

/**
 * There are three ways with which we can go "through" the grid. We can "follow" the activity
 * which is really convenient for avalanche dynamics: cells that do not topple will not change
 * anyway. There are also random functions in case someone wants to implement global dissipation
 * (different from bulk dissipation which only occurs at avalanche fronts). It can also be used
 * for a different type of (parallel) grid - as in Rossum2011.
 */
void Toppling::SetTopplingIterator(TopplingIterator toppling_iterator) {
	this->toppling_iterator = toppling_iterator;
	int size;
	switch(toppling_iterator) {
	case RANDOM_FRACTION:
	case RANDOM_ALL:
		size = sand_grid->GetWidth()*sand_grid->GetHeight();
		random_indices = new int[size];
		for (int i = 0; i < size; i++) random_indices[i] = i;
		break;
	case FOLLOW_ACTIVITY:
		active_cells.clear();
		break;
	}
}

/**
 * By default every toppling method is defined as being with or without dissipation. This
 * function overwrites this default (and spits out a warning in that case). It can be used
 * for comparative research.
 */
void Toppling::SetDissipativeMode(bool mode) {
	dissipative_mode = mode;
	if(toppling_method == Lin_etal2006) {
		if (!dissipative_mode) {
			cerr << "Warning: Toppling in Lin_etal2006 normally is with bulk-dissipation!"
					<< endl;
		}
	}
}

/**
 * Counting e.g. critical cells during avalanches is computationally expensive. So, there is
 * a boolean with which we can turn on/off this option (by this function).
 */
void Toppling::SetCounterDuringAvalanches(bool count) {
	countDuringAvalanches = count;

	noDuringAvalanches = NULL;
	if (countDuringAvalanches) {
		noDuringAvalanches = new EventCounter<int>();
	}
}

/**
 * Set maximum cell capacity of the corresponding (sand) grid. There is a check in this
 * function that enforces a capacity of twice the toppling threshold. This is mainly to
 * protect the user and can be changed if you want to. However, it means you will need
 * to set the toppling threshold first...
 */
void Toppling::SetCellCapacity(GrainType capacity) {
	if (capacity < (2*topple_threshold)) {
		cerr << "Probably you want to set capacity at least two times the " <<
				"toppling threshold" << endl;
//		assert (false);
	}
	if (!sand_grid) cerr << __FUNCTION__ << ": Grid is not set!" << endl;
	int no_cells = sand_grid->GetWidth() * sand_grid->GetHeight();
	for (int c = 0; c < no_cells; ++c) {
		sand_grid->GetCell(c).SetMaxCapacity(capacity);
	}

}

/**
 * Default destructor. If there is an array to iterate over the sand_grid in a random
 * way, it will be deleted. The sand_grid itself is just a pointer, so it will not be
 * deleted, but just set to NULL.
 */
Toppling::~Toppling() {
	toppling_method = TM_UNDEFINED;

	switch(toppling_iterator) {
	case RANDOM_FRACTION:
	case RANDOM_ALL:
		delete [] random_indices;
		break;
	case FOLLOW_ACTIVITY:
		active_cells.clear();
		break;
	}
	diss_grid = NULL;
	sand_grid = NULL;

	if (noDuringAvalanches != NULL)
		delete noDuringAvalanches;
}

/**
 * Count the number of cells at criticality (depends on toppling method what
 * exactly this constitutes, but it means one grain below threshold for toppling in the case of
 * discrete grains). The dissipation amount is 4 by default.
 * However, in the non-discrete case, there are no such things at "critical cells" because a
 * cell might be increased with a value taken from the range from 0 till the dissipation amount,
 * most often a value around dissipation amount / neighbours (4). This function should be called
 * "PseudoCritical" in that case...
 */
long int Toppling::CountCriticalCells() {
	int no_cells = sand_grid->GetWidth() * sand_grid->GetHeight();

	long int sum = 0;
	for (int c = 0; c < no_cells; ++c) {
		if (sand_grid->GetCell(c).GetHeight() == topple_threshold - GetDissipationAmount() / 4);
			sum++;
	}
	return sum;
}

/**
 * Topple grains from a specific cell to its neighbours. Read the corresponding
 * papers for the - sometimes minute - differences.
 */
bool Toppling::Topple(Cell & cell, vector<Cell*> & neighbours) {
	bool topple = false;

	//! The Mersenne Twister random generator
	static boost::mt19937 randomGenerator(Toppling::GetTopplingFeed());
	uniform_smallint<size_t> distr(0, neighbours.size()-1);

	// default is to transfer one grain to each neighbour: diss_amount = topple_threshold = 4
	GrainType decrease = ((diss_amount <= 0) ? neighbours.size() : diss_amount);

	bool uniform_increase = false;

	GrainType increase_neighbour[neighbours.size()];
	if (uniform_increase) {
		// the increase of each neighbour is exactly 1/# neighbours of total decrease
		for (unsigned int i = 0; i < neighbours.size(); ++i) increase_neighbour[i] = decrease / neighbours.size();
	} else {
		// create 4 random values that add up to decrease...
		static boost::uniform_01<boost::mt19937> zeroone(randomGenerator);
		GrainType sum_increase = 0;
		for (unsigned int i = 0; i < neighbours.size(); ++i) {
			increase_neighbour[i] = zeroone();
			sum_increase += increase_neighbour[i];
		}

		// normalise such that total sum becomes "increase"
		GrainType corr_factor = decrease / sum_increase;
		for (unsigned int i = 0; i < neighbours.size(); ++i) increase_neighbour[i] *= corr_factor;
	}

	// make sure total "increase" equals "decrease " (bulk conservative)
//	assert (sum_increase == decrease);

	if (cell.GetHeight() >= topple_threshold) {
		topple = true;

		switch(toppling_method) {
		case Manna_Lin2010: { // stochastic, but conserves sand quantity
			cell.Decrease(decrease);
			for (unsigned int n = 0; n < neighbours.size(); ++n) {
				int neigh = distr(randomGenerator);
				neighbours[neigh]->Increase(increase_neighbour[n]);
			}
			break;
		}
		case Bak_Tang_Wiesenfeld1987: {
			// it is absolutely not clear from the paper what happens to boundary sites where there is a wall
			// I want to preserve the determinism and conservation along the non-dissipatory border
			// so we do not decrease by topple_threshold, but by the number of neighbours
			cell.Decrease(decrease); // deterministic, conserved
			for (unsigned int n = 0; n < neighbours.size(); ++n) {
				neighbours[n]->Increase(increase_neighbour[n]);
			}
			break;
		}
		case Lin_etal2006: {
			cell.Decrease(decrease);

			if (dissipative_mode) {
				for (unsigned int n = 0; n < neighbours.size(); ++n) {
					static boost::uniform_01<boost::mt19937> zeroone(randomGenerator);
					if (zeroone() > diss_rate)
						neighbours[n]->Increase(increase_neighbour[n]);
				}
			} else {
				// similar as BTW, bulk-conservation, different from authors!
				for (unsigned int n = 0; n < neighbours.size(); ++n) {
					neighbours[n]->Increase(increase_neighbour[n]);
				}
			}
			break;
		}
		case Rossum2011_diss: {
			// dissipative in specific spatial locations
			topple = false; // toppling ceases directly...

			// if cell has a certain weight, transfer one grain to a neighbour in the given direction
			if (cell.GetHeight() > 0) {
				int dir = cell.GetDirection();
				cell.Transfer(*neighbours[dir], 1);
				neighbours[dir]->SetDirection(dir);

				float f = 0.01;
				static boost::uniform_01<boost::mt19937> zeroone(randomGenerator);
				if (zeroone() < f) {
					int dir2 = distr(randomGenerator); //(dir + 2) % 4;
					int dir1 = distr(randomGenerator); //(dir + 1) % 4;
					neighbours[dir2]->SetDirection(dir1);
				}
			}
			break;
		}
		case Rossum2011: {
			assert (diss_threshold > 0);
			GrainType diss = diss_grid->GetCell(cell.GetId()).GetHeight();
			cell.Decrease(decrease);

			// deterministic, like Bak_Tang_Wiesenfeld1987, but with threshold
			// if diss. factor is above a certain threshold, the grains will disappear and
			// the height of the neighbours will not be increased
			if (diss >= diss_threshold) {
				//cout << "Remove 4 grains" << endl;
			} else {
				for (unsigned int n = 0; n < neighbours.size(); ++n) {
					neighbours[n]->Increase(increase_neighbour[n]);
				}
			}
			break;
		}
		case TM_UNDEFINED: {
			cerr << "Undefined toppling method" << endl;
			assert(false);
		}
		}
	}

	return topple;
}

/**
 * This is the routine that is used as callback function on a change in the height
 * of an individual cell. It is set in the Sandpile constructor for every cell
 * in the actual grid (but not for the reservoir cell if it exists).
 */
void Toppling::CheckCell(Cell & cell) {
	switch(toppling_iterator) {
	case RANDOM_FRACTION:
	case RANDOM_ALL:
		break;
	case FOLLOW_ACTIVITY:
		if (cell.GetHeight() < topple_threshold)
			active_cells.erase(&cell);
		else {
			active_cells.insert(&cell);
		}
		break;
	}
}

/**
 * Topple everything that can be toppled. There have been no attempts to speed
 * things up. We just iterate over the entire sand_grid and call Topple for every
 * sand_grid cell. If none of the cells topples, we are done. If they keep toppling
 * then we end up in an infinite loop.
 */
void Toppling::Topple(long int & avalanche_size) {
	vector<Cell*> neighbours;
	bool quit;
	avalanche_size = 0;
	int it_n = 0;

	// RANDOM_ALL: go over entire grid L*L
	int iterate_number = sand_grid->GetWidth() * sand_grid->GetHeight();
	do {
		quit = true;
		switch(toppling_iterator) {
		case RANDOM_FRACTION:
			// Only L items (so we do not accidently introduce a factor depending on the system size)
			iterate_number = (float)sand_grid->GetWidth();
		case RANDOM_ALL:
			std::random_shuffle(random_indices, random_indices+sand_grid->GetWidth()*sand_grid->GetHeight(), p_boost_random);

			for (int c = 0; c < iterate_number; ++c) {
				int cell_index = random_indices[c];
				int i = cell_index % sand_grid->GetWidth();
				int j = cell_index / sand_grid->GetWidth();
				neighbours.clear();
				sand_grid->GetNeighbours(i, j, neighbours);
				if (Topple(sand_grid->GetCell(i,j), neighbours)) {
					avalanche_size++;
					quit = false;
				}
			}
			break;
		case FOLLOW_ACTIVITY:
			// active cells have to be in a different order each time, so hence we use a vector here
			// then we shuffle it randomly using the boost random generator
			// and we clear active_cells, this has also the advantage that we can use active_cells
			// within this for-loop, while elseway the iterator might become corrupted
			vector<long int> c_indices; c_indices.clear();
			set<Cell*>::iterator it;
			for (it = active_cells.begin(); it != active_cells.end(); ++it) {
				c_indices.push_back((*it)->GetId());
			}
			std::random_shuffle(c_indices.begin(), c_indices.end(), p_boost_random);
			active_cells.clear();

			if (countDuringAvalanches && !it_n) {
				long int n = sand_grid->CountGrains();
				noDuringAvalanches->AddEvent(n);
			}

			for (unsigned int c = 0; c < c_indices.size(); ++c) {
				neighbours.clear();
				int cell_index = c_indices[c];
				int i = cell_index % sand_grid->GetWidth();
				int j = cell_index / sand_grid->GetWidth();
				sand_grid->GetNeighbours(i, j, neighbours);

				if (Topple(sand_grid->GetCell(i, j), neighbours)) {
					avalanche_size++;
				}
			}

			// count number of grains, not really "nicely defined" because the way activity diffusion
			// is implemented influences the number of loops for Topple...
			if (countDuringAvalanches) {
				long int n = sand_grid->CountGrains(); // / sand_grid->GetHeight(); // * sand_grid->GetWidth());
//				cout << "Number of grains: " << n << endl;
				noDuringAvalanches->AddEvent(n);
			}

			quit = active_cells.empty();
			break;
		}
		++it_n;

	} while (!quit);

#ifdef EXTRA_ORDINARY_CHECKING
	// after all toppling, every cell should be below topple_threshold
	for (int i = 0; i < sand_grid->GetWidth(); ++i) {
		for (int j = 0; j < sand_grid->GetHeight(); ++j) {
			int h = sand_grid->GetCell(i,j).GetHeight();
			assert (h < topple_threshold);
		}
	}
#endif
}
