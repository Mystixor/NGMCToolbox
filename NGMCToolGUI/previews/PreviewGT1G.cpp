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
			ROW_FORMAT("Version", "{}.{}.{}.{}", (char)m_GT1GHeader.version.major, (char)m_GT1GHeader.version.submajor, (char)m_GT1GHeader.version.minor, (char)m_GT1GHeader.version.patch);
			ROW_SIZE("fileSize", m_GT1GHeader.fileSize);
			ROW_FORMAT("textureOffsetTableAddress", "0x{:08X}", m_GT1GHeader.textureOffsetTableAddress);
			ROW_VALUE("textureCount", m_GT1GHeader.textureCount);
			ROW_VALUE("dat_14", m_GT1GHeader.dat_14);
			ROW_VALUE("dat_18", m_GT1GHeader.dat_18);

			ImGui::EndTable();
		}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (m_GT1GHeader.textureCount)
		{
			ImGui::TextUnformatted("Preview texture");

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
				ImGui::TextUnformatted(std::format("Texture #{}", i).c_str());

				if (ImGui::BeginTable(std::format("tablePreviewerGT1GTexture{}", i).c_str(), 2, Preview::tableFlags))
				{
					ROW_BEGIN("Pixel format");
					switch (m_GT1GTextures[i].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
						ImGui::TextUnformatted("BGRA");
						break;
					case PixelFormat::RGBA8_RGBA_u8:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						ImGui::TextUnformatted("RGBA");
						break;
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						ImGui::TextUnformatted("RGB");
						break;
					case PixelFormat::ColorMap_u8:
						ImGui::TextUnformatted("Color Map index");
						break;
					default:
						ImGui::TextUnformatted("unknown");
					}

					ROW_BEGIN("Compression");
					switch (m_GT1GTextures[i].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
					case PixelFormat::RGBA8_RGBA_u8:
					case PixelFormat::ColorMap_u8:
						ImGui::TextUnformatted(std::format("raw (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						ImGui::TextUnformatted(std::format("S3TC DXT1 (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						ImGui::TextUnformatted(std::format("S3TC DXT5 (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					default:
						ImGui::TextUnformatted("unknown");
					}

					ROW_FORMAT("Flags", "0x{:08X}", m_GT1GTextures[i].Flags);

					if ((m_GT1GTextures[i].Flags >> 24) == 0x10)
					{
						ROW_FORMAT("ExtraFlags0", "0x{:08X}", m_GT1GTextures[i].ExtraFlags0);
						ROW_FORMAT("ExtraFlags1", "0x{:08X}", m_GT1GTextures[i].ExtraFlags1);
						ROW_FORMAT("ExtraFlags2", "0x{:08X}", m_GT1GTextures[i].ExtraFlags2);
					}

					ImGui::EndTable();
				}

				if (ImGui::BeginTable(std::format("tablePreviewerGT1GTexture{}MipMaps", i).c_str(), 1 + m_GT1GTextures[i].MipMapCount, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("MipMap level");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::TextUnformatted(std::format("{}", j).c_str());
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Width");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::TextUnformatted(std::format("{}", m_GT1GTextures[i].MipMaps[j].Width).c_str());
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Height");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::TextUnformatted(std::format("{}", m_GT1GTextures[i].MipMaps[j].Height).c_str());
					}

					ImGui::EndTable();
				}
			}
		}
		else
		{
			ImGui::TextUnformatted("No textures were found in this file. This is unexpected.");
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
