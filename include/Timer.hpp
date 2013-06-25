/*
Copyright (C) 2011 Baptiste Wicht

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*! \file */

#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>

namespace inlining {

/*!
 * \class Timer
 * \brief Simple class used to time some operation
 */
class Timer {
    public:
        /*!
         * Construct a new Timer and starts it.
         */
        Timer();

        /*!
         * \brief Stop the timer and return the elapsed time.
         * \return The elapsed time from the start to now.
         */
        double elapsed();
    private:
        timeval startTime;
        timeval endTime;
};

} //end of inlining

#endif
