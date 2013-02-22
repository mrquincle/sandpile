/**
 * @file Persist.h
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


#ifndef PERSIST_H_
#define PERSIST_H_

// General files
#include <Config.h>

#include <boost/program_options.hpp>

/* **************************************************************************************
 * Interface of Persist
 * **************************************************************************************/

typedef boost::function<void(Config&)> AlterConfigFunc;

/**
 * Basically this class is meant to add some persistence to configuration values. There
 * are several things that come into mind:
 * - Program without arguments creates new unique subdir for output using global config,
 *     subdir should have an easy name, preferably a number
 * - Each run / subdirectory should store the date/time
 * - Program with number as argument should load configuration from that subdir and not
 *     run again or plot again
 * - Have a "write" flag in the config file that is set to "false" after the
 *     configuration has been loaded, so it is not run - by accident - twice
 * - Plot a figure again with e.g. differently scaled axes using the same titles, or
 *     plot the same data with different titles, new configs should be retrieve from file
 * - Plot a figure using data from multiple directories
 */
class Persist {
public:
	//! Constructor Persist
	Persist(int argc, char *argv[]);

	//! Destructor ~Persist
	virtual ~Persist();

	//! Run the persistence class, quit if it returns false
	bool Start();

	//! Return configuration unit/class
	inline Config &GetConfig() { return config; }

	//! Store default values, probably you only need to do this once
	void StoreDefaults();

	//! Load default values
	bool LoadDefaults();

	//! Store configuration values
	void StoreConfig();

	//! Load configuration values
	bool LoadConfig();

	//! Alter configuration
	void AlterConfig();

	//! Set config function for adaptations
	inline void SetAlterConfigFunc(AlterConfigFunc func) { alter_function = func; }
protected:
	//! Create dir and return index
	int CreateDir();

	//! Load an actual ini file
	void LoadIni();
private:
	//! Reference to configuration class
	Config &config;

	//! A series of program options from boost lib
	boost::program_options::variables_map vm;

	//! Default options are stored/loaded from this file
	std::string default_option_file;

	//! Alter configuration
	AlterConfigFunc alter_function;

	//! The options per run
	std::string run_option_file;

	//! Boolean for status
	bool state_okay;
};

#endif /* PERSIST_H_ */
