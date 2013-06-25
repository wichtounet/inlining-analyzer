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

#ifndef INFOS_H
#define INFOS_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <utility>

#include <unordered_map>
#include <unordered_set>

#include "libelf.h"

namespace inlining {

/*!
 * \enum Type
 * \brief Define the type of shared object.
 */
enum Type {
    ELF32,  /*!< A shared object in 32 bits. */
    ELF64   /*!< A shared object in 64 bits. */
};

/*!
 * \brief The set used to store the parsed files.
 */
typedef std::unordered_set<std::string> FileSet;

/*!
 * \brief The set used to store the virtual functions.
 */
typedef std::unordered_set<std::string> VirtualFunctionSet;

/*!
 * \brief The map used to map the sizes of the function to the function names themselves.
 */
typedef std::unordered_map<std::string, unsigned int> SizeMap;

/*!
 * \class Infos
 * \brief Utility to get informations about function.
 */
class Infos {
    public:
        Infos() : m_demangled(false) {}

        /*!
         * \brief Set if the Callgrind profile contains demangled (true) or mangled (false) names.
         * \param demangled Boolean tag indicating if the Callgrind profile contains demangled (true) or mangled (false) names.
        */
        void setDemangled(bool demangled) {
            m_demangled = demangled;
        }

        /*!
         * \brief Indicate if the Callgrind profile contains demangled or  mangled names.
         * \return true if the CallGrind profile is demangled otherwise false.
        */
        bool isDemangled() {
            return m_demangled;
        }

        /*!
         * \brief Return the size of the given function in the given function
         * \param file the library name of the function
         * \param function the function name
         * \return The size of the function or 0 if the size cannot be found
        */
        unsigned int sizeOf(const std::string& file, const std::string& function);

        /*!
         * \brief Return a boolean indicating the virutality of the given function
         * \param file the library name of the function
         * \param function the function name
         * \return A boolean indicating if the function is virtual or not
        */
        bool isVirtual(const std::string& file, const std::string& function);

        /*!
         * \brief Return all the detected virtual functions.
         * \return A set containing all the virtual functions.
        */
        const VirtualFunctionSet& getVirtualFunctions() const {
            return virtualFunctions;
        }

        /*!
         * \brief Return all the function sizes.
         * \return A map matching every function to its given size.
        */
        const SizeMap& getSizes() const {
            return functionSizes;
        }

        /*!
         * \brief Return the number of libraries.
         * \return The number of libraries that have been parsed
        */
        size_t numberOfLibraries() const {
            return files.size();
        }

        /*!
         * \brief Parse the given file to extract information from.
         * \param file The file name.
        */
        void parseFile(const std::string& file);
    private:
        struct VirtualTable {
            Elf64_Xword size;
            Elf64_Addr address;

            VirtualTable(Elf64_Xword s, Elf64_Addr a) : size(s), address(a) {}
        };

        static bool tableCompare(const VirtualTable& a, const VirtualTable& b) {
            return a.address < b.address;
        }

        FileSet files;
        SizeMap functionSizes;
        VirtualFunctionSet virtualFunctions;

        bool m_demangled;

        void parseLibrary(const std::string& file, Elf* elf, Type type);
        void parseExecutable(const std::string& file, Elf* elf, Type type);

        void extractSymbols(const std::string& file, Elf* elf, Elf_Scn* section, std::vector<std::string>& symbols, std::vector<VirtualTable>& virtualTables, std::map<Elf64_Addr, Elf64_Word>& functions);
};

} //end of inlining

#endif
