#ifndef EOP_LANG_SYMBOL_H
#define EOP_LANG_SYMBOL_H

#include <string>

enum class Symbol_kind
{
	type,
	procedure,
};

struct Symbol
{
	std::string name;
	Symbol_kind kind;

	Symbol() = default;
	Symbol(const std::string& name, Symbol_kind kind);
};

auto symbols_initialize() -> void;
auto symbol_push(const char* first, const char* last, Symbol_kind kind) -> const Symbol*;
auto symbol_get(const char* first, const char* last) -> const Symbol*;

#endif
