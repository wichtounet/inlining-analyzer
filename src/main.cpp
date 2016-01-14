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

#include <iostream>
#include <string>
#include <vector>

#include "Inlining.hpp"
#include "Utils.hpp"
#include "Parameters.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace inlining;

void printUsage();

bool parse_value_arg(const std::string& arg, Inlining& inlining);

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        cout << "Not enough arguments. Provide at least the dot file to read" << endl;

        return 1;
    }

    Inlining inlining;

    //The help option can be used without file
    if (argc == 2 && string(argv[1]) == "--help") {
        printUsage();
        return 0;
    }

    //There are command line options
    if (argc > 2) {
        //Process command line options
        for (int i = 1; i < argc - 1; ++i) {
            string arg = argv[i];

            if (arg == "--demangled") {
                inlining.setDemangled(true);
            } else if (arg == "--no-issues") {
                inlining.setDisplayIssues(false);
            } else if (arg == "--no-statistic") {
                inlining.setDisplayStatistic(false);
            } else if (arg == "--filter-duplicates") {
                inlining.setFilterDuplicates(true);
            } else if (arg == "--default-filters") {
                inlining.enableDefaultFilters();
            } else if (arg == "--help") {
                printUsage();
                return 0;
            } else {
                //Try to parse it as a value arg (--x=value)
                if(parse_value_arg(arg, inlining)){
                    continue;
                }

                cout << "Unrecognized option : " << arg << endl;
                return 1;
            }
        }
    }

    //Test for existence of the file before anything else
    string file = argv[argc - 1];
    if (!fexists(file)) {
        cout << "The file \"" << file << "\" does not exist" << endl;
        return 1;
    }

    //Launch the full analysis
    inlining.analyze(file);

    return 0;
}

bool parse_value_arg(const std::string& arg, Inlining& inlining){
    if (arg.substr(0, 2) == "--" && arg.find("=") != string::npos) {
        return false;
    }

    vector<string> parts = split(arg.substr(2), "=");

    if (parts.size() == 2) {
        string key = parts[0];

        if (key == "filter") {
            string value = parts[1];

            inlining.addFilter(value);

            return true;
        }

        double value = toNumber<double>(parts[1]);

        if (key == "heavy-call-site") {
            Parameters::set(HEAVY_CALL_SITE, value);
        } else if (key == "hot-call-site") {
            Parameters::set(HOT_CALL_SITE, value);
        } else if (key == "cluster-max-size") {
            Parameters::set(CLUSTER_MAX_SIZE, value);
        } else if (key == "library-path-threshold") {
            Parameters::set(LIBRARY_PATH_THRESHOLD, value);
        } else if (key == "library-path-max-length") {
            Parameters::set(LIBRARY_PATH_MAX_LENGTH, value);
        } else if (key == "library-min-path-calls") {
            Parameters::set(LIBRARY_MIN_PATH_CALLS, value);
        } else if (key == "move-benefit-threshold") {
            Parameters::set(MOVE_BENEFIT_THRESHOLD, value);
        } else if (key == "parameters-threshold") {
            Parameters::set(PARAMETERS_THRESHOLD, value);
        } else if (key == "hierarchy-max-calls-function") {
            Parameters::set(HIERARCHY_MAX_CALLS_FUNCTION, value);
        } else if (key == "hierarchy-min-called-functions") {
            Parameters::set(HIERARCHY_MIN_CALLED_FUNCTIONS, value);
        } else if (key == "hierarchy-min-calls") {
            Parameters::set(HIERARCHY_MIN_CALLS, value);
        } else if (key == "heuristic-function-parameter-cost") {
            Parameters::set(HEURISTIC_FUNCTION_PARAMETER_COST, value);
        } else if (key == "heuristic-function-virtuality-cost") {
            Parameters::set(HEURISTIC_FUNCTION_VIRTUALITY_COST, value);
        } else if (key == "heuristic-call-site-parameter-cost") {
            Parameters::set(HEURISTIC_CALL_SITE_PARAMETER_COST, value);
        } else if (key == "heuristic-call-site-virtuality-cost") {
            Parameters::set(HEURISTIC_CALL_SITE_VIRTUALITY_COST, value);
        } else if (key == "heuristic-library-cost") {
            Parameters::set(HEURISTIC_LIBRARY_COST, value);
        } else {
            cout << "Unrecognized parameter : " << arg << endl;
            return false;
        }

        return true;
    }

    return false;
}

void printUsage() {
    cout << "Usage: analyzer [options] call_graph_file" << endl;
    cout << "Options:" << endl;
    cout << "  --help                                               display this information" << endl;
    cout << "  --demangled                                          use a call graph with demangled function names" << endl;
    cout << "  --no-issues                                          do not display the issues" << endl;
    cout << "  --no-statistic                                       do not display the statistics" << endl;
    cout << "  --filter-duplicates                                  do not display several times library issues with the same solution" << endl;
    cout << "  --default-filters                                    activate the default function filters" << endl;
    cout << "  --filter=<value>                                     filter the function <value>" << endl;
    cout << "  --heavy-call-site=<value> [10000]                    the number of calls after which a call site is considered as heavy" << endl;
    cout << "  --hot-call-site=<value> [0.001]                      the temperature after which a call site is considered as hot" << endl;
    cout << "  --cluster-max-size=<value> [10]                      the maximum size of a cluster" << endl;
    cout << "  --library-path-threshold=<value> [500]               the minimum number of calls of paths in library issues search" << endl;
    cout << "  --library-path-max-length=<value> [3]                the maximum length of path to consider in libray issues search" << endl;
    cout << "  --library-min-path-calls=<value> [10]                the minimum of calls for the full path in a library issues search" << endl;
    cout << "  --move-benefit-threshold=<value> [100]               the minimum number of calls to avoid before proposing to move a function" << endl;
    cout << "  --parameters-threshold=<value> [10]                  the number of parameters after which a function is considered as over-parameterized" << endl;
    cout << "  --hierarchy-max-calls-function=<value> [0.8]         the percentage of calls in a hierarchy after which a function should be reported" << endl;
    cout << "  --hierarchy-min-called-functions=<value> [0.2]       the percentage of called functions in a hierarchy after which the hierarchy should be reported" << endl;
    cout << "  --hierarchy-min-calls=<value> [100]                  the minimum number of calls for a virtual hierarchy issue to be displayed" << endl;
    cout << "  --heuristic-function-parameter-cost=<value> [0.10]   the cost of a parameter in the temperature of a function" << endl;
    cout << "  --heuristic-function-virtuality-cost=<value> [0.10]  the cost of the virtuality in the temperature of a function" << endl;
    cout << "  --heuristic-call-site-parameter-cost=<value> [0.39]  the cost of a parameter in the temperature of a call site" << endl;
    cout << "  --heuristic-call-site-virtuality-cost=<value> [0.39] the cost of the virtuality in the temperature of a call site" << endl;
    cout << "  --heuristic-library-cost=<value> [0.39]              the cost of a library call in the temperature" << endl;
    cout << endl;
    cout << "All the <value> have to be replaced by the numeric value you want to assign to the parameter" << endl;
    cout << "The values between brackets are the default values used by the analyzer" << endl;
}
