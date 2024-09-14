#include "stdafx.h"

#include "CachedUI.h"

namespace NGMC
{
	CachedUICommand::CachedUICommand()
		: m_Type(CachedUICommandType::Undefined), m_Strings(0)
	{
	}

	CachedUICommand::~CachedUICommand()
	{
	}

	void* CachedUICommand::Execute()
	{
		switch (m_Type)
		{
		case CachedUICommandType::Text:
		{
			ImGui::Text(m_Strings[0].c_str());
			return nullptr;
		}
		case CachedUICommandType::Undefined:
		default:
		{
			return nullptr;
		}
		}
	}

	void CachedUICommand::SetText(const char* text)
	{
		m_Type = CachedUICommandType::Text;
		m_Strings.emplace_back(text);
	}

	CachedUI::CachedUI()
	{
	}

	CachedUI::~CachedUI()
	{
	}

	void CachedUI::Render()
	{
	}

	void CachedUI::AddCommand(CachedUICommand& command)
	{
	}
}