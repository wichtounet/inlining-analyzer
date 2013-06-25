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

#ifndef SORTERS_H
#define SORTERS_H

#include <vector>

namespace inlining {

class CallGraph;

/*!
 * \class SortByTemperature
 * \brief Sort something by temperature
 */
class SortByTemperature {
    public:
        /*!
         * \brief Create the sorter.
         * \param g The call graph to use.
        */
        explicit SortByTemperature(const CallGraph& g) : graph(g) {}

        /*!
         * \brief Compare the two objects by their temperature.
         * \param a The first object.
         * \param b The second object.
         * \tparam T The type of object to compare
        */
        template<typename T>
        bool operator()(const T& a, const T& b) {
            return graph[a].temperature < graph[b].temperature;
        }
    private:
        const CallGraph& graph;
};

/*!
 * \class SortByCalls
 * \brief Sort something by the number of calls
 */
class SortByCalls {
    public:
        /*!
         * \brief Create the sorter.
         * \param g The call graph to use.
        */
        explicit SortByCalls(const CallGraph& g) : graph(g) {}

        /*!
         * \brief Compare the two objects by their number of calls.
         * \param a The first object.
         * \param b The second object.
         * \tparam T The type of object to compare
        */
        template<typename T>
        bool operator()(const T& a, const T& b) {
            return graph[a].calls < graph[b].calls;
        }
    private:
        const CallGraph& graph;
};

/*!
 * \class SortBySize
 * \brief Sort something by size
 */
class SortBySize {
    public:
        /*!
         * \brief Create the sorter.
         * \param g The call graph to use.
        */
        explicit SortBySize(const CallGraph& g) : graph(g) {}

        /*!
         * \brief Compare the two objects by their size.
         * \param a The first object.
         * \param b The second object.
         * \tparam T The type of object to compare
        */
        template<typename T>
        bool operator()(const T& a, const T& b) {
            return (graph[a].size) < (graph[b].size);
        }
    private:
        const CallGraph& graph;
};

/*!
 * \class SortByParameters
 * \brief Sort something by the number of parameters
 */
class SortByParameters {
    public:
        /*!
         * \brief Create the sorter.
         * \param g The call graph to use.
        */
        explicit SortByParameters(const CallGraph& g) : graph(g) {}

        /*!
         * \brief Compare the two objects by their number of parameters.
         * \param a The first object.
         * \param b The second object.
         * \tparam T The type of object to compare
        */
        template<typename T>
        bool operator()(const T& a, const T& b) {
            return graph[a].parameters < graph[b].parameters;
        }
    private:
        const CallGraph& graph;
};

/*!
 * \class SortBySelfCosts
 * \brief Sort something by self cost
 */
class SortBySelfCosts {
    public:
        /*!
         * \brief Create the sorter.
         * \param g The call graph to use.
        */
        explicit SortBySelfCosts(const CallGraph& g) : graph(g) {}

        /*!
         * \brief Compare the two objects by their self costs.
         * \param a The first object.
         * \param b The second object.
         * \tparam T The type of object to compare
        */
        template<typename T>
        bool operator()(const T& a, const T& b) {
            return graph[a].self_cost < graph[b].self_cost;
        }
    private:
        const CallGraph& graph;
};

/*!
 * \class SortCluster
 * \brief Sort the clusters by temperature.
 */
class SortCluster {
    public:
        /*!
         * \brief Compare the two clusters by their temperature.
         * \param a The first object.
         * \param b The second object.
        */
        bool operator()(const Cluster& a, const Cluster& b) {
            return a.temperature < b.temperature;
        }
};

/*!
 * \class SortLibraryIssue
 * \brief Sort the library issues by benefit.
 */
class SortLibraryIssue {
    public:
        /*!
         * \brief Compare the two library issues by their benefit.
         * \param a The first object.
         * \param b The second object.
        */
        bool operator()(const LibraryIssue& a, const LibraryIssue& b) {
            return a.benefit < b.benefit;
        }
};

/*!
 * \class SortHierarchyIssue
 * \brief Sort the hierarchy issues by calls.
 */
class SortHierarchyIssue {
    public:
        /*!
         * \brief Compare the two hierarchy issues by their number of calls.
         * \param a The first object.
         * \param b The second object.
         * \tparam T The type of object to compare
        */
        bool operator()(const HierarchyIssue& a, const HierarchyIssue& b) {
            return a.calls < b.calls;
        }
};

/*!
 * \class FilterByTemperature
 * \brief Filter the given values keeping only the values with a temperature greater than the given threshold.
 * \param T The type of object to filter.
 */
class FilterByTemperature {
    public:
        /*!
         * \brief Create the sorter.
         * \param g The call graph to use.
         * \param t The threshold.
        */
        FilterByTemperature(const Graph& g, double t) : graph(g), threshold(t) {}

        /*!
         * \brief Filter the given object if its temperature is lower than the threshold
         * \param a The first object.
         * \tparam T The type of object to compare
        */
        template <typename T>
        bool operator()(const T& a) const  {
            return graph[a].temperature < threshold;
        }
    private:
        const Graph& graph;
        double threshold;
};

} //end of inlining

#endif
