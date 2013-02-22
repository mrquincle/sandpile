/**
 * @file Cell.h
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


#ifndef CELL_H_
#define CELL_H_

// General files
#include <boost/function.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <Typedefs.h>

/* **************************************************************************************
 * Forward declarations and typedefs
 * **************************************************************************************/

//#include <TestCell.h>
class TestCell;

class Cell;

//! The callback function
typedef boost::function<void(Cell&)> AlteredCallback;

/* **************************************************************************************
 * Interface of Cell
 * **************************************************************************************/

//! Wind directions, do not change the order, or the algorithm does not work
enum Direction { NORTH, WEST, SOUTH, EAST };

/**
 * A cell can either decrease or increase height.
 */
//template <typename GrainType>
class Cell {
public:
	//! Constructor Cell
	Cell();

	//! Destructor ~Cell
	virtual ~Cell();

	//! Set maximum capacity per cell
	inline void SetMaxCapacity(GrainType c) { max_capacity = c; }

	//! Get maximum capacity per cell
	inline GrainType GetMaxCapacity() { return max_capacity; }

	//! The "height" of a cell, the number of sand particles in SOC models
	inline GrainType GetHeight() { return height; }

	//! Decrease pile height
	inline void Decrease(const GrainType number) {
		height -= number; if (altered_function != NULL) altered_function(*this);
	}

	//! Increase pile height
	inline void Increase(const GrainType number) {
		height += number; if (altered_function != NULL) altered_function(*this);
	}

	//! Move number of grains from one cell to another, actual number will be returned
	GrainType Transfer(Cell &cell, GrainType number);

	//! Remove all items
	inline void Clear() {
		height = 0; if (altered_function != NULL) altered_function(*this);
	}

	//! Get direction
	inline int GetDirection() { return direction; }

	//! Set direction
	inline void SetDirection(int direction) { this->direction = direction; }

	//! An identifier, necessary to go from Cell to location in a Grid
	inline void SetId(long int id) { this->id = id; }

	//! Get the identifier
	inline long int GetId() { return id; }

	//! Set callback function that is called as soon as pile height increases or decreases
	inline void SetAlteredFunction(AlteredCallback func) { altered_function = func; }

public:
	//! Set feed for neighbour order and dissipation
	inline static void SetDirectionFeed(int feed) { direction_feed = feed; }

	//! Get feed for grid for Boost randomizer
	inline static int GetDirectionFeed() { return direction_feed; }

private:

	//! Increased or decreased...
	AlteredCallback altered_function;

	//! The number of items in the cell
	GrainType height;

	//! Direction can be north, east, south, west
	int direction;

	//! Maximum number of grains in a cell
	GrainType max_capacity;

	//! The identifier (set externally with SetId)
	long int id;

	//! Feed for direction
	static int direction_feed;

	//! Only a testing function is allowed to reach private fields
	friend class TestCell;

};


#endif /* CELL_H_ */
