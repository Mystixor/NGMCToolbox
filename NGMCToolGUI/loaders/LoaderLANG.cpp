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


			auto& CTGPACKs = d["CTGPACKs"];

			auto ctgpackBuffers = std::vector<char*>(CTGPACKs.Size());
			auto ctgpackOffsets = std::vector<uint32_t>(CTGPACKs.Size());
			auto ctgpackSizes = std::vector<uint32_t>(CTGPACKs.Size());
			uint32_t langSize = 0x60 + ((CTGPACKs.Size() + 3) / 4) * 0x20;
			for (int i = 0; i < CTGPACKs.Size(); i++)
			{
				auto& CTGPACK = CTGPACKs[i];

				auto& STRPACKs = CTGPACK["STRPACKs"];

				auto strpackBuffers = std::vector<char*>(STRPACKs.Size());
				auto strpackOffsets = std::vector<uint32_t>(STRPACKs.Size());
				auto strpackSizes = std::vector<uint32_t>(STRPACKs.Size());
				uint32_t ctgpackSize = 0x60 + ((STRPACKs.Size() + 3) / 4) * 0x20;

				ctgpackOffsets[i] = langSize;

				for (int j = 0; j < STRPACKs.Size(); j++)
				{
					auto& STRPACK = STRPACKs[j];

					auto& strings = STRPACK["strings"];

					auto texts = std::vector<std::u16string>(strings.Size());
					auto textOffsets = std::vector<uint32_t>(strings.Size());
					auto textSizes = std::vector<uint32_t>(strings.Size());
					uint32_t strpackSize = 0x60 + ((strings.Size() + 3) / 4) * 0x20;

					strpackOffsets[j] = ctgpackSize;

					for (int k = 0; k < strings.Size(); k++)
					{
						texts[k] = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strings[k].GetString());
						textOffsets[k] = strpackSize;
						textSizes[k] = (texts[k].size() + 1) * sizeof(char16_t);
						strpackSize += ((textSizes[k] + 0x0F) / 0x10) * 0x10;
					}
					strpackSizes[j] = strpackSize;
					ctgpackSize += ((strpackSize + 0x0F) / 0x10) * 0x10;

					strpackBuffers[j] = new char[strpackSize];
					memset(strpackBuffers[j], 0, strpackSize);
					strcpy(strpackBuffers[j], "STRPACK");

					auto strpackNode = (LANG::STRPACKNode*)strpackBuffers[j];
					strpackNode->header.version = 0x00010000;
					strpackNode->header.dat_0C = 0x30;
					strpackNode->header.chunkSize = strpackSize;
					strpackNode->header.childCount = strings.Size();
					strpackNode->header.dat_18 = strings.Size();
					strpackNode->header.childOffsetsOffset = 0x60;
					strpackNode->header.extraDataOffset = strpackNode->header.childOffsetsOffset + ((strings.Size() + 3) / 4) * 0x10;
					memcpy(strpackNode->name, STRPACK["name"].GetString(), strlen(STRPACK["name"].GetString()));
					strpackNode->dat_40 = 0x01;
					strpackNode->dat_44 = 0x30;
					strpackNode->dat_50 = STRPACK["dat_50"].GetUint();
					for (int k = 0; k < strings.Size(); k++)
					{
						*(uint32_t*)(strpackBuffers[j] + strpackNode->header.childOffsetsOffset + k * 0x4) = textOffsets[k];
						*(uint32_t*)(strpackBuffers[j] + strpackNode->header.extraDataOffset + k * 0x4) = textSizes[k];
						memcpy(strpackBuffers[j] + textOffsets[k], texts[k].c_str(), textSizes[k]);
					}
				}
				ctgpackSizes[i] = ctgpackSize;
				langSize += ((ctgpackSize + 0x0F) / 0x10) * 0x10;

				ctgpackBuffers[i] = new char[ctgpackSize];
				memset(ctgpackBuffers[i], 0, ctgpackSize);
				strcpy(ctgpackBuffers[i], "CTGPACK");

				auto ctgpackNode = (LANG::CTGPACKNode*)ctgpackBuffers[i];
				ctgpackNode->header.version = 0x00010000;
				ctgpackNode->header.dat_0C = 0x30;
				ctgpackNode->header.chunkSize = ctgpackSize;
				ctgpackNode->header.childCount = STRPACKs.Size();
				ctgpackNode->header.dat_18 = STRPACKs.Size();
				ctgpackNode->header.childOffsetsOffset = 0x60;
				ctgpackNode->header.extraDataOffset = ctgpackNode->header.childOffsetsOffset + ((STRPACKs.Size() + 3) / 4) * 0x10;
				memcpy(ctgpackNode->name, CTGPACK["name"].GetString(), strlen(CTGPACK["name"].GetString()));
				ctgpackNode->dat_40 = 0x01;
				ctgpackNode->dat_44 = 0x30;
				ctgpackNode->dat_50 = i;
				ctgpackNode->dat_5C = 0x0F;
				for (int j = 0; j < STRPACKs.Size(); j++)
				{
					*(uint32_t*)(ctgpackBuffers[i] + ctgpackNode->header.childOffsetsOffset + j * 0x4) = strpackOffsets[j];
					*(uint32_t*)(ctgpackBuffers[i] + ctgpackNode->header.extraDataOffset + j * 0x4) = strpackSizes[j];
					memcpy(ctgpackBuffers[i] + strpackOffsets[j], strpackBuffers[j], strpackSizes[j]);
					delete[] strpackBuffers[j];
				}
			}

			MemoryBuffer langMemBuf;
			char* langBuffer = langMemBuf.PrepareBuffer(langSize);
			memset(langBuffer, 0, langSize);
			strcpy(langBuffer, "LANG");

			auto langNode = (LANG::Node*)langBuffer;
			langNode->header.version = 0x00010000;
			langNode->header.dat_0C = 0x30;
			langNode->header.chunkSize = langSize;
			langNode->header.childCount = CTGPACKs.Size();
			langNode->header.dat_18 = CTGPACKs.Size();
			langNode->header.childOffsetsOffset = 0x60;
			langNode->header.extraDataOffset = langNode->header.childOffsetsOffset + ((CTGPACKs.Size() + 3) / 4) * 0x10;
			memcpy(langNode->name, d["name"].GetString(), strlen(d["name"].GetString()));
			langNode->dat_40 = 0x01;
			langNode->dat_44 = 0x30;
			for (int i = 0; i < CTGPACKs.Size(); i++)
			{
				*(uint32_t*)(langBuffer + langNode->header.childOffsetsOffset + i * 0x4) = ctgpackOffsets[i];
				*(uint32_t*)(langBuffer + langNode->header.extraDataOffset + i * 0x4) = ctgpackSizes[i];
				memcpy(langBuffer + ctgpackOffsets[i], ctgpackBuffers[i], ctgpackSizes[i]);
				delete[] ctgpackBuffers[i];
			}

			extern FileManager fileManager;
			fileManager.RegisterFile(langMemBuf, (std::filesystem::path(filePath).filename().string() + ".LANG").c_str(), Databin::S2::FileTypeId::LANG_00);

			delete[] buffer;
		}

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
			outLANG.resize(p_Root->header.childCount);
			for (int i = 0; i < p_Root->header.childCount; i++)
			{
				LANG::CTGPACKNode* p_CTGPACK = p_Root->GetChild(i);

				outLANG[i].second.resize(p_CTGPACK->header.childCount);
				for (int j = 0; j < p_CTGPACK->header.childCount; j++)
				{
					LANG::STRPACKNode* p_STRPACK = p_CTGPACK->GetChild(j);

					outLANG[i].second[j].second.resize(p_STRPACK->header.childCount);
					for (int k = 0; k < p_STRPACK->header.childCount; k++)
					{
						std::u16string utf16le_text = p_STRPACK->GetChild(k);
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