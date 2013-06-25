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
#include <iomanip>
#include <fstream>
#include <stack>
#include <cctype>
#include <unordered_map>
#include <algorithm>
#include <functional>

#include <boost/graph/copy.hpp>
#include <boost/graph/strong_components.hpp>

#include "Timer.hpp"
#include "CallGraph.hpp"
#include "Analyzer.hpp"
#include "Logging.hpp"
#include "Sorters.hpp"
#include "Utils.hpp"
#include "Infos.hpp"
#include "Parameters.hpp"

using namespace inlining;

using std::set;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::unordered_map;
using std::map;
using std::for_each;
using std::bind1st;
using std::mem_fun;

using boost::vertex_index;
using boost::vertex_index_map;

void Analyzer::setFilterDuplicates(bool filter) {
    filterDuplicates = filter;
}

void Analyzer::addProtectedLibrary(const std::string& library) {
    protectedLibraries.insert(library);
}

void Analyzer::computeFunctionTemperature(Function function) {
    //Not enough information
    if (graph[function].size == 0) {
        graph[function].temperature = 0.0;
    } else {
        unsigned int parameters = graph[function].parameters;
        bool virtuality = graph[function].virtuality;
        unsigned int size = graph[function].size;

        double cost = 1.0 + (parameters * Parameters::get(HEURISTIC_FUNCTION_PARAMETER_COST)) + (virtuality ? Parameters::get(HEURISTIC_FUNCTION_VIRTUALITY_COST) : 0.0);
        double size_overhead = 1.0 + (size * (in_degree(function, *graph.getGraph()) - 1)) / graph.getApplicationSize();

        graph[function].temperature = (cost / size_overhead) * graph[function].frequency;
    }
}

void Analyzer::computeCallSiteTemperature(CallSite site) {
    Function caller = source(site, *graph.getGraph());

    //No way to inline it
    if (graph[caller].calls == 0) {
        graph[site].temperature= 0.0;
    } else {
        Function callee = target(site, *graph.getGraph());

        //Not enough information about the callee
        if (graph[callee].size == 0) {
            graph[site].temperature= 0.0;
        } else {
            bool virtuality = graph[callee].virtuality;
            bool library = graph[callee].module != graph[caller].module;
            unsigned int parameters = graph[callee].parameters;

            double cost = 1.0 + (parameters * Parameters::get(HEURISTIC_CALL_SITE_PARAMETER_COST)) + (virtuality ? Parameters::get(HEURISTIC_CALL_SITE_VIRTUALITY_COST) : 0.0) + (library ? Parameters::get(HEURISTIC_LIBRARY_COST) : 0.0);

            double size_overhead = 1.0;

            //Size overhead only if there is more than one calls to this functions
            if (in_degree(callee, *graph.getGraph()) > 1) {
                size_overhead += static_cast<double>(graph[callee].size) / static_cast<double>(graph.getApplicationSize());
            }

            graph[site].temperature= (cost / size_overhead) * graph[site].frequency;
        }
    }
}

void Analyzer::analyze() {
    Timer timer;

    FunctionIterator fit, fend;
    boost::tie(fit, fend) = graph.functions();
    for_each(fit, fend, bind1st(mem_fun(&Analyzer::computeFunctionTemperature), this));

    cout << "First " << endl;

    CallSiteIterator cit, cend;
    boost::tie(cit, cend) = graph.callSites();
    for_each(cit, cend, bind1st(mem_fun(&Analyzer::computeCallSiteTemperature), this));

    PERF cout << "PERF : Graph analyzed in " << timer.elapsed() << "ms" << endl;
}

vector<CallSite> Analyzer::findInterLibraryCalls() const {
    vector<CallSite> callSites;

    CallSiteIterator cit, cend;
    for (boost::tie(cit, cend) = graph.callSites(); cit != cend; ++cit) {
        Function caller = source(*cit, *graph.getGraph());
        Function callee = target(*cit, *graph.getGraph());

        if (graph[caller].module != graph[callee].module) {
            callSites.push_back(*cit);
        }
    }

    return callSites;
}

vector<CallSite> Analyzer::findVirtualCalls() const {
    vector<CallSite> callSites;

    CallSiteIterator cit, cend;
    for (boost::tie(cit, cend) = graph.callSites(); cit != cend; ++cit) {
        Function callee = target(*cit, *graph.getGraph());

        if (graph[callee].virtuality) {
            callSites.push_back(*cit);
        }
    }

    return callSites;
}

vector<Function> Analyzer::findFunctionsCalledOnce() const {
    vector<Function> functions;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        if (graph[*first].calls == 1) {
            functions.push_back(*first);
        }
    }

    return functions;
}

vector<Function> Analyzer::findLeafFunctions() const {
    vector<Function> functions;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        if (out_degree(*first, *graph.getGraph()) == 0) {
            functions.push_back(*first);
        }
    }

    return functions;
}

void findCluster(Function src, vector<CallSite>& cluster, set<Function>& visited, const Graph& graph) {
    if (visited.find(src) != visited.end()) {
        return;
    }

    //Search in all the out directions
    OutCallSiteIterator it, end;
    for (boost::tie(it, end) = out_edges(src, graph); it != end; ++it) {
        CallSite c = *it;
        Function f = target(c, graph);

        //Avoid searching again for the same call site
        if (find(cluster.begin(), cluster.end(), c) == cluster.end()) {
            cluster.push_back(c);

            findCluster(f, cluster, visited, graph);
        }
    }

    //Search in all the in directions
    InCallSiteIterator it2, end2;
    for (boost::tie(it2, end2) = in_edges(src, graph); it2 != end2; ++it2) {
        CallSite c = *it2;
        Function f = source(c, graph);

        //Avoid searching again for the same call site
        if (find(cluster.begin(), cluster.end(), c) == cluster.end()) {
            cluster.push_back(c);

            findCluster(f, cluster, visited, graph);
        }
    }

    visited.insert(src);
}

static double temperature(const std::vector<CallSite>& v, const CallGraph& graph) {
    double temperature = 0;

    for (std::vector<CallSite>::const_iterator it = v.begin(); it != v.end(); ++it) {
        temperature += graph[*it].temperature;
    }

    return temperature;
}

vector<Cluster> Analyzer::findClusters() const {
    Timer timer;

    Graph copy;
    copy_graph(*graph.getGraph(), copy);

    //Keep only the most interesting call sites
    FilterByTemperature filter(copy, Parameters::get(HOT_CALL_SITE));
    remove_edge_if(filter, copy);

    vector<Cluster> clusters;
    set<Function> visited;

    //We get the clusters of the graph
    FunctionIterator first, last;
    for (boost::tie(first, last) = vertices(copy); first != last; ++first) {
        if (visited.find(*first) == visited.end()) {
            //Search a cluster
            vector<CallSite> cluster;
            findCluster(*first, cluster, visited, copy);

            //We do not consider a single call site as a real cluster
            if (cluster.size() > 1) {
                //If the cluster is too big, we reduce it to the max size
                if (cluster.size() > Parameters::get(CLUSTER_MAX_SIZE)) {
                    sort(cluster.begin(), cluster.end(), SortByTemperature(graph));

                    cluster.erase(cluster.begin(), cluster.end() - Parameters::get(CLUSTER_MAX_SIZE));
                }

                clusters.push_back(Cluster(cluster, temperature(cluster, graph)));
            }
        }
    }

    return clusters;
}

void findPath(CallSite src, Function dest, vector<vector<CallSite>>& paths, vector<CallSite>& path, int limit, const CallGraph& graph) {
    //The path is too long
    if (limit == 0) {
        return;
    }

    path.push_back(src);

    string library = graph[target(src, *graph.getGraph())].module;

    //We are back in the dest library
    if (graph[dest].module == library) {
        paths.push_back(path); //Do a copy ? Add f to the path or implicitely ?
    } else {
        OutCallSiteIterator it, end;
        for (boost::tie(it, end) = out_edges(target(src, *graph.getGraph()), *graph.getGraph()); it != end; ++it) {
            CallSite c = *it;
            Function f = target(c, *graph.getGraph());

            if (graph[f].module == library || graph[f].module == graph[dest].module) {
                //Is this condition useful ?
                if (find(path.begin(), path.end(), c) == path.end()) {
                    findPath(c, dest, paths, path, limit - 1, graph);
                }
            }
        }
    }

    path.erase(path.end() - 1);
}

vector<vector<CallSite>> findPaths(CallSite src, Function dest, const CallGraph& graph, int L) {
    vector<vector<CallSite>> paths;
    vector<CallSite> path;

    findPath(src, dest, paths, path, L, graph);

    return paths;
}

int computeMinCalls(const vector<CallSite>& path, const CallGraph& graph) {
    unsigned int min = graph[path[0]].calls;

    for (std::vector<CallSite>::const_iterator it = path.begin(); it != path.end(); ++it) {
        min = std::min(min, graph[*it].calls);
    }

    return min;
}

unsigned long interestOfMoving(Function src, Function dest, const CallGraph& graph) {
    if (graph[src].module == graph[dest].module) {
        return 0;
    }

    unsigned long benefit = 0;

    //Search the calls from src function
    OutCallSiteIterator it, end;
    for (boost::tie(it, end) = out_edges(src, *graph.getGraph()); it != end; ++it) {
        CallSite c = *it;
        Function f = graph.target(c);

        //If moved to the library of the callee, we avoid library calls
        if (graph[f].module == graph[dest].module) {
            benefit += graph[c].calls;
        }

        //If both were in the same library, we adds more library calls
        if (graph[f].module == graph[src].module) {
            benefit -= graph[c].calls;
        }
    }

    //Search the calls to src function
    InCallSiteIterator it2, end2;
    for (boost::tie(it2, end2) = in_edges(src, *graph.getGraph()); it2 != end2; ++it2) {
        CallSite c = *it2;
        Function f = graph.source(c);

        //If the caller is in the dest library, we avoid library calls
        if (graph[f].module == graph[dest].module) {
            benefit += graph[c].calls;
        }

        //If both were in the same library, we adds more library calls
        if (graph[f].module == graph[src].module) {
            benefit -= graph[c].calls;
        }
    }

    return benefit;
}

void Analyzer::computeSolution(vector<string>& solutions, set<string>& libraryFilters, const string& function, const string& library, unsigned long benefit, unsigned long& total) const {
    if (benefit < Parameters::get(MOVE_BENEFIT_THRESHOLD)) {
        return;
    }

    if (filterDuplicates) {
        string hash = function + "->" + library;

        //Already known
        if (libraryFilters.count(hash) > 0) {
            return;
        }

        libraryFilters.insert(hash);
    }

    if (protectedLibraries.count(library) > 0) {
        solutions.push_back("Protected library : " + function + " should be moved to " + library + " benefit = " + toString<unsigned long>(benefit));
    } else {
        solutions.push_back(function + " should be moved to " + library + " benefit = " + toString<unsigned long>(benefit));
    }

    total += benefit;
}

unsigned long Analyzer::computeSolutions(Function src, Function dest, const vector<vector<CallSite>>& paths, vector<string>& solutions) const {
    set<string> libraryFilters;
    unsigned long benefit;

    unsigned long benefitSrcToDest = interestOfMoving(src, dest, graph);
    unsigned long benefitDestToSrc = interestOfMoving(dest, src, graph);

    //No benefit
    if (benefitSrcToDest <= 0 && benefitDestToSrc <= 0) {
        solutions.push_back("Benefit is not enough, neither of them should be moved");
    } else if (benefitSrcToDest > benefitDestToSrc) {
        computeSolution(solutions, libraryFilters, graph[src].name, graph[dest].module, benefitSrcToDest, benefit);

        //Consider moving the end of the paths too
        for (vector<vector<CallSite>>::const_iterator pit = paths.begin(); pit != paths.end(); ++pit) {
            Function function = graph.target((*pit)[pit->size() - 1]);

            //Don't test twice the same function in the case of a direct cycle between two functions
            if (graph[function].name != graph[src].name) {
                unsigned long b = interestOfMoving(function, dest, graph);

                computeSolution(solutions, libraryFilters, graph[function].name, graph[dest].module, b, benefit);
            }
        }
    } else {
        computeSolution(solutions, libraryFilters, graph[dest].name, graph[src].module, benefitDestToSrc, benefit);

        //Consider moving the middle of the path too
        for (vector<vector<CallSite>>::const_iterator pit = paths.begin(); pit != paths.end(); ++pit) {
            vector<CallSite> path = *pit;

            //Contains only src, dest and the last function in library(src)
            if (path.size() <= 3) {
                continue;
            }

            for (vector<CallSite>::size_type i = 1; i < path.size() - 1; ++i) {
                Function f = graph.target(path[i]);

                unsigned long b = interestOfMoving(f, src, graph);

                if (benefit >= Parameters::get(MOVE_BENEFIT_THRESHOLD)) {
                    computeSolution(solutions, libraryFilters, graph[f].name, graph[src].module, b, benefit);
                } else {
                    break; //To avoid breaking the path in too many library
                }
            }
        }
    }

    return benefit;
}

vector<LibraryIssue> Analyzer::findLibraryIssues() const {
    vector<LibraryIssue> issues;

    vector<CallSite> callSites = findInterLibraryCalls();

    for (std::vector<CallSite>::const_iterator iterator = callSites.begin(); iterator != callSites.end(); ++iterator) {
        //Consider only heavy paths
        if (graph[*iterator].calls >= Parameters::get(HEAVY_CALL_SITE)) {
            Function src = source(*iterator, *graph.getGraph());
            Function dest = target(*iterator, *graph.getGraph());
            vector<vector<CallSite>> paths;

            vector<string> solutions;
            unsigned long benefit = computeSolutions(src, dest, paths, solutions);

            issues.push_back(LibraryIssue(src, dest, paths, solutions, benefit));
        } else if (graph[*iterator].calls >= Parameters::get(LIBRARY_PATH_THRESHOLD)) {
            Function src = source(*iterator, *graph.getGraph());

            vector<vector<CallSite>> paths = findPaths(*iterator, src, graph, Parameters::get(LIBRARY_PATH_MAX_LENGTH));

            if (!paths.empty()) {
                vector<vector<CallSite>> result;

                //Filter the paths with not enough calls
                for (vector<vector<CallSite>>::const_iterator it = paths.begin(); it != paths.end(); ++it) {
                    int min = computeMinCalls(*it, graph);

                    if (min >= Parameters::get(LIBRARY_MIN_PATH_CALLS)) {
                        result.push_back(*it);
                    }
                }

                if (!result.empty()) {
                    Function dest = target(*iterator, *graph.getGraph());

                    vector<string> solutions;
                    unsigned long benefit = computeSolutions(src, dest, result, solutions);

                    issues.push_back(LibraryIssue(src, dest, result, solutions, benefit));
                }
            }
        }
    }

    return issues;
}

static void inline addLibrary(DependencyGraph& dependencyGraph, map<std::string, Library>& libraries, const std::string& libraryName) {
    if (libraryName.size() > 0 && libraries.find(libraryName) == libraries.end()) {
        Library library = add_vertex(dependencyGraph);
        dependencyGraph[library].name = libraryName;
        libraries[libraryName] = library;
    }
}

std::vector<std::vector<std::string>> Analyzer::findCircularDependencies() const {
    DependencyGraph dependencyGraph;

    map<std::string, Library> libraries;

    //Fill the dependency graph with the libraries and the dependencies amon them
    CallSiteIterator cit, cend;
    for (boost::tie(cit, cend) = graph.callSites(); cit != cend; ++cit) {
        CallSite callSite = *cit;

        std::string src = trim(graph[graph.source(callSite)].module);
        std::string target = trim(graph[graph.target(callSite)].module);

        //Add the library of the source and the target in the graph
        addLibrary(dependencyGraph, libraries, src);
        addLibrary(dependencyGraph, libraries, target);

        //Add the link between the two libraries if there is one
        if (src.size() > 0 && target.size() > 0 && src != target) {
            add_edge(libraries[src], libraries[target], dependencyGraph);
        }
    }

    std::vector<std::vector<std::string>> dependencies;

    std::vector<Library> components(num_vertices(dependencyGraph));

    //Use of Tarjan's Strongly connected components
    unsigned int num = strong_components(dependencyGraph, &components[0], vertex_index_map(get(vertex_index, dependencyGraph)));

    std::map<Library, std::vector<std::string>> circularDependencies;

    for (std::vector<Library>::size_type it = 0; it < num; ++it) {
        circularDependencies[components[it]].push_back(dependencyGraph[it].name);
    }

    //Filter the dependencies made of only one library
    for (std::map<Library, std::vector<std::string>>::const_iterator it = circularDependencies.begin(); it != circularDependencies.end(); ++it) {
        if (it->second.size() > 1) {
            dependencies.push_back(it->second);
        }
    }

    return dependencies;
}

vector<HierarchyIssue> Analyzer::findVirtualHierarchyIssues() const {
    vector<HierarchyIssue> issues;

    const VirtualFunctionSet& virtualFunctions = infos.getVirtualFunctions();

    if (virtualFunctions.size() == 0) {
        return issues;
    }

    map<string, set<string>> hierarchies;

    for (VirtualFunctionSet::const_iterator it = virtualFunctions.begin(); it != virtualFunctions.end(); ++it) {
        string hash = *it;

        vector<string> parts = split(hash, "##");

        string function = parts[1];

        //Empty function name
        if (function.size() <= 1) {
            continue;
        }

        if (infos.isDemangled()) {
            if (function.find("virtual thunk") != string::npos) {
                continue;
            }

            string::size_type pos = function.find("::");

            if (pos == string::npos) {
                continue;
            }

            string::size_type start = function.find("(");

            if (start == string::npos || start < pos) {
                continue;
            }

            while (true) {
                string::size_type pos2 = function.find("::", pos + 2);

                if (pos2 != string::npos && pos2 < start) {
                    pos = pos2;
                } else {
                    break;
                }
            }

            string functionName = function.substr(pos + 2);

            hierarchies[functionName].insert(function);
        } else {
            if (function.find("_ZN") == string::npos) {
                continue;
            }

            string::size_type pos = 0;

            while (!isdigit(function[pos])) {
                ++pos;
            }

            string length = "";

            while (isdigit(function[pos])) {
                length += function[pos];
                ++pos;
            }

            int x = toNumber<int>(length);
            pos += x;

            string functionName = function.substr(pos);

            hierarchies[functionName].insert(function);
        }
    }

    unordered_map<string, unsigned int> calls;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        calls[graph[*first].name] = graph[*first].calls;
    }

    for (map<string, set<string>>::const_iterator it = hierarchies.begin(); it != hierarchies.end(); ++it) {
        //Filters destructors
        if (it->first == "D1Ev") {
            continue;
        }

        if (it->second.size() > 1) {
            unsigned int sum = 0;
            unsigned int called = 0;

            for (set<string>::const_iterator fit = it->second.begin(); fit != it->second.end(); ++fit) {
                sum += calls[*fit];

                if (calls[*fit] > 0) {
                    called++;
                }
            }

            //Do not consider not called hierarchy
            if (sum > 0) {
                string issue = "";

                for (set<string>::const_iterator fit = it->second.begin(); fit != it->second.end(); ++fit) {
                    if (calls[*fit] == sum) {
                        issue = "Only " + *fit + " is called. This hierarchy should not be virtual";
                        break;
                    } else if (calls[*fit] > sum * Parameters::get(HIERARCHY_MAX_CALLS_FUNCTION)) {
                        issue = *fit + " is called more than " + toString<double>(Parameters::get(HIERARCHY_MAX_CALLS_FUNCTION) * 100.0) + "% of the time. Perhaps this hierarchy should not be virtual or this function should be called directly";
                        break;
                    }
                }

                //If less than the minimum percentage of functions are called
                if (issue.size() == 0 && called < Parameters::get(HIERARCHY_MIN_CALLED_FUNCTIONS) * it->second.size()) {
                    issue = "Less than " + toString<double>(Parameters::get(HIERARCHY_MIN_CALLED_FUNCTIONS) * 100.0) + "% of the functions are called. ";
                }

                if (issue.size() > 0) {
                    vector<string> members;

                    for (set<string>::const_iterator fit = it->second.begin(); fit != it->second.end(); ++fit) {
                        members.push_back(*fit + " : " + toString<unsigned int>(calls[*fit]));
                    }

                    issues.push_back(HierarchyIssue(it->first, sum, issue, members));
                }
            }
        } else if (it->second.size() == 1) {
            int sum = calls[*it->second.begin()];

            //Do not consider not called hiearchy
            if (sum > 0) {
                string issue = "The hierarchy contains only one member. This hierarchy should not be virtual";

                vector<string> members;
                members.push_back(*it->second.begin() + " : " + toString<unsigned int>(sum));

                issues.push_back(HierarchyIssue(it->first, sum, issue, members));
            }
        }
    }

    return issues;
}
