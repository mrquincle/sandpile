/**
 * @file Main.cpp
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
#include <Config.h>
#include <Persist.h>
#include <Experiment.h>

using namespace std;

/* **************************************************************************************
 * Implementation of Main
 * **************************************************************************************/


/**
 * Main function initializes the sand pile and drives it over a certain time
 * span, after each "loading" call, the system is also relaxed. At relaxation the
 * avalanche size is stored.
 */
int main(int argc, char* argv[]) {
	Persist persist(argc, argv);
	bool success = persist.Start();
	if (!success) {
		return EXIT_FAILURE;
	}

	cout << "Start SandPile " << endl;
	Config & config = persist.GetConfig();
	config.Print();

	if (config.run_experiment) {
		cout << "Run experiment" << endl;
		Experiment experiment(config);
		experiment.Run();
	} else {
		PlotFigure pf;
		pf.DrawAgain(config);
//		pf.DrawAll(config);

	}
	// We did run the experiment, so tell that to the configuration file
	if (config.run_experiment) {
		config.run_experiment = false;
		persist.StoreConfig();
	}

	cout << "Stop SandPile" << endl;
	return EXIT_SUCCESS;
}

