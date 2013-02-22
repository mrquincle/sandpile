/**
 * @file Config.h
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


#ifndef CONFIG_H_
#define CONFIG_H_

// General files
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Allow for serialisation of map and vector
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <string.h>
#include <Toppling.h>
#include <Plot.h>
#include <PlotFigureType.h>

/* **************************************************************************************
 * Interface of Config
 * **************************************************************************************/

class FigureConfig {
public:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & filename;
        ar & path;
        ar & title;
        ar & x_axis;
        ar & y_axis;
        ar & plot_mode;
        ar & output_type;
        ar & plot_type;
    }

	std::string filename;

	std::string path;

	std::string title;

	std::string x_axis;

	std::string y_axis;

	//! Default, semilog, log-log
	PlotMode plot_mode;

	//! Actual plot, or 2D picture of activity
	OutputType output_type;

	//! Default, density, or cumulative density
	PlotType plot_type;

	//! Get a description (will not be stored of course)
	std::string & GetDescription();
};

/**
 * Configuration options in this experiment. It is not meant to be general, and do
 * not change the order of the fields or the serialisation/deserialisation will
 * wreck havoc.
 */
class Config {
public:
	//! Serialise using boost
    friend class boost::serialization::access;

    //! The serialise function has access to all fields
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & system_size;
        ar & toppling_method;
        ar & boundary_type;
        ar & timespan;
        ar & no_trials;
        ar & skip;
        ar & figures;
        ar & feeds;
        ar & no_pics;
        ar & no_dots;
        ar & dissipative_mode;
        ar & dissipation_rate;
        ar & dissipation_amount;
    	ar & dissipation_cell_capacitity;
    	ar & dissipation_total;
    	ar & dissipation_threshold;
    	ar & toppling_threshold;
        ar & run_experiment;
        ar & run_id;
    }

	//! Get toppling method in the form of a string
	std::string & GetTopplingMethod();

	//! Print to console
	void Print();

//private:
    //! Side of a square grid (width/length, in papers: L)
    int system_size;

    //! One of the toppling methods, named after the scientific authors
	TopplingMethod toppling_method;

	//! Overwriting default boundary type if set to something else then BT_UNDEFINED
	BoundaryType boundary_type;

	//! The length of one trial
	long int timespan;

	//! The number of trials, can just be one, does smooth graphs if > 1
	int no_trials;

	//! The number of events skipped in the start of the trial
	int skip;

	//! A map of figures/graphs
	std::map<PlotFigureType,FigureConfig> figures;

	//! Some feeds for the random number generators
	std::vector<int> feeds;

	//! The number of pictures taken during one trial
	int no_pics;

	//! The number of dots that indicate progress (adapt to screen size)
	int no_dots;

	//! If possible this indicates the dissipative mode
	bool dissipative_mode;

	//! Dissipation rate
	double dissipation_rate;

	//! Dissipation amount (will be divided by # neighbours)
	GrainType dissipation_amount;

	//! Individual capacity per cell
	GrainType dissipation_cell_capacitity;

	//! Total number of entities in dissipation field
	GrainType dissipation_total;

	//! Threshold that couples dissipation field with height field
	GrainType dissipation_threshold;

	//! Toppling threshold (-1 means default)
	GrainType toppling_threshold;

	//! Run experiment (or do something else like plotting)
	bool run_experiment;

	//! ID for run
	int run_id;
};

#endif /* CONFIG_H_ */
