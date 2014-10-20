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

#ifndef CALL_GRAPH_H
#define CALL_GRAPH_H

#include <algorithm>

#include "Graph.hpp"

namespace inlining {

/*!
 * \class CallGraph
 * \brief A call graph
 */
class CallGraph {
    public:
        /*!
         * \brief Create a new call graph from a Graph
         * \param g The graph to create the call graph from
        */
        explicit CallGraph(Graph* g) : graph(g) {}

        /*!
         * \brief Destroy the call graph and release the resources
        */
        ~CallGraph();

        /*!
         * \brief Return iterators to the functions of the call graph
         * \return a pair of iterators to the functions, the first element of the pair is the begin iterator and the second one is the end iterator
        */
        std::pair<FunctionIterator, FunctionIterator> functions();

        /*!
         * \brief Return iterators to the functions of the call graph
         * \return a pair of iterators to the functions, the first element of the pair is the begin iterator and the second one is the end iterator
        */
        std::pair<FunctionIterator, FunctionIterator> functions() const;

        /*!
         * \brief Return iterators to the call sites of the call graph
         * \return a pair of iterators to the call sites, the first element of the pair is the begin iterator and the second one is the end iterator
        */
        std::pair<CallSiteIterator, CallSiteIterator> callSites();

        /*!
         * \brief Return iterators to the call sites of the call graph
         * \return a pair of iterators to the call sites, the first element of the pair is the begin iterator and the second one is the end iterator
        */
        std::pair<CallSiteIterator, CallSiteIterator> callSites() const;

        /*!
         * \brief Return the intern graph
         * \return The intern graph
        */
        inline Graph* getGraph() {
            return graph;
        }

        /*!
         * \brief Return the intern graph
         * \return The intern graph
        */
        inline const Graph* getGraph() const {
            return graph;
        }

        /*!
         * \brief Return the description of a call site
         * \param callSite the call site to get the description for
         * \return The description of the given call site
        */
        std::string getDescription(CallSite callSite) const;

        /*!
         * \brief Set the total number of calls of the call graph
         * \param c The total number of calls of the call graph
        */
        void setNumberOfCalls(unsigned int c);

        /*!
         * \brief Return the number of calls of the call graph
         * \return The total number of calls of the call graph
        */
        unsigned int getNumberOfCalls() const;

        /*!
         * \brief Set the application size
         * \param size The total size of the application
        */
        void setApplicationSize(unsigned int size);

        /*!
         * \brief Return the application size
         * \return The application size
        */
        unsigned int getApplicationSize() const;

        /*!
         * \brief Return the number of functions of the call graph
         * \return The number of functions of the call graph
        */
        unsigned int numberOfFunctions() const;

        /*!
         * \brief Return the number of call sites of the call graph
         * \return The number of call sites of the call graph
        */
        unsigned int numberOfCallSites() const;

        /*!
         * \brief Return the source function of the call site
         * \param callSite The call site to search the source of
         * \return The source function of the call site
        */
        Function source(CallSite callSite) const {
            return boost::source(callSite, *graph);
        }

        /*!
         * \brief Return the target function of the call site
         * \param callSite The call site to search the target of
         * \return The target function of the call site
        */
        Function target(CallSite callSite) const {
            return boost::target(callSite, *graph);
        }

        /*!
         * \brief Return the function information from the function descriptor
         * \param f the function descriptor
         * \return The function information
        */
        vertex_info& operator[](Function f) {
            return (*graph)[f];
        }

        /*!
         * \brief Return the function information from the function descriptor
         * \param f the function descriptor
         * \return The function information
        */
        const vertex_info& operator[](Function f) const {
            return (*graph)[f];
        }

        /*!
         * \brief Return the call site information from the call site descriptor
         * \param c the call site descriptor
         * \return The call site information
        */
        edge_info& operator[](CallSite c) {
            return (*graph)[c];
        }

        /*!
         * \brief Return the call site information from the call site descriptor
         * \param c the call site descriptor
         * \return The call site information
        */
        const edge_info& operator[](CallSite c) const {
            return (*graph)[c];
        }

    private:
        Graph* graph;

        unsigned int calls;
        unsigned int applicationSize;
};

} //end of inlining

#endif
