#include "token_iterator.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Lex empty input",  "[token iterator]")
{
	const char input[] = "";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin == end);
}

TEST_CASE("Lex invalid character", "[token iterator]")
{
	const char input[] = "?";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::invalid);
	++begin;
	REQUIRE(begin == end);
}

TEST_CASE("Lex identifier", "[token iterator]")
{
	const char input[] = "_an0";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::identifier);
	REQUIRE(begin->begin == input);
	REQUIRE(begin->end == input_end);

	++begin;
	REQUIRE(begin == end);
}

TEST_CASE("Lex integer", "[token iterator]")
{
	const char input[] = "23428";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::integer);
	REQUIRE(begin->begin == input);
	REQUIRE(begin->end == input_end);

	++begin;
	REQUIRE(begin == end);
}

TEST_CASE("Lex real with whole and decimal", "[token iterator]")
{
	const char input[] = "9874.34907";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::real);
	REQUIRE(begin->begin == input);
	REQUIRE(begin->end == input_end);

	++begin;
	REQUIRE(begin == end);
}

TEST_CASE("Lex real whole only", "[token iterator]")
{
	const char input[] = "3984753.";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::real);
	REQUIRE(begin->begin == input);
	REQUIRE(begin->end == input_end);

	++begin;
	REQUIRE(begin == end);
}

TEST_CASE("Lex real decimal only", "[token iterator]")
{
	const char input[] = ".39753";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::real);
	REQUIRE(begin->begin == input);
	REQUIRE(begin->end == input_end);

	++begin;
	REQUIRE(begin == end);
}

TEST_CASE("Lex comment", "[token iterator]")
{
	const char input[] = "// Hello";
	const char* input_end = input + sizeof(input) - 1;

	Token_iterator begin(input, input_end);
	Token_iterator end(input_end, input_end);

	REQUIRE(begin != end);
	REQUIRE(begin->kind == Token_kind::comment);
	REQUIRE(begin->begin == input);
	REQUIRE(begin->end == input_end);

	++begin;
	REQUIRE(begin == end);
}
