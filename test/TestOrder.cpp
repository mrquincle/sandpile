/**
 * @file TestOrder.cpp
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
 * @date	Mar 29, 2012
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */
#include <Grid.h>
#include <Multiresolution.h>

#include <Plot.h>
#include <iostream>
#include <TestOrder.h>
#include <TestCell.h>

#include <math.h>
#include <sstream>

using namespace std;

int main() {
	//	int L = 16;
	int L = 128; //64;
	L = 256;
	cout << "Create grid" << endl;
	BoundaryType boundary_type = BT_PERIODIC;
	Grid *grid = new Grid(L, L, boundary_type);

	cout << "Create test class" << endl;
	TestOrder order(grid);

	cout << "Run it" << endl;
	order.Run();

	cout << "Clean it up" << endl;

	delete grid;
	return EXIT_SUCCESS;
}

TestOrder::TestOrder(Grid *grid): TestCell(grid) {
	grid_input = GI_RANDOM;
	grid_input = GI_TWO_LINES;
}

TestOrder::~TestOrder() {

}

/**
 * For testing purposes, populate grid with one thick line of "heights"
 */
void TestOrder::Populate(int & nr_grains) {
	int seed = 238904;
	srand(seed);

	nr_grains = 0;
	switch (grid_input) {
	case GI_RANDOM:{
		for (int i = 0; i < grid->GetHeight(); ++i) {
			for (int j = 0; j < grid->GetWidth(); ++j) {
				if (!(rand() % 20)) {
					grid->GetCell(j, i).Increase(1);
					nr_grains += 1;
				}
			}
		}
	}
	break;
	case GI_LINE:
		for (int i = 0; i < grid->GetHeight(); ++i) {
			grid->GetCell(grid->GetHeight() / 2, i).Increase(10);
			grid->GetCell(grid->GetHeight() / 2 + 10, i).Increase(10);
			nr_grains += 20;
		}
		break;
	case GI_TWO_LINES:
		for (int i = 0; i < grid->GetHeight(); ++i) {
			grid->GetCell(grid->GetHeight() / 2, i).Increase(4);
			grid->GetCell(grid->GetHeight() / 2 + 10, i).Increase(4);
			grid->GetCell(grid->GetHeight() * 3/4, i).Increase(4);
			nr_grains += 12;
		}
		break;
	default:
		break;
	}
	cout << "Total number of grains for test: " << nr_grains << endl;
}


/**
 * This main function tests the order parameter and the multi-resolution implementation.
 */
void TestOrder::Run() {
	assert (grid != NULL);

	int L = grid->GetHeight();
	int nr_grains;
	Populate(nr_grains);

	// we need to make probabilities from heights by dividing through nr_grains
	for (int i = 0; i < grid->GetWidth()*grid->GetHeight(); ++i) {
		GrainType val = grid->GetCell(i).GetHeight();
		SetHeight(i, val / (GrainType)nr_grains);
	}

	cout << "Create multi-resolution data structure" << endl;
	Multiresolution *mr = new Multiresolution();
	mr->SetGrid(grid);
	mr->setMinLevel(4);
	mr->setBlockSize(12);

	cout << "Calculate order-parameter over multiple resolutions" << endl;
	CoarseCell* cc = mr->Tick();

	cout << "Copy the output data to float array" << endl;
	GrainType *cc_data = cc->GetData();
	int cc_size = cc->GetHeight() * cc->GetWidth();

	float array[cc_size];
	float max = 0;
	for (int i = 0; i < cc_size; ++i) {
		float val = (float)cc_data[i];
		if (val != 0)
			array[i] = fabs(log(val));
		else
			array[i] = 0;
		if (array[i] > max) max = array[i];
	}

	int print_first = 200;
	cout << "Print first " << print_first << " order values: ";
	for (int i = 0; i < print_first; ++i) {
		cout << (float)array[i] << ' ';
	}
	cout << endl;

	for (int i = 0; i < cc_size; ++i) {
		array[i] = array[i] / max;
	}

	int size = cc_size;

	Plot p;
	stringstream ss; ss.clear(); ss.str("");
	ss << "order_L" << L << '_' << GRID_INPUTSTR[grid_input] << '_';
	p.SetFileName(ss.str(), PL_GRID);
	p.Init(PL_GRID);
	cout << "Plot output" << endl;
	DataContainer &dc = p.GetData(0);
	dc.SetData(array, size);

	cout << "Copy the input data to float array for plotting" << endl;

	cout << "Plot input" << endl;
	DataContainer &dc2 = p.GetData(1);
	float array2[L*L];
	//	cout << "Array: " ;
	for (int i = 0; i < cc_size; ++i) {
		array2[i] = (float)grid->GetCell(i).GetHeight() * (GrainType)nr_grains / (float)10;
		//		cout << array2[i] << " ";
	}
	//	cout << endl;
	int size2 = L*L;
	dc2.SetData(array2, size2);

	cout << "Actually draw stuff" << endl;
	p.Draw(PL_GRID);

	//	delete grid;
	delete mr;
}
