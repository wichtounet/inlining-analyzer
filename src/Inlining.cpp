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

#include "Inlining.hpp"

#include "Analyzer.hpp"
#include "Timer.hpp"
#include "CallGraph.hpp"
#include "GraphReader.hpp"
#include "Sorters.hpp"
#include "Reports.hpp"
#include "Infos.hpp"
#include "Parameters.hpp"

using std::string;
using std::cout;
using std::endl;
using std::vector;

using namespace inlining;

void Inlining::analyze(const string& file) {
    Parameters::init();

    GraphReader reader(infos);
    CallGraph* graph = reader.read(file);

    Analyzer analyzer(*graph, infos);

    if (m_filter) {
        analyzer.setFilterDuplicates(true);
    }

    analyzer.analyze();

    printHeader(file, *graph);

    //Display the necessary output
    if (m_statistic || m_issues) {
        analyzer.setFilterDuplicates(true);
        analyzer.addProtectedLibrary("/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.14");
        analyzer.addProtectedLibrary("/lib/x86_64-linux-gnu/libc-2.13.so");
        analyzer.addProtectedLibrary("/lib/x86_64-linux-gnu/ld-2.13.so");

        Reports reports(*graph, analyzer);

        if (m_default) {
            reports.enableDefaultFilters();
        }

        for (vector<string>::const_iterator it = filters.begin(); it != filters.end(); ++it) {
            reports.addFilter(*it);
        }

        //If necessary print out all the statistic reports
        if (m_statistic) {
            reports.printStatisticReports();
        }

        //If necessary print out all the issues
        if (m_issues) {
            reports.printIssues();
        }
    }

    delete graph;
}

void Inlining::printHeader(const string& file, const CallGraph& graph) const {
    cout << "# ATLAS Inlining Analyzer" << endl;
    cout << "# file: " << file << endl;
    cout << "# functions: " << graph.numberOfFunctions() << endl;
    cout << "# call sites: " << graph.numberOfCallSites() << endl;
    cout << "# libraries: " << infos.numberOfLibraries() << endl;
}
