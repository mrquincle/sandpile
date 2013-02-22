/***************************************************************************************************
 * @brief
 * @file Typedefs.h
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


#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

//! Currently we use integers for the number of grains
//typedef int GrainType;
//! It can be made a template, but that's always so much work
//! However, in case we need Cell<int> as well as Cell<double> in the same program it will
//! be necessary
typedef double GrainType;



#endif /* TYPEDEFS_H_ */
