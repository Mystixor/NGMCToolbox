//#define _CRT_SECURE_NO_WARNINGS
//#pragma warning(disable : 4996)

#include <iostream>
#include <fstream>
#include <format>

#include <windows.h>

#include "zlib.h"


//	Utility

//	Returns the smaller value of a and b.
#define MIN(a, b) a < b ? a : b

//	Logs a text encoded as UTF-8 to the console.
static void Log(const std::string& text)
{
	std::cout << text << std::endl;
}

//	Logs a text encoded as UTF-8 to the console. Text will be formatted in the same way as with std::format.
template <class... _Types>
_NODISCARD static void Log(const std::format_string<_Types...> formatText, _Types&&... _Args)
{
	std::string text = std::format(formatText, _Args...);

	Log(text);
}

//	Logs a wide character text encoded as UTF-8 to the console.
static void Log(const std::wstring& text)
{
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
	char* buffer = new char[bufferSize];
	WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, bufferSize, NULL, NULL);
	Log(buffer);
	delete[] buffer;
}

//	Logs a wide character text encoded as UTF-8 to the console. Text will be formatted in the same way as with std::format.
template <class... _Types>
_NODISCARD static void Log(const std::wformat_string<_Types...> formatText, _Types&&... _Args)
{
	std::wstring text = std::format(formatText, _Args...);

	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
	char* buffer = new char[bufferSize];
	WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, bufferSize, NULL, NULL);
	Log(buffer);
	delete[] buffer;
}

//	Logs the correct usage of the unpack method to the console.
static void LogUsageUnpack()
{
	Log("Usage:\n\nNGSTool.exe unpack [path\\to\\databin\\databin] [path\\to\\output\\directory]\n");
}

//	Logs the correct usage of all available methods to the console.
static void LogUsage()
{
	LogUsageUnpack();
}


//	NG data structs

//	The databin file header struct.
struct DatabinHeader
{
	int32_t dat_00, dat_04, dat_08, dat_0C;
	uint32_t headerSize;        // 0x20 ? less than the whole header idk
	uint32_t fileDataOffset;    // relative to the headerSize
	int32_t dat_18, dat_1C;
	uint32_t fileCount;
	uint32_t fileIndicesOffset; // relative to the headerSize
	uint32_t fileCount1;
	int32_t dat_2C;
};

//	The compressed databin item header struct from NGS1.
struct DatabinItemHeader
{
	uint32_t offset;
	int32_t dat_04;
	uint32_t size;
	uint32_t sizeCompressed;
	uint32_t dat_10;
	uint16_t indexLinkedFile0, indexLinkedFile1;
	uint8_t typeLinkedFile0, typeLinkedFile1;
	uint8_t type;
	uint8_t dat_1B;
	int32_t dat_1C;
};

int wmain(int argc, wchar_t** argv)
{
	// Set console code page to UTF-8
	SetConsoleOutputCP(CP_UTF8);

	// Enable buffering to prevent VS from chopping up UTF-8 byte sequences (not needed for me?)
	//setvbuf(stdout, nullptr, _IOFBF, 1000);

	int argcNoExe = argc - 1;
	Log("\nDetected {} argument(s):\n", argcNoExe);

	for (int i = 1; i < argc; i++)
	{
		std::wstring argument = argv[i];
		if (argument.find_first_of(L' ') != std::string::npos)
		{
			argument = std::wstring(L"\"") + argument + L"\"";
		}
		Log(argument);
	}

	Log("");

	std::wstring method = argv[1];
	if (method == L"unpack")
	{
		if (argc != 4)
		{
			LogUsageUnpack();
			return 0;
		}

		std::ifstream databin(argv[2], std::ios::binary);

		if (databin)
		{
			Log(L"Beginning to unpack \"{}\"\n", argv[2]);

			std::wstring mkdirCmd = std::format(L"mkdir \"{}\" >nul 2>&1", argv[3]);
			if (_wsystem(mkdirCmd.c_str()) == -1)
			{
				Log(L"Directory \"{}\" could not be accessed. Check spelling or permissions.", argv[3]);
				return 0;
			}

			DatabinHeader header = DatabinHeader();
			databin.seekg(0, std::ios_base::beg);
			databin.read((char*)&header, sizeof(DatabinHeader));

			Log("File count:\t{}\n", header.fileCount);

			std::streampos pos = databin.tellg();

			unsigned int extractCount = 0U;
			uint32_t databinItemHeaderOffset = 0U;
			DatabinItemHeader databinItemHeader = DatabinItemHeader();
			for (unsigned int i = 0; i < header.fileCount; i++)
			{
				databin.seekg(pos, std::ios_base::beg);
				databin.seekg((std::streamoff)i * sizeof(uint32_t), std::ios_base::cur);
				databin.read((char*)&databinItemHeaderOffset, sizeof(uint32_t));

				databin.seekg((std::streamoff)header.headerSize + databinItemHeaderOffset);
				databin.read((char*)&databinItemHeader, sizeof(DatabinItemHeader));


				char* fileDataRaw = new char[databinItemHeader.sizeCompressed];
				databin.seekg((std::streamoff)header.headerSize + header.fileDataOffset + databinItemHeader.offset);
				databin.read(fileDataRaw, databinItemHeader.sizeCompressed);


				char* fileData = new char[databinItemHeader.size];

				int ret;
				//unsigned have;
				z_stream strm = z_stream();

				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				ret = inflateInit(&strm);
				if (ret != Z_OK)
					return ret;
				
				strm.avail_in = databinItemHeader.sizeCompressed;
				strm.next_in = (Bytef*)fileDataRaw;

				strm.avail_out = databinItemHeader.size;
				strm.next_out = (Bytef*)fileData;

				ret = inflate(&strm, Z_NO_FLUSH);
				//assert(ret != Z_STREAM_ERROR);
				switch (ret)
				{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR; [[fallthrough]];
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return ret;
				}

				(void)inflateEnd(&strm);

				delete[] fileDataRaw;

				char fileExtension[9] = "\x00\x00\x00\x00\x00\x00\x00\x00";
				memcpy((void*)&fileExtension[0], (void*)fileData, MIN(8, databinItemHeader.size));

				switch (databinItemHeader.type)
				{
				case 0x0000:
				{
					if (strcmp(fileExtension, "tdpack") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0001:
				{
					if (strcmp(fileExtension, "btl_dat") != 0 && strcmp(fileExtension, "stry_dat") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0003:
				{
					if (strcmp(fileExtension, "chr_dat2") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0005:
				case 0x0010:
				{
					if (strcmp(fileExtension, "TMC") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0007:
				case 0x0013:
				{
					if (strcmp(fileExtension, "GT1G0600") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0008:
				{
					if (strcmp(fileExtension, "itm_dat2") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0009:
				{
					if (strcmp(fileExtension, "MESSTR") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x000B:
				{
					if (strcmp(fileExtension, "chr_dat") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x000C:
				{
					if (strcmp(fileExtension, "rtm_dat") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x000D:
				{
					if (strcmp(fileExtension, "SND") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x000E:
				{
					if (strcmp(fileExtension, "itm_dat2") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x000F:
				{
					if (strcmp(fileExtension, "stry_dat") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0011:
				{
					if (strcmp(fileExtension, "VtxLay") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0012:
				{
					if (strcmp(fileExtension, "sprite") != 0)
					{
						memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					}
					break;
				}
				case 0x0002:
				case 0x0004:
				case 0x0006:
				case 0x000A:
				{
					memcpy((void*)&fileExtension[0], (void*)std::format("{:08X}", *(uint32_t*)fileData).c_str(), 8);
					break;
				}
				}

				std::string extension = fileExtension;
				std::wstring path = std::wstring(argv[3]) + std::format(L"\\{:05d}.", i) + std::wstring(extension.begin(), extension.end());
				std::ofstream out(path, std::ios::binary);
				out.write(fileData, databinItemHeader.size);

				delete[] fileData;

				extractCount++;

				if (extractCount % 100 == 0 && extractCount > 0)
				{
					std::cout << "Successfully extracted " << extractCount << " files" << std::endl;
				}
			}

			if (extractCount % 100 != 0 && extractCount > 0)
			{
				Log("\nSuccessfully extracted {} out of {} files and finished extraction.", extractCount, header.fileCount);
			}
		}
		else
		{
			Log(L"File \"{}\" could not be accessed. Check spelling or permissions.", argv[2]);
			return 0;
		}
	}
	else
	{
		Log(L"\nUnknown method \"{}\"!\n", method);
		LogUsage();
		return 0;
	}

	return 0;
}