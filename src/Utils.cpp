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

#include "Utils.hpp"

#include <cstdlib>

/*!
 * \brief Demangle the given mangled name
 * \param mangled The mangled name
 * \return The demangled version of the given string
*/
std::string inlining::demangle(const std::string& mangled) {
    int status;
    char* ret = abi::__cxa_demangle(mangled.c_str(), NULL, NULL, &status);
    if (status < 0 || !ret) {
        return mangled;
    }
    std::string result(ret);
    ::free(ret);
    return result;
}
