/**
 * @file Toppling.h
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


#ifndef TOPPLING_H_
#define TOPPLING_H_

// General files
#include <Grid.h>
#include <Cell.h>
#include <vector>
#include <set>
#include <EventCounter.hpp>

#include <boost/random/mersenne_twister.hpp>

/* **************************************************************************************
 * Macros and forward declarations
 * **************************************************************************************/

// Additional assertions
#define EXTRA_ORDINARY_CHECKING				0

#if EXTRA_ORDINARY_CHECKING == 0
#undef EXTRA_ORDINARY_CHECKING
#endif

class Grid;

/* **************************************************************************************
 * Interface of Toppling
 * **************************************************************************************/

/**
 * Toppling according to the scientists in the corresponding papers:
 * - Manna_Lin2010
 *     "Renormalization-group approach to the Manna sandpile"
 *     (conserving, stochastic)
 * - Bak_Tang_Wiesenfeld1987
 *     "Self-organized criticality: An explanation of the 1/f noise"
 *     (conserving, deterministic)
 * - Lin_etal2006
 *     "Effects of bulk dissipation on the critical exponents of a sandpile"
 *     (dissipating, stochastic of course)
 */
enum TopplingMethod { TM_UNDEFINED, Manna_Lin2010, Bak_Tang_Wiesenfeld1987, Lin_etal2006, Rossum2011, Rossum2011_diss };

/**
 * There are several ways to iterate over the grid and updating the values of each cell:
 * - RANDOM_ALL
 *    just pick a site at random and update its value and that of its neighbours
 * - FOLLOW_ACTIVITY
 *    maintain a list of active sites and remove sites from the list if their values does
 *    not change anymore (can end up in infinite loop if there is e.g. no dissipation)
 */
enum TopplingIterator { RANDOM_ALL, RANDOM_FRACTION, FOLLOW_ACTIVITY };

std::ostream& operator<<( std::ostream& os, const TopplingMethod& method);


/**
 * Goes over a sand_grid and topples according to a certain scheme.
 */
class Toppling {
public:
	//! Constructor Toppling
	Toppling(Grid *grid);

	//! Destructor ~Toppling
	virtual ~Toppling();

	//! Do the action
	void Topple(long int & avalanche_size);

	//! Activate or deactivate cell
	void CheckCell(Cell & cell);

	//! Set toppling method
	void SetTopplingMethod(TopplingMethod toppling_method);

	//! Set iterator
	void SetTopplingIterator(TopplingIterator toppling_iterator);

	//! Overwrite toppling threshold
	void SetTopplingThreshold(GrainType threshold);

	//! Set dissipative mode
	void SetDissipativeMode(bool mode);

	//! PFT_Dissipation grid
	inline void SetDissGrid(Grid &grid) { diss_grid = &grid; }

	//! Define if we actually will count the grains during toppling
	void SetCounterDuringAvalanches(bool count);

	//! Event counter to count number of grains during avalanches...
	inline EventCounter<int> *GetNoDuringAvalanches() { return noDuringAvalanches; };

	//! Set feed for grid (in RANDOM_ALL mode we go over all cells in random order)
	inline static void SetGridFeed(int feed) { grid_feed = feed; }

	//! Set feed for neighbour order and dissipation
	inline static void SetTopplingFeed(int feed) { toppling_feed = feed; }

	//! Get feed for grid for Boost randomizer
	inline static int GetGridFeed() { return grid_feed; }

	//! Get feed for toppling
	inline static int GetTopplingFeed() { return toppling_feed; }

	//! Count the number of cells just below threshold
	long int CountCriticalCells();

	//! Get topple threshold
	inline GrainType GetToppleThreshold() { return topple_threshold; }

	//! Set dissipation threshold
	inline void SetDissipationThreshold(GrainType th) { diss_threshold = th; }

	//! Set dissipation rate
	inline void SetDissipationRate(double rate) { diss_rate = rate; }

	//! Set dissipation amount of energy / number of grains
	inline void SetDissipationAmount(GrainType amount) { diss_amount = amount; }

	//! Get dissipation amount
	inline GrainType GetDissipationAmount() { return diss_amount; }

	//! Set dissipation cell capacity
	void SetCellCapacity(GrainType capacity);
protected:
	//! Topple specific cell
	bool Topple(Cell & cell, std::vector<Cell*> & neighbours);
private:
	//! Reference to sand_grid
	Grid *sand_grid;

	//! Reference to energy sand_grid
	Grid *diss_grid;

	//! Events during avalanches
	EventCounter<int> * noDuringAvalanches;

	//! It is expensive to count, so we should be able to turn it off
	bool countDuringAvalanches;

	//! Active cells
	std::set<Cell*> active_cells;

	//! Threshold
	GrainType topple_threshold;

	//! Turn on/off dissipative mode if possible in a model
	bool dissipative_mode;

	//! Dissipation rate
	double diss_rate;

	//! Dissipation threshold (Rossum_diss type of toppling)
	GrainType diss_threshold;

	//! The number of grains / amount of energy dissipated to neighbours
	GrainType diss_amount;

	//! Toppling method
	TopplingMethod toppling_method;

	//! Toppling iterator
	TopplingIterator toppling_iterator;

	//! An array with random_indices that is randomly shuffled all the time
	int *random_indices;

	//! Toppling feed
	static int toppling_feed;

	//! Grid feed
	static int grid_feed;

};


#endif /* TOPPLING_H_ */
