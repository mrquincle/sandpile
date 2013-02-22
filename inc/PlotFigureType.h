/**
 * @file PlotFigureType.h
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
 * @case	Self-organised criticality
 */


#ifndef PLOTFIGURETYPE_H_
#define PLOTFIGURETYPE_H_

/**
 * Plots relevant to the experiment:
 * - PFT_Avalanche: 				distribution of avalanche sizes
 * - PFT_GrainsDuringAvalanche:		distribution of # grains during avalanches
 * - PFT_GrainsBeforeAvalanche:		# grains before an avalanche is triggered
 * - PFT_GrainsDiffAvalanche:		difference in # grains before and after an avalanche
 * - PFT_Height						# grains per cell in 2D grid
 * - PFT_Dissipation				# dissipation units per cell in 2D grid (different grid)
 * - PFT_CriticalCells				# of critical cells before/after (check the code) an avalanche
 */
enum PlotFigureType { PFT_Avalanche, PFT_GrainsDuringAvalanche, PFT_GrainsBeforeAvalanche,
	PFT_GrainsDiffAvalanche, PFT_Height, PFT_Dissipation, PFT_CriticalCells, PFT_GrainsPerCell };


#endif /* PLOTFIGURETYPE_H_ */
