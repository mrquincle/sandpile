/**
 * @file Time.h
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
 * @date	Jul 25, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */


#ifndef TIME_H_
#define TIME_H_

// General files
#include <sys/time.h>
#include <iostream>

#include <boost/date_time/gregorian/gregorian.hpp>

/* **************************************************************************************
 * Interface of Time
 * **************************************************************************************/

class Time {
public:
	//! Constructor Time
	Time() {;}

	//! Destructor ~Time
	virtual ~Time() {;}

	//! Start timer
	inline void Start() {
		gettimeofday(&start, NULL);
	}

	//! Stop timer
	inline void Stop() {
		gettimeofday(&end, NULL); \
		seconds  = end.tv_sec  - start.tv_sec; \
		useconds = end.tv_usec - start.tv_usec; \
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5; \
		useconds = mtime % 1000; \
		seconds = (mtime / 1000) % 60; \
		minutes = mtime / 60000; \
	}

	//! Print result of the timer
	inline void Print() {
		std::cout << "Time: " << minutes << ":" << seconds << "." << useconds << std::endl;
	}

	inline std::string GetDate() {
		boost::gregorian::date today = boost::gregorian::day_clock::local_day();
		std::stringstream s; s << today;
		return s.str();
	}

private:
    struct timeval start, end;
    long mtime, seconds, useconds, minutes;
};

#endif /* TIME_H_ */
