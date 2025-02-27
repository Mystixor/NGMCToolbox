#include "stdafx.h"

#include "Previewer.h"

namespace NGMC
{
	extern bool g_ReadColorMapAsLuminance;


	Previewer::Previewer()
		: m_Preview(nullptr),
		m_File(nullptr), m_IsSetup(false)
	{
	}

	Previewer::~Previewer()
	{
		this->SetFile(nullptr);
	}

	void Previewer::SetFile(File* p_File)
	{
		if (m_File != p_File)
		{
			if (m_IsSetup)
			{
				m_IsSetup = false;

				delete m_Preview;
				m_Preview = nullptr;
			}

			m_File = p_File;
		}
	}

	void Previewer::OnRender()
	{
		if (m_File != nullptr)
		{
			ImGui::TextUnformatted("GENERAL");

			FileType type = m_File->GetType();

			std::string typeName = type.GetTypeName();

			std::string gameName = "";
			switch (m_File->GetType().GetGame())
			{
			case NON_GAME:
				gameName = "General";
				break;
			case SIGMA_1:
				gameName = "NGS1";
				break;
			case SIGMA_2:
				gameName = "NGS2";
				break;
			case RE_3:
				gameName = "NG3RE";
				break;
			}

			if (ImGui::BeginTable("tablePreviewerGeneral", 2, Preview::tableFlags))
			{
				ROW_CSTRING("Name", m_File->GetName());
				ROW_PRINTF("Type", "%s > %s", gameName.c_str(), typeName.c_str());
				ROW_CSTRING("Is file in memory", m_File->IsFileInMemory() ? "true" : "false");

				if (!m_File->IsFileInMemory())
				{
					std::wstring filePath = m_File->GetFilePath();
					ROW_STRING("Path", std::string(filePath.begin(), filePath.end()));
				}
				
				ROW_CSTRING("Is file loaded", m_File->IsLoaded() ? "true" : "false");
				ROW_SIZE("Size", m_File->GetSize());
				ROW_VALUE("Index in Parent", m_File->GetIndexInParent());
				ROW_VALUE("Child count", m_File->GetChildCount());

				ImGui::EndTable();
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Text("FORMAT SPECIFIC (%s > %s)", gameName.c_str(), typeName.c_str());

			if (!m_IsSetup)
			{
				this->Prepare();
			}

			if (m_IsSetup)
			{
				m_Preview->OnRender();
			}
			else
			{
				if (type.IsUnknown())
					ImGui::TextUnformatted("Format unknown. Right-click the file and set a type manually.");
				else
					ImGui::TextUnformatted("This format is not yet supported for previewing.");
			}
		}
		else
		{
			ImGui::TextUnformatted("Select an element in the content viewer to see a preview of its data here.");
		}
	}

	bool Previewer::Prepare()
	{
		if (m_File != nullptr)
		{
			using namespace Databin;

			FileType type = m_File->GetType();
			switch (type.GetGame())
			{
			case NON_GAME:
			{
				using namespace General;
				switch (type.GetId())
				{
				case FileTypeId::DDS:
				{
					m_Preview = new PreviewDDS(*m_File);
					break;
				}
				}
				break;
			}
			case SIGMA_1:
			{
				using namespace S1;
				switch (type.GetId())
				{
				case FileTypeId::databin:
				{
					m_Preview = new PreviewDatabin(*m_File);
					break;
				}
				case FileTypeId::databinItem:
				{
					m_Preview = new PreviewDatabinItem(*m_File);
					break;
				}
				case FileTypeId::GT1G_07:
				case FileTypeId::GT1G_13:
				{
					m_Preview = new PreviewGT1G(*m_File);
					break;
				}
				case FileTypeId::TMC_05:
				case FileTypeId::TMC_10:
				{
					m_Preview = new PreviewTMC(*m_File);
					break;
				}
				}
				break;
			}
			case SIGMA_2:
			{
				using namespace S2;
				switch (type.GetId())
				{
				case FileTypeId::databin:
				{
					m_Preview = new PreviewDatabin(*m_File);
					break;
				}
				case FileTypeId::databinItem:
				{
					m_Preview = new PreviewDatabinItem(*m_File);
					break;
				}
				case FileTypeId::LANG_00:
				{
					m_Preview = new PreviewLANG(*m_File);
					break;
				}
				case FileTypeId::TMC_08:
				case FileTypeId::TMC_0B:
				{
					m_Preview = new PreviewTMC(*m_File);
					break;
				}
				}
				break;
			}
			case RE_3:
			{
				using namespace RE;
				switch (type.GetId())
				{
				case FileTypeId::databin:
				{
					m_Preview = new PreviewDatabin(*m_File);
					break;
				}
				case FileTypeId::databinItem:
				{
					m_Preview = new PreviewDatabinItem(*m_File);
					break;
				}
				}
				break;
			}
			}

			if (m_Preview != nullptr)
				m_IsSetup = m_Preview->Prepare();
		}

		return m_IsSetup;
	}
}
