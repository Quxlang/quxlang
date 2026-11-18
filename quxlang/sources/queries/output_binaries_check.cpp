// Copyright 2026 Ryan P. Nicholl, rnicholl@protonmail.com

#include <quxlang/exception.hpp>
#include <quxlang/queries/specs/output_binaries_check_spec.hpp>

#include <set>
#include <vector>

rpnx::querygraph::coroutine< quxlang::output_binaries_check_spec > quxlang::output_binaries_check_impl(std::monostate)
{
    co_yield rpnx::querygraph::dependency< run_static_tests_query >(std::monostate{});
    std::map< std::string, output_query_output > const output_information = co_await rpnx::querygraph::request< output_binaries_information_query >(std::monostate{});
    backend_kind const backend = co_await rpnx::querygraph::request< target_backend_query >(std::monostate{});

    if (backend == backend_kind::cortado)
    {
        for (std::pair< std::string const, output_query_output > const& output_entry : output_information)
        {
            co_yield rpnx::querygraph::dependency< output_cortado_input_query >(output_entry.first);
        }
        for (std::pair< std::string const, output_query_output > const& output_entry : output_information)
        {
            co_await rpnx::querygraph::request< output_cortado_input_query >(output_entry.first);
        }
        co_await rpnx::querygraph::request< run_static_tests_query >(std::monostate{});
        co_return true;
    }
    if (backend != backend_kind::llvm)
    {
        throw compiler_bug("Unsupported target backend");
    }

    std::vector< rpnx::querygraph::request< llvm_unit_check_query > > checks;
    for (std::pair< std::string const, output_query_output > const& output_entry : output_information)
    {
        std::vector< llvm_output_query_input > const& identities = co_await rpnx::querygraph::request< llvm_compilation_unit_identities_query >(output_entry.first);
        backend_llvm_options const& options = co_await rpnx::querygraph::request< output_llvm_backend_options_query >(output_entry.first);
        for (llvm_output_query_input const& identity : identities)
        {
            if (!build_type_uses_lto(*options.build_type) || !identity.unit.type_is< std::monostate >())
            {
                checks.emplace_back(identity);
                continue;
            }
            // A link-time-optimized component is lowered as one support unit and one unit per owned procedure.
            llvm_component_catalog const& catalog = co_await rpnx::querygraph::request< output_llvm_catalog_query >({identity.output_name, identity.component});
            std::set< type_symbol > procedures = catalog.inlinable_functions;
            if (catalog.root_routine == llvm_backend::root_routine_emission::definition && !catalog.asm_functions.contains(catalog.target_name))
            {
                procedures.insert(catalog.target_name);
            }
            llvm_output_query_input unit = identity;
            unit.unit = llvm_support_data_unit{};
            checks.emplace_back(unit);
            for (type_symbol const& procedure : procedures)
            {
                unit.unit = procedure;
                checks.emplace_back(unit);
            }
        }
    }
    for (rpnx::querygraph::request< llvm_unit_check_query >& check : checks)
    {
        co_yield rpnx::querygraph::dependency(check);
    }
    co_await rpnx::querygraph::request< run_static_tests_query >(std::monostate{});
    for (rpnx::querygraph::request< llvm_unit_check_query >& check : checks)
    {
        co_await check;
    }
    co_return true;
}
