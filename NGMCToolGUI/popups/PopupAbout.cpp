#include "stdafx.h"
#include "PopupAbout.h"

extern uint32_t g_ApplicationVersion;

namespace NGMC
{
	static constexpr const char* appTitle = "NINJA GAIDEN MASTER COLLECTION TOOLBOX";
	static constexpr const char* longDesc = "A toolbox with the aim to aid the modding of as many file formats from the Ninja Gaiden Master Collection as possible.";

	PopupAbout::PopupAbout()
		: Popup("About")
	{
		m_Type = PopupType::TypePopupAbout;
	}

	void PopupAbout::Setup()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		m_WindowMinSize.x = 2 * style.WindowPadding.x + ImGui::CalcTextSize(longDesc).x;
		m_WindowMaxSize.x = m_WindowMinSize.x;

		MarkAsReady();
	}

	void PopupAbout::OnRenderContent()
	{
		ImGui::Dummy({ 0.0f, 0.0f });
		ImGui::SetCursorPosX((ImGui::CalcTextSize(longDesc).x - ImGui::CalcTextSize(appTitle).x) / 2);
		ImGui::TextUnformatted(appTitle);
		ImGui::Dummy({ 0.0f, 0.0f });
		ImGui::TextUnformatted(longDesc);
		ImGui::Dummy({ 0.0f, 0.0f });

		ImGui::Separator();

		ImGui::Dummy({ 0.0f, 0.0f });

		if (ImGui::BeginTable("tableAbout", 2))
		{
			ROW_FORMAT("Version", "{:X}.{:X}.{:X}", (g_ApplicationVersion >> 16) & 0xFF, (g_ApplicationVersion >> 8) & 0xFF, g_ApplicationVersion & 0xFF);
			ROW_CSTRING("Author", "Mystixor");
			ROW_CSTRING("Contributors", "");

			ImGui::EndTable();
		}

		if (ImGui::BeginTable("tableBottomButtons", 2))
		{
			ImGui::TableSetupColumn("padding", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);
			if (ImGui::Button("Close"))
			{
				Close();
			}

			ImGui::EndTable();
		}
	}
}
