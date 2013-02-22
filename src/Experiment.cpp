/**
 * @file Experiment.cpp
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
 * @date	Jul 29, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	
 */


// General files
#include <iostream>
#include <map>

#include <Experiment.h>
#include <SandPile.h>
#include <Time.h>

using namespace std;

/* **************************************************************************************
 * Implementation of Experiment
 * **************************************************************************************/

/**
 * Create a new experiment, by creating a new sandpile and the corresponding counters for
 * plotting.
 */
Experiment::Experiment(Config & cfg): config(cfg) {
	counters.clear();

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

	sandpile = new SandPile(config.system_size, config.toppling_method, config.boundary_type);

//	cout << "config.dissipation_total = " << config.dissipation_total << endl;
//	cout << "config.dissipation_cell_capacity = " << config.dissipation_cell_capacitity << endl;
	sandpile->GetToppling()->SetDissipativeMode(config.dissipative_mode);
	sandpile->GetToppling()->SetDissipationThreshold(config.dissipation_threshold);
	sandpile->GetToppling()->SetTopplingThreshold(config.toppling_threshold);
	sandpile->GetToppling()->SetCellCapacity(config.toppling_threshold * 4);
	sandpile->GetToppling()->SetDissipationRate(config.dissipation_rate);
	sandpile->GetToppling()->SetDissipationAmount(config.dissipation_amount);

	if (sandpile->GetDissToppling())
		sandpile->GetDissToppling()->SetCellCapacity(config.dissipation_cell_capacitity);

	counters.insert(make_pair<PlotFigureType,EventCounter<CounterType> *>(
			PFT_GrainsBeforeAvalanche,new EventCounter<CounterType>()));
	counters.insert(make_pair<PlotFigureType,EventCounter<CounterType>*>(
			PFT_GrainsDiffAvalanche,new EventCounter<CounterType>()));
	counters.insert(make_pair<PlotFigureType,EventCounter<CounterType>*>(
			PFT_GrainsPerCell,new EventCounter<CounterType>()));
	counters.insert(make_pair<PlotFigureType,EventCounter<CounterType>*>(
			PFT_Avalanche,new EventCounter<CounterType>()));
	counters.insert(make_pair<PlotFigureType,EventCounter<CounterType>*>(
			PFT_CriticalCells,new EventCounter<CounterType>()));

//	counters.insert(make_pair<PlotFigureType,EventCounter<CounterType>*>(
//			PFT_GrainsDuringAvalanche,sandpile->GetGrainsDuringAvalanches()));

	// After random generator initialisation
	if (sandpile->GetDissToppling())
		sandpile->Populate(config.dissipation_total/5, 5);

}

/**
 * Delete all the counters except for the "during avalanche" one, because that one
 * is part of the sandpile.
 */
Experiment::~Experiment() {
	std::map<PlotFigureType,EventCounter<CounterType>*>::iterator i;
	for (i = counters.begin(); i != counters.end(); ++i) {
		if (i->first != PFT_GrainsDuringAvalanche)
			delete i->second;
	}
	counters.clear();

	// Destruction...
	delete sandpile;

}

/**
 * Individual tick. In the sandpile case this is dropping a grain and calculating the
 * subsequent avalanche with some additional calculations for plotting.
 */
void Experiment::Tick(long int t) {
	std::map<PlotFigureType,EventCounter<CounterType>*>::const_iterator i;
	std::map<PlotFigureType,EventCounter<CounterType>*>::const_iterator j;
	std::map<PlotFigureType,EventCounter<CounterType>*>::const_iterator k;
	std::map<PlotFigureType,EventCounter<CounterType>*>::const_iterator l;
	std::map<PlotFigureType,EventCounter<CounterType>*>::const_iterator m;

	int L2 = config.system_size * config.system_size;
	// Do we need to calculate grains before the avalanche?
	bool calculate_before = false;
	i = counters.find(PFT_GrainsBeforeAvalanche);
	if (i != counters.end()) calculate_before = true;

	bool calculate_diff = false;
	j = counters.find(PFT_GrainsDiffAvalanche);
	if (j != counters.end()) calculate_diff = true;

	bool calculate_avalanches = false;
	k = counters.find(PFT_Avalanche);
	if (k != counters.end()) calculate_avalanches = true;

	bool calculate_critical_cells = false;
	l = counters.find(PFT_CriticalCells);
	if (l != counters.end()) calculate_critical_cells = true;

	bool calculate_grains_per_cell = false;
	m = counters.find(PFT_GrainsPerCell);
	if (m != counters.end()) calculate_grains_per_cell = true;

	// Drop grain
	sandpile->Drive();

	// Calculate number of grains before a possible avalanche
	long int no_grains_before;
	if (calculate_before || calculate_diff)
		sandpile->GetValue(no_grains_before, GVT_HEIGHT_SCALED);

	long int cells;
	if (calculate_critical_cells) {
		sandpile->GetValue(cells, GVT_CRITICAL_CELLS);
	}

	// Calculate the actual avalanche
	int avalanche_size = sandpile->Relax(t > config.skip);

	if (avalanche_size > 0) {
		if (calculate_before)
			i->second->AddEvent(no_grains_before / (double)L2);

		if (calculate_diff) {
			long int no_grains_after;
			sandpile->GetValue(no_grains_after, GVT_HEIGHT_SCALED);
			// shifted with one, so we can use log for display
			long int diff = no_grains_before - no_grains_after + 1;
			assert (diff >= 0);
			j->second->AddEvent(diff / (double)L2);
		}

		if (calculate_avalanches) {
			k->second->AddEvent(avalanche_size);
		}

		if (calculate_critical_cells) {
			long int cells_after;
			sandpile->GetValue(cells_after, GVT_CRITICAL_CELLS);
			// can also be negative! so don't use log
			long int diff = cells_after - cells;
			l->second->AddEvent(diff / (double)L2);
		}

		if (calculate_grains_per_cell) {
			int L = config.system_size;
			int patch_L = 1; // 4x4
			dp.len = (L*L) / (patch_L*patch_L);
			dp.values = new float[dp.len];
			sandpile->Coarsen(dp.values, dp.len, patch_L);

			float resolution = 1000;

			for (int i = 0; i < dp.len; ++i) {
				float val = (int)(dp.values[i] * resolution) / resolution;
				m->second->AddEvent(val);
			}
			delete [] dp.values;
		}

	}

	// Show progress with "ppm" files, these are no diagrams
	if (!(t % (config.timespan/config.no_pics))) {
		dp.len = config.system_size*config.system_size;
		dp.values = new float[dp.len];
		dp.time_id = t;
//		sandpile->GetValues(dp.values, GVT_HEIGHT_SCALED);
		sandpile->GetValues(dp.values, GVT_NCN);
		plot_figure.Draw(dp, config, PFT_Height);

		if (config.toppling_method == Rossum2011) {
			sandpile->GetValues(dp.values, GVT_DISSIPATION);
			plot_figure.Draw(dp, config, PFT_Dissipation);
		}
		delete [] dp.values;
	}
}

/**
 * Basically times everything and shows a "progress bar"...
 */
void Experiment::Trial(int trial) {

	// Perform the experiment
	sandpile->Clear();

	cout << "Progress [" << trial << "]: " << endl;
	timer.Start();
	for (long int t = 0; t < config.timespan; ++t) {
		Tick(t);
		if (!(t % (config.timespan/config.no_dots))) {
			cout << "."; flush(cout);
		}
	}
	// Flush the status bar
	cout << endl;

	// Measure the time the experiment took
	timer.Stop();
	timer.Print();
}

/**
 * Runs the experiment and returns a boolean expressing success. Trial and tick
 * do not return booleans but can convey status information using private fields if
 * they'd like to.
 */
bool Experiment::Run() {
	for (int trial = 0; trial < config.no_trials; trial++) {
		Trial(trial);
	}
	Plot();
	return true;
}

/**
 * Plot data from all "registered" counters.
 */
void Experiment::Plot() {
	std::map<PlotFigureType,EventCounter<CounterType>*>::iterator i;
	for (i = counters.begin(); i != counters.end(); ++i) {
		dp.events = &i->second->getEvents();
		dp.id = config.run_id;
		plot_figure.Draw(dp, config, i->first);
	}
}

