#ifndef EOP_LANG_TOKEN_ITERATOR_H
#define EOP_LANG_TOKEN_ITERATOR_H

#include <cstddef>
#include <iterator>

enum class Token_kind
{
	invalid,
	identifier,
	integer,
	real,
	dot,
	comment,
	forward_slash,
	open_paren,
	close_paren,
	open_bracket,
	close_bracket,
	open_brace,
	close_brace,
	comma,
	space,
	semicolon,
	ampersand,
	double_ampersand,
	pipe,
	double_pipe,
	minus,
	bang,
	bang_equals,
	star,
	percent,
	plus,
	equals,
	double_equals,
	less,
	greater,
	less_equals,
	greater_equals,
	colon,
	tilde,
};

struct Token
{
	const char* begin;
	const char* end;
	Token_kind kind;

	Token() = default;
	Token(const char* begin, const char* end, Token_kind kind);
};

auto operator==(const Token& x, const Token& y) -> bool;
auto operator!=(const Token& x, const Token& y) -> bool;

class Token_iterator
{
public:
	using value_type = Token;
	using reference = const value_type&;
	using pointer = const value_type*;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::forward_iterator_tag;

private:
	const char* m_end;
	value_type m_value;

public:
	Token_iterator() = default;
	Token_iterator(const char* begin, const char* end);

	auto operator*() const -> reference;
	auto operator->() const -> pointer;

	auto operator++() -> Token_iterator&;
	auto operator++(int) -> Token_iterator;
};

auto operator==(const Token_iterator& x, const Token_iterator& y) -> bool;
auto operator!=(const Token_iterator& x, const Token_iterator& y) -> bool;

#endif
