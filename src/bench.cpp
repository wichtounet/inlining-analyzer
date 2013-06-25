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
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "boost/graph/breadth_first_search.hpp"
#include <boost/graph/depth_first_search.hpp>

#include "Timer.hpp"
#include "Graph.hpp"
#include "CallGraph.hpp"
#include "Infos.hpp"
#include "InfosOld.hpp"
#include "Analyzer.hpp"
#include "Parameters.hpp"

#include <unistd.h>
#include <ios>

#define BENCH
#include "GraphReader.cpp"

static const double GRAPHS = 5;

static double reading = 0;
static double extraction = 0;
static double foreachVertices = 0;
static double foreachEdges = 0;
static double dfs = 0;
static double bfs = 0;
static double analyzeVertices = 0;
static double analyzeEdges = 0;
static double clustering = 0;
static double library = 0;
static double circular = 0;

using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;

using namespace inlining;

void benchReading(Graph* graph, string file) {
    Timer timer;

    dynamic_properties dp;
    dp.property("node_id", get(&vertex_info::name, *graph));
    dp.property("label", get(&vertex_info::label, *graph));
    dp.property("filename", get(&vertex_info::file_name, *graph));
    dp.property("module", get(&vertex_info::module, *graph));
    dp.property("label", get(&edge_info::label, *graph));

    ref_property_map<Graph*, string> gname(get_property(*graph, graph_name));
    dp.property("name", gname);

    ifstream inFile;
    inFile.open(file.c_str());

    read_graphviz(inFile, *graph, dp, "node_id");

    inFile.close();

    reading += timer.elapsed();
}

void benchExtraction(CallGraph* graph) {
    Timer timer;

    Infos infos;
    GraphReader reader(infos);
    reader.extractInformation(*graph);

    extraction += timer.elapsed();
}

static double fakeSum = 0;

void benchForeachVertices(const CallGraph& graph) {
    double sum = 0;

    Timer timer;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        sum += graph[*first].temperature;
    }

    foreachVertices += timer.elapsed();

    fakeSum += sum;
}

void benchForeachEdges(const CallGraph& graph) {
    double sum = 0;

    Timer timer;

    CallSiteIterator first, last;
    for (boost::tie(first, last) = graph.callSites(); first != last; ++first) {
        sum += graph[*first].temperature;
    }

    foreachEdges += timer.elapsed();

    fakeSum += sum;
}

class CountBFSVisitor : public default_bfs_visitor {
    public:
        unsigned int* count;
        explicit CountBFSVisitor(unsigned int* c) : default_bfs_visitor(), count(c) {}
        CountBFSVisitor(const CountBFSVisitor& vis) : default_bfs_visitor(), count(vis.count) {}
        void discover_vertex(Function, const Graph&) {
            (*count)++;
        }
};

void benchBFS(CallGraph* graph) {
    Timer timer;

    unsigned int count = 0;

    CountBFSVisitor vis(&count);

    FunctionIterator first, last;
    boost::tie(first, last) = graph->functions();

    breadth_first_search(*graph->getGraph(), *first, visitor(vis));

    bfs += timer.elapsed();
}

class CountDFSVisitor : public default_dfs_visitor {
    public:
        unsigned int* count;
        explicit CountDFSVisitor(unsigned int* c) : default_dfs_visitor(), count(c) {}
        CountDFSVisitor(const CountDFSVisitor& vis) : default_dfs_visitor(), count(vis.count) {}
        void discover_vertex(Function, const Graph&) {
            (*count)++;
        }
};

void benchDFS(CallGraph* graph) {
    Timer timer;

    unsigned int count = 0;

    CountDFSVisitor vis(&count);

    FunctionIterator first, last;
    boost::tie(first, last) = graph->functions();

    depth_first_search(*graph->getGraph(), boost::visitor(vis));

    dfs += timer.elapsed();
}

void benchAnalyzingVertices(CallGraph& graph) {
    Infos infos;
    Analyzer analyzer(graph, infos);

    Timer timer;

    FunctionIterator fit, fend;
    for (boost::tie(fit, fend) = graph.functions(); fit != fend; ++fit) {
        analyzer.computeFunctionTemperature(*fit);
    }

    analyzeVertices += timer.elapsed();
}

void benchAnalyzingEdges(CallGraph& graph) {
    Infos infos;
    Analyzer analyzer(graph, infos);

    Timer timer;

    CallSiteIterator cit, cend;
    for (boost::tie(cit, cend) = graph.callSites(); cit != cend; ++cit) {
        analyzer.computeCallSiteTemperature(*cit);
    }

    analyzeEdges += timer.elapsed();
}

void benchClustering(CallGraph& graph) {
    Infos infos;
    Analyzer analyzer(graph, infos);

    Timer timer;

    vector<Cluster> clusters = analyzer.findClusters();

    clustering += timer.elapsed();
}

void benchLibraryIssues(CallGraph& graph) {
    Infos infos;
    Analyzer analyzer(graph, infos);

    Timer timer;

    vector<LibraryIssue> issues = analyzer.findLibraryIssues();

    library += timer.elapsed();
}

void benchCircularDependencies(CallGraph& graph) {
    Infos infos;
    Analyzer analyzer(graph, infos);

    Timer timer;

    vector<vector<string>> circularDependencies = analyzer.findCircularDependencies();

    circular += timer.elapsed();
}

void benchHierarchy(CallGraph& graph) {
    Infos infos;
    Analyzer analyzer(graph, infos);

    Timer timer;

    analyzer.findVirtualHierarchyIssues();

    cout << "\tHierarchy done in " << timer.elapsed() << "ms" << endl; // (" << circularDependencies.size() << ")" << endl;
}

void displayResults() {
    cout << "Results" << endl;
    cout << "\tReading done in " << (reading / GRAPHS) << "ms" << endl;
    cout << "\tExtraction done in " << (extraction / GRAPHS) << "ms" << endl;
    cout << "\tForeach vertices done in " << (foreachVertices / GRAPHS) << "ms" << endl;
    cout << "\tForeach edges done in " << (foreachEdges / GRAPHS) << "ms" << endl;
    cout << "\tBFS done in " << (bfs / GRAPHS) << "ms" << endl;
    cout << "\tDFS done in " << (dfs / GRAPHS) << "ms" << endl;
    cout << "\tAnalyzing vertices done in " << (analyzeVertices / GRAPHS) << "ms" << endl;
    cout << "\tAnalyzing edges done in " << (analyzeEdges / GRAPHS) << "ms" << endl;
    cout << "\tClustering done in " << (clustering / GRAPHS) << "ms" << endl;
    cout << "\tLibrary issues done in " << (library / GRAPHS) << "ms" << endl;
    cout << "\tCircular dependencies done in " << (circular / GRAPHS) << "ms" << endl;
}

void initResults() {
    reading = 0;
    extraction = 0;
    foreachVertices = 0;
    foreachEdges = 0;
    dfs = 0;
    bfs = 0;
    analyzeVertices = 0;
    analyzeEdges = 0;
    clustering = 0;
    library = 0;
    circular = 0;
}

void benchGraph(int size) {
    std::stringstream out;
    out << size;

    string file = "../graphs/sparse" + out.str();

    Parameters::init();

    for (int i = 1; i <= GRAPHS; ++i) {
        cout << "Bench " << i << " for graph of size " << size << ":" << endl;

        std::stringstream out2;
        out2 << i;

        string graphFile = file + "_" + out2.str();

        Graph* graph = new Graph();
        CallGraph* callGraph = new CallGraph(graph);

        benchReading(graph, graphFile);
        benchExtraction(callGraph);
        benchForeachVertices(*callGraph);
        benchForeachEdges(*callGraph);
        benchDFS(callGraph);
        benchBFS(callGraph);
        benchAnalyzingEdges(*callGraph);
        benchAnalyzingVertices(*callGraph);
        benchClustering(*callGraph);
        benchLibraryIssues(*callGraph);
        benchCircularDependencies(*callGraph);
        //benchHierarchy(*callGraph); Not interesting because Infos do not parse file

        delete callGraph;
    }

    displayResults();
    initResults();
}

void process_mem_usage(double& vm_usage, double& resident_set) {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    vm_usage     = 0.0;
    resident_set = 0.0;

    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    unsigned long vsize;
    long rss;

    // Get all the informations until we have to two good ones
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                >> utime >> stime >> cutime >> cstime >> priority >> nice
                >> O >> itrealvalue >> starttime >> vsize >> rss;

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage     = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

void benchMemory(int size) {
    std::stringstream out;
    out << size;

    string file = "../graphs/sparse" + out.str() + "_1";

    Graph* graph = new Graph();

    dynamic_properties dp;
    dp.property("node_id", get(&vertex_info::name, *graph));
    dp.property("label", get(&vertex_info::label, *graph));
    dp.property("filename", get(&vertex_info::file_name, *graph));
    dp.property("module", get(&vertex_info::module, *graph));
    dp.property("label", get(&edge_info::label, *graph));

    ref_property_map<Graph*, string> gname(get_property(*graph, graph_name));
    dp.property("name", gname);

    ifstream inFile;
    inFile.open(file.c_str());

    read_graphviz(inFile, *graph, dp, "node_id");

    inFile.close();

    sleep(2);

    double vm, rss;
    process_mem_usage(vm, rss);

    cout << "Graph of size " << size << " takes vm:" << (vm / 1024.0) << "MB rss:" << (rss / 1024.0) << "MB" << endl;

    delete graph;
}

void benchInfos(string file) {
    cout << "Parse " << file << endl;

    double mean = 0;
    for (int i = 0; i < 27; ++i) {
        InfosOld infos;
        Timer timer;
        infos.parseFile(file);
        double time = timer.elapsed();
        if (i >= 2) {
            mean += time;
        }
    }
    mean /= 20;

    cout << "Old parsing done in " << mean << "ms" << endl;

    mean = 0;
    for (int i = 0; i < 27; ++i) {
        Infos infos;
        Timer timer;
        infos.parseFile(file);
        double time = timer.elapsed();
        if (i >= 2) {
            mean += time;
        }
    }
    mean /= 20;

    cout << "ELF parsing done in " << mean << "ms" << endl;
}

void benchGraphs() {
    benchGraph(100);
    benchGraph(1000);
    benchGraph(10000);
    benchGraph(40000);
    benchGraph(100000);
    benchGraph(1000000);
}

void benchMemory() {
    benchMemory(100);
    benchMemory(1000);
    benchMemory(10000);
    benchMemory(40000);
    benchMemory(100000);
    benchMemory(1000000);
}

void benchFiles() {
    //Parse libs
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libCaloCalibHitRec.so");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libRooFitCore.so");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libdyninstAPI.so.7.0");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libinstructionAPI.so.7.0");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libGaudiSvc.so");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libclang.so");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libEnhancedDisassembly.so");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libLTO.so");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libwireshark.so.0.1.0");
    benchInfos("/home/wichtounet/dev/lbl/bench-files/libQtWebKit.so.4.7.2");

    //Parse executables
    benchInfos("/home/wichtounet/dev/lbl/bench-files/skype");
    benchInfos("/home/wichtounet/dev/llvm/base/build/Release+Asserts/bin/clang");
    benchInfos("/home/wichtounet/dev/lbl/inlining/bin/analyzer");
    benchInfos("/home/wichtounet/dev/lbl/virtual/bin/virtual");
    benchInfos("/home/wichtounet/dev/eddi/eddic/bin/eddic");
}

int main(int, const char*[]) {
    benchGraphs();
    benchFiles();
    benchMemory();

    return 0;
}
