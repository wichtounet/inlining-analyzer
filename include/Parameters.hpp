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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <map>

namespace inlining {

/*!
 * \enum Parameter
 * \brief A customizable parameter of the application.
 */
enum Parameter {
    HEAVY_CALL_SITE,                        /*!< the number of calls after which a call site is considered as heavy [default:10000] */
    HOT_CALL_SITE,                          /*!< the temperature after which a call site is considered as hot [default:0.001] */
    CLUSTER_MAX_SIZE,                       /*!< the maximum size of a cluster [default:10] */
    LIBRARY_PATH_THRESHOLD,                 /*!< the minimum number of calls of paths in library issues search [default:500] */
    LIBRARY_PATH_MAX_LENGTH,                /*!< the maximum length of path to consider in library issues search [default:3] */
    LIBRARY_MIN_PATH_CALLS,                 /*!< the minimum of calls for the full path in a library issues search [default:10] */
    MOVE_BENEFIT_THRESHOLD,                 /*!< the minimum number of calls to avoid before proposing to move a function [default:100] */
    PARAMETERS_THRESHOLD,                   /*!< the number of parameters after which a function is considered as over-parameterized [default:10] */

    HIERARCHY_MAX_CALLS_FUNCTION,           /*!< the percentage of calls in a hierarchy after which a function should be reported [default:0.8] */
    HIERARCHY_MIN_CALLED_FUNCTIONS,         /*!< the percentage of called functions in a hierarchy after which the hierarchy should be reported [default:0.2] */
    HIERARCHY_MIN_CALLS,                    /*!< the minimum number of calls for a virtual hierarchy issue to be displayed [default:100] */

    /* Heuristic parameters */
    HEURISTIC_FUNCTION_PARAMETER_COST,      /*!< the cost of a parameter in the temperature of a function [default:0.10] */
    HEURISTIC_FUNCTION_VIRTUALITY_COST,     /*!< the cost of the virtuality in the temperature of a function [default:0.10] */

    HEURISTIC_CALL_SITE_PARAMETER_COST,     /*!< the cost of a parameter in the temperature of a call site [default:0.39] */
    HEURISTIC_CALL_SITE_VIRTUALITY_COST,    /*!< the cost of the virtuality in the temperature of a call site [default:0.39] */

    HEURISTIC_LIBRARY_COST                  /*!< the cost of a library call in the temperature [default:0.39] */
};

/*!
 * \class Parameters
 * \brief Class responsible of the managing of the algorithm parameters.
 */
class Parameters {
    public:
        /*!
         * \brief Init the parameters to the default values.
        */
        static void init();

        /*!
         * \brief Set the parameter to the given value.
         * \param param The parameter to set.
         * \param value The value to set to the parameter.
         */
        static void set(Parameter param, double value) {
            parameters[param] = value;
        }

        /*!
         * \brief Get the current value of the parameter.
         * \param param The parameter to get the value for.
         * \return  The value of the parameter.
         */
        static double get(const Parameter param) {
            return parameters[param];
        }

    private:
        static std::map<Parameter, double> parameters;

        static void setDefault(Parameter param, double value);
};

} //end of inlining

#endif
