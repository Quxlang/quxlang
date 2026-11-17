// Copyright 2024-2026 Ryan P. Nicholl, rnicholl@protonmail.com

#ifndef QUXLANG_BACKENDS_ASM_GNU_ASM_CONVERTER_HEADER_GUARD
#define QUXLANG_BACKENDS_ASM_GNU_ASM_CONVERTER_HEADER_GUARD

#include "quxlang/asm/asm.hpp"
#include "quxlang/data/machine.hpp"
#include "quxlang/backends/asm/symbol_format.hpp"
#include "quxlang/variant_utils.hpp"
#include "rpnx/unimplemented.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <string_view>

namespace quxlang
{
    inline std::string to_lower_str(std::string str)
    {
        for (char& i : str)
        {
            if (i >= 'A' && i <= 'Z')
            {
                i = i - 'A' + 'a';
            }
        }
        return str;
    }

    /**
     * Converts an assembly routine using GNU operand syntax into textual assembly accepted by LLVM's integrated assembler.
     * ARM32 literal pools are emitted immediately after their owning routine.
     * RISC-V routines declare instruction_set so LLVM bitcode symbol scanning uses the configured ISA.
     */
    template < typename It >
    inline std::string convert_to_gnu_asm(It begin, It end, std::string const& name, cpu architecture, std::string_view instruction_set, bool emit_elf_type = false)
    {
        std::string const asm_name = format_asm_symbol_name(name);
        std::string result;
        if (architecture == cpu::riscv_32 || architecture == cpu::riscv_64)
        {
            result = ".option push\n.option arch, " + std::string(instruction_set) + "\n.option norelax\n";
        }
        result += ".text\n";
        result += ".global " + asm_name + "\n";
        if (emit_elf_type)
        {
            result += ".type " + asm_name + ", %function\n";
        }
        result += asm_name + ":\n";

        bool riscv = architecture == cpu::riscv_32 || architecture == cpu::riscv_64;
        std::map< std::string, std::string > symbol_aliases;
        std::string alias_prefix;
        if (riscv)
        {
            alias_prefix = ".LQUXLANG_RISCV_";
            std::string_view digits = "0123456789abcdef";
            for (char character : name)
            {
                std::uint8_t byte = static_cast< std::uint8_t >(character);
                alias_prefix += digits[byte >> 4];
                alias_prefix += digits[byte & 15];
            }
            alias_prefix += '_';
        }
        // RISC-V call operands require bare identifiers even when ELF permits quoted symbol names.
        auto format_operand = [&](std::string operand) -> std::string
        {
            std::size_t position = 0;
            while ((position = operand.find('"', position)) != std::string::npos)
            {
                std::size_t end_quote = position + 1;
                while (end_quote < operand.size() && operand[end_quote] != '"')
                {
                    if (operand[end_quote] == '\\')
                    {
                        ++end_quote;
                    }
                    ++end_quote;
                }
                if (end_quote >= operand.size())
                {
                    throw compiler_bug("Unterminated quoted assembly symbol");
                }
                std::string symbol = operand.substr(position, end_quote - position + 1);
                std::map< std::string, std::string >::iterator alias = symbol_aliases.find(symbol);
                if (alias == symbol_aliases.end())
                {
                    std::string identifier = alias_prefix + std::to_string(symbol_aliases.size());
                    alias = symbol_aliases.emplace(symbol, std::move(identifier)).first;
                    result += ".set " + alias->second + ", " + symbol + "\n";
                }
                operand.replace(position, end_quote - position + 1, alias->second);
                position += alias->second.size();
            }
            return operand;
        };

        It pos = begin;
        while (pos != end)
        {
            asm_statement const statement = *pos++;
            if (typeis< asm_instruction >(statement))
            {
                asm_instruction const& instruction = as< asm_instruction >(statement);
                bool requires_identifier = riscv && (instruction.opcode_mnemonic == "CALL" || instruction.opcode_mnemonic == "TAIL");
                std::string instruction_text = "    " + to_lower_str(instruction.opcode_mnemonic);
                for (std::size_t i = 0; i < instruction.operands.size(); i++)
                {
                    instruction_text += (i == 0 ? " " : ", ");
                    instruction_text += requires_identifier ? format_operand(instruction.operands[i]) : instruction.operands[i];
                }
                result += instruction_text + "\n";
                continue;
            }

            if (typeis< asm_label >(statement))
            {
                asm_label const& label = as< asm_label >(statement);
                result += label.name + ":\n";
                continue;
            }

            rpnx::unimplemented();
        }
        if (architecture == cpu::arm_32)
        {
            result += ".ltorg\n";
        }
        if (architecture == cpu::riscv_32 || architecture == cpu::riscv_64)
        {
            result += ".option pop\n";
        }
        return result;
    }
} // namespace quxlang

#endif // QUXLANG_BACKENDS_ASM_GNU_ASM_CONVERTER_HEADER_GUARD
