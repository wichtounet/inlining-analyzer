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

#ifndef INFOS_OLD_H
#define INFOS_OLD_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <utility>

namespace inlining {

/*!
 * \class InfosOld
 * \brief Utility to get informations about function. This version is the old version of Infos, it does not use libelf but fork to readelf.
 */
class InfosOld {
    public:
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
         * \brief Parse the given file to extract information from.
         * \param file The file name.
        */
        void parseFile(const std::string& file);
    private:
        std::map<std::string, bool> files;
        std::map<std::string, unsigned int> functions;
        std::set<std::string> virtualFunctions;
};

} //end of inlining

#endif
