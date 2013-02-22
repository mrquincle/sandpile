/**
 * @file SandPile.h
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


#ifndef SANDPILE_H_
#define SANDPILE_H_

// General files
#include <Grid.h>
#include <Toppling.h>
#include <EventCounter.hpp>

#include <boost/random/mersenne_twister.hpp>

/* **************************************************************************************
 * Interface of SandPile
 * **************************************************************************************/

/**
 * Different types of values that can be requested from the sandpile.
 * GVT_HEIGHT:				height of a cell
 * GVT_HEIGHT_SCALED:		height of a cell divided by its maximum capacity
 * GVT_CRITICAL_CELLS:		number of critical cells in the grid
 * GVT_DISSIPATION:			"height" of a cell in dissipation grid
 * GVT_DIRECTION:			"direction" of a cell in dissipation grid
 * GVT_NCN:					non-critical neighbourhood (none of the neighbours are critical)
 * GVT_ORDERPARAM1:			order parameter 1, row sums, then sum_ij { r_i r_j |i-j| } plus same for column sums
 * ...
 * GVT_NOF_TYPES:			total GridValueType
 */
enum GridValueType { GVT_HEIGHT, GVT_HEIGHT_SCALED, GVT_CRITICAL_CELLS, GVT_DISSIPATION,
	GVT_DIRECTION, GVT_NCN, GVT_ORDERPARAM1, GVT_NOF_TYPES };

/**
 * The sandpile has access to a sand_grid with a specific topology. Most commonly a rectangular
 * sand_grid. In each sand_grid cell there are a number of grains, called the "height". There are
 * toppling rules defined that takes as input the height of a cell and adjust subsequently
 * the height of that cell and of its neighbours.
 */
class SandPile {
public:
	//! Constructor SandPile
	SandPile(int L, TopplingMethod toppling_method, BoundaryType type = BT_UNDEFINED);

	//! Destructor ~SandPile
	virtual ~SandPile();

	//! Populate with a certain number of particles
	void Populate(int no_cells, GrainType no_particles);

	//! Set feed for driving
	inline static void SetDriveFeed(int feed) { drive_feed = feed; }

	//! Set feed for dissipation
	inline static void SetDissipationFeed(int feed) { diss_feed = feed; }

	//! Loading mechanism, pick random spot and add a grain
	void Drive();

	//! Relax, measure/store the avalanche size and return it
	int Relax(bool measure=true);

	//! Clear
	void Clear();

	//! Print
	void Print();

	//! Coarsen e.g. height field into patches of size patch_L * patch_L
	void Coarsen(float *values, int array_size, int patch_L);

	//! The number plus size of avalanches
	std::map<int, int> & GetAvalanches() { return avalanches.getEvents(); }

	//! Get values for display
	void GetValues(float *values, const GridValueType gvt);

	//! Get certain general/average values
	void GetValue(long int &value, const GridValueType gvt);

	//! Number of grains during avalanches...
	inline EventCounter<int> *GetGrainsDuringAvalanches() { return toppling->GetNoDuringAvalanches(); }

	//! Get toppling
	inline Toppling *GetToppling() { return toppling; };

	//! Get toppling on dissipation grid
	inline Toppling *GetDissToppling() { return diss_toppling; };
protected:
	//! Create and use a dissipation grid
	void DissipationGrid(TopplingMethod method, int width, int height);

private:
	//! System size
	int L;

	//! The sand_grid upon which the sandpile is defined
	Grid * grid;

	//! An additional grid that stores e.g. an "energy-like" field separate from the grains
	Grid * diss_grid;

	//! The toppling procedure for grains
	Toppling *toppling;

	//! The toppling procedure for energy/dissipation
	Toppling *diss_toppling;

	//! Boundary type used for sand_grid
	BoundaryType boundary_type;

	//! PFT_Avalanche counter
	EventCounter<int> avalanches;

	//! Dissipation grid feed
	static int diss_feed;

	//! Driving feed for dropping random grains on grid
	static int drive_feed;
};

#endif /* SANDPILE_H_ */
