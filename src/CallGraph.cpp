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
#include <cstdlib>
#include <sys/time.h>

#include "CallGraph.hpp"

using namespace boost;
using namespace std;
using namespace inlining;

CallGraph::~CallGraph() {
    delete graph;
}

//Iterators

pair<FunctionIterator, FunctionIterator> CallGraph::functions() {
    return vertices(*graph);
}

pair<FunctionIterator, FunctionIterator> CallGraph::functions() const {
    return vertices(*graph);
}

pair<CallSiteIterator, CallSiteIterator> CallGraph::callSites() {
    return edges(*graph);
}

pair<CallSiteIterator, CallSiteIterator> CallGraph::callSites() const {
    return edges(*graph);
}

//Utilities

string CallGraph::getDescription(CallSite callSite) const {
    return (*this)[source(callSite)].name + " --> " + (*this)[target(callSite)].name;
}

//General setters

void CallGraph::setNumberOfCalls(unsigned int c) {
    calls = c;
}

unsigned int CallGraph::getNumberOfCalls() const {
    return calls;
}

void CallGraph::setApplicationSize(unsigned int size) {
    applicationSize = size;
}

unsigned int CallGraph::getApplicationSize() const {
    return applicationSize;
}

unsigned int CallGraph::numberOfFunctions() const {
    return num_vertices(*graph);
}

unsigned int CallGraph::numberOfCallSites() const {
    return num_edges(*graph);
}
