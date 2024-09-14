#pragma once

#include "stdafx.h"

namespace NGMC
{
	enum CachedUICommandType
	{
		Undefined,
		Text
	};

	class CachedUICommand
	{
	public:
		CachedUICommand();
		~CachedUICommand();

		void* Execute();

		void SetText(const char* text);

	private:
		CachedUICommandType m_Type;
		std::vector<std::string> m_Strings;
	};

	class CachedUI
	{
	public:
		CachedUI();
		~CachedUI();

		void Render();

		void AddCommand(CachedUICommand& command);

	private:
		std::vector<CachedUICommand> m_Cmds;
	};
}