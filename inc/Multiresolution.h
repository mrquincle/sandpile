/**
 * @file Multiresolution.h
 * @brief Ability to get and set coarse cells in a grid
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


#ifndef MULTIRESOLUTION_H_
#define MULTIRESOLUTION_H_

#include <Grid.h>

#include <vector>

/**
 * A CoarseCell is like a matrix with some additional
 */
class CoarseCell {
public:
	CoarseCell(int width, int height) {
		val = new GrainType[width*height];
		this->width = width;
		this->height = height;
	}

	~CoarseCell() {
		delete [] val;
		width = height = -1;
	}
	//! Get the width of the entire cell
	inline int GetWidth() { return width; }
	//! Get the height of the entire cell
	inline int GetHeight() { return height; }
	//! Get the value at [i,j] in the cell
	inline GrainType GetValue(int i, int j) { return val[i+j*width]; }
	//! Get subcell [b_i,b_j] of size b_s, and get value [i,j] in that subcell
	inline GrainType GetValue(int b_i, int b_j, int b_s, int i, int j) {
		int offset = b_i*b_s+b_j*b_s*width;
		return val[offset+i+j*width];
	}
	//! Set value at [i,j] in cell
	inline void SetValue(int i, int j, GrainType value) { val[i+j*width] = value; }
	//! Set value at [k] in cell
	inline void SetValue(int k, GrainType value) { val[k] = value; }
	//! Set value in subcell [b_i,b_j] of size b_s at [i,j] in that subcell
	inline void SetValue(int b_i, int b_j, int b_s, int i, int j, GrainType value) {
		int offset = b_i*b_s+b_j*b_s*width;
		val[offset+i+j*width] = value;
	}
	//! Get data as array
	inline GrainType *GetData() { return val; }
	//! Get a subcell [gi,gj] and copy values to it
	CoarseCell *GetCell(int level, int gi, int gj);
private:
	GrainType *val;
	int width, height;
};

/**
 * A class that knows how to operate on multiple resolution scales on a grid. The current implementation
 * is a bit shaky.
 */
class Multiresolution {
public:
	//! Default constructor
	Multiresolution();

	//! Default destructor
	~Multiresolution();

	//! Set grid to operate on
	inline void SetGrid(Grid * grid) { this->grid = grid; }

	//! Return result of calculations in the form of a matrix, CoarseCell
	CoarseCell* Tick();

	//! Calculate order parameter
	GrainType Calculate(CoarseCell & cell);

	//! Get block size of 2D representation of calculation
	inline int getBlockSize() const { return block_size; }

	//! Set block size of 2D representation of calculation
	inline void setBlockSize(int blockSize) { block_size = blockSize; }

	//! Get finest level of granularity
	inline int getMinLevel() const { return min_level; }

	//! Set finest level of granularity
	inline void setMinLevel(int minLevel) { 	min_level = minLevel; }

protected:
	//! Get vector of cells on indicated level
	bool GetCoarseCells(int level, std::vector<CoarseCell*> & cells);

private:
	//! Reference to grid
	Grid * grid;

	//! Minimum level over which
	int min_level;

	//! Order parameter is represented as a 2D grid again with certain block size per value
	int block_size;
};


#endif /* MULTIRESOLUTION_H_ */
