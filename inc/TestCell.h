/***************************************************************************************************
 * @brief
 * @file TestCell.h
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common Hybrid 
 * Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from thread pools and 
 * TCP/IP components to control architectures and learning algorithms. This software is published 
 * under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless, we 
 * personally strongly object against this software used by the military, in the bio-industry, for 
 * animal experimentation, or anything that violates the Universal Declaration of Human Rights.
 *
 * Copyright © 2012 Anne van Rossum <anne@almende.com>
 ***************************************************************************************************
 * @author 	Anne C. van Rossum
 * @date	Jun 5, 2012
 * @project	Replicator FP7
 * @company	Almende B.V. & Distributed Organisms B.V.
 * @case	Self-organised criticality
 **************************************************************************************************/


#ifndef TESTCELL_H_
#define TESTCELL_H_

#include <Typedefs.h>

class Grid;

/**
 * Gives access to private members of cell for testing purposes. This is not intended to be done on
 * a normal use basis.
 */
class TestCell {
public:
	TestCell(Grid *grid);

	~TestCell();

	//! Just overwrites the height in a cell with index "i" by a certain value
	void SetHeight(int i, GrainType value);

protected:
	Grid *grid;
};


#endif /* TESTCELL_H_ */
