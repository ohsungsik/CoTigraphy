// \file test_command_line_parser.cpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include <CommandLineParser.hpp>

namespace CoTigraphy
{
	// CommandLineOption 테스트
	class UnitTest_CommandLineOption : public ::testing::Test
	{
	protected:
		CommandLineOption option;
	};

	TEST_F(UnitTest_CommandLineOption, IsValid_True)
	{
		option = CommandLineOption{ L"--version", L"-v", L"Print version", false, true, [&](const std::wstring&) {} };
		EXPECT_TRUE(option.IsValid());
	}
	TEST_F(UnitTest_CommandLineOption, IsValid_FALSE)
	{
		option = CommandLineOption{ L"", L"-v", L"Print version", false, true, [&](const std::wstring&) {} };
		EXPECT_FALSE(option.IsValid());

		option = CommandLineOption{ L"--version", L"", L"Print version", false, true, [&](const std::wstring&) {} };
		EXPECT_FALSE(option.IsValid());

		option = CommandLineOption{ L"--version", L"-v", L"", false, true, [&](const std::wstring&) {} };
		EXPECT_FALSE(option.IsValid());
	}


	// CommandLineParser 테스트
	class UnitTest_CommandLineParser : public ::testing::Test
	{
	protected:
		CommandLineParser parser;
	};

	TEST_F(UnitTest_CommandLineParser, AddOption_TriggersDeath)
	{
		const CommandLineOption option1{ L"--test1", L"-t", L"Option1", false, false, [&](const std::wstring&) {} };
		EXPECT_TRUE(parser.AddOption(option1).IsSucceeded());

		const CommandLineOption option2{ L"--test2", L"-t", L"Option2", false, false, [&](const std::wstring&) {} };
		EXPECT_DEATH(
			std::ignore = parser.AddOption(option2),
			""
		);

		const CommandLineOption option3{ L"", L"-t", L"Option2", false, false, [&](const std::wstring&) {} };
		EXPECT_DEATH(
			std::ignore = parser.AddOption(option3),
			""
		);

		const CommandLineOption option4{ L"--test2", L"", L"Option2", false, false, [&](const std::wstring&) {} };
		EXPECT_DEATH(
			std::ignore = parser.AddOption(option4),
			""
		);

		const CommandLineOption option5{ L"--test2", L"-t", L"", false, false, [&](const std::wstring&) {} };
		EXPECT_DEATH(
			std::ignore = parser.AddOption(option5),
			""
		);
	}

	// AddOption Success
	TEST_F(UnitTest_CommandLineParser, AddOption_SucceedsAndPostconditionHolds)
	{
		const CommandLineOption option{ L"--version", L"-v", L"Print version", false, false,
			[&](const std::wstring&) {} };

		EXPECT_TRUE(parser.AddOption(option).IsSucceeded());
	}

	// 정상 옵션 처리
	TEST_F(UnitTest_CommandLineParser, Parse_Vector_ValueOptionSuccess)
	{
		bool called = false;
		std::wstring lastValue = L"";
		const CommandLineOption opt{ L"--opt", L"-o", L"Value option", true, false,
			[&](const std::wstring& val) { called = true; lastValue = val; } };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		const std::vector<std::wstring> args{ L"--opt", L"value" };
		const Error err = parser.Parse(args);

		EXPECT_TRUE(err.IsSucceeded());
		EXPECT_TRUE(called);
		EXPECT_EQ(lastValue, L"value");
	}

	// 존재하지 않는 옵션 처리
	TEST_F(UnitTest_CommandLineParser, Parse_Vector_UnknownOption)
	{
		bool called = false;
		std::wstring lastValue = L"";
		const CommandLineOption opt{ L"--opt", L"-o", L"Value option", true, false,
			[&](const std::wstring& val) { called = true; lastValue = val; } };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		const std::vector<std::wstring> args{ L"--unknown" };
		const Error err = parser.Parse(args);

		EXPECT_TRUE(err.IsFailed());
		EXPECT_EQ(err, eErrorCode::CommandLineArgumentNotFound);
	}

	// RequiresValue = true, 정상 값 제공
	TEST_F(UnitTest_CommandLineParser, ProcessToken_ValueOptionSuccess)
	{
		bool called = false;
		std::wstring lastValue = L"";
		const CommandLineOption opt{ L"--opt", L"-o", L"Value option", true, false,
			[&](const std::wstring& val) { called = true; lastValue = val; } };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		const std::vector<std::wstring> args{ L"--opt", L"value" };
		const Error error = parser.Parse(args);

		EXPECT_TRUE(error.IsSucceeded());
		EXPECT_TRUE(called);
		EXPECT_EQ(lastValue, L"value");
	}

	// RequiresValue = true, 값 누락
	TEST_F(UnitTest_CommandLineParser, ProcessToken_FailsWhenValueMissing)
	{
		CommandLineOption opt{ L"--opt", L"-o", L"Value option", true, false,
			[&](const std::wstring&) {} };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		const std::vector<std::wstring> args{ L"--opt" };
		const Error error = parser.Parse(args);

		EXPECT_TRUE(error.IsFailed());
		EXPECT_EQ(error, MAKE_ERROR_FROM_WIN32(E_INVALIDARG));

		opt = { L"--opt1", L"-o1", L"Value option1", true, false,
			[&](const std::wstring&) {} };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());
	}

	// RequiresValue = true, 빈 값 제공
	TEST_F(UnitTest_CommandLineParser, ProcessToken_FailsWhenValueEmpty)
	{
		const CommandLineOption opt{ L"--opt", L"-o", L"Value option", true, false,
			[&](const std::wstring&) {} };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		const std::vector<std::wstring> args{ L"--opt", L"" };
		const Error error = parser.Parse(args);

		EXPECT_TRUE(error.IsFailed());
		EXPECT_EQ(error, MAKE_ERROR_FROM_WIN32(E_INVALIDARG));
	}

	// RequiresValue = false, 핸들러 없음
	TEST_F(UnitTest_CommandLineParser, ProcessToken_SucceedsWhenNoHandler)
	{
		const CommandLineOption opt{ L"--opt", L"-o", L"No handler option", false, false, nullptr };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		const std::vector<std::wstring> args{ L"--opt", L"" };
		const Error error = parser.Parse(args);

		EXPECT_TRUE(error.IsFailed());
		EXPECT_EQ(error, MAKE_ERROR_FROM_WIN32(E_INVALIDARG));
	}

	// EarlyExit 경로 테스트
	TEST_F(UnitTest_CommandLineParser, Parse_Vector_EarlyExitOption)
	{
		bool called = false;
		const CommandLineOption helpOpt{ L"--help", L"-h", L"Help", false, true,
			[&](const std::wstring&) { called = true; } };
		EXPECT_TRUE(parser.AddOption(helpOpt).IsSucceeded());

		const CommandLineOption OptionOpt{ L"--opt", L"-o", L"option", false, false,
			[&](const std::wstring&) { called = true; } };
		EXPECT_TRUE(parser.AddOption(OptionOpt).IsSucceeded());

		std::vector<std::wstring> args{ L"--help" };
		Error error = parser.Parse(args);

		EXPECT_EQ(error.GetErrorCode(), eErrorCode::EarlyExit);
		EXPECT_TRUE(called);

		// EarlyExit 우선 확인
		called = false;
		args = { L"--help", L"--opt" };
		error = parser.Parse(args);
		EXPECT_EQ(error.GetErrorCode(), eErrorCode::EarlyExit);
		EXPECT_TRUE(called);

		called = false;
		args = { L"--opt", L"--help", };
		error = parser.Parse(args);
		EXPECT_EQ(error.GetErrorCode(), eErrorCode::EarlyExit);
		EXPECT_TRUE(called);
	}

	// PrintHelpTo 간단 확인
	TEST_F(UnitTest_CommandLineParser, PrintHelpTo_GeneratesHelpOutput)
	{
		EXPECT_TRUE(parser.Parse(std::vector<std::wstring>{}).IsSucceeded());

		// 아무 옵션을 추가하지 않고 실행하면 기본으로 help가 출력되어야 함.
		std::wstringstream ss;
		parser.PrintHelpTo(ss);
		EXPECT_NE(ss.str().find(L"CoTigraphy"), std::wstring::npos);
		EXPECT_NE(ss.str().find(L"Available options:"), std::wstring::npos);
		EXPECT_NE(ss.str().find(L"For more information"), std::wstring::npos);

		const CommandLineOption opt{ L"--opt", L"-o", L"Test option", false, false,
			[&](const std::wstring&) {} };
		EXPECT_TRUE(parser.AddOption(opt).IsSucceeded());

		ss.clear();
		parser.PrintHelpTo(ss);

		EXPECT_NE(ss.str().find(L"--opt"), std::wstring::npos);
		EXPECT_NE(ss.str().find(L"-o"), std::wstring::npos);
		EXPECT_NE(ss.str().find(L"Test option"), std::wstring::npos);
	}
}	// namespace CoTigraphy
