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

#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cxxabi.h>

namespace inlining {

/*!
 * \brief Return the converted number from
 * \param text the string to convert to a number
 * \return The number in the string
*/
template <class T>
T toNumber(const std::string& text) {
    std::stringstream ss(text);
    T result;
    ss >> result;
    return result;
}

/*!
 * \brief Return a string representation of the given value
 * \param value The value to convert to string
 * \return A string representation of the value
*/
template <class T>
std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

/*!
 * \struct Pow
 * \brief Meta template to calculate the Nth power of B at compile time
 * \tparam B The base
 * \tparam N The power we want
*/
template<unsigned int B, unsigned int N >
struct Pow {
    static const unsigned long Value = B * Pow<B, N - 1>::Value;    /*!< The value of the specific pow. */
};

/*!
 * \struct Pow
 * \brief Partial specialization of Pow<B, N> for the O power of B
 * \tparam B The base
 * \tparam 0 Specialization of the power B
*/
template<unsigned int B>
struct Pow<B, 0> {
    static const unsigned long Value = 1;   /*!< The value of the specific pow. */
};

/*!
 * \brief Demangle the given mangled name
 * \param mangled The mangled name
 * \return The demangled version of the given string
*/
std::string demangle(const std::string& mangled);

/*!
 * \brief Split the given value in parts separated by the given separator.
 * \param value The string to split.
 * \param separator The separator to use.
 * \return A vector containing all the parts of the string.
*/
static inline std::vector<std::string> split(const std::string& value, const std::string& separator) {
    size_t end = value.find(separator);
    std::vector<std::string> parts;

    if (end == std::string::npos) {
        return parts;
    }

    parts.push_back(value.substr(0, end));
    parts.push_back(value.substr(end + separator.size()));

    return parts;
}

/*!
 * \brief Trim the given value by removing the trailing spaces at the beginning and at the end of the string.
 * \param value The string to trim.
 * \return The trimmed version of the string.
*/
static inline std::string trim(const std::string& value) {
    std::string trimmed;

    std::stringstream trimmer;
    trimmer << value;
    trimmer >> trimmed;

    return trimmed;
}

/*!
 * \brief Test if the given file exists or not.
 * \param filename The name of the file to test.
 * \return true if the file exists otherwise false.
*/
static inline bool fexists(const std::string& filename) {
    std::ifstream ifile(filename.c_str());
    return ifile;
}

} //end of inlining

#endif
