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

#include <algorithm>
#include <iostream>

#include "Reports.hpp"
#include "Analyzer.hpp"
#include "Sorters.hpp"
#include "Parameters.hpp"
#include "Logging.hpp"
#include "Timer.hpp"

using std::set;
using std::cout;
using std::endl;
using std::vector;
using std::string;

using namespace inlining;

void Reports::addFilter(const std::string& filter) {
    filters.insert(filter);
}

void Reports::enableDefaultFilters() {
    filters.insert("malloc");
    filters.insert("free");
    filters.insert("memcpy");
    filters.insert("exit");
    filters.insert("(below main)");
}

bool Reports::filter(const Function& function) const {
    return filters.find(graph[function].name) != filters.end();
}

bool Reports::filter(const CallSite& callSite) const {
    return filter(target(callSite, *graph.getGraph())) || filter(source(callSite, *graph.getGraph()));
}

std::vector<Function> Reports::getBiggestFunctions(unsigned int top) const {
    return getTopFunctions(top, SortBySize(graph));
}

std::vector<Function> Reports::getMostCostlyFunctions(unsigned int top) const {
    return getTopFunctions(top, SortBySelfCosts(graph));
}

std::vector<Function> Reports::getMostInterestingFunctions(unsigned int top) const {
    return getTopFunctions(top, SortByTemperature(graph));
}

std::vector<Function> Reports::getMostCalledFunctions(unsigned int top) const {
    return getTopFunctions(top, SortByCalls(graph));
}

std::vector<CallSite> Reports::getMostCalledCallSites(unsigned int top) const {
    return getTopCallSites(top, SortByCalls(graph));
}

std::vector<Function> Reports::getMostParameterizedFunctions(unsigned int top) const {
    return getTopFunctions(top, SortByParameters(graph));
}

std::vector<CallSite> Reports::getMostInterestingCallSites(unsigned int top) const {
    return getTopCallSites(top, SortByTemperature(graph));
}

std::vector<Function> Reports::getTiniestFunctions(unsigned int top) const {
    std::vector<Function> functions;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        if (graph[*first].size > 0 && !filter(*first)) {
            functions.push_back(*first);
        }
    }

    keepMin(top, SortBySize(graph), functions);

    return functions;
}

std::vector<CallSite> Reports::getHeaviestInterLibraryCalls(unsigned int top) const {
    std::vector<CallSite> callSites = analyzer.findInterLibraryCalls();

    std::vector<CallSite> filtered;
    for (std::vector<CallSite>::const_iterator it = callSites.begin(); it != callSites.end(); ++it) {
        if (!filter(*it)) {
            filtered.push_back(*it);
        }
    }

    keepTop(top, SortByCalls(graph), filtered);

    return filtered;
}

std::vector<CallSite> Reports::getHeaviestVirtualCalls(unsigned int top) const {
    std::vector<CallSite> callSites = analyzer.findVirtualCalls();

    std::vector<CallSite> filtered;

    for (std::vector<CallSite>::const_iterator it = callSites.begin(); it != callSites.end(); ++it) {
        if (!filter(*it)) {
            filtered.push_back(*it);
        }
    }

    keepTop(top, SortByCalls(graph), filtered);

    return filtered;
}

void Reports::printStatisticReports(unsigned int top) const {
    cout << endl << "# Statistic Reports" << endl << endl;

    Timer timer;

    printFunctionHeuristics(top);
    printCallSiteHeuristics(top);
    printHeavyInterLibrariesCalls(top);
    printHeavyVirtualCalls(top);
    printMostCalledFunctions(top);
    printMostCalledCallSites(top);
    printTiniestFunctions(top);
    printMostParameterizedFunctions(top);
    printMostCostlyFunctions(top);
    printBiggestFunctions(top);
    printParetoFunctions();

    PERF cout << "PERF: Statistic reports produced in " << timer.elapsed() << "ms" << endl;
}

std::vector<Function> Reports::getOverParameterizedFunctions() const {
    std::vector<Function> functions;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        if (graph[*first].parameters > Parameters::get(PARAMETERS_THRESHOLD) && !filter(*first)) {
            functions.push_back(*first);
        }
    }

    sort(functions.rbegin(), functions.rend(), SortByParameters(graph));

    return functions;
}

void Reports::printIssues() const {
    cout << endl << "# Found issues" << endl << endl;

    Timer timer;

    printLibraryIssues();
    printClusters();
    printCircularDependencies();
    printVirtualHierarchyIssues();
    printFunctionsWithTooManyParameters();

    PERF cout << "PERF: Issues produced in " << timer.elapsed() << "ms" << endl;
}

void Reports::printLibraryIssues() const {
    vector<LibraryIssue> issues = analyzer.findLibraryIssues();

    if (!issues.empty()) {
        sort(issues.rbegin(), issues.rend(), SortLibraryIssue());

        cout << "Library issues : " << endl;

        for (vector<LibraryIssue>::const_iterator it = issues.begin(); it != issues.end(); ++it) {
            if (filter(it->src) || filter(it->dest)) {
                continue;
            }

            cout << "\tFunctions " << graph[it->src].name << "[" << graph[it->src].module << "] and  " << graph[it->dest].name << "[" << graph[it->dest].module << "] should be in the same library " << endl;

            for (vector<string>::const_iterator sit = it->solutions.begin(); sit != it->solutions.end(); ++sit) {
                cout << "\t\t" << *sit << endl;
            }
        }
    }
}

void Reports::printClusters() const {
    vector<Cluster> clusters = analyzer.findClusters();

    if (!clusters.empty()) {
        cout << "There are " << clusters.size() << " clusters" << endl;

        sort(clusters.rbegin(), clusters.rend(), SortCluster());

        for (vector<Cluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it) {
            cout << "\tCluster of " << it->callSites.size() << " call sites" << endl;

            for (vector<CallSite>::const_iterator cit = it->callSites.begin(); cit != it->callSites.end(); ++cit) {
                cout << "\t\t" << graph.getDescription(*cit) << endl;
            }

            cout << "\t\tTemperature : " << it->temperature << endl;
        }
    }
}

void Reports::printCircularDependencies() const {
    vector<vector<string>> circularDependencies = analyzer.findCircularDependencies();

    if (!circularDependencies.empty()) {
        cout << "There are " << circularDependencies.size() << " circular dependencies in the graph" << endl;

        for (vector<vector<string>>::const_iterator it = circularDependencies.begin(); it != circularDependencies.end(); ++it) {
            cout << "Circular dependency detected between these libraries : " << endl;
            for (std::vector<std::string>::const_iterator vit = it->begin(); vit != it->end(); ++vit) {
                cout << "\t" << *vit << endl;
            }
        }
    }
}

void Reports::printVirtualHierarchyIssues() const {
    vector<HierarchyIssue> hierarchyIssues = analyzer.findVirtualHierarchyIssues();

    if (!hierarchyIssues.empty()) {
        sort(hierarchyIssues.rbegin(), hierarchyIssues.rend(), SortHierarchyIssue());

        //Only if there is at least one hierarchy bigger than the threshold
        if (hierarchyIssues.begin()->calls >= Parameters::get(HIERARCHY_MIN_CALLS)) {
            cout << "Virtual hierarchy issues" << endl;

            for (vector<HierarchyIssue>::const_iterator it = hierarchyIssues.begin(); it != hierarchyIssues.end(); ++it) {
                if (it->calls < Parameters::get(HIERARCHY_MIN_CALLS)) {
                    break; //We can break directly because the collection is ordered
                }

                cout << "Function hierarchy for " << it->name << " calls = " << it->calls << endl;
                cout << "\t" << it->issue << endl;

                cout << "\tMembers of the hierarchy" << endl;

                for (vector<string>::const_iterator fit = it->members.begin(); fit != it->members.end(); ++fit) {
                    cout << "\t\tFunction " << *fit << endl;
                }
            }
        }
    }
}

void Reports::printFunctionsWithTooManyParameters() const {
    vector<Function> overParameterized = getOverParameterizedFunctions();

    if (!overParameterized.empty()) {
        cout << "Functions with too many parameters" << endl;

        for (vector<Function>::const_iterator it = overParameterized.begin(); it != overParameterized.end(); ++it) {
            cout << "\t" << graph[*it].name << " : " << graph[*it].parameters << " parameters" << endl;
        }
    }
}

void Reports::printBiggestFunctions(unsigned int top) const {
    vector<Function> functions = getBiggestFunctions(top);

    cout << "Top " << top << " biggest functions : " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cout << "\tFunction " << graph[*iterator].name << " = " << graph[*iterator].size << endl;
    }
}

void Reports::printFunctionHeuristics(unsigned int top) const {
    vector<Function> functions = getMostInterestingFunctions(top);

    cout << "Top " << top << " function temperature : " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cout << "\tFunction " << graph[*iterator].name << " = " << graph[*iterator].temperature << endl;
    }
}

void Reports::printCallSiteHeuristics(unsigned int top) const {
    vector<CallSite> callSites = getMostInterestingCallSites(top);

    cout << "Top " << top << " call site temperature : " << endl;
    for (vector<CallSite>::const_iterator iterator = callSites.begin(); iterator != callSites.end(); ++iterator) {
        cout << "\tCall site " << graph.getDescription(*iterator) << " = " << graph[*iterator].temperature << endl;
    }
}

void Reports::printHeavyInterLibrariesCalls(unsigned int top) const {
    vector<CallSite> callSites = getHeaviestInterLibraryCalls(top);

    cout << "Top " << top << "  heavy calls between libraries : " << endl;
    for (vector<CallSite>::const_iterator iterator = callSites.begin(); iterator != callSites.end(); ++iterator) {
        cout << "\tCall site " << graph.getDescription(*iterator) << " = " << graph[*iterator].calls << endl;
    }
}

void Reports::printHeavyVirtualCalls(unsigned int top) const {
    vector<CallSite> callSites = getHeaviestVirtualCalls(top);

    cout << "Top " << top << "  heavy virtual calls : " << endl;
    for (vector<CallSite>::const_iterator iterator = callSites.begin(); iterator != callSites.end(); ++iterator) {
        cout << "\tCall site " << graph.getDescription(*iterator) << " = " << graph[*iterator].calls << endl;
    }
}

void Reports::printMostCalledFunctions(unsigned int top) const {
    vector<Function> functions = getMostCalledFunctions(top);

    cout << "Top " << top << " most called functions : " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cout << "\tFunction " << graph[*iterator].name << " = " << graph[*iterator].calls << endl;
    }
}

void Reports::printMostCalledCallSites(unsigned int top) const {
    vector<CallSite> callSites = getMostCalledCallSites(top);

    cout << "Top " << top << " most called call sites : " << endl;
    for (vector<CallSite>::const_iterator iterator = callSites.begin(); iterator != callSites.end(); ++iterator) {
        cout << "\tCall site " << graph.getDescription(*iterator) << " = " << graph[*iterator].calls << endl;
    }
}

void Reports::printTiniestFunctions(unsigned int top) const {
    vector<Function> functions = getTiniestFunctions(top);

    cout << "Top " << top << " tiniest functions : " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cout << "\tFunction " << graph[*iterator].name << " = " << graph[*iterator].size << endl;
    }
}

void Reports::printMostParameterizedFunctions(unsigned int top) const {
    vector<Function> functions = getMostParameterizedFunctions(top);

    cout << "Top " << top << " most parameterized : " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cout << "\tFunction " << graph[*iterator].name << " = " << graph[*iterator].parameters << endl;
    }
}

void Reports::printMostCostlyFunctions(unsigned int top) const {
    vector<Function> functions = getMostCostlyFunctions(top);

    cout << "Top " << top << " most costly : " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cout << "\tFunction " << graph[*iterator].name << " = " << graph[*iterator].self_cost << endl;
    }
}

void Reports::printFunctionsCalledOnce() const {
    vector<Function> functions = analyzer.findFunctionsCalledOnce();

    cout << "Functions called once" << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        if (!filter(*iterator)) {
            cout << "\tFunction " << graph[*iterator].name << endl;
        }
    }
}

void Reports::printLeafFunctions() const {
    vector<Function> functions = analyzer.findLeafFunctions();

    cout << "Leaf functions " << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        if (!filter(*iterator)) {
            cout << "\tFunction " << graph[*iterator].name << endl;
        }
    }
}

void Reports::printParetoFunctions() const {
    vector<Function> functions;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        functions.push_back(*first);
    }

    sort(functions.rbegin(), functions.rend(), SortBySelfCosts(graph));

    double cumulativeCost = 0;
    unsigned int number = 0;

    cout << "Functions taking 80% of the run time" << endl;
    for (vector<Function>::const_iterator iterator = functions.begin(); iterator != functions.end(); ++iterator) {
        cumulativeCost += graph[*iterator].self_cost;

        if (cumulativeCost > 80.0) {
            break;
        }

        ++number;

        cout << "\tFunction " << graph[*iterator].name << endl;
    }

    cout << (100.0 * static_cast<double>(number) / static_cast<double>(graph.numberOfFunctions())) << "% of the functions take 80% of the run time" << endl;
}
