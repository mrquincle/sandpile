/**
 * @file PlotFigure.cpp
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
#include <fstream>

#include <PlotFigure.h>
#include <Plot.h>
#include <Time.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace boost;
using namespace boost::filesystem;
using namespace std;

/* **************************************************************************************
 * Implementation of PlotFigure
 * **************************************************************************************/

PlotFigure::PlotFigure() {

}

PlotFigure::~PlotFigure() {

}

void PlotFigure::Draw(DataForPlot &data, Config &config, PlotFigureType pf) {
	vector<DataForPlot> v;
	v.clear();
	v.push_back(data);
	Draw(v, config, pf);
}

/**
 * Plot a certain figure with a series of data arrays.
 */
void PlotFigure::Draw(vector<DataForPlot> &data, Config &config, PlotFigureType pf) {
	Plot ap;
//	ap.SetDimensions(1.905, 1.925, 0.0001, 0.006);
	string append;
	std::map<PlotFigureType,FigureConfig>::iterator i;

	if (data.empty()) {
		cerr << "There is no data found!" << endl;
		return;
	}

	int cnt = 0;
	vector<DataForPlot>::iterator d_i;
	for (d_i = data.begin(); d_i != data.end(); ++d_i, ++cnt) {
		ap.GetData(cnt).SetID(d_i->id);
		switch (pf) {
		case PFT_Avalanche:
			i = config.figures.find(PFT_Avalanche);
			ap.GetData(cnt).SetData(*d_i->events);
			break;
		case PFT_GrainsBeforeAvalanche:
			i = config.figures.find(PFT_GrainsBeforeAvalanche);
			ap.GetData(cnt).SetData(*d_i->events);
			break;
		case PFT_GrainsDuringAvalanche:
			i = config.figures.find(PFT_GrainsDuringAvalanche);
			ap.GetData(cnt).SetData(*d_i->events);
			break;
		case PFT_GrainsDiffAvalanche:
			i = config.figures.find(PFT_GrainsDiffAvalanche);
			ap.GetData(cnt).SetData(*d_i->events);
			break;
		case PFT_GrainsPerCell:
			i = config.figures.find(PFT_GrainsPerCell);
//			ap.SetDimensions(2, 4, 0, 500000);
			ap.GetData(cnt).SetData(*d_i->events);
			break;
		case PFT_CriticalCells:
			i = config.figures.find(PFT_CriticalCells);
			ap.GetData(cnt).SetData(*d_i->events);
			break;
		case PFT_Height:
			i = config.figures.find(PFT_Height);
			append = boost::lexical_cast<std::string>(d_i->time_id);
			ap.GetData(cnt).SetData(d_i->values, d_i->len);
			break;
		case PFT_Dissipation:
			i = config.figures.find(PFT_Dissipation);
			append = boost::lexical_cast<std::string>(d_i->time_id);
			ap.GetData(cnt).SetData(d_i->values, d_i->len);
			break;
		}
	}

	if (i != config.figures.end()) {
		FigureConfig fc = i->second;
		ap.SetPath(fc.path);
		ap.SetFileName(fc.filename + append + data.front().suffix, fc.output_type);
		ap.Init(fc.output_type);
		ap.SetTitle(fc.title);
		ap.SetXAxis(fc.x_axis);
		ap.SetYAxis(fc.y_axis);
		ap.SetPlotMode(fc.plot_mode);
		ap.SetPlotType(fc.plot_type);
		// first store and then plot (while plotting we might corrupt the data)
		if (data.front().data2file) ap.Store();
		ap.Draw(fc.output_type);
	}
}

/**
 * Sometimes you might want to replot the data using different scales for example. That is
 * why by default the data is stored. It can be retrieved using the proper arguments for
 * plot.Draw. This might also be used to do calculations later on, like deciding upon the
 * slope of the loglog plot.
 */
void PlotFigure::DrawAgain(Config &config) {
	Time timer;

	path dir;
	string dirname = boost::lexical_cast<std::string>(config.run_id);
	dir = dirname;
	dirname = dirname + '/';
	if (dirname.size() < 2) return;
	if (!exists(dir)) {
		cerr << "Cannot find dir \"" << dirname << "\" to plot" << endl;
		return;
	}

	std::map<PlotFigureType,FigureConfig>::iterator i;
	for (i = config.figures.begin(); i != config.figures.end(); ++i) {

		// Only replot PL plots, not the PPMs
		if ( i->second.output_type != PL_GRAPH) continue;

		// Todo: make this a bit less spaghetti-like
		int data_id = 0;

		Plot ap;
		std::map<DataDecoratorType,int> * datamap = new std::map<DataDecoratorType,int>();
		ap.SetPath(dirname);
		DataContainer &data = ap.GetData(data_id);
		data.SetData(*datamap);
		data.clear();
//		data.SetID(config.run_id);

		// load from stored .data file
		string pfile = dirname + i->second.filename + ".data"; //"soc.svg.data";
		cout << "Open " << pfile << endl;
		ifstream ifile(pfile.c_str());
		if ( ifile.is_open() ) {
			data.read(ifile);
		} else {
			cerr << "Couldn't open file" << endl;
		}
		//	cout << "Slope is: " << data.CalculateSlope() << endl;

		DataForPlot dp;
		dp.data2file = false;
		dp.file2data = false;
		dp.events = datamap;
		// Draw again, but now with suffix "_re"
		dp.suffix = "_re";
		dp.id = config.run_id;

		Draw(dp, config, i->first);

		delete datamap;
	}
}

/**
 * Draw all figures in one figure
 */
void PlotFigure::DrawAll(Config &config) {

	std::map<PlotFigureType,FigureConfig>::iterator i;
	for (i = config.figures.begin(); i != config.figures.end(); ++i) {

		// Only replot PL plots, not the PPMs
		if ( i->second.output_type != PL_GRAPH) continue;

		cout << "Draw " << i->second.GetDescription() << endl;

		std::vector < std::map<DataDecoratorType,int> * > maps;
		std::vector < DataForPlot> dps;

		for (int r = 0; r < config.run_id+1; ++r) {
			path dir;
			string dirname = boost::lexical_cast<std::string>(r);
			dir = dirname;
			dirname = dirname + '/';
			if (dirname.size() < 2) return;
			if (!exists(dir)) {
				cerr << "Cannot find dir \"" << dirname << "\" to plot" << endl;
				continue;
			}

			Plot ap;
			std::map<DataDecoratorType,int> * datamap = new std::map<DataDecoratorType,int>();
			maps.push_back(datamap);
			ap.SetPath(dirname);
			DataContainer &data = ap.GetData(r);
//			data.SetID(r);
			data.SetData(*datamap);
			data.clear();

			// load from stored .data file
			string pfile = dirname + i->second.filename + ".data";
			cout << "Open " << pfile << endl;
			ifstream ifile(pfile.c_str());
			if ( ifile.is_open() ) {
				data.read(ifile);
			} else {
				cerr << "Couldn't open file" << endl;
			}

			DataForPlot dp;
			dp.data2file = false;
			dp.file2data = false;
			dp.events = datamap;
			dp.id = r;
			// Draw again, but now with suffix "_all"
			dp.suffix = "_all";

			dps.push_back(dp);
		}

		Draw(dps, config, i->first);

		std::vector < std::map<DataDecoratorType,int> * >::iterator m_i;
		for (m_i = maps.begin(); m_i != maps.end(); ++m_i) {
			delete *m_i;
		}
		maps.clear();
	}
}
