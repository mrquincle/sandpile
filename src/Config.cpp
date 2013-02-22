/**
 * @file Config.cpp
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
 * @date	Jul 28, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */


// General files
#include <Config.h>
#include <Toppling.h>

using namespace std;

/* **************************************************************************************
 * Implementation of Config
 * **************************************************************************************/

string & FigureConfig::GetDescription() {
	string & type = *new string();
	type = title;
	return type;
}

/**
 * Config whatever might seem relevant to the user.
 */
void Config::Print() {
	cout << "[*] Toppling method: " << toppling_method << endl;

	cout << "[*] Boundary Type: " << boundary_type << endl;

	cout << "[*] Dissipation: " << (dissipative_mode ? "yes" : "no") << endl;

	// If there is dissipation show relevant parameters
	if (dissipative_mode) {
		cout << "[*] Dissipation rate: " << dissipation_rate << endl;
	}

	// If there is a separate dissipation grid
	if ((toppling_method == Rossum2011) || (toppling_method == Rossum2011_diss)) {
		cout << "[*] Dissipation cell capacity: " << dissipation_cell_capacitity << endl;
		cout << "[*] Dissipation total capacity: " << dissipation_total << endl;
		cout << "[*] Dissipation threshold: " << dissipation_threshold << endl;
	}

	cout << "[*] Toppling threshold: ";
	if (toppling_threshold < 0) cout << "default" << endl;
	else cout << toppling_threshold << endl;

	cout << "[*] Skip the first " << skip << " items" << endl;

	cout << "[*] Number of pictures will be " << no_pics << endl;

	cout << "[*] Timespan: " << timespan << " (drops of one grain)" << endl;

	cout << "[*] System size (L): " << system_size << endl;

	cout << "[*] Run id: " << run_id << endl;

	cout << "[*] Run experiment? " << (run_experiment ? "yes" : "no") << endl;

	std::map<PlotFigureType,FigureConfig>::iterator i;
	for (i = figures.begin(); i != figures.end(); ++i) {
		cout << "[*] Figure: " << i->second.GetDescription() << endl;
	}

	cout << "Progress \"bar\" size:" << endl;
	for (int i = 1; i <= no_dots; ++i) cout << ".";
	cout << endl;

}
