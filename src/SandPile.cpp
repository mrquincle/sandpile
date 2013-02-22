/**
 * @file SandPile.cpp
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
#include <SandPile.h>

#include <boost/random/uniform_int.hpp>
#include <boost/bind.hpp>
#include <boost/random/uniform_01.hpp>

using namespace std;
using namespace boost;

/* **************************************************************************************
 * Implementation of SandPile
 * **************************************************************************************/

int SandPile::diss_feed = 1233480;

int SandPile::drive_feed = 233480;

/**
 * System size is denoted by L in statistical physics literature. The constructor creates one or
 * two grids depending on the toppling method. In case of the latter DissipationGrid() is called.
 */
SandPile::SandPile(int L, TopplingMethod toppling_method, BoundaryType type) {
	this->L = L;

	switch (toppling_method) {
	case Rossum2011:
	case Rossum2011_diss:
		boundary_type = BT_PERIODIC;
		break;
	case Lin_etal2006:
		boundary_type = BT_PERIODIC;
		break;
	case Manna_Lin2010:
		boundary_type = BT_CIRCULAR;
		break;
	case Bak_Tang_Wiesenfeld1987:
		boundary_type = BT_WALL_DISSIPATING;
		break;
	default:
		cerr << "Unknown toppling method. So what is the boundary condition?" << endl;
	}

	if ((type != BT_UNDEFINED)  && (type != boundary_type)) {
		cerr << "Warning, overwriting default boundary type \"" << boundary_type
				<< "\" by \"" << type << "\"" << endl;
		boundary_type = type;
	} else {
		cout << "Standard boundary type \"" << type << "\"" << endl;
	}

	// For testing the dissipation grid on itself (without sandpile)
	if (toppling_method == Rossum2011_diss) {
		toppling = NULL;
		grid = NULL;
		DissipationGrid(Rossum2011_diss, L, L);
		return;
	}

	// Create sand grid
	grid = new Grid(L, L, boundary_type);
	toppling = new Toppling(grid);
	toppling->SetTopplingMethod(toppling_method);
	toppling->SetTopplingIterator(FOLLOW_ACTIVITY);
	toppling->SetCounterDuringAvalanches(false);

	// Set callback function for every cell in the grid
	for (int i = 0; i < L*L; ++i) {
		AlteredCallback callback (boost::bind(&Toppling::CheckCell, toppling, _1));
		grid->GetCell(i).SetAlteredFunction(callback);
	}

	diss_grid = NULL;
	diss_toppling = NULL;
	if (toppling_method == Rossum2011) {
		DissipationGrid(Rossum2011_diss, L, L);
	}
}

/**
 * Only called when there is actually a dissipation grid needed. It creates a separate
 * grid and a distinct toppling object.
 */
void SandPile::DissipationGrid(TopplingMethod method, int width, int height) {
	// We only know one toppling method for dissipation
	assert (method = Rossum2011_diss);

	// Create dissipation grid
	diss_grid = new Grid(width, height, BT_PERIODIC);
	if (toppling != NULL)
		toppling->SetDissGrid(*diss_grid);

	// Fill cells in dissipation grid with
	diss_toppling = new Toppling(diss_grid);
	diss_toppling->SetTopplingMethod(method);
	diss_toppling->SetTopplingIterator(RANDOM_ALL);
	diss_toppling->SetCounterDuringAvalanches(false);

	// some tests...
#ifdef LINE
	// create just one line of dissipation
	for (int i = 0; i < L; ++i) {
		diss_grid->GetCell(i,0).Increase(3);
	}
#endif

#ifdef SQUARE
	// create a square in the middle
	int s = L / 10;
	assert (s > 1);
	for (int j = L/2-s; j < L/2+s; ++j) {
		for (int i = L/2-s; i < L/2+s; ++i) {
			diss_grid->GetCell(i,j).Increase(3);
		}
	}
#endif
}

/**
 * This function is only meant for the dissipation grid, not for the default sandpile
 * grid.
 * There should be L spots with max_particles, so no. of particles scales with system size
 */
void SandPile::Populate(int no_cells, GrainType no_particles) {
	if (!diss_grid) {
		cerr << "Do not populate if there is no dissipation grid defined" << endl;
		return;
	}

	static boost::mt19937 randomGenerator(diss_feed);
	assert (no_cells < L*L);
	double place = no_cells/(double)(L*L);
	cout << "Place if zerone() < " << place << endl;
	long int sum = 0;
	for (int i = 0; i < L*L; ++i) {
		static boost::uniform_01<boost::mt19937> zeroone(randomGenerator);
		bool dissipate = (zeroone() < place);
		if (dissipate) {
			diss_grid->GetCell(i).Increase(no_particles);
			sum += no_particles;
		} else
			diss_grid->GetCell(i).Clear();
	}
	// a minimum number of particles is needed...
	assert (sum > 10);
	cout << "Added " << sum << " particles to dissipation grid" << endl;
}

/**
 * Destroy what is created before...
 */
SandPile::~SandPile() {
	if (grid != NULL) delete grid;
	if (diss_grid != NULL) delete diss_grid;
	if (toppling != NULL) delete toppling;
}

/**
 * Clean the sand
 */
void SandPile::Clear() {
	for (int i = 0; i < L*L; ++i) {
		grid->GetCell(i).Clear();
	}
}

/**
 * Adds one "grain" to a random position on the sand_grid. In case of circular boundary
 * it is important not to drop it somewhere else... We only return when we successfully
 * dropped a grain in the designated area.
 */
void SandPile::Drive() {
	static boost::mt19937 randomGenerator(drive_feed);
	assert (grid != NULL);

	uniform_int<size_t> dist_x(0, grid->GetWidth()-1);
	uniform_int<size_t> dist_y(0, grid->GetHeight()-1);

	bool success = false;

	do {
		int x = dist_x(randomGenerator);
		int y = dist_y(randomGenerator);
		if (boundary_type == BT_CIRCULAR) {
			// only within the circle
			if (grid->WithinCircle(x, y)) {
				success = true;
				grid->GetCell(x,y).Increase(1);
			}
		} else if (boundary_type == BT_WALL_DISSIPATING) {
			// only at the wall... but doesn't seem to matter
			if (y < grid->GetHeight() / 2)
				grid->GetCell(x,0).Increase(1);
			else
				grid->GetCell(0,x).Increase(1);
			success = true;
		} else {
			// totally random spot
			grid->GetCell(x,y).Increase(1);
			success = true;
		}
	} while (!success);
}

/**
 * The relaxation process "calculates" all the avalanches till all activity ceases.
 */
int SandPile::Relax(bool measure) {
	long int avalanche_size = 0;

	// Dissemination in dissipation grid
	if (diss_toppling != NULL)
		diss_toppling->Topple(avalanche_size);

	// Dissemination in default grain grid
	if (toppling != NULL)
		toppling->Topple(avalanche_size);

	if ((avalanche_size > 0) && measure) {
		avalanches.AddEvent(avalanche_size);
		return avalanche_size;
	}

	return 0;
}

/**
 * Get values that seem to be relevant for debugging or (scientific) insight. With
 * the Plot class, they can be easily plotted in the form of a .ppm file. Very useful
 * to keep track of the heights of all cells at once. Or to see the structure of the
 * dissipation regions.
 */
void SandPile::GetValues(float *values, const GridValueType gvt) {
	vector<Cell*> neighbours;
	for (int i = 0; i < L*L; ++i) {
		switch (gvt) {
		case GVT_HEIGHT_SCALED:
			values[i] = grid->GetCell(i).GetHeight() / (float)grid->GetCell(i).GetMaxCapacity();
			break;
		case GVT_HEIGHT:
			values[i] = grid->GetCell(i).GetHeight();
			break;
		case GVT_NCN:
			neighbours.clear();
			grid->GetNeighbours(i % grid->GetWidth(), i / grid->GetWidth(), neighbours);
			values[i] = 0;
			for (unsigned int n = 0; n < neighbours.size(); ++n) {
				if (neighbours[n]->GetHeight() >= toppling->GetToppleThreshold() - toppling->GetDissipationAmount() / neighbours.size()) values[i] = 1.0; //++;
//				if (neighbours[n]->GetHeight() >= toppling->GetToppleThreshold()) values[i] = 1.0; //++;
			}
//			values[i] = values[i] / neighbours.size();
			break;
		case GVT_CRITICAL_CELLS:
			values[i] = (grid->GetCell(i).GetHeight() >= (toppling->GetToppleThreshold() - toppling->GetDissipationAmount() / neighbours.size()) ? grid->GetCell(i).GetMaxCapacity() : 0);
			break;
		case GVT_DISSIPATION:
			if (diss_grid == NULL) {
				cerr << __FUNCTION__ << ": There is no dissipation grid!" << endl;
				assert (false);
			}
			//			if ((!i % L)) cout << endl;
			//			cout << diss_grid->GetCell(i).GetHeight() << " ";
			values[i] = diss_grid->GetCell(i).GetHeight() / (float)diss_grid->GetCell(i).GetMaxCapacity();
			break;
		case GVT_DIRECTION:
			if (diss_grid == NULL) {
				cerr << __FUNCTION__ << ": There is no dissipation grid!" << endl;
				assert (false);
			}
			values[i] = diss_grid->GetCell(i).GetDirection() / (float)4;
			break;
		}
			}
	// we need something special here, we use the top-left for largest scale values
	if (gvt == GVT_ORDERPARAM1) {
//		values
	}
}

/**
 * Fill the given array of values with the number of grains in "patches".
 */
void SandPile::Coarsen(float *values, int array_size, int patch_L) {
	if (patch_L == 1) {
		GetValues(values, GVT_HEIGHT);
		return;
	}
	assert (array_size == ((L*L) / (patch_L*patch_L)));
	assert ((L % patch_L) == 0);

	int patch_width = L / patch_L;

	for (int j = 0; j < L; j+=patch_L) {
		for (int i = 0; i < L; i+=patch_L) {
			int sum = 0;
			for (int q = 0; q < patch_L; ++q) {
				for (int p = 0; p < patch_L; ++p) {
					// cout << "GetCell " << i << "+" << p << "," << j << "+" << q << endl;
					sum += grid->GetCell(i+p,j+q).GetHeight();
				}
			}
			values[i/patch_L + (j/patch_L)*patch_width] = sum;
		}
	}
}

/**
 * Get average/total value. Maybe "misuse" grid->reservoir cell. However... this does for
 * most models only capture boundary dissipation. Each time bulk dissipation occurs it is
 * just by decrementing a cell more than incrementing its neighbours, and this is nowhere
 * stored. Hence, for slower but more faithful execution use CountGrains for now.
 */
void SandPile::GetValue(long int &value, const GridValueType gvt) {
	switch(gvt) {
	case GVT_HEIGHT_SCALED:
		value = grid->CountGrains();
		break;
	case GVT_CRITICAL_CELLS:
		value = toppling->CountCriticalCells();
		break;
	case GVT_DISSIPATION: //todo
		if (diss_grid == NULL) {
			cerr << "There is no dissipation grid!" << endl;
			assert (false);
		}
		value = -1;
		break;
	case GVT_DIRECTION: //todo?
		if (diss_grid == NULL) {
			cerr << "There is no dissipation grid!" << endl;
			assert (false);
		}
		value = -1;
		break;
	case GVT_HEIGHT:
	case GVT_NCN:
		cerr << "Not implemented as average" << endl;
		assert (false);
		break;
	}
}

/**
 * Print whatever is relevant for now...
 */
void SandPile::Print() {
	//	avalanches.Print(0);
	if (diss_grid != NULL) diss_grid->Print();
	//	avalanches.Print(1);
}
