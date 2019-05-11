/*
 * Copyright © 2019 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <glib.h>

#include "regex.hh"
#include "regex-builtins.hh"
#include "regex-builtins-patterns.hh"

#include "vtepcre2.h"

namespace vte::base {

RegexBuiltins::RegexBuiltins()
{
        m_builtins.reserve(8);

        compile_builtin(REGEX_URL_AS_IS, InternalBuiltinsTags::eURL);
        compile_builtin(REGEX_URL_HTTP,  InternalBuiltinsTags::eHTTP);
        compile_builtin(REGEX_URL_FILE,  InternalBuiltinsTags::eFILE);
        compile_builtin(REGEX_URL_VOIP,  InternalBuiltinsTags::eVOIP);
        compile_builtin(REGEX_EMAIL,     InternalBuiltinsTags::eEMAIL);
        compile_builtin(REGEX_NEWS_MAN,  InternalBuiltinsTags::eNEWS_MAN);
}

void
RegexBuiltins::compile_builtin(char const* pattern,
                               InternalBuiltinsTags tag)
{
        GError* error{nullptr};
        auto regex = Regex::compile(Regex::Purpose::eMatch,
                                    pattern,
                                    PCRE2_ZERO_TERMINATED,
                                    PCRE2_UTF | PCRE2_UCP | PCRE2_NO_UTF_CHECK | PCRE2_MULTILINE,
                                    &error);
        if (error) {
                g_printerr("Failed to compile builtin regex %d: %s\n", int(tag), error->message);
                g_error_free(error);
                return;
        }

        regex->jit(PCRE2_JIT_COMPLETE, &error);
        if (error) {
                g_printerr("Failed to complete JIT compile builtin regex %d: %s\n", int(tag), error->message);
                g_clear_error(&error);
        }

        regex->jit(PCRE2_JIT_PARTIAL_SOFT, &error);
        if (error) {
                g_printerr("Failed to partial-soft JIT compile builtin regex %d: %s\n", int(tag), error->message);
                g_clear_error(&error);
        }

        m_builtins.emplace_back(take_ref(regex), int(tag));
}

int
RegexBuiltins::transform_match(char*& match,
                               int tag) const noexcept
{
        switch (InternalBuiltinsTags(tag)) {
        case InternalBuiltinsTags::eURL:
        case InternalBuiltinsTags::eFILE:
        case InternalBuiltinsTags::eNEWS_MAN:
        case InternalBuiltinsTags::eVOIP:
                /* No transformation */
                return int(BuiltinsTags::eURI);

        case InternalBuiltinsTags::eHTTP: {
                auto v = match;
                match = g_strdup_printf("http://%s", match);
                g_free(v);
                return int(BuiltinsTags::eURI);
        }

        case InternalBuiltinsTags::eEMAIL:
                if (g_ascii_strncasecmp ("mailto:", match, 7) != 0) {
                        auto v = match;
                        match = g_strdup_printf ("mailto:%s", match);
                        g_free(v);
                }
                return int(BuiltinsTags::eURI);
        }

        return -1;
}

} // namespace vte::base
