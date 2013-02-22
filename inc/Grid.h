/**
 * @file Grid.h
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


#ifndef GRID_H_
#define GRID_H_

// General files
#include <vector>
#include <Cell.h>

/**
 * The different possible boundary types. The "undefined" can also be seen as the "default"
 * one. Every toppling method does have its default boundary method as defined by its
 * authors. However, in the config files or using the setup binary it is possible to
 * overwrite it like you wish.
 * <ul>
 * <li>BT_UNDEFINED				default / undefined
 * <li>BT_PERIODIC				north connected to south, east to west
 * <li>BT_DISSIPATING			all four borders of the square are outside the grid
 * <li>BT_WALL_DISSIPATING		two reflecting "walls", and two dissipating walls
 * <li>BT_CIRCULAR				a circle within the square area
 * <li>BT_RANDOM_NEIGHBOURS		no dissipation, every cell is connected with 4 neighbours
 * <li>BT_FULLY_CONNECTED		no dissipation, every cell connected to 4 different nodes
 * 								each time
 * </ul>
 */
enum BoundaryType { BT_UNDEFINED, BT_PERIODIC, BT_DISSIPATING, BT_WALL_DISSIPATING,
	BT_CIRCULAR, BT_RANDOM_NEIGHBOURS, BT_FULLY_CONNECTED };

/**
 * Make it easy to use boundary type in (stdout) streams.
 */
std::ostream& operator<<( std::ostream& os, const BoundaryType& type );

/* **************************************************************************************
 * Interface of Grid
 * **************************************************************************************/

/**
 * A 2-dimensional sand_grid, the terms "width" and "height" refer to the dimensions of the
 * sand_grid. The tiles are squares and each sand_grid cell is connected to four neighbours.
 */
class Grid {
public:
	//! Constructor Grid
	Grid(int width, int height, BoundaryType boundary_type);

	//! Destructor ~Grid
	virtual ~Grid();

	//! Get neighbours of given cell, sand_grid is defined periodically
	void GetNeighbours(int i, int j, std::vector<Cell*> & neighbours);

	//! Width
	inline int GetWidth() { return width; }

	//! PFT_Height
	inline int GetHeight() { return height; }

	//! Total number of grains
	GrainType CountGrains();

	//! Return cell given coordinates
	Cell & GetCell(int i, int j);

	//! Return cell by index
	Cell & GetCell(int n);

	//! Print content of every cell
	void Print();

	//! Within largest circle
	bool WithinCircle(int i, int j);

	//! Set feed for random neighbours
	inline static void SetNeighbourFeed(int feed) { neighbour_feed = feed; }

	//! Get feed for random neighbours
	inline static int GetNeighbourFeed() { return neighbour_feed; }

private:
	//! Width of the grid
	int width;

	//! Height of the grid
	int height;

	//! Use 1-dimensional array for 2-dimensional grid
	Cell *cells;

	//! Type of boundary (periodic, or removing/dissipating)
	BoundaryType boundary_type;

	//! Reservoir is just one cell outside of the grid
	Cell reservoir;

	//! An array with indices that is randomly shuffled once, or all the time
	int *random_indices;

	//! Random neighbour feed
	static int neighbour_feed;

};

#endif /* GRID_H_ */
