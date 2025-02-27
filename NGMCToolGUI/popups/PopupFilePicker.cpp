#include "stdafx.h"

#include "PopupFilePicker.h"

#include "FileManager.h"

namespace NGMC
{
	extern FileManager fileManager;

	PopupFilePicker::PopupFilePicker()
		: Popup("File picker"),
		m_RootFile(nullptr), m_OutFile(nullptr), m_AcceptType(FileType()),
		m_SelectedFile(nullptr),
		m_IsStepping(true), m_ListItemCount(0)
	{
		m_Type = PopupType::TypePopupFilePicker;
		m_WindowMinSize = { 300.0f, 500.0f };
	}

	void PopupFilePicker::Setup(File* p_RootFile, File** p_OutFile, FileType acceptType)
	{
		m_RootFile = p_RootFile;
		m_OutFile = p_OutFile;
		m_AcceptType = acceptType;

		MarkAsReady();
	}

	void PopupFilePicker::OnRenderFile(File* p_File, bool isNotClipped)
	{
		unsigned int childCount = p_File->GetChildCount();

		//	Never clip nodes that have children so that its children do not vanish when their parent is no longer on-screen
		if (!isNotClipped && childCount > 0)
		{
			isNotClipped = true;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y));
		}

		bool opened = false;
		if (isNotClipped)
		{
			//	Get display name and unique identifier for ImGui
			const char* name = p_File->GetName();
			std::string identifier = std::string(name) + std::format("##{:X}", (uintptr_t)p_File);

			bool isSelected = p_File == m_SelectedFile;

			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_NavLeftJumpsBackHere;

			if (childCount == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (isSelected)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}


			opened = ImGui::TreeNodeEx(identifier.c_str(), flags);


			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					//	Single left-click selects the file
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						//	Holding Ctrl keeps previous selection and toggles selection of this file
						if (isSelected)
							m_SelectedFile = nullptr;
						else
							m_SelectedFile = p_File;
					}
					else
						m_SelectedFile = p_File;
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					//	Double left-click selects and loads the file
					m_SelectedFile = p_File;
					p_File->Load();
					childCount = p_File->GetChildCount();
				}
			}


			if (m_IsStepping && m_ListItemCount + 1 == m_Clipper.DisplayEnd)
			{
				m_IsStepping = m_Clipper.Step();
			}
		}

		m_ListItemCount++;

		if (opened)
		{
			for (unsigned int i = 0; i < childCount; i++)
			{
				OnRenderFile(p_File->GetChild(i), m_Clipper.DisplayStart - 1 <= m_ListItemCount && m_ListItemCount < m_Clipper.DisplayEnd + 1);
			}

			if (isNotClipped)
				ImGui::TreePop();
		}
	}

	void PopupFilePicker::OnRenderContent()
	{
		const float footer_height_to_reserve = 2.0f * ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if(ImGui::BeginChild("childWindowFilePicker", { 0.0f, -footer_height_to_reserve }))
		{
			m_Clipper.Begin(m_ListItemCount);
			m_ListItemCount = 0;
			m_IsStepping = m_Clipper.Step();

			if (m_RootFile == nullptr)
			{
				for (unsigned int i = 0; i < fileManager.GetFileCount(); i++)
				{
					OnRenderFile(fileManager.GetFile(i), m_Clipper.DisplayStart - 1 <= m_ListItemCount && m_ListItemCount < m_Clipper.DisplayEnd + 1);
				}
			}
			else
			{
				OnRenderFile(m_RootFile, m_Clipper.DisplayStart - 1 <= m_ListItemCount && m_ListItemCount < m_Clipper.DisplayEnd + 1);
			}

			m_Clipper.End();
		}
		ImGui::EndChild();

		ImGui::Separator();

		if (ImGui::BeginTable("tableBottomButtons", 3))
		{
			ImGui::TableSetupColumn("Accept", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("padding", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Cancel", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (m_SelectedFile != nullptr && (m_AcceptType.IsUnknown() || m_SelectedFile->GetType() == m_AcceptType))
			{
				if (ImGui::Button("Accept") || ImGui::IsKeyPressed(ImGuiKey_Enter, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false))
				{
					if (m_SelectedFile != nullptr)
					{
						*m_OutFile = m_SelectedFile;
						Close();
					}
				}
			}
			else
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
				ImGui::TextUnformatted(std::format("Select a file of type \"{}\"", m_AcceptType.GetTypeName()).c_str());
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
