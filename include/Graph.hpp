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

#ifndef Graph_H
#define Graph_H

#include <boost/graph/adjacency_list.hpp>

namespace inlining {

/*!
 * \struct vertex_info
 * \brief struct containing all the information about a vertex of the graph
 */
struct vertex_info {
    std::string name;           /*!< The name of the function */
    std::string label;          /*!< The label */
    std::string file_name;      /*!< The filename the function has been defined in */
    std::string module;         /*!< The shared object of the function */
    double self_cost;           /*!< The cost of the function */
    double inclusive_cost;      /*!< The inclusive cost of the function */
    double temperature;         /*!< The temperature of the function */
    double frequency;           /*!< The frequency of the function */
    unsigned int calls;         /*!< The number of calls of the function */
    unsigned int parameters;    /*!< The number of parameters of the function */
    unsigned int size;          /*!< The size of the function */
    bool virtuality;            /*!< Indicate if the function is virtual (true) or not (false) */
};

/*!
 * \struct edge_info
 * \brief struct containing all the information about an edge of the graph
 */
struct edge_info {
    std::string name;       /*!< The name of the call site */
    std::string label;      /*!< The label */
    unsigned int calls;     /*!< The number of calls of the call site */
    double temperature;     /*!< The temperature of the call site */
    double frequency;       /*!< The frequency of the call site */
    double cost;            /*!< The cost of the call site */
};

/*!
 * \brief The properties of the graph, only the name
 */
typedef boost::property < boost::graph_name_t, std::string > GraphProperties;

/*!
 * \brief A graph. Used to store the call graph.
 */
typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::bidirectionalS, vertex_info, edge_info, GraphProperties> Graph;

//Iterators

/*!
 * \brief Iterator on the call sites of a graph.
 */
typedef boost::graph_traits<Graph>::edge_iterator CallSiteIterator;

/*!
 * \brief Iterator on the functions of a graph.
 */
typedef boost::graph_traits<Graph>::vertex_iterator FunctionIterator;

/*!
 * \brief Iterator on the out call sites of a function.
 */
typedef boost::graph_traits<Graph>::out_edge_iterator OutCallSiteIterator;

/*!
 * \brief Iterator on the in call sites of a function.
 */
typedef boost::graph_traits<Graph>::in_edge_iterator InCallSiteIterator;

/*!
 * \brief Iterator on the adjacent functions of a function.
 */
typedef boost::graph_traits<Graph>::adjacency_iterator AdjacentFunctionIterator;

//Descriptors

/*!
 * \brief A call site descriptor.
 */
typedef boost::graph_traits<Graph>::edge_descriptor CallSite;

/*!
 * \brief A function descriptor.
 */
typedef boost::graph_traits<Graph>::vertex_descriptor Function;

//---
//Graph for circular dependencies analysis
//---

/*!
 * \struct library_info
 * \brief struct containing all the information about a library in the library dependency graph.
 */
struct library_info {
    std::string name;           /*!< The name of the library */
};

/*!
 * \brief A dependency graph. Used only to generate the circular dependencies.
 */
typedef boost::adjacency_list < boost::setS, boost::vecS, boost::directedS, vertex_info, edge_info, library_info> DependencyGraph;

//Descriptors

/*!
 * \brief A dependency descriptor.
 */
typedef boost::graph_traits<DependencyGraph>::edge_descriptor Dependency;

/*!
 * \brief A library descriptor.
 */
typedef boost::graph_traits<DependencyGraph>::vertex_descriptor Library;

} //end of inlining

#endif
