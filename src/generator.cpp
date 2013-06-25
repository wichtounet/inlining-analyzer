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
#include <cstdlib>
#include <cmath>

#include "Utils.hpp"

using namespace std;
using namespace inlining;

double rand_double() {
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

unsigned int digits(int number) {
    int digits = 0;

    while (number > 0) {
        ++digits;
        number/=10;
    }

    return digits;
}

void generate(string file, int vertices, int edges) {
    if (vertices == 0) {
        return;
    }

    srand(time(NULL) );

    ofstream outFile;
    outFile.open(file.c_str());

    outFile << "digraph {" << endl;

    int lib = 1 + sqrt(vertices) / 4;

    string* libraries = new string[lib];

    libraries[0] = "executable";

    for (int i = 1; i < lib; ++i) {
        libraries[i] = "lib" + toString<int>(i) + ".so";
    }

    string* functions = new string[vertices];

    functions[0] = "main";
    outFile << "\t\"main\" [label=\"main\\n99.81%\\n(0.15%)\\n0×\", module = \"executable\"];" << endl;

    for (int vertice = 1; vertice < vertices; vertice++) {
        stringstream name;
        name << "_ZN11GraphReader" << (13 + digits(vertice)) << "parseVertices" << vertice << "E";

        int parameters = rand() % 10;

        if (parameters == 0) {
            name << "v";
        } else {
            for (int i = 0; i < parameters; ++i) {
                name << "j";
            }
        }

        functions[vertice] = name.str();

        double inclusive_cost = rand_double() * (2000.0 / static_cast<double>(vertices));
        double self_cost = rand_double() * (200.0 / static_cast<double>(vertices));
        unsigned int calls = rand() % edges;

        string module = libraries[rand() % lib];

        outFile << "\t\"" << functions[vertice] << "\" [label=\"" << name << "\\n" << inclusive_cost << "%\\n(" << self_cost << "%)\\n" << calls << "×\", module = \"" << module << "\"];" << endl;
    }

    for (int edge = 0; edge < edges; edge++) {
        string name1 = functions[rand() % vertices];
        string name2 = functions[rand() % (vertices - 1) + 1]; //Cannot call main

        double cost = rand_double() * (200.0 / static_cast<double>(vertices));
        unsigned int calls = rand() % edges;

        outFile << "\t\"" << name1 << "\" -> \"" << name2 << "\" [label=\"" << cost << "%\\n" << calls << "×\"];" << endl;
    }

    outFile << "}" << endl;

    outFile.close();

    delete[] functions;
}

void generateFive(string file, int vertices, int edges) {
    generate(file + "_1", vertices, edges);
    generate(file + "_2", vertices, edges);
    generate(file + "_3", vertices, edges);
    generate(file + "_4", vertices, edges);
    generate(file + "_5", vertices, edges);
}

int main(int argc, const char* argv[]) {
    int vertices = 100000;
    int edges = 600000;

    if (argc == 1) {
        //Generate graphs for the benchmark
        generateFive("/home/wichtounet/dev/lbl/graphs/sparse100", 100, 1000);
        generateFive("/home/wichtounet/dev/lbl/graphs/sparse1000", 1000, 10000);
        generateFive("/home/wichtounet/dev/lbl/graphs/sparse10000", 10000, 100000);
        generateFive("/home/wichtounet/dev/lbl/graphs/sparse40000", 40000, 400000);
        generateFive("/home/wichtounet/dev/lbl/graphs/sparse100000", 100000, 1000000);
        generateFive("/home/wichtounet/dev/lbl/graphs/sparse1000000", 1000000, 10000000);

        return 0;
    }

    if (argc > 2) {
        vertices = toNumber<int>(argv[2]);
    }

    if (argc > 3) {
        edges = toNumber<int>(argv[3]);
    }

    generate(argv[1], vertices, edges);

    return 0;
}
