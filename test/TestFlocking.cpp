/**
 * @file TestFlocking.cpp
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
 * @date	Oct 27, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */

#include <Config.h>
#include <Persist.h>
#include <SandPile.h>
#include <EventCounter.hpp>
#include <PlotFigure.h>
#include <Time.h>

/* **************************************************************************************
 * Test of flocking part of algorithm (dissipation grid)
 * **************************************************************************************/

using namespace std;

int main(int argc, char* argv[]) {
	Persist persist(argc, argv);

	bool success = persist.Start();
	if (!success) {
		return EXIT_FAILURE;
	}

	cout << "Start TestFlocking " << endl;
	Config & config = persist.GetConfig();

	// overwrite by Rossum2011_diss for TestFlocking
	config.toppling_method = Rossum2011_diss;
	config.boundary_type = BT_PERIODIC;
    config.timespan = 10000;
    config.system_size = 256;
    config.dissipation_cell_capacitity = 8;
    config.dissipation_total = config.system_size * config.dissipation_cell_capacitity;
    config.no_pics = 100;
//    config.figures.clear();

	config.Print();

	if (config.feeds.size() >= 6) {
		Toppling::SetGridFeed(config.feeds[0]);
		Toppling::SetTopplingFeed(config.feeds[1]);
		Cell::SetDirectionFeed(config.feeds[2]);
		SandPile::SetDriveFeed(config.feeds[3]);
		SandPile::SetDissipationFeed(config.feeds[4]);
		Grid::SetNeighbourFeed(config.feeds[5]);
	} else {
		cerr << "Not enough feeds for random generators!" << endl;
	}

	SandPile * sandpile = new SandPile(config.system_size, config.toppling_method, config.boundary_type);


	assert (sandpile->GetDissToppling());

	sandpile->GetDissToppling()->SetCellCapacity(config.dissipation_cell_capacitity);
	// drop 5 grains in cells till we have the dissipation_total
	sandpile->Populate(config.dissipation_total/5, 5);

	DataForPlot dp;
	PlotFigure plot_figure;
	Time timer;

	cout << "Progress [" << 0 << "]: " << endl;
	timer.Start();
	for (long int t = 0; t < config.timespan; ++t) {

		sandpile->Relax(false);

		// Show progress with "ppm" files, these are no diagrams
		if (!(t % (config.timespan/config.no_pics))) {
			dp.len = config.system_size*config.system_size;
			dp.values = new float[dp.len];
			dp.time_id = t;

			sandpile->GetValues(dp.values, GVT_DISSIPATION);
			plot_figure.Draw(dp, config, PFT_Dissipation);
			delete [] dp.values;
		}
		if (!(t % (config.timespan/config.no_dots))) {
			cout << "."; flush(cout);
		}
	}
	// Flush the status bar
	cout << endl;

	// Measure the time the experiment took
	timer.Stop();
	timer.Print();

	// Destruction...
	delete sandpile;
}
