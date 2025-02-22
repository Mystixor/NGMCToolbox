#include "stdafx.h"

#include "LoaderLANG.h"

#include <codecvt>
#include <filesystem>

#include "rapidjson/document.h"

#include "DataReader.h"
#include "FileManager.h"

namespace NGMC
{
	bool ImportFileFromJSON(const wchar_t* filePath)
	{
		bool isSuccess = false;

		std::ifstream stream(filePath);
		if (stream)
		{
			stream.seekg(0, std::ios::end);
			size_t jsonSize = stream.tellg();

			char* buffer = new char[jsonSize];
			stream.seekg(0, std::ios::beg);
			stream.read(buffer, jsonSize);

			stream.close();

			rapidjson::Document d;
			auto errorCode = d.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).GetParseError();

			if (errorCode == rapidjson::kParseErrorNone)
			{
				bool isBigEndian = false;
				if (d.HasMember("isBigEndian") && d["isBigEndian"].IsBool())
				{
					isBigEndian = d["isBigEndian"].GetBool();
				}

				if ((d.HasMember("CTGPACKs") && d["CTGPACKs"].IsArray()) && (d.HasMember("name") && d["name"].IsString()))
				{
					auto& CTGPACKs = d["CTGPACKs"];

					auto ctgpackBuffers = std::vector<char*>(CTGPACKs.Size());
					auto ctgpackOffsets = std::vector<uint32_t>(CTGPACKs.Size());
					auto ctgpackSizes = std::vector<uint32_t>(CTGPACKs.Size());
					uint32_t langSize = 0x60 + ((CTGPACKs.Size() + 3) / 4) * 0x20;

					int32_t ctgpackSkips = 0;

					for (int i = 0; i < CTGPACKs.Size(); i++)
					{
						auto& CTGPACK = CTGPACKs[i];

						if ((!CTGPACK.HasMember("STRPACKs") || !CTGPACK["STRPACKs"].IsArray()) || (!CTGPACK.HasMember("name") || !CTGPACK["name"].IsString()))
						{
							ctgpackSkips++;
							continue;
						}

						auto& STRPACKs = CTGPACK["STRPACKs"];

						auto strpackBuffers = std::vector<char*>(STRPACKs.Size());
						auto strpackOffsets = std::vector<uint32_t>(STRPACKs.Size());
						auto strpackSizes = std::vector<uint32_t>(STRPACKs.Size());
						uint32_t ctgpackSize = 0x60 + ((STRPACKs.Size() + 3) / 4) * 0x20;

						ctgpackOffsets[i] = langSize;

						int32_t strpackSkips = 0;

						for (int j = 0; j < STRPACKs.Size(); j++)
						{
							auto& STRPACK = STRPACKs[j];

							if ((!STRPACK.HasMember("strings") || !STRPACK["strings"].IsArray()) || (!STRPACK.HasMember("name") || !STRPACK["name"].IsString()) || (!STRPACK.HasMember("dat_50") || !STRPACK["dat_50"].IsUint()))
							{
								strpackSkips++;
								continue;
							}

							auto& strings = STRPACK["strings"];

							auto texts = std::vector<std::u16string>(strings.Size());
							auto textOffsets = std::vector<uint32_t>(strings.Size());
							auto textSizes = std::vector<uint32_t>(strings.Size());
							uint32_t strpackSize = 0x60 + ((strings.Size() + 3) / 4) * 0x20;

							strpackOffsets[j] = ctgpackSize;

							int32_t stringSkips = 0;

							for (int k = 0; k < strings.Size(); k++)
							{
								if (!strings[k].IsString())
								{
									stringSkips++;
									continue;
								}

								texts[k] = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strings[k].GetString());
								textOffsets[k] = strpackSize;
								textSizes[k] = (texts[k].size() + 1) * sizeof(char16_t);
								strpackSize += ((textSizes[k] + 0x0F) / 0x10) * 0x10;
							}

							if (stringSkips > 0)
							{
								std::wcout << L"Skipped " << stringSkips << L" string(s) due to formatting errors." << std::endl;
							}

							strpackSizes[j] = strpackSize;
							ctgpackSize += ((strpackSize + 0x0F) / 0x10) * 0x10;

							strpackBuffers[j] = new char[strpackSize];
							memset(strpackBuffers[j], 0, strpackSize);
							strcpy(strpackBuffers[j], "STRPACK");

							auto strpackNode = (LANG::STRPACKNode*)strpackBuffers[j];
							strpackNode->header.version = isBigEndian ? 0x000100FF : 0x00010000;
							strpackNode->header.dat_0C = SwapBytesIf((unsigned long)0x30, isBigEndian);
							strpackNode->header.chunkSize = SwapBytesIf((unsigned long)strpackSize, isBigEndian);
							int32_t childCount = strings.Size() - stringSkips;
							strpackNode->header.childCount = SwapBytesIf((unsigned long)childCount, isBigEndian);
							strpackNode->header.dat_18 = SwapBytesIf((unsigned long)childCount, isBigEndian);
							int32_t childOffsetsOffset = 0x60;
							strpackNode->header.childOffsetsOffset = SwapBytesIf((unsigned long)childOffsetsOffset, isBigEndian);
							int32_t extraDataOffset = childOffsetsOffset + ((strings.Size() + 3) / 4) * 0x10;
							strpackNode->header.extraDataOffset = SwapBytesIf((unsigned long)extraDataOffset, isBigEndian);

							memcpy(strpackNode->name, STRPACK["name"].GetString(), strlen(STRPACK["name"].GetString()));
							strpackNode->dat_40 = SwapBytesIf((unsigned long)0x01, isBigEndian);
							strpackNode->dat_44 = SwapBytesIf((unsigned long)0x30, isBigEndian);
							strpackNode->dat_50 = SwapBytesIf((unsigned long)STRPACK["dat_50"].GetUint(), isBigEndian);
							for (int k = 0; k < childCount; k++)
							{
								*(uint32_t*)(strpackBuffers[j] + childOffsetsOffset + k * 0x4) = SwapBytesIf((unsigned long)textOffsets[k], isBigEndian);
								*(uint32_t*)(strpackBuffers[j] + extraDataOffset + k * 0x4) = SwapBytesIf((unsigned long)textSizes[k], isBigEndian);
								SwapU16StringEndiannessIf(texts[k], isBigEndian);
								memcpy(strpackBuffers[j] + textOffsets[k], texts[k].c_str(), textSizes[k]);
							}
						}

						if (strpackSkips > 0)
						{
							std::wcout << L"Skipped " << strpackSkips << L" STRPACK(s) due to formatting errors." << std::endl;
						}

						ctgpackSizes[i] = ctgpackSize;
						langSize += ((ctgpackSize + 0x0F) / 0x10) * 0x10;

						ctgpackBuffers[i] = new char[ctgpackSize];
						memset(ctgpackBuffers[i], 0, ctgpackSize);
						strcpy(ctgpackBuffers[i], "CTGPACK");

						auto ctgpackNode = (LANG::CTGPACKNode*)ctgpackBuffers[i];
						ctgpackNode->header.version = isBigEndian ? 0x000100FF : 0x00010000;
						ctgpackNode->header.dat_0C = SwapBytesIf((unsigned long)0x30, isBigEndian);
						ctgpackNode->header.chunkSize = SwapBytesIf((unsigned long)ctgpackSize, isBigEndian);
						int32_t childCount = STRPACKs.Size() - strpackSkips;
						ctgpackNode->header.childCount = SwapBytesIf((unsigned long)childCount, isBigEndian);
						ctgpackNode->header.dat_18 = SwapBytesIf((unsigned long)childCount, isBigEndian);
						int32_t childOffsetsOffset = 0x60;
						ctgpackNode->header.childOffsetsOffset = SwapBytesIf((unsigned long)childOffsetsOffset, isBigEndian);
						int32_t extraDataOffset = childOffsetsOffset + ((STRPACKs.Size() + 3) / 4) * 0x10;
						ctgpackNode->header.extraDataOffset = SwapBytesIf((unsigned long)extraDataOffset, isBigEndian);
						memcpy(ctgpackNode->name, CTGPACK["name"].GetString(), strlen(CTGPACK["name"].GetString()));
						ctgpackNode->dat_40 = SwapBytesIf((unsigned long)0x01, isBigEndian);
						ctgpackNode->dat_44 = SwapBytesIf((unsigned long)0x30, isBigEndian);
						//ctgpackNode->dat_50 = SwapBytesIf((unsigned long)i, isBigEndian);	// why did I even add this in the first place?
						ctgpackNode->dat_5C = isBigEndian ? 0xFFFFFFFE : 0x0F;
						for (int j = 0; j < childCount; j++)
						{
							*(uint32_t*)(ctgpackBuffers[i] + childOffsetsOffset + j * 0x4) = SwapBytesIf((unsigned long)strpackOffsets[j], isBigEndian);
							*(uint32_t*)(ctgpackBuffers[i] + extraDataOffset + j * 0x4) = SwapBytesIf((unsigned long)strpackSizes[j], isBigEndian);
							memcpy(ctgpackBuffers[i] + strpackOffsets[j], strpackBuffers[j], strpackSizes[j]);
							delete[] strpackBuffers[j];
						}
					}

					if (ctgpackSkips > 0)
					{
						std::wcout << L"Skipped " << ctgpackSkips << L" CTGPACK(s) due to formatting errors." << std::endl;
					}


					MemoryBuffer langMemBuf;
					char* langBuffer = langMemBuf.PrepareBuffer(langSize);
					memset(langBuffer, 0, langSize);
					strcpy(langBuffer, "LANG");

					auto langNode = (LANG::Node*)langBuffer;
					langNode->header.version = isBigEndian ? 0x000100FF : 0x00010000;
					langNode->header.dat_0C = SwapBytesIf((unsigned long)0x30, isBigEndian);
					langNode->header.chunkSize = SwapBytesIf((unsigned long)langSize, isBigEndian);
					int32_t childCount = CTGPACKs.Size() - ctgpackSkips;
					langNode->header.childCount = SwapBytesIf((unsigned long)childCount, isBigEndian);
					langNode->header.dat_18 = SwapBytesIf((unsigned long)childCount, isBigEndian);
					int32_t childOffsetsOffset = 0x60;
					langNode->header.childOffsetsOffset = SwapBytesIf((unsigned long)childOffsetsOffset, isBigEndian);
					int32_t extraDataOffset = childOffsetsOffset + ((CTGPACKs.Size() + 3) / 4) * 0x10;
					langNode->header.extraDataOffset = SwapBytesIf((unsigned long)extraDataOffset, isBigEndian);
					memcpy(langNode->name, d["name"].GetString(), strlen(d["name"].GetString()));
					langNode->dat_40 = SwapBytesIf((unsigned long)0x01, isBigEndian);
					langNode->dat_44 = SwapBytesIf((unsigned long)0x30, isBigEndian);
					for (int i = 0; i < childCount; i++)
					{
						*(uint32_t*)(langBuffer + childOffsetsOffset + i * 0x4) = SwapBytesIf((unsigned long)ctgpackOffsets[i], isBigEndian);
						*(uint32_t*)(langBuffer + extraDataOffset + i * 0x4) = SwapBytesIf((unsigned long)ctgpackSizes[i], isBigEndian);
						memcpy(langBuffer + ctgpackOffsets[i], ctgpackBuffers[i], ctgpackSizes[i]);
						delete[] ctgpackBuffers[i];
					}

					extern FileManager fileManager;
					fileManager.RegisterFile(langMemBuf, (std::filesystem::path(filePath).filename().string() + ".LANG").c_str(), Databin::S2::FileTypeId::LANG_00);
				
					isSuccess = true;
				}
			}

			delete[] buffer;
		}

		if (!isSuccess)
			std::wcout << L"File \"" << filePath << L"\" is not a valid LANG.JSON file." << std::endl;

		return isSuccess;
	}

	LoaderLANG::LoaderLANG(const wchar_t* filePath)
		: m_MemBuf(filePath)
	{
	}

	LoaderLANG::LoaderLANG(MemoryBuffer& memBuf)
		: m_MemBuf(memBuf)
	{
	}

	LoaderLANG::LoaderLANG(File& file)
	{
		if (file.IsFileInMemory())
		{
			m_MemBuf = file.GetMemoryBuffer();
		}
		else
		{
			m_MemBuf = MemoryBuffer(file.GetFilePath());
		}
	}

	LoaderLANG::~LoaderLANG()
	{
	}

	bool LoaderLANG::GetRoot(LANG::Node& outRoot)
	{
		bool isSuccess = false;

		LANG::Node* p_Root = (LANG::Node*)m_MemBuf.GetBaseAddress();

		if (p_Root)
		{
			outRoot = *p_Root;
			isSuccess = true;
		}

		return isSuccess;
	}

	bool LoaderLANG::ParseLANG(std::vector<std::pair<LANG::CTGPACKNode, std::vector<std::pair<LANG::STRPACKNode, std::vector<std::string>>>>>& outLANG)
	{
		bool isSuccess = false;

		LANG::Node* p_Root = (LANG::Node*)m_MemBuf.GetBaseAddress();
		if (p_Root)
		{
			bool isBigEndian = p_Root->header.version.v[0] == 255;

			int32_t rootChildCount = SwapBytesIf((unsigned long)p_Root->header.childCount, isBigEndian);
			outLANG.resize(rootChildCount);
			for (int i = 0; i < rootChildCount; i++)
			{
				LANG::CTGPACKNode* p_CTGPACK = p_Root->GetChild(i);

				int32_t ctgpackChildCount = SwapBytesIf((unsigned long)p_CTGPACK->header.childCount, isBigEndian);
				outLANG[i].second.resize(ctgpackChildCount);
				for (int j = 0; j < ctgpackChildCount; j++)
				{
					LANG::STRPACKNode* p_STRPACK = p_CTGPACK->GetChild(j);

					int32_t strpackChildCount = SwapBytesIf((unsigned long)p_STRPACK->header.childCount, isBigEndian);
					outLANG[i].second[j].second.resize(strpackChildCount);
					for (int k = 0; k < strpackChildCount; k++)
					{
						std::u16string utf16le_text = p_STRPACK->GetChild(k);

						SwapU16StringEndiannessIf(utf16le_text, isBigEndian);

						std::string utf8_text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(utf16le_text);
						outLANG[i].second[j].second[k] = utf8_text;
					}

					outLANG[i].second[j].first = *p_STRPACK;
				}

				outLANG[i].first = *p_CTGPACK;
			}

			isSuccess = true;
		}

		return isSuccess;
	}
}