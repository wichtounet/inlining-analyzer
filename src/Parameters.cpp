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

#include "Parameters.hpp"

using namespace inlining;

std::map<Parameter, double> Parameters::parameters;

void Parameters::setDefault(Parameter param, double value) {
    //We check if the value has already been set, in order to not override a user-defined value
    if (parameters.find(param) == parameters.end()) {
        parameters[param] = value;
    }
}

void Parameters::init() {
    setDefault(HEAVY_CALL_SITE, 10000);
    setDefault(HOT_CALL_SITE, 0.001);

    setDefault(CLUSTER_MAX_SIZE, 10);

    setDefault(LIBRARY_PATH_THRESHOLD, 500);
    setDefault(LIBRARY_PATH_MAX_LENGTH, 3);
    setDefault(LIBRARY_MIN_PATH_CALLS, 10);
    setDefault(MOVE_BENEFIT_THRESHOLD, 100);

    setDefault(PARAMETERS_THRESHOLD, 10);

    setDefault(HIERARCHY_MAX_CALLS_FUNCTION, 0.80);
    setDefault(HIERARCHY_MIN_CALLED_FUNCTIONS, 0.20);
    setDefault(HIERARCHY_MIN_CALLS, 100);

    setDefault(HEURISTIC_FUNCTION_PARAMETER_COST, 0.10);
    setDefault(HEURISTIC_CALL_SITE_PARAMETER_COST, 0.10);
    setDefault(HEURISTIC_FUNCTION_VIRTUALITY_COST, 0.39);
    setDefault(HEURISTIC_CALL_SITE_VIRTUALITY_COST, 0.39);
    setDefault(HEURISTIC_LIBRARY_COST, 0.39);
}
