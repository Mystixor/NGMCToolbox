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

			ImGui::Text(std::format("LANG \"{}\"", m_Root.name).c_str());

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
											ImGui::TableSetColumnIndex(0); ImGui::Text(m_Content[i].second[j].second[k].c_str());
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
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Signature");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", header.magic).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Version");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}.{}.{}.{}", header.version.v[0], header.version.v[1], header.version.v[2], header.version.v[3]).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_0C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.dat_0C).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("chunkSize");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(header.chunkSize).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("childCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.childCount).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_18");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.dat_18).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.dat_1C).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("childOffsetsOffset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.childOffsetsOffset).c_str());
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("extraDataOffset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.extraDataOffset).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_28");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.dat_28).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_2C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.dat_2C).c_str());

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderLANGHeader(LANG::Node& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANG2", 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Chunk Header");
			ImGui::TableSetColumnIndex(1); OnRenderChunkHeader(node.header);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", node.name).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_40");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_40).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_44");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_44).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_48");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_48).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_4C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_4C).c_str());

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderCTGPACKHeader(LANG::CTGPACKNode& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANGCTGPACK2", 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Header");
			ImGui::TableSetColumnIndex(1); OnRenderChunkHeader(node.header);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", node.name).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_40");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_40).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_44");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_44).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_48");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_48).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_4C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_4C).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_50");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_50).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_54");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_54).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_58");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_58).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_5C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_5C).c_str());

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderSTRPACKHeader(LANG::STRPACKNode& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANGCTGPACKSTRPACK2", 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Header");
			ImGui::TableSetColumnIndex(1); OnRenderChunkHeader(node.header);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", node.name).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_40");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_40).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_44");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_44).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_48");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_48).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_4C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_4C).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_50");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_50).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_54");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_54).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_58");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_58).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_5C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", node.dat_5C).c_str());

			ImGui::EndTable();
		}
	}
}