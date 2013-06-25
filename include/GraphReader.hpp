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

#ifndef GraphReader_H
#define GraphReader_H

#include "Graph.hpp"

namespace inlining {

class Infos;
class CallGraph;

/*!
 * \class GraphReader
 * \brief A reader for a call graph.
 *        Reads a callgraph from a .dot and creates a CallGraph to use
 */
class GraphReader {
    public:
        /*!
         * \brief Construct a new GraphReader.
         * \param i The Infos object to use to retrieve information about the shared object.
         */
        explicit GraphReader(Infos& i) : infos(i) {}

        /*!
         * \brief Read a dot file and creates a new call graph. It is up to the user to delete the CallGraph
         * \param file The path to the file to read
         */
        CallGraph* read(const std::string& file);

        //Only here for bench
        /*!
         * \brief extract informations about the call graph
         * \param graph The call graph to extract informations for
         */
        void extractInformation(CallGraph& graph);
    private:
        Infos& infos;

        void parseVertices(CallGraph& graph);
        void parseEdges(CallGraph& graph);
};

} //end of inlining

#endif
