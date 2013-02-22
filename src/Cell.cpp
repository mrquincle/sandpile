/**
 * @file Cell.cpp
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


#include <Cell.h>

#include <boost/random/uniform_smallint.hpp>

using namespace boost;
using namespace std;

/* **************************************************************************************
 * Implementation of Cell
 * **************************************************************************************/

int Cell::direction_feed = 33480;


Cell::Cell() {
	height = 0;

	// here 4 is neighbour size
	static boost::mt19937 randomGenerator(direction_feed);
	uniform_smallint<size_t> distr(0, 4-1);

	direction = distr(randomGenerator);
	max_capacity = 10;
	altered_function = NULL;
}

/**
 * Default destructor
 */
Cell::~Cell() { }

/**
 * Transfer grains from "this" cell to the cell given as argument. It is because of
 * capacity constraints in the target cell and a limited number of grains in the source
 * cell not always possible to transfer the number of grains indicated by the parameter.
 * Hence this function returns the actual number of grains transferred between the
 * two cells.
 */
GrainType Cell::Transfer(Cell &cell, GrainType number) {
	GrainType target_max = max_capacity - cell.GetHeight();
	GrainType source_max = height;

	GrainType transfer = target_max;
	transfer = (transfer < number) ? transfer : number;
	transfer = (transfer < source_max) ? transfer : source_max;

	height -= transfer;
	cell.height += transfer;

	return transfer;
}

