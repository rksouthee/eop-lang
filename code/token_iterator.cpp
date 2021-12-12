#include "token_iterator.h"

#include <algorithm>

auto is_letter(char c) -> bool
{
	switch (c)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z': {
		return true;
	} break;

	default: {
		return false;
	} break;
	}
}

auto is_digit(char c) -> bool
{
	switch (c)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		return true;
	} break;

	default: {
		return false;
	} break;
	}
}

Token::Token(const char* begin, const char* end, Token_kind kind) :
	begin(begin),
	end(end),
	kind(kind)
{
}

auto operator==(const Token& x, const Token& y) -> bool
{
	return x.begin == y.begin;
}

auto operator!=(const Token& x, const Token& y) -> bool
{
	return !(x == y);
}

Token_iterator::Token_iterator(const char* begin, const char* end) :
	m_end(end)
{
	if (begin != end)
	{
		m_value.end = begin;
		++*this;
	}
	else
	{
		m_value.begin = end;
	}
}

auto Token_iterator::operator*() const -> reference
{
	return m_value;
}

auto Token_iterator::operator->() const -> pointer
{
	return &**this;
}

auto Token_iterator::operator++() -> Token_iterator&
{
	m_value.begin = m_value.end;
	++m_value.end;

	switch (*m_value.begin)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_': {
		m_value.end = std::find_if_not(m_value.end, m_end, [] (char c) -> bool {
			return is_letter(c) || c == '_' || is_digit(c);
		});
		m_value.kind = Token_kind::identifier;
	} break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		m_value.end = std::find_if_not(m_value.end, m_end, is_digit);
		if (m_value.end != m_end && *m_value.end == '.')
		{
			m_value.end = std::find_if_not(m_value.end + 1, m_end, is_digit);
			m_value.kind = Token_kind::real;
		}
		else
		{
			m_value.kind = Token_kind::integer;
		}
	} break;

	case '.': {
		if (m_value.end != m_end && is_digit(*m_value.end))
		{
			m_value.end = std::find_if_not(m_value.end + 1, m_end, is_digit);
			m_value.kind = Token_kind::real;
		}
		else
		{
			m_value.kind = Token_kind::dot;
		}
	} break;

	case '/': {
		if (m_value.end != m_end && *m_value.end == '/')
		{
			m_value.end = std::find(m_value.end + 1, m_end, '\n');
			m_value.kind = Token_kind::comment;
		}
		else
		{
			m_value.kind = Token_kind::forward_slash;
		}
	} break;

	case '(': {
		m_value.kind = Token_kind::open_paren;
	} break;

	case ')': {
		m_value.kind = Token_kind::close_paren;
	} break;

	case '[': {
		m_value.kind = Token_kind::open_bracket;
	} break;

	case ']': {
		m_value.kind = Token_kind::close_bracket;
	} break;

	case '{': {
		m_value.kind = Token_kind::open_brace;
	} break;

	case '}': {
		m_value.kind = Token_kind::close_brace;
	} break;

	case ',': {
		m_value.kind = Token_kind::comma;
	} break;

	case ' ': {
		m_value.kind = Token_kind::space;
	} break;

	case '\n': {
		m_value.kind = Token_kind::newline;
	} break;

	case '\t': {
		m_value.kind = Token_kind::tab;
	} break;

	case ';': {
		m_value.kind = Token_kind::semicolon;
	} break;

	case '&': {
		if (m_value.end != m_end && *m_value.end == '&')
		{
			++m_value.end;
			m_value.kind = Token_kind::double_ampersand;
		}
		else
		{
			m_value.kind = Token_kind::ampersand;
		}
	} break;

	case '|': {
		if (m_value.end != m_end && *m_value.end == '|')
		{
			++m_value.end;
			m_value.kind = Token_kind::double_pipe;
		}
		else
		{
			m_value.kind = Token_kind::pipe;
		}
	} break;

	case '-': {
		m_value.kind = Token_kind::minus;
	} break;

	case '!': {
		if (m_value.end != m_end && *m_value.end == '=')
		{
			++m_value.end;
			m_value.kind = Token_kind::bang_equals;
		}
		else
		{
			m_value.kind = Token_kind::bang;
		}
	} break;

	case '*': {
		m_value.kind = Token_kind::star;
	} break;

	case '%': {
		m_value.kind = Token_kind::percent;
	} break;

	case '+': {
		m_value.kind = Token_kind::plus;
	} break;

	case '=': {
		if (m_value.end != m_end && *m_value.end == '=')
		{
			++m_value.end;
			m_value.kind = Token_kind::double_equals;
		}
		else
		{
			m_value.kind = Token_kind::equals;
		}
	} break;

	case '<': {
		if (m_value.end != m_end && *m_value.end == '=')
		{
			++m_value.end;
			m_value.kind = Token_kind::less_equals;
		}
		else
		{
			m_value.kind = Token_kind::less;
		}
	} break;

	case '>': {
		if (m_value.end != m_end && *m_value.end == '=')
		{
			++m_value.end;
			m_value.kind = Token_kind::greater_equals;
		}
		else
		{
			m_value.kind = Token_kind::greater;
		}
	} break;

	case ':': {
		m_value.kind = Token_kind::colon;
	} break;

	case '~': {
		m_value.kind = Token_kind::tilde;
	} break;

	default: {
		m_value.kind = Token_kind::invalid;
	} break;
	}

	return *this;
}

auto Token_iterator::operator++(int) -> Token_iterator
{
	Token_iterator tmp = *this;
	++*this;
	return tmp;
}

auto operator==(const Token_iterator& x, const Token_iterator& y) -> bool
{
	return *x == *y;
}

auto operator!=(const Token_iterator& x, const Token_iterator& y) -> bool
{
	return !(x == y);
}
