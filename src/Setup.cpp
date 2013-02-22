/**
 * @file Setup.cpp
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
 * @date	Aug 1, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	
 */


// General files
#include <Config.h>
#include <Persist.h>

#include <boost/bind.hpp>

using namespace std;
using namespace boost;

/* **************************************************************************************
 * Implementation of Setup
 * **************************************************************************************/

enum SetupConf { SC_DIFFERENT_TIMES, SC_DIFFERENT_SIZES, SC_DIFFERENT_DISS_TOTAL } ;

/**
 * The different ways in which we can alter a setup. What I want to find out is how the
 * density of grains before an avalanche commences changes with system size. The idea is
 * that with BTW this graph becomes "smaller" and "smaller". It is obvious from the plots
 * by the way that t=200,000 is too small for L=256. It is actually already too small for
 * L=128 and starts to become homogeneous around this time for L=64. However, for a
 * non-BTW system - which does not start with boundary driving - this time span might be
 * sufficient.
 */
void AlterConfig(Config &config) {
	cout << "Alter " << config.run_id << endl;
	config.run_experiment = true;
	config.toppling_method = Manna_Lin2010;
	config.toppling_method = Rossum2011;
	config.toppling_method = Lin_etal2006;
	config.dissipative_mode = true;
	config.system_size = 64;
	config.dissipation_cell_capacitity = 10;
	config.dissipation_threshold = 3;
	config.timespan = 1000000;
	SetupConf sc = SC_DIFFERENT_SIZES;
	config.dissipation_total = config.dissipation_cell_capacitity * config.system_size;
	config.dissipation_rate = 0.1; //001;

	// will be divided by 4, to transfer 0.2 grains to each neighbour
	config.dissipation_amount = 4; //0.8;

	// overwrite default boundary type by - four - dissipating boundaries
	config.boundary_type = BT_DISSIPATING;
//	config.boundary_type = BT_FULLY_CONNECTED;
//	config.boundary_type = BT_RANDOM_NEIGHBOURS;

	// Toppling threshold (multiple of 4)
	config.toppling_threshold = 4;//*5;

	// set it shorter temp
	config.timespan = 1000000;
	config.no_pics = 100;

	switch (sc) {
	case SC_DIFFERENT_TIMES:
		config.timespan = (config.run_id + 1) * 100000;
		break;
	case SC_DIFFERENT_SIZES:
		// 32, 64, 128, 256
		config.system_size = 1 << (config.run_id + 5);
		config.dissipation_total = config.dissipation_cell_capacitity * config.system_size;
//		config.system_size = 4 * (config.run_id + 4);
		break;
	case SC_DIFFERENT_DISS_TOTAL:
		// 1/4, 1/2, 1, 2, 4
		double factor = (1 << (config.run_id + 3)) / 32.0;
		cout << "Factor: " << factor << endl;
		config.dissipation_total = config.dissipation_cell_capacitity * config.system_size * factor;
	}
	// Skip a huge part of the start up process
	config.skip = config.timespan / 2;
//	config.skip = config.timespan - config.no_pics * 10;

	cout << "Take care that config.skip is large enough for your system size!" << endl;

	ostringstream t;

	std::map<PlotFigureType,FigureConfig>::iterator i;
	i = config.figures.find(PFT_GrainsBeforeAvalanche);
	if (i != config.figures.end()) {
		t.clear(); t.str("");
		t << "Grains before avalanches, model=" << config.toppling_method <<
				" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
		(*i).second.title = t.str();
	}

	i = config.figures.find(PFT_GrainsDuringAvalanche);
	if (i != config.figures.end()) {
		t.clear(); t.str("");
		t << "Grains during avalanches, model=" << config.toppling_method <<
				" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
		(*i).second.title = t.str();
	}

	i = config.figures.find(PFT_CriticalCells);
	if (i != config.figures.end()) {
		t.clear(); t.str("");
		t << "Critical cells, model=" << config.toppling_method <<
				" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
		(*i).second.title = t.str();
	}

	i = config.figures.find(PFT_GrainsPerCell);
	if (i != config.figures.end()) {
		t.clear(); t.str("");
		t << "Grains per cell, model=" << config.toppling_method <<
				" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
		(*i).second.title = t.str();
	}

	i = config.figures.find(PFT_Avalanche);
	if (i != config.figures.end()) {
		t.clear(); t.str("");
		t << "Avalanches, model=" << config.toppling_method <<
				" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
		(*i).second.title = t.str();
	}

	config.Print();
}

/**
 * The main function
 */
int main(int argc, char* argv[]) {
	cout << "Start setup" << endl;
	Persist persist(argc, argv);

	AlterConfigFunc alter (boost::bind(&AlterConfig, _1));
	persist.SetAlterConfigFunc(alter);

	if (persist.GetConfig().run_id != -1) {
		cerr << "Run id specified on CLI, probably you do not want to setup!" << endl;
		return EXIT_FAILURE;
	}

	int runs = 5;
	for (int i = 0; i < runs; ++i) {
		// set id so we do not load it again, but create a new run
		persist.GetConfig().run_id = -1;
		persist.Start();
	}

	return EXIT_SUCCESS;
}

