/**
 * @file Multiresolution.cpp
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
 * @date	Mar 28, 2012
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */

#include <Multiresolution.h>

#include <iostream>
#include <math.h>

using namespace std;

CoarseCell *CoarseCell::GetCell(int level, int gi, int gj) {
	assert (height == width);
	assert (level > 0);
	float max_level = log(height) / log(2);
	assert (level < max_level);
	int L = height / level;
	CoarseCell *result = new CoarseCell(L,L);
	for (int i = 0; i < L; ++i) {
		for (int j = 0; j < L; ++j) {
			GrainType val = GetValue(gi, gj, L, i, j);
			result->SetValue(i, j, val);
		}
	}
	return result;
}

Multiresolution::Multiresolution() {
	min_level = 2;
}

Multiresolution::~Multiresolution() {

}

/**
 * Iterate over different resolution levels. We start with level=3 and go to coarser and coarser
 * levels of resolution. So, we skip level=1 (which is the pixels themselves) and level=2 (which are
 * the 2x2 block pixels). Hence, level=3 corresponds to blocks of 4x4 pixels.
 *
 * In a grid of L=8, there are 8x8 (level=1) original pixels, and there are 4 level=3 pixels. In a grid
 * of L=16, there are 16 level=3 pixels, in a grid of L=256, there are 4096 level=3 pixels.
 *
 * In a grid of L=8, there is only 1 level=4 pixel (of 8x8 original pixels). In a grid of L=16, there
 * are 4 level=4 pixels, for L=256, there are 1024 level=4 pixels.
 */
CoarseCell* Multiresolution::Tick() {
	assert (grid->GetHeight() == grid->GetWidth()); // assert square grid
	int L = grid->GetWidth();
	CoarseCell *result = new CoarseCell(L,L);

	// individual cell size in result is 16x16 in 64x64 size grid, so we can fit 16 levels
	// coarsest level is one value, which will be copied to a 16x16 block of pixels
	// for 256x256 grid, each level will occupy 64x64 pixel blocks, etc.
	int rc_L = log(L) / log(2);
	cout << "Iterate from level " << rc_L+1 << " down to level " << min_level << endl;

	int r = 0;
	int fcnt = 0;
	int toggle = 0;
	int p = 0;

	for (int l = rc_L+1; l > min_level; --l) {
		std::vector<CoarseCell*> & cells = *new std::vector<CoarseCell*>();
		GetCoarseCells(l, cells);
//		cout << "Results: ";
		for (int c = 0; c < cells.size(); ++c) {
			GrainType res = Calculate(*cells[c]);

#define BLOCK_FORM1

#ifdef BLOCK_FORM
			int fL = L / block_size;
			for (int i = 0; i < block_size; ++i) {
				for (int j = 0; j < block_size; ++j) {
//					cout << "SetValue(" << r % rc_L << "," << r / rc_L << "," << block_size << "," << i << "," << j << "," << res << ")" << endl;
//					int offset = (r % fL)*block_size+(r / fL)*block_size*L;
//					cout << "Indices [r=" << r << "]: {" << (r % fL) << "," << (r / fL) << "}: " << offset << "+" << i+j*L << "=" << offset+i+j*L << endl;
					result->SetValue(r % fL, r / fL, block_size, i, j, res);
				}
			}
			++r;
#endif

#ifdef BLOCK_FORM1
			for (int i = 0; i < block_size; ++i) {
				for (int j = 0; j < block_size; ++j) {
					if (toggle)
						result->SetValue(p, fcnt, block_size, i, j, res);
					else
						result->SetValue(fcnt, p, block_size, i, j, res);
				}
			}

//			if (toggle)
//				cout << "SetValue (toggle=1): " << p << "," << fcnt << endl;
//			else
//				cout << "SetValue (toggle=0): " << fcnt << "," << p << endl;

			if (p >= (fcnt-toggle)) {
				p = 0;
				toggle = 1 - toggle;
				if (toggle) fcnt++;
			} else
				++p;
			++r;
#endif

//			cout << r << endl;

#ifdef LINE_FORM
			for (int repeat = 0; repeat < 12; ++repeat)
				result->SetValue(r+repeat, res);
			r += 12;
#endif

//			cout << (float)res;
//			if (c - (cells.size()+1)) cout << " ";
		}
		cout << "Total number of cells: " << r << endl;
		delete &cells;
	}


	return result;
}

/**
 * Calculates the order parameter. We calculate first the sum of the values over each row. Then calculate
 * the sum over row pairs multiplied by the distance between them: \sum { r_m * r_n * |m-n| }. The same
 * is done for the columns.
 * This value becomes larger the more the values are equally distributed over the rows. If all the grains
 * are accumulated in one row, the value that describes the "row contribution" is the smallest.
 *
 * TODO: What we actually want is a figure that does not increase if subsequently the "column contribution"
 * is large.
 */
GrainType Multiresolution::Calculate(CoarseCell & cell) {
	int L = cell.GetWidth();
	GrainType row_sums[L];
	GrainType col_sums[L];
	// calculate sum over each row
	for (int j = 0; j < L; ++j) {
		row_sums[j] = 0; // 0.00000000000001;
		for (int i = 0; i < L; ++i) row_sums[j] += cell.GetValue(i,j);
	}
	// calculate sum over each column
	for (int i = 0; i < L; ++i) {
		col_sums[i] = 0; //0.00000000000001;
		for (int j = 0; j < L; ++j) col_sums[i] += cell.GetValue(i,j);
	}

	// calculate each pair of rows
	GrainType sr = 0;
	for (int r0 = 0; r0 < L; ++r0) {
		for (int r1 = 0; r1 < L; ++r1) {
			if (r0 != r1) {
				GrainType distance_punishment = abs(r0 - r1)*(GrainType)abs(r0 - r1);
				sr += (row_sums[r0] * row_sums[r1]) / distance_punishment;
			}
		}
	}

	GrainType sc = 0;
	for (int c0 = 0; c0 < L; ++c0) {
		for (int c1 = 0; c1 < L; ++c1) {
			if (c0 != c1) {
				GrainType distance_punishment = abs(c0 - c1)*(GrainType)abs(c0 - c1);
				sc += (col_sums[c0] * col_sums[c1]) / distance_punishment;
			}
		}
	}
	// actually should be sr * sc
	return sr + sc;
//	return sc;
}

/**
 * Get coarse cells on a certain level. Level 1 corresponds to the cells themselves and
 * the maximum level is equal to log base 2 of L (e.g. maxLevel=8 for L=256).
 * To use a multiresolution scheme the grid needs to be square and the size L is required
 * to be a power of 2.
 */
bool Multiresolution::GetCoarseCells(int level, std::vector<CoarseCell*> & cells) {
	assert (grid->GetHeight() == grid->GetWidth()); // assert square grid
	int L = grid->GetWidth();

	if (level <= 1) {
		cerr << "Level should be higher than 1" << endl;
		return false;
	}

	int maxLevel = log(L) / log(2);
	if (level > maxLevel + 1) {
		cerr << "Level should be lower than maximum level " << maxLevel + 1 << endl;
		return false;
	}
//	else {
//		cout << "Maximum level is " << maxLevel << endl;
//	}

	cout << "Calculate for level " << level << endl;

	if (pow(2,maxLevel) != L) {
		cerr << "Size L=" << L << " should be power of 2 (e.g. 2^" << maxLevel << "=" << pow(2,maxLevel) << ")" << endl;
		return false;
	}
//	else {
//		cout << "Size L=" << L << " is power of 2 (e.g. 2^" << maxLevel << "=" << pow(2,maxLevel) << ")" << endl;
//	}

	// width/height of individual coarse grained cells
	int cs_L = pow(2,level-1);

	// number of coarse cells over the width/height of the grid
	int nof_cs_L = L / cs_L;
	// number of coarse cells
	int nof_cs = nof_cs_L*nof_cs_L;


	cout << "Number of coarse cells over width is " << nof_cs_L << endl;
	cout << "Total number of coarse cells is " << nof_cs << endl;
	cout << "Dimension of these coarse cells: " << cs_L << "x" << cs_L << endl;
	assert (nof_cs * cs_L*cs_L == (L*L));

	// create vector with nof_cs cells of size cs_L*cs_L
	std::vector<CoarseCell> *result = new std::vector<CoarseCell>();
	for (int n = 0; n < nof_cs; ++n) {
		CoarseCell *cs = new CoarseCell(cs_L, cs_L);
		int y = n / nof_cs_L;
		int x = n - (nof_cs_L * y);
//		cout << "Array [cell id=" << n << "]: {";
		for (int j = 0; j < cs_L; ++j) {
			for (int i = 0; i < cs_L; ++i) {
				int index = x*cs_L+i + (y*cs_L+j)*L;
//				cout << index;
				GrainType val =  grid->GetCell(index).GetHeight();
//				if (val) cout << "[" << val << "]";
				cs->SetValue(i, j, val);
//				if (cs_L - (i+1))
//					cout << " ";
			}
//			if (cs_L - (j+1))
//				cout << ", ";
		}
//		cout << "}" << endl;
		cells.push_back(cs);
	}

	return true;
}



