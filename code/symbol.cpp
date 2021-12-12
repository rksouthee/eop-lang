#include "symbol.h"

#include <cassert>

#include <unordered_map>
#include <vector>

using Symbol_table = std::unordered_map<std::string, Symbol>;
static std::vector<Symbol_table> s_symbols;

auto symbols_initialize() -> void
{
	s_symbols.clear();
	s_symbols.emplace_back();
}

Symbol::Symbol(const std::string& name, Symbol_kind kind) :
	name(name),
	kind(kind)
{
}

auto symbol_push_scope() -> void
{
	s_symbols.emplace_back();
}

auto symbol_pop_scope() -> void
{
	assert(s_symbols.size() > 1);
	s_symbols.pop_back();
}

auto symbol_push(const char* first, const char* last, Symbol_kind kind) -> const Symbol*
{
	assert(!s_symbols.empty());
	// TODO May want to check if the already exists
	std::string name(first, last);
	return &(s_symbols.back()[name] = Symbol(name, kind));
}

auto symbol_get(const char* begin, const char* end) -> const Symbol*
{
	for (std::size_t i = s_symbols.size(); i > 0; --i)
	{
		auto& symbols = s_symbols[i - 1];
		if (auto iter = symbols.find(std::string(begin, end)); iter != symbols.end())
		{
			return &iter->second;
		}
	}
	return nullptr;
}
