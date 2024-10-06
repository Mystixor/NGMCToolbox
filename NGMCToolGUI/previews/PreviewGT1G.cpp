#include "stdafx.h"

#include "PreviewGT1G.h"

namespace NGMC
{
	extern bool g_ReadColorMapAsLuminance;

	PreviewGT1G::PreviewGT1G(File& file)
		: BasePreviewImage(file),
		m_GT1GHeader(GT1G::Header()), m_GT1GTextures(0), m_PreviewGT1GTexIdx(0U), m_PreviewGT1GMipMapIdx(0U)
	{
	}

	void PreviewGT1G::OnRender()
	{
		using namespace GT1G;

		if (ImGui::BeginTable("tablePreviewerGT1G", 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Version");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}.{}.{}.{}", (char)m_GT1GHeader.version.major, (char)m_GT1GHeader.version.submajor, (char)m_GT1GHeader.version.minor, (char)m_GT1GHeader.version.patch).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fileSize");
			std::string prettyFileSize = GetPrettySize(m_GT1GHeader.fileSize);
			ImGui::TableSetColumnIndex(1); ImGui::Text(prettyFileSize.c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("textureOffsetTableAddress");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GHeader.textureOffsetTableAddress).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("textureCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_GT1GHeader.textureCount).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_14");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_GT1GHeader.dat_14).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_18");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_GT1GHeader.dat_18).c_str());

			ImGui::EndTable();
		}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (m_GT1GHeader.textureCount)
		{
			ImGui::Text("Preview texture");

			ImGui::PushItemWidth(100.0f);
			if (ImGui::InputInt("Texture index", (int*)&m_PreviewGT1GTexIdx))
			{
				m_PreviewGT1GTexIdx = max(0, min((int)m_PreviewGT1GTexIdx, (int32_t)m_GT1GHeader.textureCount - 1));
				m_PreviewGT1GMipMapIdx = 0;
				FreeTexture();
				PrepareGT1GTexture(m_PreviewGT1GTexIdx);
			}
			if (ImGui::InputInt("Mip map level", (int*)&m_PreviewGT1GMipMapIdx))
			{
				m_PreviewGT1GMipMapIdx = max(0, min((int)m_PreviewGT1GMipMapIdx, (int32_t)m_GT1GTextures[m_PreviewGT1GTexIdx].MipMapCount - 1));
				FreeTexture();
				PrepareGT1GTexture(m_PreviewGT1GTexIdx);
			}
			ImGui::PopItemWidth();

			if (m_Image != nullptr)
			{
				if (m_GT1GTextures[m_PreviewGT1GTexIdx].Format == PixelFormat::ColorMap_u8)
				{
					if (ImGui::Checkbox("Render Color Map as raw data (color index)", &g_ReadColorMapAsLuminance))
					{
						FreeTexture();
						PrepareGT1GTexture(m_PreviewGT1GTexIdx);
					}
				}

				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
				if (ImGui::BeginTable("tablePreviewerGT1GImage", 1, Preview::tableFlags, { (float)m_ImageDisplayWidth + 1, (float)m_ImageDisplayHeight + 1 }))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImVec2 cursorPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos({ cursorPos.x + 0, cursorPos.y + 1 });
					ImGui::Image(m_Image, ImVec2(m_ImageDisplayWidth, m_ImageDisplayHeight));
					ImGui::EndTable();
				}
				ImGui::PopStyleVar();
			}

			for (unsigned int i = 0; i < m_GT1GHeader.textureCount; i++)
			{
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::Text(std::format("Texture #{}", i).c_str());

				if (ImGui::BeginTable(std::format("tablePreviewerGT1GTexture{}", i).c_str(), 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Pixel format");
					ImGui::TableSetColumnIndex(1);

					switch (m_GT1GTextures[i].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
						ImGui::Text("BGRA"); break;
					case PixelFormat::RGBA8_RGBA_u8:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						ImGui::Text("RGBA"); break;
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						ImGui::Text("RGB"); break;
					case PixelFormat::ColorMap_u8:
						ImGui::Text("Color Map index"); break;
					default:
						ImGui::Text("unknown");
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Compression");
					ImGui::TableSetColumnIndex(1);

					switch (m_GT1GTextures[i].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
					case PixelFormat::RGBA8_RGBA_u8:
					case PixelFormat::ColorMap_u8:
						ImGui::Text(std::format("raw (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						ImGui::Text(std::format("S3TC DXT1 (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						ImGui::Text(std::format("S3TC DXT5 (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					default:
						ImGui::Text("unknown");
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Flags");
					ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].Flags).c_str());

					if ((m_GT1GTextures[i].Flags >> 24) == 0x10)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("ExtraFlags0");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].ExtraFlags0).c_str());

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("ExtraFlags1");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].ExtraFlags1).c_str());

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("ExtraFlags2");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].ExtraFlags2).c_str());
					}

					ImGui::EndTable();
				}

				if (ImGui::BeginTable(std::format("tablePreviewerGT1GTexture{}MipMaps", i).c_str(), 1 + m_GT1GTextures[i].MipMapCount, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("MipMap level");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::Text(std::format("{}", j).c_str());
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Width");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::Text(std::format("{}", m_GT1GTextures[i].MipMaps[j].Width).c_str());
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Height");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::Text(std::format("{}", m_GT1GTextures[i].MipMaps[j].Height).c_str());
					}

					ImGui::EndTable();
				}
			}
		}
		else
		{
			ImGui::Text("No textures were found in this file. This is unexpected.");
		}
	}

	bool PreviewGT1G::Prepare()
	{
		LoaderGT1G loader = LoaderGT1G(&m_File);

		{
			using namespace GT1G;

			loader.GetHeader(m_GT1GHeader);

			m_GT1GTextures.reserve(m_GT1GTextures.size() + m_GT1GHeader.textureCount);

			unsigned int* mipMapCounts = new unsigned int[m_GT1GHeader.textureCount];
			PixelFormat* formats = new PixelFormat[m_GT1GHeader.textureCount];
			unsigned int* widths = new unsigned int[m_GT1GHeader.textureCount];
			unsigned int* heights = new unsigned int[m_GT1GHeader.textureCount];
			uint32_t* flags = new uint32_t[m_GT1GHeader.textureCount];
			uint32_t* extraFlags0s = new uint32_t[m_GT1GHeader.textureCount];
			uint32_t* extraFlags1s = new uint32_t[m_GT1GHeader.textureCount];
			uint32_t* extraFlags2s = new uint32_t[m_GT1GHeader.textureCount];

			loader.GetTexturesInfo(m_GT1GHeader, mipMapCounts, formats, widths, heights, flags, extraFlags0s, extraFlags1s, extraFlags2s);

			for (unsigned int i = 0; i < m_GT1GHeader.textureCount; i++)
			{
				m_GT1GTextures.emplace_back(mipMapCounts[i], formats[i], flags[i], extraFlags0s[i], extraFlags1s[i], extraFlags2s[i], std::vector<GT1GMipMap>(mipMapCounts[i]));

				unsigned int denom = 1;
				for (unsigned int j = 0; j < mipMapCounts[i]; j++)
				{
					m_GT1GTextures[i].MipMaps[j].Width = widths[i] / denom;
					m_GT1GTextures[i].MipMaps[j].Height = heights[i] / denom;
					denom *= 2;
				}
			}

			delete[] extraFlags2s;
			delete[] extraFlags1s;
			delete[] extraFlags0s;
			delete[] flags;
			delete[] heights;
			delete[] widths;
			delete[] formats;
			delete[] mipMapCounts;

			m_IsSetup = PrepareGT1GTexture(m_PreviewGT1GTexIdx);
		}

		return m_IsSetup;
	}

	bool PreviewGT1G::PrepareGT1GTexture(unsigned int textureIndex)
	{
		bool isSuccess = false;

		LoaderGT1G loader = LoaderGT1G(&m_File);

		if (m_PreviewGT1GTexIdx < loader.GetTextureCount())
		{
			using namespace GT1G;

			unsigned int textureIndex = m_PreviewGT1GTexIdx;
			unsigned int mipLevel = m_PreviewGT1GMipMapIdx;

			MemoryBuffer imageDataBuffer;
			if (loader.GetMipData(imageDataBuffer, textureIndex, mipLevel))
			{
				DDSFormat format = DDSFormat::unsupported;

				switch (m_GT1GTextures[textureIndex].Format)
				{
				case PixelFormat::RGBA8_BGRA_u8:
					format = DDSFormat::BGRA8;
					break;
				case PixelFormat::RGBA8_RGBA_u8:
					format = DDSFormat::RGBA8;
					break;
				case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
				case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
					format = DDSFormat::DXT1;
					break;
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
					format = DDSFormat::DXT5;
					break;
				case PixelFormat::ColorMap_u8:
					if (g_ReadColorMapAsLuminance)
					{
						format = DDSFormat::LUMINANCE;
					}
					else
					{
						format = DDSFormat::BGRA8;
					}
					break;
				}

				if (format != DDSFormat::unsupported)
				{
					m_ImageDisplayWidth = m_GT1GTextures[textureIndex].MipMaps[0].Width;
					m_ImageDisplayHeight = m_GT1GTextures[textureIndex].MipMaps[0].Height;

					UploadTexture(
						(unsigned char*)imageDataBuffer.GetBaseAddress(),
						m_GT1GTextures[textureIndex].MipMaps[mipLevel].Width,
						m_GT1GTextures[textureIndex].MipMaps[mipLevel].Height,
						format
					);

					isSuccess = true;
				}
			}
		}

		return isSuccess;
	}
}