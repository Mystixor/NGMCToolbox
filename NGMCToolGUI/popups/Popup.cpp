#include "stdafx.h"

#include "Popup.h"

namespace NGMC
{
	Popup::Popup(const char* popupName)
		: m_Type(PopupType::TypePopupBase), m_WindowMinSize(300.0f, 100.0f), m_WindowMaxSize(FLT_MAX, FLT_MAX), m_PopupName(popupName),
		m_Open(false), m_IsReady(false), m_File(nullptr)
	{
	}

	void Popup::OnRender()
	{
		if (m_IsReady && m_Open)
		{
			constexpr ImGuiWindowFlags flags =
				//ImGuiWindowFlags_AlwaysAutoResize |	// For some reason this breaks ImGuiStyleVar_WindowMinSize, but auto-resizing still works without it anyway?
				//ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoDocking |
				//ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings;

			std::string popupName = m_PopupName + std::format("##{}", (uintptr_t)this);
			ImGui::OpenPopup(popupName.c_str());

			ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, { 0.0f, 0.0f, 0.0f, 0.25f });
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(max(ImGui::CalcTextSize(m_PopupName.c_str()).x + 32.0f, m_WindowMinSize.x), m_WindowMinSize.y));
			ImGui::SetNextWindowSizeConstraints({ max(ImGui::CalcTextSize(m_PopupName.c_str()).x + 32.0f, m_WindowMinSize.x), m_WindowMinSize.y }, m_WindowMaxSize);
			if (ImGui::BeginPopupModal(popupName.c_str(), &m_Open, flags))
			{
				OnRenderContent();
				ImGui::EndPopup();
			}
			//ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}
	}

	void Popup::Open()
	{
		m_Open = true;
	}

	void Popup::Close()
	{
		m_Open = false;
	}

	bool Popup::IsSameFile(File* p_File) const
	{
		return (uintptr_t)m_File == (uintptr_t)p_File;
	}

	bool Popup::IsOpen() const
	{
		return m_Open;
	}

	PopupType Popup::GetType() const
	{
		return m_Type;
	}

	File* Popup::GetFile() const
	{
		return m_File;
	}

	void Popup::SetFile(File* p_File)
	{
		m_File = p_File;
	}

	void Popup::MarkAsReady()
	{
		m_IsReady = true;
	}

	void Popup::OnRenderContent()
	{
	}
}