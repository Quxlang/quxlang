// Copyright 2026 Ryan P. Nicholl, rnicholl@protonmail.com

#ifndef QUXLANG_QUERIES_OUTPUT_BINARIES_CHECK_HEADER_GUARD
#define QUXLANG_QUERIES_OUTPUT_BINARIES_CHECK_HEADER_GUARD

#include <variant>

namespace quxlang
{
    /**
     * output_binaries_check_query evaluates the frontend queries of every configured output.
     * The query performs no optimization, code generation, or linking and produces no artifacts.
     */
    struct output_binaries_check_query
    {
        static constexpr auto query_id = "output_binaries_check";
        using input_type = std::monostate;
        using output_type = bool;
    };
} // namespace quxlang

#endif // QUXLANG_QUERIES_OUTPUT_BINARIES_CHECK_HEADER_GUARD
