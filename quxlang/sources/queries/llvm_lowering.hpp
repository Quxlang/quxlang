// Copyright 2026 Ryan P. Nicholl, rnicholl@protonmail.com
#ifndef QUXLANG_LLVM_LOWERING_HEADER_GUARD
#define QUXLANG_LLVM_LOWERING_HEADER_GUARD
#include <quxlang/llvm-backend-types.hpp>
#include <quxlang/queries/output_llvm_input.hpp>
#include <rpnx/querygraph/querygraph.hpp>

#include <optional>
namespace quxlang
{
    /** Selects whether the assembled backend input of a unit is lowered by the LLVM backend or discarded. */
    enum class llvm_unit_action
    {
        lower,
        check,
    };

    /**
     * Assembles the backend input of one unit from borrowed semantic data without retaining a packet query result.
     * The lower action invokes the LLVM backend while the borrowed data is alive and returns the preoptimized unit.
     * The check action evaluates the same frontend queries, does not invoke the LLVM backend, and returns no value.
     */
    template < rpnx::querygraph::query_handler_spec_c HandlerSpec >
    auto lower_llvm_unit(llvm_output_query_input input, llvm_unit_action action) -> typename rpnx::querygraph::coroutine< HandlerSpec >::template cosubroutine< std::optional< rpnx::cow< llvm_backend::llvm_preoptimized_unit > > >;
} // namespace quxlang
#endif
