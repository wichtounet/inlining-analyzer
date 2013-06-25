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

#ifndef REPORTS_H
#define REPORTS_H

#include <string>
#include <vector>
#include <set>

#include "CallGraph.hpp"
#include "Graph.hpp"

namespace inlining {

class Analyzer;

/*!
 * \class Reports
 * \brief Utility class to get statistic report and issues about the call graph
 */
class Reports {
    public:
        /*!
         * \brief Construct a new Reports
         * \param g The call graph
         * \param a The analyzer
        */
        Reports(const CallGraph& g, const Analyzer& a) : graph(g), analyzer(a) {}

        /*!
         * \brief Add a filter
         * \param filter the filter to add
        */
        void addFilter(const std::string& filter);

        /*!
         * \brief Enable the default filters
        */
        void enableDefaultFilters();

        /*!
         * \brief Indicate if the given function has to be filtered
         * \param f The function to test
         * \return true if the function has to be filtered otherwise false
        */
        bool filter(const Function& f) const;

        /*!
         * \brief Indicate if the given call site has to be filtered
         * \param cs The call site to test
         * \return true if the call site has to be filtered otherwise false
        */
        bool filter(const CallSite& cs) const;

        /*!
         * \brief Return the top biggest functions
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top biggest functions of the application
        */
        std::vector<Function> getBiggestFunctions(unsigned int top = 20) const;

        /*!
         * \brief Return the top most costly functions
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top most costly functions of the application
        */
        std::vector<Function> getMostCostlyFunctions(unsigned int top = 20) const;

        /*!
         * \brief Return the top most interesting functions
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top most interesting functions of the application
        */
        std::vector<Function> getMostInterestingFunctions(unsigned int top = 20) const;

        /*!
         * \brief Return the top most called functions
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top most called functions of the application
        */
        std::vector<Function> getMostCalledFunctions(unsigned int top = 20) const;

        /*!
         * \brief Return the top most called call sites
         * \param top The number of call sites to return, 20 by default
         * \return A vector containing the top most called call sites of the application
        */
        std::vector<CallSite> getMostCalledCallSites(unsigned int top = 20) const;

        /*!
         * \brief Return the top most parameterized functions
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top most parameterized functions of the application
        */
        std::vector<Function> getMostParameterizedFunctions(unsigned int top = 20) const;

        /*!
         * \brief Return the top most interesting call sites
         * \param top The number of call sites to return, 20 by default
         * \return A vector containing the top most interesting call sites of the application
        */
        std::vector<CallSite> getMostInterestingCallSites(unsigned int top = 20) const;

        /*!
         * \brief Return the top tiniest functions
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top tiniest functions of the application
        */
        std::vector<Function> getTiniestFunctions(unsigned int top = 20) const;

        /*!
         * \brief Return the top heaviest inter library call sites
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top inter library call sites of the application
        */
        std::vector<CallSite> getHeaviestInterLibraryCalls(unsigned int top = 20) const;

        /*!
         * \brief Return the top heaviest virtual call sites
         * \param top The number of functions to return, 20 by default
         * \return A vector containing the top heaviest virtual call sites of the application
        */
        std::vector<CallSite> getHeaviestVirtualCalls(unsigned int top = 20) const;

        /*!
         * \brief Print all the statistic report
         * \param top The number of elements to print in each report
        */
        void printStatisticReports(unsigned int top = 20) const;

        /*!
         * \brief Print all issues
        */
        void printIssues() const;

        /*!
         * \brief Print all the library issues.
        */
        void printLibraryIssues() const;

        /*!
         * \brief Print all the clusters.
        */
        void printClusters() const;

        /*!
         * \brief Print all the circular dependencies.
        */
        void printCircularDependencies() const;

        /*!
         * \brief Print the virtual hierarchy issues.
        */
        void printVirtualHierarchyIssues() const;

        /*!
         * \brief Print all the functions with too many parameters.
        */
        void printFunctionsWithTooManyParameters() const;

        /*!
         * \brief Print the top function temperature
         * \param top The number of functions to print, 20 by default
        */
        void printFunctionHeuristics(unsigned int top = 20) const;

        /*!
         * \brief Print the top call sites temperature
         * \param top The number of call sites to print, 20 by default
        */
        void printCallSiteHeuristics(unsigned int top = 20) const;

        /*!
         * \brief Print the top heaviest inter library calls
         * \param top The number of calls to print, 20 by default
        */
        void printHeavyInterLibrariesCalls(unsigned int top = 20) const;

        /*!
         * \brief Print the top heaviest virtual calls
         * \param top The number of calls to print, 20 by default
        */
        void printHeavyVirtualCalls(unsigned int top = 20) const;

        /*!
         * \brief Print the top most called functions
         * \param top The number of functions to print, 20 by default
        */
        void printMostCalledFunctions(unsigned int top = 20) const;

        /*!
         * \brief Print the top most called call sites
         * \param top The number of call sites to print, 20 by default
        */
        void printMostCalledCallSites(unsigned int top = 20) const;

        /*!
         * \brief Print the top tiniest functions
         * \param top The number of functions to print, 20 by default
        */
        void printTiniestFunctions(unsigned int top = 20) const;

        /*!
         * \brief Print the top most parameterized functions
         * \param top The number of functions to print, 20 by default
        */
        void printMostParameterizedFunctions(unsigned int top = 20) const;

        /*!
         * \brief Print the top most costly functions
         * \param top The number of functions to print, 20 by default
        */
        void printMostCostlyFunctions(unsigned int top = 20) const;

        /*!
         * \brief Print the top biggest functions
         * \param top The number of functions to print, 20 by default
        */
        void printBiggestFunctions(unsigned int top = 20) const;

        /*!
         * \brief Print the functions called once
        */
        void printFunctionsCalledOnce() const;

        /*!
         * \brief Print the leaf functions
        */
        void printLeafFunctions() const;

        /*!
         * \brief Print the functions that takes 80% of the run time and print how many % represent these functions
        */
        void printParetoFunctions() const;

        /*!
         * \brief Return the functions that have too many parameters
         * \return A vector containing all the functions that have too many parameters
        */
        std::vector<Function> getOverParameterizedFunctions() const;

        /*!
         * \brief Return all the top great functions according to the given comparator
         * \param top The number of functions to return
         * \param compare The comparator to use
         * \return A vector containing the top great functions
        */
        template <class Compare>
        std::vector<Function> getTopFunctions(unsigned int top, Compare compare) const {
            std::vector<Function> functions;

            FunctionIterator first, last;
            for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
                if (!filter(*first)) {
                    functions.push_back(*first);
                }
            }

            keepTop(top, compare, functions);

            return functions;
        }

        /*!
         * \brief Return all the top great call sites according to the given comparator
         * \param top The number of call sites to return
         * \param compare The comparator to use
         * \return A vector containing the top great call sites
        */
        template <class Compare>
        std::vector<CallSite> getTopCallSites(unsigned int top, Compare compare) const {
            std::vector<CallSite> callSites;

            CallSiteIterator first, last;
            for (boost::tie(first, last) = graph.callSites(); first != last; ++first) {
                if (!filter(*first)) {
                    callSites.push_back(*first);
                }
            }

            keepTop(top, compare, callSites);

            return callSites;
        }
    private:
        std::set<std::string> filters;
        const CallGraph& graph;
        const Analyzer& analyzer;

        void printSolution(std::set<std::string>& libraryFilters, const std::string& function, const std::string& library, unsigned int benefit) const;
};

/*!
 * \brief Filter the values to keep only the top elements according to the given comparator
 * \param top The number of elements to keep
 * \param compare The comparator to use
 * \param values The values to filter
*/
template <class Compare, class T>
void keepTop(unsigned int top, Compare compare, std::vector<T>& values) {
    std::sort(values.rbegin(), values.rend(), compare);

    if (values.size() > top) {
        values.erase(values.begin() + top, values.end());
    }
}

/*!
 * \brief Filter the values to keep only the top min elements according to the given comparator
 * \param top The number of elements to keep
 * \param compare The comparator to use
 * \param values The values to filter
*/
template <class Compare, class T>
void keepMin(unsigned int top, Compare compare, std::vector<T>& values) {
    std::sort(values.begin(), values.end(), compare);

    if (values.size() > top) {
        values.erase(values.begin() + top, values.end());
    }
}

} //end of inlining

#endif
