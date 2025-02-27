#include "stdafx.h"

#include "PreviewLANG.h"

namespace NGMC
{
	PreviewLANG::PreviewLANG(File& file)
		: BasePreview(file)
	{
	}

	void PreviewLANG::OnRender()
	{
		ImGui::PushID(this);
		{
			ImGui::Dummy({});

			ImGui::TextUnformatted(std::format("LANG \"{}\"", m_Root.name).c_str());

			if (ImGui::BeginTable("tablePreviewerLANG", 1, Preview::tableFlags))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				if (ImGui::TreeNode("Header"))
				{
					OnRenderLANGHeader(m_Root);

					ImGui::TreePop();
				}

				for (unsigned int i = 0; i < m_Root.header.childCount; i++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					if (ImGui::TreeNode(std::format("CTGPACK #{} \"{}\"", i, m_Content[i].first.name).c_str()))
					{
						if (ImGui::BeginTable("tablePreviewerLANGCTGPACK", 1, Preview::tableFlags))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);

							if (ImGui::TreeNode("Header"))
							{
								OnRenderCTGPACKHeader(m_Content[i].first);

								ImGui::TreePop();
							}

							for (unsigned int j = 0; j < m_Content[i].first.header.childCount; j++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);

								if (ImGui::TreeNode(std::format("STRPACK #{} \"{}\"", j, m_Content[i].second[j].first.name).c_str()))
								{
									if (ImGui::BeginTable("tablePreviewerLANGCTGPACKSTRPACK", 1, Preview::tableFlags))
									{
										ImGui::TableNextRow();
										ImGui::TableSetColumnIndex(0);

										if (ImGui::TreeNode("Header"))
										{
											OnRenderSTRPACKHeader(m_Content[i].second[j].first);

											ImGui::TreePop();
										}

										for (unsigned int k = 0; k < m_Content[i].second[j].first.header.childCount; k++)
										{
											ImGui::TableNextRow();
											ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(m_Content[i].second[j].second[k].c_str());
										}

										ImGui::EndTable();
									}

									ImGui::TreePop();
								}
							}

							ImGui::EndTable();
						}

						ImGui::TreePop();
					}
				}

				ImGui::EndTable();
			}
		}
		ImGui::PopID();
	}

	bool PreviewLANG::Prepare()
	{
		m_IsSetup = true;

		LoaderLANG loader(m_File);

		if (!loader.GetRoot(m_Root))
		{
			m_IsSetup = false;
		}
		else
		{
			if (!loader.ParseLANG(m_Content))
				m_IsSetup = false;
		}

		return m_IsSetup;
	}

	void PreviewLANG::OnRenderChunkHeader(Chunk::Header& header)
	{
		uintptr_t headerAddr = (uintptr_t)&header;
		if (ImGui::BeginTable(std::vformat("header##{:08X}", std::make_format_args(headerAddr)).c_str(), 2, Preview::tableFlags))
		{
			ROW_QUOTE("Signature", header.magic);
			ROW_FORMAT("Version", "{}.{}.{}.{}", header.version.v[0], header.version.v[1], header.version.v[2], header.version.v[3]);
			ROW_VALUE("dat_0C", header.dat_0C);
			ROW_VALUE("chunkSize", header.chunkSize);
			ROW_VALUE("childCount", header.childCount);
			ROW_VALUE("dat_18", header.dat_18);
			ROW_VALUE("dat_1C", header.dat_1C);
			ROW_VALUE("childOffsetsOffset", header.childOffsetsOffset);
			ROW_VALUE("extraDataOffset", header.extraDataOffset);
			ROW_VALUE("dat_28", header.dat_28);
			ROW_VALUE("dat_2C", header.dat_2C);

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderLANGHeader(LANG::Node& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANG2", 2, Preview::tableFlags))
		{
			ROW_BEGIN("Chunk Header");
			OnRenderChunkHeader(node.header);

			ROW_QUOTE("Identifier", node.name);
			ROW_VALUE("dat_40", node.dat_40);
			ROW_VALUE("dat_44", node.dat_44);
			ROW_VALUE("dat_48", node.dat_48);
			ROW_VALUE("dat_4C", node.dat_4C);

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderCTGPACKHeader(LANG::CTGPACKNode& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANGCTGPACK2", 2, Preview::tableFlags))
		{
			ROW_BEGIN("Header");
			OnRenderChunkHeader(node.header);

			ROW_QUOTE("Identifier", node.name);
			ROW_VALUE("dat_40", node.dat_40);
			ROW_VALUE("dat_44", node.dat_44);
			ROW_VALUE("dat_48", node.dat_48);
			ROW_VALUE("dat_4C", node.dat_4C);
			ROW_VALUE("dat_50", node.dat_50);
			ROW_VALUE("dat_54", node.dat_54);
			ROW_VALUE("dat_58", node.dat_58);
			ROW_VALUE("dat_5C", node.dat_5C);

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderSTRPACKHeader(LANG::STRPACKNode& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANGCTGPACKSTRPACK2", 2, Preview::tableFlags))
		{
			ROW_BEGIN("Header");
			OnRenderChunkHeader(node.header);

			ROW_QUOTE("Identifier", node.name);
			ROW_VALUE("dat_40", node.dat_40);
			ROW_VALUE("dat_44", node.dat_44);
			ROW_VALUE("dat_48", node.dat_48);
			ROW_VALUE("dat_4C", node.dat_4C);
			ROW_VALUE("dat_50", node.dat_50);
			ROW_VALUE("dat_54", node.dat_54);
			ROW_VALUE("dat_58", node.dat_58);
			ROW_VALUE("dat_5C", node.dat_5C);

			ImGui::EndTable();
		}
	}
}
