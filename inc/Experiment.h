/**
 * @file Experiment.h
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


#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

// General files
#include <map>
#include <string>

#include <Config.h>
#include <EventCounter.hpp>
#include <PlotFigure.h>
#include <SandPile.h>
#include <Time.h>

/* **************************************************************************************
 * Interface of Experiment
 * **************************************************************************************/

typedef double CounterType;

/**
 * Do the experiment and plot what is necessary.
 */
class Experiment {
public:
	//! Constructor Experiment
	Experiment(Config & cfg);

	//! Destructor ~Experiment
	virtual ~Experiment();

	//! Run once
	bool Run();
protected:
	//! A run exists out of a number of trials
	void Trial(int trial);

	//! A trial exists out of a number of ticks
	void Tick(long int t);

	//! Plot at the end of a run
	void Plot();
private:
	//! Store all the configuration options
	Config & config;

	//! We use a sandpile in this experiment
	SandPile *sandpile;

	//! Map with different types to plots to make
	std::map<PlotFigureType,EventCounter<CounterType>*> counters;

	//! Create timer to time trial
	Time timer;

	//! Object to push to PlotFigure
	DataForPlot dp;

	//! Wrapper around Plot
	PlotFigure plot_figure;

};

#endif /* EXPERIMENT_H_ */
