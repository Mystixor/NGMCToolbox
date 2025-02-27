#include "stdafx.h"

#include "PreviewDatabinItem.h"

namespace NGMC
{
	PreviewDatabinItem::PreviewDatabinItem(File& file)
		: BasePreview(file),
		m_DatabinItemHeaderS1(Databin::S1::ItemHeader()),
		m_DatabinItemHeaderS2(Databin::S2::ItemHeader()),
		m_DatabinItemHeaderRE(Databin::RE::ItemHeader())
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
			case RE_3:
			{
				offset = m_DatabinItemHeaderRE.offset;
				dat_04 = m_DatabinItemHeaderRE.dat_04;
				size = m_DatabinItemHeaderRE.size;
				sizeCompressed = m_DatabinItemHeaderRE.sizeCompressed;
				dat_10 = m_DatabinItemHeaderRE.dat_10;
				break;
			}
			}

			ROW_FORMAT("offset", "0x{:08X}", offset);
			ROW_VALUE("dat_04", dat_04);
			ROW_SIZE("size", size);
			ROW_SIZE("sizeCompressed", sizeCompressed);
			ROW_VALUE("dat_10", dat_10);


			switch (game)
			{
			case SIGMA_1:
			{
				ROW_VALUE("indexLinkedFile0", m_DatabinItemHeaderS1.indexLinkedFile0);
				ROW_VALUE("indexLinkedFile1", m_DatabinItemHeaderS1.indexLinkedFile1);
				ROW_FORMAT("typeLinkedFile0", "0x{:02X} ({})", m_DatabinItemHeaderS1.typeLinkedFile0, GetFileExtension((Databin::S1::FileTypeId)m_DatabinItemHeaderS1.typeLinkedFile0));
				ROW_FORMAT("typeLinkedFile1", "0x{:02X} ({})", m_DatabinItemHeaderS1.typeLinkedFile1, GetFileExtension((Databin::S1::FileTypeId)m_DatabinItemHeaderS1.typeLinkedFile1));
				ROW_FORMAT("type", "0x{:02X} ({})", m_DatabinItemHeaderS1.type, GetFileExtension((Databin::S1::FileTypeId)m_DatabinItemHeaderS1.type));
				ROW_VALUE("dat_1B", m_DatabinItemHeaderS1.dat_1B);
				ROW_VALUE("dat_1C", m_DatabinItemHeaderS1.dat_1C);
				break;
			}
			case SIGMA_2:
			{
				ROW_VALUE("indexLinkedFile", m_DatabinItemHeaderS2.indexLinkedFile);
				ROW_FORMAT("typeLinkedFile", "0x{:02X} ({})", m_DatabinItemHeaderS2.typeLinkedFile, GetFileExtension((Databin::S2::FileTypeId)m_DatabinItemHeaderS2.typeLinkedFile));
				ROW_FORMAT("type", "0x{:02X} ({})", m_DatabinItemHeaderS2.type, GetFileExtension((Databin::S2::FileTypeId)m_DatabinItemHeaderS2.type));
				break;
			}
			case RE_3:
			{
				ROW_FORMAT("type", "0x{:08X} ({})", m_DatabinItemHeaderRE.type, GetFileExtension((Databin::RE::FileTypeId)m_DatabinItemHeaderRE.type));
				ROW_VALUE("indexLinkedFile", m_DatabinItemHeaderRE.indexLinkedFile);
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
		case RE_3:
			m_IsSetup = loader.LoadItemHeader(m_DatabinItemHeaderRE, m_File.GetIndexInParent());
			break;
		}

		return m_IsSetup;
	}
}
