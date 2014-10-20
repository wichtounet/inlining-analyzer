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
#include <cctype>

#include "GraphReader.hpp"
#include "CallGraph.hpp"
#include "Utils.hpp"
#include "Timer.hpp"
#include "Graph.hpp"
#include "Logging.hpp"
#include "Infos.hpp"

#include "boost/graph/graphviz.hpp"
#include "boost/graph/adjacency_list.hpp"

using namespace std;
using namespace boost;
using namespace inlining;

CallGraph* GraphReader::read(const string& file) {
    DEBUG cout << "DEBUG : Importing the .dot file " << file << endl;

    Timer timer;

    Graph* graph = new Graph();
    dynamic_properties dp;

    dp.property("node_id", get(&vertex_info::name, *graph));

    //Vertex properties

    dp.property("label", get(&vertex_info::label, *graph));
    dp.property("filename", get(&vertex_info::file_name, *graph));
    dp.property("module", get(&vertex_info::module, *graph));

    //Edge properties

    dp.property("label", get(&edge_info::label, *graph));

    // Use ref_property_map to turn a graph property into a property map
    ref_property_map<Graph*, string> gname(get_property(*graph, graph_name));
    dp.property("name", gname);

    ifstream inFile;
    inFile.open(file.c_str());

    bool status = read_graphviz(inFile, *graph, dp, "node_id");

    inFile.close();

    DEBUG cout << "DEBUG : Status : " << status << endl;
    DEBUG cout << "DEBUG : Number of edges : " << num_edges(*graph) << endl;
    DEBUG cout << "DEBUG : Number of vertices : " << num_vertices(*graph) << endl;
    PERF cout << "PERF : Importation finished in " << timer.elapsed() << "ms" << endl;

    CallGraph* callGraph = new CallGraph(graph);

    extractInformation(*callGraph);

    return callGraph;
}

//Must be called after parseVertices() in order to have the total of calls computed
void GraphReader::parseEdges(CallGraph& graph) {
    CallSiteIterator first, last;
    for (boost::tie(first, last) = graph.callSites(); first != last; ++first) {
        auto& callSite = graph[*first];

        string label = callSite.label;

        if (label.length() == 0) {
            continue;
        }

        //Only the number of calls is included
        if (label.find("\\n") == string::npos) {
            string callsToken;
            callsToken = label.substr(0, label.length() - 2);

            callSite.cost = 0.0;
            callSite.calls = toNumber<unsigned int>(callsToken);
        } else { //Both the number of calls and the cost are included
            string::size_type mid = label.find("\\n");

            string costToken = label.substr(0, mid);
            string callsToken = label.substr(mid + 2, label.length() - 2 - mid - 2);

            callSite.cost = toNumber<double>(costToken);
            callSite.calls = toNumber<unsigned int>(callsToken);
        }

        callSite.frequency = callSite.calls / static_cast<double>(graph.getNumberOfCalls());
    }
}

static int countParameters(const string& name) {
    string::size_type start = name.rfind(")");

    if (start == string::npos) {
        return 0;
    }

    string::size_type i = start - 1;
    unsigned int parameters = 0;
    unsigned int inTemplate = 0;    //To avoid counting parameters in a template declaration
    unsigned int inParenth = 0;     //To avoid counting parameters in a function pointer declaration

    //Parse from the right
    while (!(inParenth == 0 && name[i] == '(')) {
        if (!isspace(name[i]) && parameters == 0) {
            parameters = 1;
        }

        if (name[i] ==')') {
            inParenth++;
        } else if (name[i] == '(') {
            inParenth--;
        }

        if (inParenth == 0) {
            if (name[i] == '>') {
                inTemplate++;
            } else if (name[i] == '<') {
                inTemplate--;
            }

            if (name[i] == ',' && inTemplate == 0) {
                ++parameters;
            }
        }
        --i;
    }

    return parameters;
}

void GraphReader::parseVertices(CallGraph& graph) {
    unsigned int totalCalls = 0;
    unsigned int totalSize = 0;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        auto& function = graph[*first];

        //Demangle only if necessary
        if (infos.isDemangled()) {
            function.parameters = countParameters(function.name);
        } else {
            function.parameters = countParameters(demangle(function.name));
        }

#ifdef BENCH
        function.size = 1 + (rand() % 1000);
        function.virtuality = (rand() % 6) < 2;
#else
        function.size = infos.sizeOf(function.module, function.name);
        function.virtuality = infos.isVirtual(function.module, function.name);
#endif

        totalSize += function.size;

        string label = function.label;

        if (label.length() == 0) {
            continue;
        }

        string::size_type firstPos = label.find("\\n");
        string::size_type secondPos = label.find("\\n", firstPos + 2);
        string::size_type thirdPos = label.find("\\n", secondPos + 2);

        string inclusiveCostToken = label.substr(firstPos + 2, secondPos - firstPos - 3);
        string selfCostToken = label.substr(secondPos + 3, thirdPos - secondPos - 4);
        string callsToken = label.substr(thirdPos + 2, label.length() - thirdPos - 4);

        function.inclusive_cost = toNumber<double>(inclusiveCostToken);
        function.self_cost = toNumber<double>(selfCostToken);
        function.calls = toNumber<unsigned int>(callsToken);

        totalCalls += function.calls;
    }

    //Once we have the total of calls we can compute the frequency
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        auto& function = graph[*first];
        function.frequency = static_cast<double>(function.calls) / static_cast<double>(totalCalls);
    }

    graph.setNumberOfCalls(totalCalls);
    graph.setApplicationSize(totalSize);
}

void GraphReader::extractInformation(CallGraph& graph) {
    Timer timer;

    parseVertices(graph);
    parseEdges(graph);

    PERF cout << "PERF : Information extracted in " << timer.elapsed() << "ms" << endl;
}
