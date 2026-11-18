// Copyright 2026 Ryan P. Nicholl, rnicholl@protonmail.com

#ifndef QUXLANG_QUERIES_LLVM_UNIT_CHECK_HEADER_GUARD
#define QUXLANG_QUERIES_LLVM_UNIT_CHECK_HEADER_GUARD

#include <quxlang/queries/output_llvm_input.hpp>

namespace quxlang
{
    /** Evaluates every frontend query that lowering one LLVM unit consumes, without invoking the LLVM backend. */
    struct llvm_unit_check_query
    {
        static constexpr auto query_id = "llvm_unit_check";
        using input_type = llvm_output_query_input;
        using output_type = bool;
    };
} // namespace quxlang

#endif // QUXLANG_QUERIES_LLVM_UNIT_CHECK_HEADER_GUARD
