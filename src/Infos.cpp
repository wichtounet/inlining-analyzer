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
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h>
#include <stddef.h>

#include "Utils.hpp"
#include "Logging.hpp"
#include "Infos.hpp"
#include "Timer.hpp"

static const int BYTES_PER_LINE = 16;

using std::endl;
using std::string;
using std::cout;
using std::vector;
using std::hex;
using std::dec;
using std::map;
using std::set;

using namespace inlining;

unsigned int Infos::sizeOf(const string& file, const string& function) {
    if (files.find(file) == files.end()) {
        parseFile(file);
        files.insert(file);
    }

    return functionSizes[file + "##" + function];
}

bool Infos::isVirtual(const string& file, const string& function) {
    if (files.find(file) == files.end()) {
        parseFile(file);
        files.insert(file);
    }

    return virtualFunctions.find(file + "##" + function) != virtualFunctions.end();
}

/* Types */

struct Relocation {
    Elf64_Addr address;
    unsigned int symbol;

    Relocation(Elf64_Addr a, unsigned int s) : address(a), symbol(s) {}
};

/* Sorter functions */

static bool relocationCompare(const Relocation& a, const Relocation& b) {
    return a.address < b.address;
}

/* Elf Utilities */
static Elf_Scn* getSection(Elf* elf, string sectionName);
static unsigned int getBaseAddress(Elf* elf);
static inline unsigned long parse8Bytes(unsigned char* bytes, unsigned int i);
static inline unsigned long parse4Bytes(unsigned char* bytes, unsigned int i);

/* ELF Parsing implementation */

void Infos::parseFile(const string& file) {
    DEBUG cout << "DEBUG : Parse file " << file << endl;

    Timer timer;

    //Can happen in the call graph
    if (trim(file).size() == 0) {
        return;
    }

    if (!fexists(file.c_str())) {
        ERROR cout << "ERROR : " << file << " does not exist" << endl;
        return;
    }

    //Open the file
    int fd = open(file.c_str(), O_RDONLY, 0);
    if (fd < 0) {
        ERROR cout << "ERROR : open " << file << " failed" << endl;
        return;
    }

    //Verify the version
    if (elf_version(EV_CURRENT) == EV_NONE) {
        ERROR cout << "ERROR : ELF library initialization failed : " << elf_errmsg(-1) << endl;
        close(fd);
        return;
    }

    //Read elf file
    Elf* elf = elf_begin(fd , ELF_C_READ , NULL);
    if (elf == NULL) {
        ERROR cout << "ERROR : elf_begin() failed : " << elf_errmsg(-1) << endl;
        close(fd);
        return;
    }

    //Do the parsing only if it is an ELF file
    if (elf_kind(elf) == ELF_K_ELF) {
        GElf_Ehdr ehdr;
        if (gelf_getehdr(elf, &ehdr) == NULL) {
            ERROR cout << "ERROR : Unable to open the ELF header" << endl;
        } else {
            int i = gelf_getclass(elf);

            if (i == ELFCLASSNONE) {
                ERROR cout << "ERROR : No class in the header" << endl;
            } else {
                Type type = i == ELFCLASS32 ? ELF32 : ELF64;

                if (ehdr.e_type == ET_EXEC) {
                    parseExecutable(file, elf, type);
                } else if (ehdr.e_type == ET_DYN) {
                    parseLibrary(file, elf, type);
                } else {
                    ERROR cout << "ERROR : Unrecognized format" << endl;
                }
            }
        }
    } else {
        ERROR cout << "ERROR : " << file << "is not an ELF object" << endl;
    }

    elf_end(elf);
    close(fd);

    PERF cout << "PERF: File parsed in " << timer.elapsed() << "ms : " << file << endl;
    PERF cout << "PERF: \t set size = " << virtualFunctions.size() << " map size = " << functionSizes.size() << endl;
}

static void parseRelocationTable(Type type, Elf_Scn* relocationTable, vector<Relocation>& relocations);

void Infos::parseLibrary(const string& file, Elf* elf, Type type) {
    DEBUG cout << "DEBUG: Start to parse library, " << (type == ELF32 ? 32 : 64) << "bits" << endl;

    //Get the .dynsym section
    Elf_Scn* symbolTable = getSection(elf, ".dynsym");

    vector<string> symbols;
    vector<VirtualTable> virtualTables;
    map<Elf64_Addr, Elf64_Word> functions;

    //Extract functions and virtual tables
    extractSymbols(file, elf, symbolTable, symbols, virtualTables, functions);

    DEBUG cout << "DEBUG: Number of virtual tables " << virtualTables.size() << endl;

    //No virtual tables
    if (virtualTables.size() == 0) {
        return;
    }

    //Get the relocation table section, depending on the architecture
    Elf_Scn* relocationTable = type == ELF32 ? getSection(elf, ".rel.dyn") : getSection(elf, ".rela.dyn");

    vector<Relocation> relocations;

    //Get all the relocations entries
    parseRelocationTable(type, relocationTable, relocations);

    //Sort the vectors in order to use fast algorithm
    sort(virtualTables.begin(), virtualTables.end(), tableCompare);
    sort(relocations.begin(), relocations.end(), relocationCompare);

    vector<VirtualTable>::const_iterator vit = virtualTables.begin();

    for (vector<Relocation>::const_iterator sit = relocations.begin(); sit != relocations.end(); ++sit) {
        //This line is lower than the current virtual table -> go to the next one
        if (sit->address >= vit->address + vit->size) {
            ++vit;

            //We searched every virtual table
            if (vit == virtualTables.end()) {
                break;
            }
        }

        //This line is part of the current virtual table
        if (sit->address >= vit->address && sit->address < vit->address + vit->size) {
            virtualFunctions.insert(file + "##" + symbols[sit->symbol]);
        }
    }
}

void Infos::parseExecutable(const string& file, Elf* elf, Type type) {
    DEBUG cout << "DEBUG: Start to parse executable, " << (type == ELF32 ? 32 : 64) << "bits" << endl;

    //Get the .symtab section
    Elf_Scn* section = getSection(elf, ".symtab");

    vector<string> symbols;
    vector<VirtualTable> virtualTables;
    map<Elf64_Addr, Elf64_Word> functions;

    //Extract functions and virtual tables
    extractSymbols(file, elf, section, symbols, virtualTables, functions);

    DEBUG cout << "DEBUG: Number of virtual tables " << virtualTables.size() << endl;

    //No virtual tables
    if (virtualTables.size() == 0) {
        return;
    }

    //Sort the virtual tables in order to use an efficient algorithm
    sort(virtualTables.begin(), virtualTables.end(), tableCompare);

    Elf_Scn* rodataSection = getSection(elf, ".rodata");

    //Get the content of the section
    Elf_Data* data = elf_rawdata(rodataSection, NULL);
    if (data == 0 || data->d_size == 0) {
        return; //Empy section
    }

    //Get the base address of the program
    unsigned int address = getBaseAddress(elf);

    GElf_Shdr rodataHeader;
    gelf_getshdr(rodataSection, &rodataHeader);

    //Calculate the base address of the rodata section
    address += rodataHeader.sh_offset;

    unsigned char* bytes = static_cast<unsigned char*>(data->d_buf);

    vector<VirtualTable>::const_iterator vit = virtualTables.begin();

    unsigned int functionSize = type == ELF32 ? 8 : 16;

    //We go through the rodata line by line (16 bytes by 16 bytes)
    for (unsigned int i = 0; i < data->d_size; address += BYTES_PER_LINE) {
        //This line is lower than the current virtual table -> go to the next one
        if (address > vit->address + vit->size) {
            ++vit;

            //We searched every vtable
            if (vit == virtualTables.end()) {
                break;
            }

            //We go to the previous line, because a virtual table can start in the middle of a line
            i -= BYTES_PER_LINE;
            address -= BYTES_PER_LINE;
        }

        //This line is part of the current virtual table
        if (address >= vit->address && address <= vit->address + vit->size) {
            for (unsigned int a = 0; a < 32 / functionSize && i < data->d_size; ++a, i += functionSize / 2) {
                //Parse the function address depending on the architecture
                unsigned long function = type == ELF32 ? parse4Bytes(bytes, i) : parse8Bytes(bytes, i);

                if (function > 0 && functions.find(function) != functions.end()) {
                    virtualFunctions.insert(file + "##" + symbols[functions[function]]);
                }
            }
        } else {
            i += BYTES_PER_LINE; //skip one line
        }
    }
}

void Infos::extractSymbols(const string& file, Elf* elf, Elf_Scn* section, vector<string>& symbols, vector<VirtualTable>& virtualTables, map<Elf64_Addr, Elf64_Word>& functions) {
    //Get the header of the section
    GElf_Shdr header;
    gelf_getshdr(section, &header);

    //Get the content of the section
    Elf_Data* data = elf_getdata(section, NULL);
    if (data == 0 || data->d_size == 0) {
        return; //There is no symbols
    }

    //Number of symbols
    int count = header.sh_size / header.sh_entsize;

    //Iterate through every symbol
    for (int i = 0; i < count; ++i) {
        GElf_Sym symbol;
        gelf_getsym(data, i, &symbol);

        //Name of the symbol
        string name = elf_strptr(elf, header.sh_link, symbol.st_name);
        string storeName = m_demangled ? demangle(name) : name;

        symbols.push_back(storeName);

        if (GELF_ST_TYPE(symbol.st_info) == STT_FUNC) {
            functions[symbol.st_value] = i;

            Elf64_Xword size = symbol.st_size;

            //If size == 0, we do not have enough information
            if (size > 0) {
                string functionIndex = file + "##" + storeName;

                if (functionSizes[functionIndex] != size) {
                    WARN cout << "WARNING: Function " << functionIndex << " has already been set with a size of " << functionSizes[functionIndex] << ". New size = " << size << endl;
                }

                functionSizes[functionIndex] = size;

                //If the function is made of @@, we store both versions
                if (storeName.find("@@") != string::npos) {
                    string functionShort = storeName.substr(0, storeName.find("@@"));
                    functionSizes[file + "##" + functionShort] = size;
                }
            }
        } else if (GELF_ST_BIND(symbol.st_info) == STB_WEAK && name.find("_ZTV") != string::npos) {
            virtualTables.push_back(VirtualTable(symbol.st_size, symbol.st_value));
        }
    }
}

static void parseRelocationTable(Type type, Elf_Scn* relocationTable, vector<Relocation>& relocations) {
    //Get the header of the section
    GElf_Shdr header;
    gelf_getshdr(relocationTable, &header);

    //Get the content of the section
    Elf_Data* data = elf_getdata(relocationTable, NULL);
    if (data == 0 || data->d_size == 0) {
        WARN cout << "WARNING : no symbols in the symbol table" << endl;
        return;
    }

    //Number of symbols
    int count = header.sh_size / header.sh_entsize;

    if (type == ELF32) {
        //Iterate through every relocation entry
        for (int i = 0; i < count; ++i) {
            GElf_Rel rel;
            gelf_getrel(data, i, &rel);

            unsigned int symbol = GELF_R_SYM(rel.r_info);

            //0 is not a symbol
            if (symbol > 0) {
                relocations.push_back(Relocation(rel.r_offset, symbol));
            }
        }
    } else {
        //Iterate through every relocation entry
        for (int i = 0; i < count; ++i) {
            GElf_Rela rela;
            gelf_getrela(data, i, &rela);

            unsigned int symbol = GELF_R_SYM(rela.r_info);

            //0 is not a symbol
            if (symbol > 0) {
                relocations.push_back(Relocation(rela.r_offset, symbol));
            }
        }
    }
}

/* ELF Utilities */

static Elf_Scn* getSection(Elf* elf, string sectionName) {
    Elf_Scn* section = NULL;

    size_t shstrndx;

#ifdef BROKEN_ELF
    if (elf_getshstrndx(elf , &shstrndx) != 1) {
        return NULL;
    }
#else
    if (elf_getshdrstrndx(elf , &shstrndx) != 0) {
        return NULL;
    }
#endif

    //Search every section
    while ((section = elf_nextscn(elf, section)) != NULL) {
        GElf_Shdr shdr;
        if ((gelf_getshdr(section, &shdr)) == &shdr) { //Only if there is a header for this section
            char* name = elf_strptr(elf, shstrndx, shdr.sh_name);
            if (name == sectionName) {
                break;
            }
        }
    }

    return section;
}

static unsigned int getBaseAddress(Elf* elf) {
    GElf_Ehdr ehdr;
    gelf_getehdr(elf, &ehdr);

    //Base entry of the program
    Elf64_Addr entry = ehdr.e_entry;

    //Get the section containing the base entry
    Elf_Scn* text = getSection(elf, ".text");

    GElf_Shdr textHeader;
    gelf_getshdr(text, &textHeader);

    //Remove the offset of the .text section
    return entry - textHeader.sh_offset;
}

static inline unsigned long parse8Bytes(unsigned char* bytes, unsigned int i) {
    unsigned long value = 0;

    //The eight bytes are parsed in the reverse order
    value += Pow<256, 0>::Value * static_cast<unsigned long>(bytes[i + 0]);
    value += Pow<256, 1>::Value * static_cast<unsigned long>(bytes[i + 1]);
    value += Pow<256, 2>::Value * static_cast<unsigned long>(bytes[i + 2]);
    value += Pow<256, 3>::Value * static_cast<unsigned long>(bytes[i + 3]);
    value += Pow<256, 4>::Value * static_cast<unsigned long>(bytes[i + 4]);
    value += Pow<256, 5>::Value * static_cast<unsigned long>(bytes[i + 5]);
    value += Pow<256, 6>::Value * static_cast<unsigned long>(bytes[i + 6]);
    value += Pow<256, 7>::Value * static_cast<unsigned long>(bytes[i + 7]);

    return value;
}

static inline unsigned long parse4Bytes(unsigned char* bytes, unsigned int i) {
    unsigned long value = 0;

    //The four bytes are parsed in the reverse order
    value += Pow<256, 0>::Value * static_cast<unsigned long>(bytes[i + 0]);
    value += Pow<256, 1>::Value * static_cast<unsigned long>(bytes[i + 1]);
    value += Pow<256, 2>::Value * static_cast<unsigned long>(bytes[i + 2]);
    value += Pow<256, 3>::Value * static_cast<unsigned long>(bytes[i + 3]);

    return value;
}
