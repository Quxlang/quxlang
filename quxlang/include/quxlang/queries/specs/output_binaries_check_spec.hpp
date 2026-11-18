// Copyright 2026 Ryan P. Nicholl, rnicholl@protonmail.com

#ifndef QUXLANG_QUERIES_SPECS_OUTPUT_BINARIES_CHECK_SPEC_HEADER_GUARD
#define QUXLANG_QUERIES_SPECS_OUTPUT_BINARIES_CHECK_SPEC_HEADER_GUARD

#include <quxlang/queries/llvm_compilation_unit_identities.hpp>
#include <quxlang/queries/llvm_unit_check.hpp>
#include <quxlang/queries/output_binaries_check.hpp>
#include <quxlang/queries/output_binaries_information.hpp>
#include <quxlang/queries/output_cortado_input.hpp>
#include <quxlang/queries/output_llvm_backend_options.hpp>
#include <quxlang/queries/output_llvm_catalog.hpp>
#include <quxlang/queries/run_static_tests.hpp>
#include <quxlang/queries/target_backend.hpp>

#include <rpnx/querygraph/querygraph.hpp>

namespace quxlang
{
    struct output_binaries_check_spec
    {
        using query = output_binaries_check_query;
        using dependencies = rpnx::typelist< run_static_tests_query, output_binaries_information_query, target_backend_query, llvm_compilation_unit_identities_query, output_llvm_backend_options_query, output_llvm_catalog_query, llvm_unit_check_query, output_cortado_input_query >;
    };

    rpnx::querygraph::coroutine< output_binaries_check_spec > output_binaries_check_impl(std::monostate input);
} // namespace quxlang

#endif // QUXLANG_QUERIES_SPECS_OUTPUT_BINARIES_CHECK_SPEC_HEADER_GUARD
