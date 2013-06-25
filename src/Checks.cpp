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

#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "CallGraph.hpp"

using std::vector;
using std::cout;
using std::endl;

using namespace inlining;

void checkFunctions(const CallGraph& graph) {
    int zero = 0;
    int zeroCalls = 0;

    cout << "Check functions for weird values" << endl;

    FunctionIterator first, last;
    for (boost::tie(first, last) = graph.functions(); first != last; ++first) {
        if (graph[*first].size == 0) {
            ++zero;
            cout << "Function " << graph[*first].name << " has a size of zero" << endl;
        }

        if (graph[*first].calls == 0) {
            ++zeroCalls;
            cout << "Function " << graph[*first].name << " has never been called" << endl;
        }
    }

    cout << "There are " << zero << " functions with a size of zero" << endl;
    cout << "There are " << zeroCalls << " functions that have never been called" << endl;
}
