/**
 * @file PlotFigure.h
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


#ifndef PLOTFIGURE_H_
#define PLOTFIGURE_H_

// General files
#include <Config.h>
#include <EventCounter.hpp>
#include <PlotFigureType.h>

#include <vector>

/* **************************************************************************************
 * Interface of PlotFigure
 * **************************************************************************************/

typedef double DataDecoratorType;

/**
 * Data can be delivered in the form of a raw series of float values or in the form of
 * an EventCounter
 */
struct DataForPlot {
	DataForPlot(): data2file(true), file2data(true) { };
	std::map<DataDecoratorType, int> *events;
	float *values;
	int len;
	//! Time id is used to be able to plot a series of pictures with "quasi" time stamps
	long int time_id;

	//! This identifier is the run and will be used as index in the plot
	int id;

	std::string suffix;
	bool data2file;
	bool file2data;
};

class PlotFigure {
public:
	//! Constructor PlotFigure
	PlotFigure();

	//! Destructor ~PlotFigure
	virtual ~PlotFigure();

	//! Draw figure
	void Draw(std::vector<DataForPlot> &data, Config &config, PlotFigureType pf);

	//! Draw figure
	void Draw(DataForPlot &data, Config &config, PlotFigureType pf);

	//! Draw figure again
	void DrawAgain(Config &config);

	//! Draw all in one figure
	void DrawAll(Config &config);

protected:

private:

};

#endif /* PLOTFIGURE_H_ */
