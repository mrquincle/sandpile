/**
 * @file Persist.cpp
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
#include <Persist.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::program_options;
using namespace boost::filesystem;

/* **************************************************************************************
 * Implementation of Persist
 * **************************************************************************************/

/**
 * Persist uses the serialisation library from boost to load/store configuration options.
 */
Persist::Persist(int argc, char *argv[]): config(*new Config()),
		default_option_file("global_config.ini"),
		alter_function(NULL),
		run_option_file("config.ini") {

	options_description desc("Allowed options");
	desc.add_options() (
			"help,?", "produce help message")
			("run", value<int>(), "previous run \"arg\" (\"arg\" is a number)")
			;

	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);
	state_okay = true;
	if (vm.count("help")) {
		cout << desc << endl;
		state_okay = false;
		return;
	}

	config.run_id = -1;
	if (vm.count("run")) {
		int index = vm["run"].as<int>();
		cout << "Run experiment " << index << " (should have \"" << run_option_file << "\" file)\n";
		config.run_id = index;
	}
}

/**
 * For the persistence class you will only need the constructor and this function normally.
 * If the argument parsing in the constructor went properly we continue here to check if we
 * had a variable "run" set to a specific value. In that case we load a run from the
 * corresponding directory. If it is not set, we load the defaults from the main directory
 * and create a directory for a new run. In that directory we store the configuration, so we
 * can rerun everything if we need to.
 */
bool Persist::Start() {
	if (!state_okay) return false;

	if (config.run_id >= 0) {
		return LoadConfig();
	} else {
		// only very first time (without even a default configuration file)
		if (!LoadDefaults()) StoreDefaults();
		// create a new directory
		config.run_id = CreateDir();
		// we allow for adaptations per run
		AlterConfig();
		// store defaults in new directory
		StoreConfig();
	}
	return true;
}

/**
 * Default destructor
 */
Persist::~Persist() { }

/**
 * Load ini files with the program options lib (in contrast with the serialisation lib).
 */
void Persist::LoadIni() {
	options_description file("Configuration");
	file.add_options() (
			"L", value<int>(), "system size")
			("toppling_method", value<int>(), "toppling method")
			("timespan", value<long int>(), "time span")
			("no_trials", value<int>(), "number of trials")
			("skip", value<int>(), "skip counting/visualising for first ticks")
			("no_pics", value<int>(), "number of ticks before we take a picture")
			("no_dots", value<int>(), "number of dots in progress bar")
			//	std::vector<FigureConfig> figures;
			;

	ifstream in( "sandpile.cfg" );
	store(parse_config_file(in, file), vm);
	notify(vm);

	if (vm.count("L")) {
		int size = vm["L"].as<int>();
		cout << "Size " << size << endl;
		config.system_size = size;
	}

	if (vm.count("toppling_method")) {
		TopplingMethod tm = (TopplingMethod)vm["toppling_method"].as<int>();
		config.toppling_method = tm;
	}

}

/**
 * Time span of 1 million and 128x128 cells takes to simulate 45 minutes
 *              100,000 and 64x64 cells takes 40 seconds
 * BTW use a time span of 100.000 for 50x50 cells, but how many times!?
 */
void Persist::StoreDefaults() {
	config.timespan = 100000;
	config.system_size = 32;
	config.no_dots = 100;
	config.no_pics = 10;
	config.no_trials = 1;
	config.toppling_method = Lin_etal2006;
	config.boundary_type = BT_UNDEFINED;
	config.toppling_threshold = -1;
	config.dissipative_mode = true;
	config.dissipation_rate = 0.1;
	config.dissipation_threshold = 3;
	config.dissipation_cell_capacitity = 10;
	config.dissipation_amount = -1; //default
	config.dissipation_total = config.system_size * config.dissipation_cell_capacitity;
	config.run_experiment = true;
	config.run_id = 0;
	config.figures.clear();
	config.feeds.clear();

	FigureConfig fc;
	PlotFigureType pft;
	ostringstream t;

	pft = PFT_GrainsDuringAvalanche;
	fc.filename = "during_avalanches";
	t.clear(); t.str("");
	t << "Grains during avalanches, model=" << config.toppling_method <<
			" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
	fc.title = t.str();
	fc.x_axis = "Number of grains (E)";
	fc.y_axis = "P(E)";
	fc.plot_mode = PM_DEFAULT;
	fc.plot_type = PT_CUMULATIVE_DENSITY;
	fc.output_type = PL_GRAPH;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	pft = PFT_GrainsBeforeAvalanche;
	fc.filename = "before_avalanches";
	t.clear(); t.str("");
	t << "Grains before avalanches, model=" << config.toppling_method <<
			" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
	fc.title = t.str();
	fc.x_axis = "Number of grains (E)";
	fc.y_axis = "P(E)";
	fc.plot_mode = PM_DEFAULT;
	fc.plot_type = PT_CUMULATIVE_DENSITY;
	fc.output_type = PL_GRAPH;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	pft = PFT_CriticalCells;
	fc.filename = "critical_cells";
	t.clear(); t.str("");
	t << "Critical cells, model=" << config.toppling_method <<
			" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
	fc.title = t.str();
	fc.x_axis = "Number of critical cells (C)";
	fc.y_axis = "P(C)";
	fc.plot_mode = PM_DEFAULT;
	fc.plot_type = PT_CUMULATIVE_DENSITY;
	fc.output_type = PL_GRAPH;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	pft = PFT_GrainsPerCell;
	fc.filename = "grains_per_cell";
	t.clear(); t.str("");
	t << "Grains per cell, model=" << config.toppling_method <<
			" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
	fc.title = t.str();
	fc.x_axis = "Grains per cell (G)";
	fc.y_axis = "#";
	fc.plot_mode = PM_DEFAULT;
	fc.plot_type = PT_DEFAULT;
	fc.output_type = PL_GRAPH;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	pft = PFT_Avalanche;
	fc.filename = "avalanches";
	t.clear(); t.str("");
	t << "Avalanches, model=" << config.toppling_method <<
			" (L=" << config.system_size << ")" << " (T=" << config.timespan << ")";
	fc.title = t.str();
	fc.x_axis = "Avalanche size (S)";
	fc.y_axis = "P(S)";
	fc.plot_mode = PM_LOGLOG;
	fc.plot_type = PT_DEFAULT;
	fc.output_type = PL_GRAPH;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	pft = PFT_Height;
	fc.filename = "height";
	fc.title = "Height distribution over the grid";
	fc.x_axis = "";
	fc.y_axis = "";
	fc.plot_mode = PM_DEFAULT;
	fc.plot_type = PT_DENSITY;
	fc.output_type = PL_GRID;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	pft = PFT_Dissipation;
	fc.filename = "dissipation";
	fc.title = "Dissipation value distribution over the grid";
	fc.x_axis = "";
	fc.y_axis = "";
	fc.plot_mode = PM_DEFAULT;
	fc.plot_type = PT_DENSITY;
	fc.output_type = PL_GRID;
	config.figures.insert(std::make_pair<PlotFigureType,FigureConfig>(pft,fc));

	cout << "Store default options to " << default_option_file << endl;
	ofstream ofile;
	ofile.open(default_option_file.c_str());
	if (!ofile.is_open()) {
		cerr << "Could not store global configuration options!" << endl;
		return;
	}
	boost::archive::text_oarchive oa(ofile);
	oa << config;
}

/**
 * Load default configuration values. Returns false if we do not succeed in
 * opening the configuration file.
 */
bool Persist::LoadDefaults() {
	ifstream ifile;
	ifile.open(default_option_file.c_str());
	if (!ifile.is_open()) return false;
	boost::archive::text_iarchive oa(ifile);
	oa >> config;
	return true;
}

/**
 * Load configuration if indicated on command line.
 */
bool Persist::LoadConfig() {
	if (config.run_id < 0) {
		cerr << "Do not call LoadConfig if not asked for on command line" << endl;
		return false;
	}
	string c = lexical_cast<std::string>(config.run_id) + "/" + run_option_file;
	ifstream ifile;
	ifile.open(c.c_str());
	if (!ifile.is_open()) {
		cerr << "Could not load configuration options from \"" << c << "\"" << endl;
		return false;
	}
	boost::archive::text_iarchive oa(ifile);
	oa >> config;
	return true;
}

/**
 * Store configuration, it is loaded beforehand from the global configuration file.
 */
void Persist::StoreConfig() {
	config.feeds.clear();
	srand(time(NULL));
	int no_feeds = 10; // that should be enough feeds for now
	for (int i = 0; i < no_feeds; ++i) {
		int f = rand();
		config.feeds.push_back(f);
	}

	string c = lexical_cast<std::string>(config.run_id) + "/" + run_option_file;
	ofstream ofile;
	ofile.open(c.c_str());
	if (!ofile.is_open()) {
		cerr << "Could not store configuration options in \"" << c << "\"" << endl;
		return;
	}
	std::map<PlotFigureType,FigureConfig>::iterator i;
	for (i = config.figures.begin(); i != config.figures.end(); ++i) {
		i->second.path = lexical_cast<std::string>(config.run_id) + "/";
	}
	boost::archive::text_oarchive oa(ofile);
	oa << config;
}

/**
 * Alter configuration, can be hooked up to a user defined function using the
 * boost library with "bind".
 */
void Persist::AlterConfig() {
//	config.run_experiment = false; // by default make run_experiment false
	if (alter_function != NULL) alter_function(config);
}

/**
 * Create a directory for a new run.
 */
int Persist::CreateDir() {
	// Create directory for the output
	bool created = false;
	int index = 0;
	string work_dir;
	do {
		stringstream s; s << index++ << "/";
		work_dir = s.str();
		path dir = work_dir;
		if (!exists(dir)) {
			create_directories(s.str());
			created = true;
		}
	} while (!created);
//	cout << "Created directory " << work_dir << endl;
	return index - 1;
}

