#include "parser.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parse empty input", "[parser]")
{
	const char input[] = "";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse forward declare procedure", "[parser]")
{
	const char input[] = "void main();";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse empty procedure", "[parser]")
{
	const char input[] = "int main() { }";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse forward declare structure", "[parser]")
{
	const char input[] = "struct name;";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse empty structure", "[parser]")
{
	const char input[] = "struct name { }";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse operator procedure", "[parser]")
{
	const char input[] = "bool operator==(const T& x, const T& y);";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse structure data member", "[parser]")
{
	const char input[] =
		"struct singleton"
		"{"
		"T int;"
		"};";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse structure constructor", "[parser]")
{
	const char input[] =
		"struct singleton"
		"{"
		"singleton() { }"
		"};";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse structure destructor", "[parser]")
{
	const char input[] =
		"struct singleton"
		"{"
		"pointer(int) p_int;"
		"~singleton() { free(p_int); }"
		"};";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse typedef", "[parser]")
{
	const char input[] =
		"int main()"
		"{"
		"typedef int Integer;"
		"}";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse template type construction", "[parser]")
{
	const char input[] =
		"int main()"
		"{"
		"typedef pair Type;"
		"Type<int, int> foo;"
		"}";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse template specialization", "[parser]")
{
	const char input[] =
		"template <typename T>"
		"struct Value_type;"
		"template <typename T>"
		"struct Value_type<pointer(T)>;";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(parse(input, input_end));
}

TEST_CASE("Parse scoping", "[parser]")
{
	const char input[] =
		"int main()"
		"{"
		"typedef Pair type;"
		"}"
		"int fun()"
		"{"
		"type<int, int> foo;"
		"}";
	const char* input_end = input + sizeof(input) - 1;
	REQUIRE(!parse(input, input_end));
}
