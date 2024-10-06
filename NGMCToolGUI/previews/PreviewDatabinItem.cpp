#include "stdafx.h"

#include "PreviewDatabinItem.h"

namespace NGMC
{
	PreviewDatabinItem::PreviewDatabinItem(File& file)
		: BasePreview(file),
		m_DatabinItemHeaderS1(Databin::S1::ItemHeader()),
		m_DatabinItemHeaderS2(Databin::S2::ItemHeader())
	{
	}

	void PreviewDatabinItem::OnRender()
	{
		if (ImGui::BeginTable("tablePreviewerDatabinItem", 2, Preview::tableFlags))
		{
			GAME game = m_File.GetType().GetGame();

			uint32_t offset = 0U;
			int32_t dat_04 = 0;
			uint32_t size = 0U;
			uint32_t sizeCompressed = 0U;
			uint32_t dat_10 = 0U;
			switch (game)
			{
			case SIGMA_1:
			{
				offset = m_DatabinItemHeaderS1.offset;
				dat_04 = m_DatabinItemHeaderS1.dat_04;
				size = m_DatabinItemHeaderS1.size;
				sizeCompressed = m_DatabinItemHeaderS1.sizeCompressed;
				dat_10 = m_DatabinItemHeaderS1.dat_10;
				break;
			}
			case SIGMA_2:
			{
				offset = m_DatabinItemHeaderS2.offset;
				dat_04 = m_DatabinItemHeaderS2.dat_04;
				size = m_DatabinItemHeaderS2.size;
				sizeCompressed = m_DatabinItemHeaderS2.sizeCompressed;
				dat_10 = m_DatabinItemHeaderS2.dat_10;
				break;
			}
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("offset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", offset).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_04");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", dat_04).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("size");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(size).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("sizeCompressed");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(sizeCompressed).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_10");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", dat_10).c_str());

			switch (game)
			{
			case SIGMA_1:
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("indexLinkedFile0");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.indexLinkedFile0).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("indexLinkedFile1");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.indexLinkedFile1).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("typeLinkedFile0");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS1.typeLinkedFile0, GetFileExtension((Databin::S1::FileTypeId)m_DatabinItemHeaderS1.typeLinkedFile0)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("typeLinkedFile1");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS1.typeLinkedFile1, GetFileExtension((Databin::S1::FileTypeId)m_DatabinItemHeaderS1.typeLinkedFile1)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("type");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS1.type, GetFileExtension((Databin::S1::FileTypeId)m_DatabinItemHeaderS1.type)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1B");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.dat_1B).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1C");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.dat_1C).c_str());

				break;
			}
			case SIGMA_2:
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("indexLinkedFile");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS2.indexLinkedFile).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("typeLinkedFile");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS2.typeLinkedFile, GetFileExtension((Databin::S2::FileTypeId)m_DatabinItemHeaderS2.typeLinkedFile)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("type");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS2.type, GetFileExtension((Databin::S2::FileTypeId)m_DatabinItemHeaderS2.type)).c_str());

				break;
			}
			}

			ImGui::EndTable();
		}
	}

	bool PreviewDatabinItem::Prepare()
	{
		GAME game = m_File.GetType().GetGame();
		LoaderDatabin loader = LoaderDatabin(game, m_File);

		switch (game)
		{
		case SIGMA_1:
			m_IsSetup = loader.LoadItemHeader(m_DatabinItemHeaderS1, m_File.GetIndexInParent());
			break;
		case SIGMA_2:
			m_IsSetup = loader.LoadItemHeader(m_DatabinItemHeaderS2, m_File.GetIndexInParent());
			break;
		}

		return m_IsSetup;
	}
}