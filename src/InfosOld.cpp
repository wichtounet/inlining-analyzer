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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cctype>
#include <stdio.h>

#include "InfosOld.hpp"
#include "Logging.hpp"
#include "Timer.hpp"
#include "Utils.hpp"

static const int BUFFER_SIZE = 1024;

using std::cout;
using std::endl;
using std::hex;
using std::string;
using std::stringstream;
using std::ifstream;
using std::vector;
using std::map;
using std::set;
using std::pair;

using namespace inlining;

unsigned int InfosOld::sizeOf(const string& file, const string& function) {
    if (!files[file]) {
        parseFile(file);
        files[file] = true;
    }

    return functions[file + "##" + function];
}

bool InfosOld::isVirtual(const string& file, const string& function) {
    if (!files[file]) {
        parseFile(file);
        files[file] = true;
    }

    return find(virtualFunctions.begin(), virtualFunctions.end(), file + "##" + function) != virtualFunctions.end();
}

/*
 * \brief Parse the given file to extract informations
 * \param file The executable file to parse
 * \param virtualFunctions Map indicating the virtuality of the functions
 * \param functionSizes Map indicating the sizes of the functions
 */
static void parseExecutable(const string& file, std::set<string>& virtualFunctions, std::map<string, unsigned int>& functionSizes);

/*
 * \brief Parse the given file to extract informations
 * \param file The library file to parse
 * \param virtualFunctions Map indicating the virtuality of the functions
 * \param functionSizes Map indicating the sizes of the functions
 */
static void parseLibrary(const string& file, std::set<string>& virtualFunctions, std::map<string, unsigned int>& functionSizes);

/*
 * \brief Get the function name from the function address.
 * \param function The function address
 * \param functions The map of the function address to the names
 */
static string searchFunction(string function, const std::vector<std::pair<string, string>>& functions);

void InfosOld::parseFile(const string& file) {
    if (file.find(".c") != string::npos || file.find(".cpp") != string::npos) {
        ERROR cout << "ERROR : Trying to parse source file" << endl;
    } else if (file.find(".h") != string::npos  || file.find(".hpp") != string::npos) {
        ERROR cout << "ERROR : Trying to parse header file" << endl;
    } else {
        if (!fexists(file)) {
            ERROR cout << "ERROR : Trying to parse not existing file \"" << file << "\"" << endl;
        } else if (file.find(".so") == string::npos) {
            parseExecutable(file, virtualFunctions, functions);
        } else {
            parseLibrary(file, virtualFunctions, functions);
        }
    }
}

struct VTable {
    string name;
    unsigned int size;
    string address;
    unsigned int addressInt;

    VTable(string n, unsigned int s, string a, unsigned int aInt) : name(n), size(s), address(a), addressInt(aInt) {}
};

static bool tableCompare(const VTable& a, const VTable& b) {
    return a.addressInt < b.addressInt;
}

static void parseVirtualTable(const string& line, vector<VTable>& tables) {
    string::size_type addressStart = line.find(":") + 2;

    string address = "";
    while (!isspace(line[addressStart])) {
        address += line[addressStart];
        addressStart++;
    }

    string sizeStr = "";
    while (!isalpha(line[addressStart])) {
        sizeStr += line[addressStart];
        addressStart++;
    }

    unsigned int size = 0;
    std::istringstream iss(sizeStr);
    iss >> size;

    unsigned int addressInt = 0;
    std::istringstream iss2(address);
    iss2 >> hex >> addressInt;

    string::size_type nameStart = line.find("_ZTV");
    string className = line.substr(nameStart, line.size() - nameStart);

    VTable table(className, size, address, addressInt);

    tables.push_back(table);
}

static std::pair<string, string> parseFunction(const string& file, const string& line, std::map<string, unsigned int>& functionSizes) {
    string::size_type addressStart = line.find(":") + 2;
    string address;

    while (!isspace(line[addressStart])) {
        address += line[addressStart];
        addressStart++;
    }

    string sizeStr;
    while (!isalpha(line[addressStart])) {
        sizeStr += line[addressStart];
        addressStart++;
    }

    unsigned int size;
    std::istringstream iss(sizeStr);
    iss >> size;

    string::size_type nameStart = line.rfind(" ") + 1; //First char after the last space
    string mangledName = line.substr(nameStart, line.size() - nameStart - 1); // "-1" to avoid carriage return

    string functionIndex = file + "##" + mangledName;

    //If size == 0, we do not have enough information
    if (size > 0) {
        if (functionSizes[functionIndex] != size) {
            WARN cout << "WARNING: Function " << functionIndex << " has already been set with a size of " << functionSizes[functionIndex] << ". New size = " << size << endl;
        }

        functionSizes[functionIndex] = size;

        //If the function is made of @@, we store both versions
        if (mangledName.find("@@") != string::npos) {
            string functionShort = mangledName.substr(0, mangledName.find("@@"));
            functionSizes[file + "##" + functionShort] = size;
        }
    }

    return make_pair(address, mangledName);
}

static void parseLibrary(const string& file, set<string>& virtualFunctions, std::map<string, unsigned int>& functionSizes) {
    DEBUG cout << "DEBUG : Parse library " << file << endl;

    string command = "readelf -a --wide ";
    command += file;

    std::vector<string> relocations;
    FILE* stream;
    char buffer[BUFFER_SIZE];
    std::vector<VTable> tables;

    stream = popen(command.c_str(), "r");

    bool symbolTable = false;
    bool relocationTable = false;

    string symbolTableEnd = "Histogram for bucket list length";
    string symbolTablePrefix = "Symbol table '.dynsym'";

    string relocationTableEnd = "Relocation section '.rel.plt'";
    string relocationTablePrefix = "Relocation section '.rel.dyn'";

    while (fgets(buffer, BUFFER_SIZE, stream) != NULL) {
        string line = buffer;

        if (line.find("Class:") != string::npos) {
            if (line.find("ELF64") != string::npos) {
                relocationTableEnd = "Relocation section '.rela.plt'";
                relocationTablePrefix = "Relocation section '.rela.dyn'";
            }

            break;
        }
    }

    while (fgets(buffer, BUFFER_SIZE, stream) != NULL) {
        string line = buffer;

        if (!line.compare(0, relocationTableEnd.size(), relocationTableEnd)) {
            relocationTable = false;
        }

        if (!line.compare(0, symbolTableEnd.size(), symbolTableEnd)) {
            symbolTable = false;
        }

        if (symbolTable) {
            if (line.find("_ZTV") != string::npos && line.find("WEAK") != string::npos) {
                parseVirtualTable(line, tables);
            } else if (line.find(" FUNC ") != string::npos) {
                parseFunction(file, line, functionSizes);
            }
        }

        //Store data for read the relocation table later
        if (relocationTable) {
            relocations.push_back(buffer);
        }

        if (!line.compare(0, symbolTablePrefix.size(), symbolTablePrefix)) {
            symbolTable = true;
        }

        if (!line.compare(0, relocationTablePrefix.size(), relocationTablePrefix)) {
            relocationTable = true;
        }
    }

    pclose(stream);

    DEBUG cout << "DEBUG: Number of virtual tables " << tables.size() << endl;

    //No virtual tables
    if (tables.size() == 0) {
        return;
    }

    //Sort both vector for efficient algorithm
    sort(relocations.begin(), relocations.end());
    sort(tables.begin(), tables.end(), tableCompare);

    vector<VTable>::const_iterator vit = tables.begin();

    vector<string>::const_iterator sit;
    for (sit = relocations.begin(); sit != relocations.end(); ++sit) {
        string line = *sit;

        string address = line.substr(0, line.find(" "));

        size_t lineAddress;
        std::istringstream iss(address);
        iss >> hex >> lineAddress;

        //This line is lower than the current virtual table -> go to the next one
        if (lineAddress >= vit->addressInt + vit->size) {
            ++vit;

            //We searched every vtable
            if (vit == tables.end()) {
                break;
            }
        }

        //This line is part of the current virtual table
        if (lineAddress >= vit->addressInt && lineAddress < vit->addressInt + vit->size) {
            if (line.find("_ZN") != string::npos) {
                string::size_type mangleStart = line.find("_ZN");

                string virtualFunction = line.substr(mangleStart, line.size() - mangleStart);

                string::size_type spaceStart = virtualFunction.find(" ");
                if (spaceStart != string::npos) {
                    virtualFunction = virtualFunction.substr(0, spaceStart);
                }

                string::size_type lineEndStart = virtualFunction.find("\n");
                if (lineEndStart != string::npos) {
                    virtualFunction = virtualFunction.substr(0, lineEndStart);
                }

                virtualFunctions.insert(file + "##" + virtualFunction);
            }
        }
    }
}

static void parseExecutable(const string& file, set<string>& virtualFunctions, std::map<string, unsigned int>& functionSizes) {
    DEBUG cout << "DEBUG : Parse executable " << file << endl;

    string command = "readelf -a --wide -x.rodata ";
    command += file;

    FILE* stream;
    char buffer[BUFFER_SIZE];
    std::vector<VTable> tables;
    std::vector<std::pair<string, string>> functions;
    vector<string> rodata;

    stream = popen(command.c_str(), "r");

    bool symbolTable = false;
    bool rodataTable = false;

    string rodataEnd = "Notes at offset";
    string symbolTableEnd = "Histogram for bucket list length";
    string symbolTablePrefix = "Symbol table '.symtab'";
    string rodataPrefix = "Hex dump of section '.rodata'";

    while (fgets(buffer, BUFFER_SIZE, stream) != NULL) {
        string line = buffer;

        if (rodataTable && line == "\n") {
            rodataTable = false;
        }

        if (!line.compare(0, symbolTableEnd.size(), symbolTableEnd)) {
            symbolTable = false;
        }

        if (symbolTable) {
            if (line.find("_ZTV") != string::npos && line.find("_ZTVN") == string::npos) {
                parseVirtualTable(line, tables);
            } else if (line.find(" FUNC ") != string::npos) {
                functions.push_back(parseFunction(file, line, functionSizes));
            }
        }

        if (rodataTable) {
            rodata.push_back(buffer);
        }

        if (!line.compare(0, symbolTablePrefix.size(), symbolTablePrefix)) {
            symbolTable = true;
        }

        if (!line.compare(0, rodataPrefix.size(), rodataPrefix)) {
            rodataTable = true;
        }
    }

    pclose(stream);

    DEBUG cout << "DEBUG: Number of virtual tables " << tables.size() << endl;

    //No virtual tables
    if (tables.size() == 0) {
        return;
    }

    //Sort both vector for efficient algorithm
    sort(rodata.begin(), rodata.end());
    sort(tables.begin(), tables.end(), tableCompare);

    vector<VTable>::const_iterator vit = tables.begin();

    string::size_type functionSize = functions.begin()->first.size();
    unsigned int spaces = 0;

    //Blocks of 8 chars are separated by a space
    if (functionSize == 16) {
        spaces = 1;
    }

    vector<string>::const_iterator sit;
    for (sit = rodata.begin(); sit != rodata.end(); ++sit) {
        string line = *sit;

        string address = line.substr(0, line.find(" ", 3)); //Line start by two spaces

        size_t lineAddress;
        std::istringstream iss(address);
        iss >> hex >> lineAddress;

        //This line is lower than the current virtual table -> go to the next one
        if (lineAddress > vit->addressInt + vit->size) {
            ++vit;

            //We searched every vtable
            if (vit == tables.end()) {
                break;
            }
        }

        //This line is part of the current virtual table
        if (lineAddress >= vit->addressInt && lineAddress <= vit->addressInt + vit->size) {
            string::size_type firstStart = 13;

            for (unsigned int a = 0; a < 32 / functionSize; a++) {
                string::size_type start = firstStart + a * functionSize + a + a * spaces;

                string function = line.substr(start, functionSize + spaces);

                virtualFunctions.insert(file + "##" + searchFunction(function, functions));
            }
        }
    }
}

static string reverseBytes(const string& address) {
    string reverse = address;

    for (unsigned int i = 0; i < address.size(); i += 2) {
        reverse[i] = address[address.size() - 1 - i - 1];
        reverse[i + 1] = address[address.size() - 1 - i];
    }

    return reverse;
}

string searchFunction(string function, const vector<pair<string, string>>& functions) {
    if (function.find(" ") != string::npos) {
        function.erase(function.find(" "), 1);
    }

    function = reverseBytes(function);

    for (vector<pair<string, string>>::const_iterator fit = functions.begin(); fit != functions.end(); ++fit) {
        if (fit->first == function) {
            return fit->second;
        }
    }

    return "";
}
