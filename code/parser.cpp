#include "parser.h"
#include "token_iterator.h"
#include "filter_iterator.h"
#include "symbol.h"

#include <string_view>

struct Not_whitespace_or_comment
{
	auto operator()(const Token& x) const -> bool
	{
		switch (x.kind)
		{
		case Token_kind::comment:
		case Token_kind::newline:
		case Token_kind::tab:
		case Token_kind::space: {
			return false;
		} break;

		default: {
			return true;
		} break;
		}
	}
};

using Iterator = Filter_iterator<Token_iterator, Not_whitespace_or_comment>;

static Iterator s_token_iter;
static Iterator s_token_end;

auto peek(Token_kind kind) -> bool
{
	return s_token_iter != s_token_end && s_token_iter->kind == kind;
}

auto peek(std::string_view keyword) -> bool
{
	if (!peek(Token_kind::identifier))
	{
		return false;
	}

	return std::equal(s_token_iter->begin, s_token_iter->end, keyword.begin(), keyword.end());
}

auto match(Token_kind kind) -> bool
{
	if (peek(kind))
	{
		++s_token_iter;
		return true;
	}
	return false;
}

auto match(std::string_view keyword) -> bool
{
	if (peek(keyword))
	{
		++s_token_iter;
		return true;
	}
	return false;
}

auto parse_expression() -> bool;

auto parse_additive() -> bool;

/*
 * additive_list	= additive {"," additive}.
 */
auto parse_additive_list() -> bool
{
	do {
		if (!parse_additive())
		{
			return false;
		}
	} while (match(Token_kind::comma));

	return true;
}

/*
 * primary		= literal | identifier | "(" expression ")" | basic_type | template_name | "typename".
 */
auto parse_primary() -> bool
{
	// template_name = (structure_name | procedure_name) ["<" additive_list ">"].
	if (peek(Token_kind::identifier))
	{
		const Symbol* symbol = symbol_get(s_token_iter->begin, s_token_iter->end);
		if (symbol && (symbol->kind == Symbol_kind::type || symbol->kind == Symbol_kind::procedure))
		{
			++s_token_iter;
			if (match(Token_kind::less))
			{
				if (!parse_additive_list())
				{
					return false;
				}

				if (!match(Token_kind::greater))
				{
					return false;
				}
			}
			return true;
		}
	}

	// literal = boolean | integer | real.
	if (match("true") || match("false") || match(Token_kind::integer) || match(Token_kind::real))
	{
		return true;
	}

	// "(" expression ")"
	if (match(Token_kind::open_paren))
	{
		if (!parse_expression())
		{
			return false;
		}

		return match(Token_kind::close_paren);
	}

	// basic_type = "bool" | "int" | "double".
	if (match("bool") || match("int") || match("double"))
	{
		return true;
	}

	// "typename"
	if (match("typename"))
	{
		return true;
	}

	if (match(Token_kind::identifier))
	{
		return true;
	}

	return false;
}

/*
 * postfix		= primary {"." identifier
 * 				| "(" [expression_list] ")"
 * 				| "[" expression "]"
 * 				| "&"}.
 */
auto parse_postfix() -> bool
{
	if (!parse_primary())
	{
		return false;
	}

	while (true)
	{
		if (match(Token_kind::dot))
		{
			if (!match(Token_kind::identifier))
			{
				return false;
			}
		}
		else if (match(Token_kind::open_paren))
		{
			if (!peek(Token_kind::close_paren))
			{
				do {
					if (!parse_expression())
					{
						return false;
					}
				} while (match(Token_kind::comma));
			}

			if (!match(Token_kind::close_paren))
			{
				return false;
			}
		}
		else if (match(Token_kind::open_bracket))
		{
			if (!parse_expression())
			{
				return false;
			}

			if (!match(Token_kind::close_bracket))
			{
				return false;
			}
		}
		else if (match(Token_kind::ampersand))
		{
		}
		else
		{
			break;
		}
	}

	return true;
}

/*
 * prefix		= ["-" | "!" | "const"] postfix.
 */
auto parse_prefix() -> bool
{
	if (match(Token_kind::minus))
	{
	}
	else if (match(Token_kind::bang))
	{
	}
	else if (match("const"))
	{
	}
	return parse_postfix();
}

auto match_multiplicative() -> bool
{
	if (s_token_iter == s_token_end)
	{
		return false;
	}

	switch (s_token_iter->kind)
	{
	case Token_kind::star:
	case Token_kind::forward_slash:
	case Token_kind::percent: {
		++s_token_iter;
		return true;
	} break;

	default: {
		return false;
	} break;
	}
}

/*
 * multiplicative	= prefix {("*" | "/" | "%") prefix}.
 */
auto parse_multiplicative() -> bool
{
	if (!parse_prefix())
	{
		return false;
	}

	while (match_multiplicative())
	{
		if (!parse_prefix())
		{
			return false;
		}
	}

	return true;
}

auto match_additive() -> bool
{
	if (s_token_iter == s_token_end)
	{
		return false;
	}

	switch (s_token_iter->kind)
	{
	case Token_kind::plus:
	case Token_kind::minus: {
		++s_token_iter;
		return true;
	} break;

	default: {
		return false;
	} break;
	}
}

/*
 * additive		= multiplicative {("+" | "-") multiplicative}.
 */
auto parse_additive() -> bool
{
	if (!parse_multiplicative())
	{
		return false;
	}

	while (match_additive())
	{
		if (!parse_multiplicative())
		{
			return false;
		}
	}

	return true;
}

auto match_relational() -> bool
{
	if (s_token_iter == s_token_end)
	{
		return false;
	}

	switch (s_token_iter->kind)
	{
	case Token_kind::less:
	case Token_kind::greater:
	case Token_kind::less_equals:
	case Token_kind::greater_equals: {
		++s_token_iter;
		return true;
	} break;

	default: {
		return false;
	} break;
	}
}

/*
 * relational		= additive {("<" | ">" | "<=" | ">=") additive}.
 */
auto parse_relational() -> bool
{
	if (!parse_additive())
	{
		return false;
	}

	while (match_relational())
	{
		if (!parse_additive())
		{
			return false;
		}
	}

	return true;
}

auto match_equality() -> bool
{
	if (s_token_iter == s_token_end)
	{
		return false;
	}

	switch (s_token_iter->kind)
	{
	case Token_kind::double_equals:
	case Token_kind::bang_equals: {
		++s_token_iter;
		return true;
	} break;

	default: {
		return false;
	} break;
	}
}

/*
 * equality		= relational {("==" | "!=") relational}.
 */
auto parse_equality() -> bool
{
	if (!parse_relational())
	{
		return false;
	}

	while (match_equality())
	{
		if (!parse_relational())
		{
			return false;
		}
	}

	return true;
}

/*
 * conjunction		= equality {"&&" equality}.
 */
auto parse_conjunction() -> bool
{
	if (!parse_equality())
	{
		return false;
	}

	while (match(Token_kind::double_ampersand))
	{
		if (!parse_expression())
		{
			return false;
		}
	}

	return true;
}

/*
 * disjunction		= conjunction {"||" conjunction}.
 */
auto parse_disjunction() -> bool
{
	if (!parse_conjunction())
	{
		return false;
	}

	while (match(Token_kind::double_pipe))
	{
		if (!parse_conjunction())
		{
			return false;
		}
	}

	return true;
}

/*
 * expression		= disjunction.
 */
auto parse_expression() -> bool
{
	return parse_disjunction();
}

/*
 * enumeration		= "enum" identifier "{" identifier_list "}" ";".
 */
auto parse_enumeration() -> bool
{
	if (!match("enum"))
	{
		return false;
	}

	if (!match(Token_kind::identifier))
	{
		return false;
	}

	if (!match(Token_kind::open_brace))
	{
		return false;
	}

	if (!peek(Token_kind::close_brace))
	{
		do
		{
			if (!match(Token_kind::identifier))
			{
				return false;
			}
		}
		while (match(Token_kind::comma));
	}

	if (!match(Token_kind::close_brace))
	{
		return false;
	}

	if (!match(Token_kind::semicolon))
	{
		return false;
	}

	return true;
}

/*
 * parameter		= expression [identifier].
 */
auto parse_parameter() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	match(Token_kind::identifier);

	return true;
}

/*
 * parameter_list	= parameter {"," parameter}.
 */
auto parse_parameter_list() -> bool
{
	do {
		if (!parse_parameter())
		{
			return false;
		}
	} while (match(Token_kind::comma));
	return true;
}

/*
 * structure_name	= identifier.
 */
auto parse_structure_name() -> const Symbol*
{
	if (!peek(Token_kind::identifier))
	{
		return nullptr;
	}

	const Symbol* symbol = symbol_push(s_token_iter->begin, s_token_iter->end, Symbol_kind::type);
	++s_token_iter;
	return symbol;
}

/*
 * expression_list	= expression {"," expression }.
 */
auto parse_expression_list() -> bool
{
	do {
		if (!parse_expression())
		{
			return false;
		}
	} while (match(Token_kind::comma));
	return true;
}

/*
 * initializer		= identifier "(" [expression_list] ")".
 */
auto parse_initializer() -> bool
{
	if (!match(Token_kind::identifier))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		if (!parse_expression_list())
		{
			return false;
		}

		if (!match(Token_kind::close_paren))
		{
			return false;
		}
	}

	return true;
}

/*
 * initializer_list	= initializer {"," initializer}.
 */
auto parse_initializer_list() -> bool
{
	do {
		if (!parse_initializer())
		{
			return false;
		}
	} while (match(Token_kind::comma));
	return true;
}

auto parse_statement() -> bool;

/*
 * initialization	= "(" expression_list ")" | "=" expression.
 */
auto parse_initialization() -> bool
{
	if (match(Token_kind::open_paren))
	{
		if (!parse_expression_list())
		{
			return false;
		}

		if (!match(Token_kind::close_paren))
		{
			return false;
		}
	}
	else if (match(Token_kind::equals))
	{
		if (!parse_expression())
		{
			return false;
		}
	}

	return false;
}

/*
 * construction		= expression identifier [initialization] ";".
 */
auto parse_construction() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::identifier))
	{
		return false;
	}

	if (!peek(Token_kind::semicolon))
	{
		if (!parse_initialization())
		{
			return false;
		}
	}

	return match(Token_kind::semicolon);
}

/*
 * assignment		= expression "=" expression ";".
 */
auto parse_assignment() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::equals))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	return match(Token_kind::semicolon);
}

/*
 * simple_statement	= expression ";".
 */
auto parse_simple_statement() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	return match(Token_kind::semicolon);
}

/*
 * return		= "return" [expression] ";".
 */
auto parse_return() -> bool
{
	if (!match("return"))
	{
		return false;
	}

	if (!peek(Token_kind::semicolon))
	{
		if (!parse_expression())
		{
			return false;
		}
	}

	return match(Token_kind::semicolon);
}

/*
 * conditional		= "if" "(" expression ")" statement ["else" statement].
 */
auto parse_conditional() -> bool
{
	if (!match("if"))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	if (!parse_statement())
	{
		return false;
	}

	if (match("else"))
	{
		if (!parse_statement())
		{
			return false;
		}
	}

	return true;
}

auto parse_case() -> bool;

/*
 * switch		= "switch" "(" expression ")" "{" {case} "}".
 */
auto parse_switch() -> bool
{
	if (!match("switch"))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	if (!match(Token_kind::open_brace))
	{
		return false;
	}

	while (peek("case"))
	{
		if (!parse_case())
		{
			return false;
		}
	}

	return match(Token_kind::close_brace);
}

/*
 * case			= "case" expression ":" {statement}.
 */
auto parse_case() -> bool
{
	if (!match("case"))
	{
		return false;
	}

	while (!peek(Token_kind::close_brace) && !peek("break"))
	{
		if (!parse_statement())
		{
			return false;
		}
	}

	return true;
}

/*
 * while		= "while" "(" expression ")" statement.
 */
auto parse_while() -> bool
{
	if (!match("while"))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	return parse_statement();
}

/*
 * do			= "do" statement "while" "(" expression ")" ";".
 */
auto parse_do() -> bool
{
	if (!match("do"))
	{
		return false;
	}

	if (!parse_statement())
	{
		return false;
	}

	if (!match("while"))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	return match(Token_kind::semicolon);
}

/*
 * break		= "break" ";".
 */
auto parse_break() -> bool
{
	if (!match("break"))
	{
		return false;
	}

	return match(Token_kind::semicolon);
}

/*
 * goto			= "goto" identifier ";".
 */
auto parse_goto() -> bool
{
	if (!match("goto"))
	{
		return false;
	}

	if (!match(Token_kind::identifier))
	{
		return false;
	}

	return match(Token_kind::semicolon);
}

auto parse_compound() -> bool;

/*
 * control_statement	= return | conditional | switch | while | do | compound | break | goto.
 */
auto parse_control_statement() -> bool
{
	if (peek("return"))
	{
		return parse_return();
	}

	if (peek("if"))
	{
		return parse_conditional();
	}

	if (peek("switch"))
	{
		return parse_switch();
	}

	if (peek("while"))
	{
		return parse_while();
	}

	if (peek("do"))
	{
		return parse_do();
	}

	if (peek(Token_kind::open_brace))
	{
		return parse_compound();
	}

	if (peek("break"))
	{
		return parse_break();
	}

	if (peek("goto"))
	{
		return false;
	}

	return false;
}

/*
 * compound		= "{" {statement} "}".
 */
auto parse_compound() -> bool
{
	symbol_push_scope();
	if (!match(Token_kind::open_brace))
	{
		return false;
	}

	while (!match(Token_kind::close_brace))
	{
		if (!parse_statement())
		{
			return false;
		}
	}

	symbol_pop_scope();
	return true;
}

auto parse_typedef() -> bool;

/*
 * statement		= [identifier ":"]
 * 				(simple_statement | assignment
 * 				| construction | control_statement
 * 				| typedef).
 */
auto parse_statement() -> bool
{
	Iterator start = s_token_iter;

	if (match(Token_kind::identifier) && match(Token_kind::colon))
	{
		return true;
	}
	s_token_iter = start;

	if (parse_simple_statement())
	{
		return true;
	}
	s_token_iter = start;

	if (parse_assignment())
	{
		return true;
	}
	s_token_iter = start;

	if (parse_construction())
	{
		return true;
	}
	s_token_iter = start;

	if (parse_control_statement())
	{
		return true;
	}
	s_token_iter = start;

	if (parse_typedef())
	{
		return true;
	}

	return false;
}

/*
 * body		= compound.
 */
auto parse_body() -> bool
{
	return parse_compound();
}

/*
 * constructor	= structure_name "(" [parameter_list] ")" [":" initializer_list] body.
 */
auto parse_constructor(const Symbol& symbol) -> bool
{
	if (!match(symbol.name))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!peek(Token_kind::close_paren))
	{
		if (!parse_parameter_list())
		{
			return false;
		}
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	if (match(Token_kind::colon))
	{
		if (!parse_initializer_list())
		{
			return false;
		}
	}

	if (!parse_body())
	{
		return false;
	}

	return true;
}

/*
 * data_member		= expression identifier ["[" expression "]"] ";".
 */
auto parse_data_member() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	if (!match(Token_kind::identifier))
	{
		return false;
	}

	if (match(Token_kind::open_bracket))
	{
		if (!parse_expression())
		{
			return false;
		}

		if (!match(Token_kind::close_bracket))
		{
			return false;
		}
	}

	return match(Token_kind::semicolon);
}

/*
 * destructor		= "~" structure_name "(" ")" body.
 */
auto parse_destructor(const Symbol& symbol) -> bool
{
	if (!match(Token_kind::tilde))
	{
		return false;
	}

	if (!match(symbol.name))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	return parse_body();
}

/*
 * assign		= "void" "operator" "=" "(" parameter ")" body.
 */
auto parse_assign() -> bool
{
	if (!match("void"))
	{
		return false;
	}

	if (!match("operator"))
	{
		return false;
	}

	if (!match(Token_kind::equals))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_parameter())
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	return parse_body();
}

/*
 * index		= expression "operator" "[" "]" "(" parameter ")" body.
 */
auto parse_index() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	if (!match("operator"))
	{
		return false;
	}

	if (!match(Token_kind::open_bracket))
	{
		return false;
	}

	if (!match(Token_kind::close_bracket))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_parameter())
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	return parse_body();
}


/*
 * typedef		= "typedef" expression identifier ";".
 */
auto parse_typedef() -> bool
{
	if (!match("typedef"))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	if (!peek(Token_kind::identifier))
	{
		return false;
	}

	const Symbol* symbol = symbol_push(s_token_iter->begin, s_token_iter->end, Symbol_kind::type);
	if (!symbol)
	{
		return false;
	}
	++s_token_iter;

	return match(Token_kind::semicolon);
}

/*
 * apply		= expression "operator" "(" ")" "(" [parameter_list] ")" body.
 */
auto parse_apply() -> bool
{
	if (!parse_expression())
	{
		return false;
	}

	if (!match("operator"))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!match(Token_kind::close_paren))
	{
		if (!parse_parameter_list())
		{
			return false;
		}

		if (!match(Token_kind::close_paren))
		{
			return false;
		}
	}

	return parse_body();
}

/*
 * member		= data_member | constructor | destructor | assign | apply | index | typedef.
 */
auto parse_member(const Symbol& symbol) -> bool
{
	if (peek(symbol.name))
	{
		if (!parse_constructor(symbol))
		{
			return false;
		}
	}
	else if (peek(Token_kind::tilde))
	{
		if (!parse_destructor(symbol))
		{
			return false;
		}
	}
	else if (peek("typedef"))
	{
		return parse_typedef();
	}
	else
	{
		Iterator start = s_token_iter;

		if (parse_data_member())
		{
			return true;
		}
		s_token_iter = start;

		if (parse_assign())
		{
			return true;
		}
		s_token_iter = start;

		if (parse_apply())
		{
			return true;
		}
		s_token_iter = start;

		if (parse_index())
		{
			return true;
		}

		s_token_iter = start;
		return false;
	}

	return true;
}

/*
 * structure_body	= "{" {member} "}".
 */
auto parse_structure_body(const Symbol& symbol) -> bool
{
	if (!match(Token_kind::open_brace))
	{
		return false;
	}

	while (!match(Token_kind::close_brace))
	{
		if (!parse_member(symbol))
		{
			return false;
		}
	}

	return true;
}

/*
 * structure		= "struct" structure_name [structure_body] ";".
 */
auto parse_structure() -> bool
{
	if (!match("struct"))
	{
		return false;
	}

	const Symbol* symbol = parse_structure_name();
	if (!symbol)
	{
		return false;
	}

	if (match(Token_kind::semicolon))
	{
		return true;
	}

	if (!parse_structure_body(*symbol))
	{
		return false;
	}

	return match(Token_kind::semicolon);
}

/*
 * procedure_name	= identifier | operator.
 * operator		= "operator" ("==" | "<" | "+" | "-" | "*" | "/" | "%").
 */
auto parse_procedure_name() -> bool
{
	if (match("operator"))
	{
		if (s_token_iter == s_token_end)
		{
			return false;
		}

		switch (s_token_iter->kind)
		{
		case Token_kind::double_equals:
		case Token_kind::less:
		case Token_kind::plus:
		case Token_kind::minus:
		case Token_kind::star:
		case Token_kind::forward_slash:
		case Token_kind::percent: {
			++s_token_iter;
			return true;
		} break;

		default: {
			return false;
		} break;
		}
	}
	else
	{
		if (!peek(Token_kind::identifier))
		{
			return false;
		}

		// TODO Should we check the returned result to see if it was a procedure?
		symbol_push(s_token_iter->begin, s_token_iter->end, Symbol_kind::procedure);
		++s_token_iter;
		return true;
	}
}

/*
 * procedure		= (expression | "void") procedure_name "(" [parameter_list] ")" (body | ";").
 */
auto parse_procedure() -> bool
{
	if (!match("void") && !parse_expression())
	{
		return false;
	}

	if (!parse_procedure_name())
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!peek(Token_kind::close_paren))
	{
		do
		{
			if (!parse_parameter())
			{
				return false;
			}
		}
		while (match(Token_kind::comma));
	}

	if (!match(Token_kind::close_paren))
	{
		return false;
	}

	if (match(Token_kind::semicolon))
	{
		return true;
	}

	return parse_body();
}

/*
 * constraint		= "requires" "(" expression ")".
 */
auto parse_constraint() -> bool
{
	if (!match("requires"))
	{
		return false;
	}

	if (!match(Token_kind::open_paren))
	{
		return false;
	}

	if (!parse_expression())
	{
		return false;
	}

	return match(Token_kind::close_paren);
}

/*
 * template_decl	= "template" "<" [parameter_list] ">" [constraint].
 */
auto parse_template_decl() -> bool
{
	if (!match("template"))
	{
		return false;
	}

	if (!match(Token_kind::less))
	{
		return false;
	}

	if (!peek(Token_kind::greater))
	{
		if (!parse_parameter_list())
		{
			return false;
		}
	}

	if (!match(Token_kind::greater))
	{
		return false;
	}

	if (peek("requires"))
	{
		return parse_constraint();
	}

	return true;
}

/*
 * specialization	= "struct" structure_name "<" additive_list ">" [structure_body] ";".
 */
auto parse_specialization() -> bool
{
	if (!match("struct"))
	{
		return false;
	}

	const Symbol* symbol = parse_structure_name();
	if (!symbol)
	{
		return false;
	}

	if (!match(Token_kind::less))
	{
		return false;
	}

	if (!parse_additive_list())
	{
		return false;
	}

	if (!match(Token_kind::greater))
	{
		return false;
	}

	if (!peek(Token_kind::semicolon))
	{
		if (!parse_structure_body(*symbol))
		{
			return false;
		}
	}

	if (!match(Token_kind::semicolon))
	{
		return false;
	}

	return true;
}

/*
 * template		= template_decl (structure | procedure | specialization).
 */
auto parse_template() -> bool
{
	if (!parse_template_decl())
	{
		return false;
	}

	if (peek("struct"))
	{
		Iterator start = s_token_iter;

		if (parse_structure())
		{
			return true;
		}
		s_token_iter = start;

		if (parse_specialization())
		{
			return true;
		}
	}

	return parse_procedure();
}


auto parse_declaration() -> bool
{
	if (peek("enum"))
	{
		return parse_enumeration();
	}
	else if (peek("struct"))
	{
		return parse_structure();
	}
	else if (peek("template"))
	{
		return parse_template();
	}

	return parse_procedure();
}

auto parse(const char* begin, const char* end) -> bool
{
	symbols_initialize();

	Token_iterator token_begin(begin, end);
	Token_iterator token_end(end, end);
	s_token_iter = Iterator(token_begin, token_end, Not_whitespace_or_comment());
	s_token_end = Iterator(token_end, token_end, Not_whitespace_or_comment());

	while (s_token_iter != s_token_end && parse_declaration())
	{
	}

	return s_token_iter == s_token_end;
}
