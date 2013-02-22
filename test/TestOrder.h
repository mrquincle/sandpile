/***************************************************************************************************
 * @brief
 * @file TestOrder.h
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


#ifndef TESTORDER_H_
#define TESTORDER_H_

class Grid;
#include <TestCell.h>

enum GRID_INPUT { GI_LINE, GI_TWO_LINES, GI_RANDOM, GI_COUNT };
const std::string GRID_INPUTSTR[] = { "GI_LINE", "GI_TWO_LINES", "GI_RANDOM", "GI_COUNT" };

class TestOrder: public TestCell {
public:
	TestOrder(Grid *grid);

	~TestOrder();

	void Populate(int & nr_grains);

	void Run();
private:
	GRID_INPUT grid_input;
};


#endif /* TESTORDER_H_ */
