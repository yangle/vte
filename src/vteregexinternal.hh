/*
 * Copyright © 2015 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "regex.hh"

static inline auto wrapper_from_regex(vte::base::Regex* regex)
{
        return reinterpret_cast<VteRegex*>(regex);
}

static inline auto regex_from_wrapper(VteRegex* regex)
{
        return reinterpret_cast<vte::base::Regex*>(regex);
}

static inline auto regex_array_from_wrappers(VteRegex** regexes)
{
        return const_cast<vte::base::Regex const**>(reinterpret_cast<vte::base::Regex**>(regexes));
}

bool _vte_regex_has_purpose(VteRegex* regex,
                            vte::base::Regex::Purpose purpose);

bool _vte_regex_has_multiline_compile_flag(VteRegex* regex);

/* GRegex translation */
VteRegex* _vte_regex_new_gregex(vte::base::Regex::Purpose purpose,
                                GRegex* gregex);

uint32_t _vte_regex_translate_gregex_match_flags(GRegexMatchFlags flags);
