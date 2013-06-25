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

#ifndef Analyzer_H
#define Analyzer_H

#include <vector>
#include <string>

#include "Graph.hpp"

namespace inlining {

class CallGraph;
class Infos;

/*!
 * \struct LibraryIssue
 * \brief Structure containing the information about a library issue
*/
struct LibraryIssue {
    Function src;                               /*!< The function that initiates the heavy call site  */
    Function dest;                              /*!< The target function  */
    std::vector<std::vector<CallSite>> paths;   /*!< All the problematic paths from dest to src  */
    std::vector<std::string> solutions;         /*!< The proposed solutions for this issue  */
    unsigned long benefit;                      /*!< The benefit */

    /*!
     * \brief Construct a new LibraryIssue.
     * \param p_src The function that initiates the heavy call site
     * \param p_dest The target function
     * \param p_paths All the problematic paths from dest to src
     * \param p_solutions The proposed solutions for this issue
     * \param b The benefit
    */
    LibraryIssue(Function p_src, Function p_dest, std::vector<std::vector<CallSite>> p_paths, std::vector<std::string> p_solutions, unsigned long b) : src(p_src), dest(p_dest), paths(p_paths), solutions(p_solutions), benefit(b) {}
};

/*!
 * \struct Cluster
 * \brief Structure containing the information about a cluster
*/
struct Cluster {
    std::vector<CallSite> callSites;        /*!< The call sites of the cluster  */
    double temperature;                     /*!< The temperature of this cluster  */

    /*!
     * \brief Construct a new Cluster.
     * \param cs The call sites of the cluster
     * \param t The temperature of this cluster
    */
    Cluster(std::vector<CallSite> cs, double t) : callSites(cs), temperature(t) {}
};

/*!
 * \struct HierarchyIssue
 * \brief Structure containing the information about a hierarchy issue
*/
struct HierarchyIssue {
    std::string name;                       /*!< The name of the hierarchy  */
    unsigned int calls;                     /*!< The number of calls of this hierarchy  */
    std::string issue;                      /*!< The issue related to this hiearchy  */
    std::vector<std::string> members;       /*!< The members of the hierarchy  */

    /*!
     * \brief Construct a new HierarchyIssue.
     * \param n The name of the hierarchy
     * \param c The number of calls of this hierarchy
     * \param i The issue related to this hiearchy
     * \param m The members of the hierarchy
    */
    HierarchyIssue(std::string n, unsigned int c, std::string i, std::vector<std::string> m) : name(n), calls(c), issue(i), members(m) {}
};

/*!
 * \class Analyzer
 * \brief Class used to find issues in the call graph
 */
class Analyzer {
    public:
        /**
         * Construct an analyzer for th given call graph
         *
         * \param g The call graph to analyze.
         * \param i The infos to use for the analysis.
         */
        Analyzer(CallGraph& g, Infos& i) : graph(g), infos(i), filterDuplicates(false) {}

        /**
         * \brief launch the analyze on the call graph
         */
        void analyze();

        /*!
         * \brief Configure the filtering of duplicates option
         * \param filter Indicate if we must activate (true) or disable (false) the duplicates filtering.
         */
        void setFilterDuplicates(bool filter);

        /*!
         * \brief Set the given library as protected to avoid giving solutions moving files into it.
         * \param library The library that must be considered as protected.
         */
        void addProtectedLibrary(const std::string& library);

        /**
         * \brief Find every call site between two different libraries
         *
         * \return a vector containing all the call sites between two different libraries
         */
        std::vector<CallSite> findInterLibraryCalls() const;

        /**
         * \brief Find every virtual call sites.
         *
         * \return a vector containing all the virtual call sites
         */
        std::vector<CallSite> findVirtualCalls() const;

        /**
         * \brief Find all the function called only once
         *
         * \return A vector containing all the functions called only once
         */
        std::vector<Function> findFunctionsCalledOnce() const;

        /**
         * \brief Find all the leaf functions. A function is leaf when it calls no other functions.
         *
         * \return A vector containing all the leaf functions.
         */
        std::vector<Function> findLeafFunctions() const;

        /**
         * \brief Find library issues.
         *
         * \return A vector containing all the library issues.
         */
        std::vector<LibraryIssue> findLibraryIssues() const;

        /**
         * \brief Find hot clusters.
         *
         * \return A vector containing all the clusters (vector of CallSite)
         */
        std::vector<Cluster> findClusters() const;

        /**
         * \brief Find circular dependencies.
         *
         * \return A vector containing the description of every circular dependencies in the graph.
         */
        std::vector<std::vector<std::string>> findCircularDependencies() const;

        /**
         * \brief Find the virtual hiearchy issues.
         *
         * \return A vector containing the description of every virtual hiearchy issue in the graph.
         */
        std::vector<HierarchyIssue> findVirtualHierarchyIssues() const;

        /**
         * \brief Compute the temperature of the given function.
         *
         * \param function The function descriptor.
         *
         * \return The temperature of the function.
         */
        void computeFunctionTemperature(Function function);

        /**
         * \brief Compute the temperature of the given call site.
         *
         * \param  site The call site descriptor.
         *
         * \return The temperature of the call site.
         */
        void computeCallSiteTemperature(CallSite site);

    private:
        CallGraph& graph;
        Infos& infos;

        std::set<std::string> protectedLibraries;
        bool filterDuplicates;

        unsigned long computeSolutions(Function src, Function dest, const std::vector<std::vector<CallSite>>& paths, std::vector<std::string>& solutions) const;
        void computeSolution(std::vector<std::string>& solutions, std::set<std::string>& libraryFilters, const std::string& function, const std::string& library, unsigned long benefit, unsigned long& total) const;
};

} //end of inlining

#endif
