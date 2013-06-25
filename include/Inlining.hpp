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

#ifndef INLINING_H
#define INLINING_H

#include <string>
#include <vector>

#include "Infos.hpp"
#include "CallGraph.hpp"

/*!
 * \namespace inlining
 * \brief Contains all the classes, structures and functions of the inlinning analyzer.
 */
namespace inlining {

/*!
 * \class Inlining
 * \brief Simple entry point for the library use if only simple output is needed
 */
class Inlining {
    public:
        Inlining() : m_issues(true), m_statistic(true), m_filter(false), m_default(false) {}

        /*!
         * \brief Analyze the given call graph and outputs every information about it in the console
         * \param file The path to the call graph DOT file
        */
        void analyze(const std::string& file);

        /*!
         * \brief Indicate if the Callgrind profile contains demangled (true) or mangled (false) names.
         * \param demangled Boolean tag indicating if the Callgrind profile contains demangled (true) or mangled (false) names.
        */
        void setDemangled(bool demangled) {
            infos.setDemangled(demangled);
        }

        /*!
         * \brief Indicate if the issues have to be displayed or not.
         * \param issues Boolean tag indicating if the analyzer must display the issues (true) or not (false).
        */
        void setDisplayIssues(bool issues) {
            m_issues = issues;
        }

        /*!
         * \brief Indicate if the statistic reports have to be displayed or not.
         * \param statistic Boolean tag indicating if the analyzer must display the statistic reports (true) or not (false).
        */
        void setDisplayStatistic(bool statistic) {
            m_statistic = statistic;
        }

        /*!
         * \brief Configure the filtering of duplicates option
         * \param filter Indicate if we must activate (true) or disable (false) the duplicates filtering.
         */
        void setFilterDuplicates(bool filter) {
            m_filter = filter;
        }

        /*!
         * \brief Add a filter
         * \param filter the filter to add
        */
        void addFilter(const std::string& filter) {
            filters.push_back(filter);
        }

        /*!
         * \brief Enable the default filters
        */
        void enableDefaultFilters() {
            m_default = true;
        }
    private:
        Infos infos;
        bool m_issues;
        bool m_statistic;
        bool m_filter;
        bool m_default;
        std::vector<std::string> filters;

        void printHeader(const std::string& file, const CallGraph& graph) const;
};

} //end of inlining

#endif
