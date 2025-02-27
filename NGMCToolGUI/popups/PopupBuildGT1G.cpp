#include "stdafx.h"

#include "PopupBuildGT1G.h"

#include "loaders/LoaderDDS.h"
#include "FileManager.h"

namespace NGMC
{
	using namespace GT1G;

	extern FileManager fileManager;

	namespace BGT1G
	{
		static enum FormatEnum
		{
			RGBA8_BGRA_u8,
			RGBA8_RGBA_u8,
			CompressedRgbS3tcDxt1Ext_06,
			CompressedRgbS3tcDxt1Ext_59,
			CompressedRgbaS3tcDxt5Ext_08,
			CompressedRgbaS3tcDxt5Ext_5B,
			ColorMap_u8,
			unspecified
		};
		static std::vector<const char*> formatNames = {
			"RGBA8_BGRA_u8",
			"RGBA8_RGBA_u8",
			"CompressedRgbS3tcDxt1Ext_06",
			"CompressedRgbS3tcDxt1Ext_59",
			"CompressedRgbaS3tcDxt5Ext_08",
			"CompressedRgbaS3tcDxt5Ext_5B",
			"ColorMap_u8",
			"Select format"
		};
		static std::vector<PixelFormat> formats = {
			PixelFormat::RGBA8_BGRA_u8,
			PixelFormat::RGBA8_RGBA_u8,
			PixelFormat::CompressedRgbS3tcDxt1Ext_06,
			PixelFormat::CompressedRgbS3tcDxt1Ext_59,
			PixelFormat::CompressedRgbaS3tcDxt5Ext_08,
			PixelFormat::CompressedRgbaS3tcDxt5Ext_5B,
			PixelFormat::ColorMap_u8,
			PixelFormat::unsupportedFormat
		};

		static std::vector<const char*> textureMode = {
			"Select DDS texture from disk",
			"Select DDS texture from file list"
		};
	}

	using namespace BGT1G;

	PopupBuildGT1G::PopupBuildGT1G()
		: Popup("Build GT1G"), m_TextureCount(0U)
	{
		m_Type = PopupType::TypePopupBuildGT1G;
		m_WindowMinSize = { 500.0f, 500.0f };
		//m_WindowMinSize = { 500.0f, 420.0f };
	}

	void PopupBuildGT1G::Setup()
	{
		m_TextureCount = 0U;

		m_OutputName.reserve(256);
		m_OutputName = "output.GT1G";

		UpdateBufferSizes();

		MarkAsReady();
	}

	void PopupBuildGT1G::OnRenderContent()
	{
		static float spacing = 1.0f;

		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputInt("Texture Count", (int*)&m_TextureCount))
		{
			if ((int)m_TextureCount < 0)
				m_TextureCount = 0U;

			UpdateBufferSizes();
		}
		ImGui::PopItemWidth();

		ImGui::Dummy(ImVec2(0.0f, spacing));

		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + 3 * ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("childWindowPopupBuildGT1GTextureList", { 0.0f, -footer_height_to_reserve }, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar))
		{
			if (m_TextureCount == 0)
				ImGui::TextUnformatted("Increase count above to edit textures here.");

			for (unsigned int i = 0; i < m_TextureCount; i++)
			{
				if (ImGui::TreeNode(std::format("Texture #{}", i).c_str()))
				{
					if (ImGui::BeginTable("tablePopupBuildGT1G", 2))
					{
						ImGui::TableSetupColumn("inputs", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("buttons", ImGuiTableColumnFlags_WidthFixed, 60.0f);

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						ImGui::PushItemWidth(ImGui::GetWindowSize().x - 120.0f);

						if (ImGui::BeginCombo("##textureMode", textureMode[m_IsUsingTextureFilePaths[i] ? 0 : 1]))
						{
							if (ImGui::Selectable(textureMode[0], m_IsUsingTextureFilePaths[i]))
							{
								m_IsUsingTextureFilePaths[i] = true;
							}
							if (ImGui::Selectable(textureMode[1], !m_IsUsingTextureFilePaths[i]))
							{
								m_IsUsingTextureFilePaths[i] = false;
							}
							ImGui::EndCombo();
						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("File input:");

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						if (m_IsUsingTextureFilePaths[i])
						{
							if (ImGui::BeginTable("tableFileInputForBorders", 1, ImGuiTableFlags_BordersOuter))
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (m_TextureFilePathDisplays[i].length() == 0)
								{
									ImGui::TextUnformatted("No file selected");
								}
								else
								{
									ImGui::TextUnformatted(m_TextureFilePathDisplays[i].c_str());
								}

								ImGui::EndTable();
							}

							if (ImGui::BeginItemTooltip())
							{
								ImGui::TextUnformatted("Text field does not support UTF-16, but if you pick a file using the Browse button it will work correctly nonetheless");
								ImGui::EndTooltip();
							}

							ImGui::TableSetColumnIndex(1);

							if (ImGui::Button("Browse##ddsTextureInput"))
							{
								std::vector<std::wstring> filePaths;
								if (OpenFileDialog(filePaths, false, false))
								{
									if (filePaths.size())
									{
										m_TextureFilePaths[i] = filePaths[0];
										//m_TextureFilePathDisplays[i] = GetStringFromWString(filePaths[0].c_str()).c_str();	// messes up string due to bug in ImGui
										m_TextureFilePathDisplays[i] = GetStringFromWStringSimple(filePaths[0].c_str()).c_str();

										LoaderDDS loader(filePaths[0].c_str());

										DDS_HEADER header;
										if (loader.GetHeader(header))
										{
											m_Widths[i] = header.dwWidth;
											m_Heights[i] = header.dwHeight;
											m_MipMapCounts[i] = header.dwMipMapCount;

											switch (loader.GetPixelFormat())
											{
											case DDSFormat::BGRA8:
												m_FormatIDs[i] = FormatEnum::RGBA8_BGRA_u8;
												break;
											case DDSFormat::RGBA8:
												m_FormatIDs[i] = FormatEnum::RGBA8_RGBA_u8;
												break;
											case DDSFormat::DXT1:
												m_FormatIDs[i] = FormatEnum::CompressedRgbS3tcDxt1Ext_06;
												break;
											case DDSFormat::DXT5:
												m_FormatIDs[i] = FormatEnum::CompressedRgbaS3tcDxt5Ext_08;
												break;
											case DDSFormat::LUMINANCE:
												m_FormatIDs[i] = FormatEnum::ColorMap_u8;
												break;
											default:
												m_FormatIDs[i] = FormatEnum::unspecified;
											}
											m_Formats[i] = formats[m_FormatIDs[i]];
										}
									}
								}
							}
						}
						else
						{
							if (ImGui::BeginTable("tableFileInputForBorders", 1, ImGuiTableFlags_BordersOuter))
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (m_TextureFilePointers[i] == nullptr)
								{
									ImGui::TextUnformatted("No file selected");
								}
								else
								{
									ImGui::TextUnformatted(m_TextureFilePointers[i]->GetName());
								}
								ImGui::EndTable();
							}

							ImGui::TableSetColumnIndex(1);

							if (ImGui::Button("Browse##listTextureInput"))
							{
								m_FilePicker.Setup(nullptr, &m_TextureFilePointers[i], FileType(Databin::General::FileTypeId::DDS));
								m_FilePicker.Open();
							}
						}

						ImGui::Dummy(ImVec2(0.0f, spacing));


						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Width:");

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::InputInt("##width", (int*)&m_Widths[i], 0, 0))
						{
							if ((int)m_Widths[i] < 0)
								m_Widths[i] = 0U;
						}
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
						{
							ImGui::SetTooltip("powers of 2 only");
						}

						ImGui::Dummy(ImVec2(0.0f, spacing));


						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Height:");

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::InputInt("##height", (int*)&m_Heights[i], 0, 0))
						{
							if ((int)m_Heights[i] < 0)
								m_Heights[i] = 0U;
						}
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
						{
							ImGui::SetTooltip("powers of 2 only");
						}

						ImGui::Dummy(ImVec2(0.0f, spacing));


						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("MipMap Count:");

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::InputInt("##mipMapCount", (int*)&m_MipMapCounts[i]))
						{
							if ((int)m_MipMapCounts[i] < 1)
								m_MipMapCounts[i] = 1U;
						}

						ImGui::Dummy(ImVec2(0.0f, spacing));


						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Format:");

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						if (ImGui::BeginCombo("##format", formatNames[m_FormatIDs[i]]))
						{
							for (unsigned int j = 0; j < formatNames.size() - 1; j++)
							{
								bool isSelected = j == m_FormatIDs[i];
								if (ImGui::Selectable(formatNames[j], &isSelected))
								{
									m_FormatIDs[i] = j;
									m_Formats[i] = formats[j];
								}
							}
							ImGui::EndCombo();
						}

						ImGui::PopItemWidth();

						ImGui::EndTable();
					}

					ImGui::TreePop();

					if(i + 1 < m_TextureCount)
						ImGui::Separator();
				}
			}
		}
		ImGui::EndChild();

		ImGui::Dummy(ImVec2(0.0f, spacing));

		ImGui::PushItemWidth(ImGui::GetWindowSize().x - 97.0f);
		ImGui::TextUnformatted("File name:");
		if (ImGui::InputText("##inputFileName", (char*)m_OutputName.c_str(), 256))
		{
			m_OutputName = m_OutputName.c_str();
		}

		if (ImGui::BeginTable("tableBottomButtons", 4))
		{
			ImGui::TableSetupColumn("saveToDisk", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("saveToList", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("padding", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Cancel", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			bool isSaveToDisk = ImGui::Button("Save to disk");

			ImGui::TableSetColumnIndex(1);
			bool isSaveToFileList = ImGui::Button("Save to file list");

			ImGui::TableSetColumnIndex(3);
			if (ImGui::Button("Cancel"))
			{
				Close();
			}

			if (isSaveToDisk || isSaveToFileList)
			{
				MemoryBuffer* imageDatas = new MemoryBuffer[m_TextureCount];
				for (unsigned int i = 0; i < m_TextureCount; i++)
				{
					if (m_IsUsingTextureFilePaths[i])
					{
						LoaderDDS loader(m_TextureFilePaths[i].c_str());
						loader.GetImageData(imageDatas[i]);
					}
					else
					{
						LoaderDDS loader(m_TextureFilePointers[i]);
						loader.GetImageData(imageDatas[i]);
					}
				}

				MemoryBuffer outBuffer;
				if (BuildGT1G(
					outBuffer,
					m_TextureCount,
					m_TextureCount ? imageDatas : nullptr,
					m_TextureCount ? &m_Widths[0] : nullptr,
					m_TextureCount ? &m_Heights[0] : nullptr,
					m_TextureCount ? &m_MipMapCounts[0] : nullptr,
					m_TextureCount ? &m_Formats[0] : nullptr)
					)
				{
					if (isSaveToDisk)
					{
						std::wstring filePath;
						if (SaveFileDialog(filePath, std::wstring(m_OutputName.begin(), m_OutputName.end())))
						{
							bool isSuccess = outBuffer.SaveToFile(filePath.c_str());

							if (isSuccess)
								Close();

							Log(L"File {} saved to \"{}\".", isSuccess ? L"was" : L"could not be", filePath);
						}
					}
					else if (isSaveToFileList)
					{
						fileManager.RegisterFile(outBuffer, m_OutputName.c_str(), FileType(Databin::S1::FileTypeId::GT1G_07));

						Close();

						Log("File was saved to memory as \"{}\".", m_OutputName);
					}
				}

				delete[] imageDatas;
			}

			ImGui::EndTable();
		}
		ImGui::PopItemWidth();

		std::vector<File*> textureFilePointers = m_TextureFilePointers;
		m_FilePicker.OnRender();
		for (unsigned int i = 0; i < m_TextureCount; i++)
		{
			if (textureFilePointers[i] != m_TextureFilePointers[i])
			{
				LoaderDDS loader(m_TextureFilePointers[i]);

				DDS_HEADER header;
				if (loader.GetHeader(header))
				{
					m_Widths[i] = header.dwWidth;
					m_Heights[i] = header.dwHeight;
					m_MipMapCounts[i] = header.dwMipMapCount;

					switch (loader.GetPixelFormat())
					{
					case DDSFormat::BGRA8:
						m_FormatIDs[i] = FormatEnum::RGBA8_BGRA_u8;
						break;
					case DDSFormat::RGBA8:
						m_FormatIDs[i] = FormatEnum::RGBA8_RGBA_u8;
						break;
					case DDSFormat::DXT1:
						m_FormatIDs[i] = FormatEnum::CompressedRgbS3tcDxt1Ext_06;
						break;
					case DDSFormat::DXT5:
						m_FormatIDs[i] = FormatEnum::CompressedRgbaS3tcDxt5Ext_08;
						break;
					case DDSFormat::LUMINANCE:
						m_FormatIDs[i] = FormatEnum::ColorMap_u8;
						break;
					default:
						m_FormatIDs[i] = FormatEnum::unspecified;
					}
					m_Formats[i] = formats[m_FormatIDs[i]];
				}
			}
		}
	}

	void PopupBuildGT1G::UpdateBufferSizes()
	{
		for (unsigned int i = m_TextureFilePaths.size(); i < m_TextureCount; i++)
		{
			m_TextureFilePaths.emplace_back();
			m_TextureFilePaths.back().reserve(256);

			m_TextureFilePathDisplays.emplace_back();
			m_TextureFilePathDisplays.back().reserve(256);

			m_IsUsingTextureFilePaths.emplace_back(false);
			m_TextureFilePointers.emplace_back(nullptr);
			m_Widths.emplace_back(0);
			m_Heights.emplace_back(0);
			m_MipMapCounts.emplace_back(1);
			m_Formats.emplace_back(PixelFormat::unsupportedFormat);
			m_FormatIDs.emplace_back(formatNames.size() - 1);
		}

		m_TextureFilePaths.erase(m_TextureFilePaths.begin() + m_TextureCount, m_TextureFilePaths.end());
		m_TextureFilePaths.shrink_to_fit();

		m_TextureFilePathDisplays.erase(m_TextureFilePathDisplays.begin() + m_TextureCount, m_TextureFilePathDisplays.end());
		m_TextureFilePathDisplays.shrink_to_fit();

		m_IsUsingTextureFilePaths.erase(m_IsUsingTextureFilePaths.begin() + m_TextureCount, m_IsUsingTextureFilePaths.end());
		m_TextureFilePointers.erase(m_TextureFilePointers.begin() + m_TextureCount, m_TextureFilePointers.end());
		m_Widths.erase(m_Widths.begin() + m_TextureCount, m_Widths.end());
		m_Heights.erase(m_Heights.begin() + m_TextureCount, m_Heights.end());
		m_MipMapCounts.erase(m_MipMapCounts.begin() + m_TextureCount, m_MipMapCounts.end());
		m_Formats.erase(m_Formats.begin() + m_TextureCount, m_Formats.end());
		m_FormatIDs.erase(m_FormatIDs.begin() + m_TextureCount, m_FormatIDs.end());
	}
}
