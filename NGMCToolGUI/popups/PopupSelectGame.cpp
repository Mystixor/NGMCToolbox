#include "stdafx.h"
#include "PopupSelectGame.h"

namespace NGMC
{
	PopupSelectGame::PopupSelectGame()
		: Popup("Select game")
	{
		m_Type = PopupType::TypePopupSelectGame;
	}

	constexpr static float spacing = 2.0f;

	void PopupSelectGame::Setup(File* p_File)
	{
		if (p_File != nullptr)
		{
			SetFile(p_File);

			//m_WindowMinSize.y = 2 * ImGui::GetStyle().WindowPadding.y + 1 * ImGui::GetFrameHeight() + spacing + 3 * ImGui::GetFrameHeightWithSpacing();
			//m_WindowMaxSize.y = m_WindowMinSize.y;

			MarkAsReady();
		}
		else
		{
			Close();
		}
	}

	void PopupSelectGame::OnRenderContent()
	{
		ImGui::TextUnformatted("Please select the Game which this file belongs to:");

		if (ImGui::BeginTable("tableFileInputForBorders", 1, ImGuiTableFlags_BordersOuter))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::TextUnformatted((std::string("\"") + GetFile()->GetName() + "\"").c_str());

			ImGui::EndTable();
		}
		
		ImGui::Dummy({ 0.0f, spacing });

		if (ImGui::BeginTable("tableBottomButtons", 3))
		{
			ImGui::TableSetupColumn("S1", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("S2", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("3RE", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			GAME newGame = NON_GAME;

			if (ImGui::Button("Sigma"))
			{
				newGame = SIGMA_1;
				Close();
			}
			
			ImGui::TableSetColumnIndex(1);

			if (ImGui::Button("Sigma 2"))
			{
				newGame = SIGMA_2;
				Close();
			}
			
			ImGui::TableSetColumnIndex(2);

			if (ImGui::Button("Razor's Edge"))
			{
				newGame = RE_3;
				Close();
			}

			if (newGame != NON_GAME)
			{
				using namespace Databin;

				switch (GetFile()->GetType().GetId())
				{
				case General::databin:
				{
					switch (newGame)
					{
					case SIGMA_1:
						GetFile()->SetType(FileType(S1::FileTypeId::databin));
						break;
					case SIGMA_2:
						GetFile()->SetType(FileType(S2::FileTypeId::databin));
						break;
					case RE_3:
						GetFile()->SetType(FileType(RE::FileTypeId::databin));
						break;
					}
				}
				}
			}

			ImGui::EndTable();
		}
	}
}
