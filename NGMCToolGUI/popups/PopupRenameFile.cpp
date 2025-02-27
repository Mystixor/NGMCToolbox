#include "stdafx.h"
#include "PopupRenameFile.h"

namespace NGMC
{
	PopupRenameFile::PopupRenameFile()
		: Popup("Rename file")
	{
		m_Type = PopupType::TypePopupRenameFile;
	}

	constexpr static float spacing = 2.0f;

	void PopupRenameFile::Setup(File* p_File)
	{
		if (p_File != nullptr)
		{
			m_NewName.reserve(128);
			m_NewName = p_File->GetName();
			SetFile(p_File);

			m_WindowMinSize = { 300.0f, 2 * ImGui::GetStyle().WindowPadding.y + 1 * ImGui::GetFrameHeight() + spacing + 3 * ImGui::GetFrameHeightWithSpacing()};
			m_WindowMaxSize.y = m_WindowMinSize.y;

			MarkAsReady();
		}
		else
		{
			Close();
		}
	}

	void PopupRenameFile::OnRenderContent()
	{
		ImGui::TextUnformatted("File name:");
		
		ImGui::PushItemWidth(ImGui::GetWindowSize().x - 2 * ImGui::GetStyle().WindowPadding.x);
		if (ImGui::InputText("##newName", (char*)m_NewName.c_str(), 128))
		{
			m_NewName = m_NewName.c_str();
		}
		ImGui::PopItemWidth();

		ImGui::Dummy({ 0.0f, spacing });

		if (ImGui::BeginTable("tableBottomButtons", 3))
		{
			ImGui::TableSetupColumn("Rename", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("padding", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Cancel", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			if (ImGui::Button("Rename"))
			{
				GetFile()->SetName(m_NewName.c_str());
				Close();
			}

			ImGui::TableSetColumnIndex(2);
			if (ImGui::Button("Cancel"))
			{
				Close();
			}

			ImGui::EndTable();
		}
	}
}
