//#define _CRT_SECURE_NO_WARNINGS
//#pragma warning(disable : 4996)

#include <iostream>
#include <fstream>
#include <format>
//#include <filesystem>

#include "zlib.h"

#define MIN(a, b) a < b ? a : b

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

struct FileHeader
{
	uint32_t offset;
	int32_t dat_04;
	uint32_t size;
	uint32_t sizeCompressed;
	uint32_t dat_10;
	uint16_t dat_14, dat_16, dat_18, dat_1A;
	int32_t dat_1C;
};

int main(int argc, char** argv)
{
	std::cout << "\nDetected " << argc << " argument(s):\n" << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::string argument = argv[i];
		if (argument.find_first_of(' ') != std::string::npos)
		{
			argument = "\"" + argument + "\"";
		}
		std::cout << argument << " ";
	}
	std::cout << std::endl;

	if (argc < 4)
	{
		std::cout << "\nUsage:\n\nNGSTool.exe unpack [path\\to\\databin\\databin] [path\\to\\output\\directory]" << std::endl;
		return 0;
	}

	std::string method = argv[1];
	std::cout << method << std::endl;
	if (method == "unpack")
	{
		std::ifstream databin(argv[2], std::ios::binary);
		std::cout << argv[2] << std::endl;

		if (databin)
		{
			DatabinHeader* header = new DatabinHeader;
			databin.read((char*)header, sizeof(DatabinHeader));

			std::cout << "File count:\t" << header->fileCount << std::endl;

			size_t pos = databin.tellg();

			unsigned int extractCount = 0;
			uint32_t fileHeaderOffset;
			FileHeader* fileHeader = new FileHeader;
			for (unsigned int i = 0; i < header->fileCount; i++)
			{
				databin.seekg(pos + i * sizeof(uint32_t), std::ios_base::beg);
				databin.read((char*)&fileHeaderOffset, sizeof(uint32_t));

				databin.seekg(header->headerSize + fileHeaderOffset);
				databin.read((char*)fileHeader, sizeof(FileHeader));


				char* fileDataRaw = new char[fileHeader->sizeCompressed];
				databin.seekg(header->headerSize + header->fileDataOffset + fileHeader->offset);
				databin.read(fileDataRaw, fileHeader->sizeCompressed);

				char* fileData = new char[fileHeader->size];


				int ret;
				//unsigned have;
				z_stream strm;

				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				ret = inflateInit(&strm);
				if (ret != Z_OK)
					return ret;
				
				strm.avail_in = fileHeader->sizeCompressed;
				strm.next_in = (Bytef*)fileDataRaw;

				strm.avail_out = fileHeader->size;
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
					//return ret;
				}

				char fileExtension[9] = "\x00\x00\x00\x00\x00\x00\x00\x00";
				memcpy((void*)&fileExtension[0], (void*)fileData, MIN(8, fileHeader->size));

				switch (fileHeader->dat_1A)
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

				std::ofstream out(std::string(argv[3]) + std::format("\\{:05d}_{:04X}.", i, fileHeader->dat_1A) + fileExtension, std::ios::binary);
				out.write(fileData, fileHeader->size);

				extractCount++;

				if (extractCount % 100 == 0 && extractCount > 0)
				{
					std::cout << "Successfully extracted " << extractCount << " files" << std::endl;
				}

				(void)inflateEnd(&strm);

				delete[] fileData;
				delete[] fileDataRaw;
			}

			if (extractCount % 100 != 0 && extractCount > 0)
			{
				std::cout << "\nSuccessfully extracted " << extractCount << " out of " << header->fileCount << " files and finished extraction" << std::endl;
			}

			delete fileHeader;
			delete header;
		}
	}
	else
	{
		std::cout << "\nUnknown method \"" << method << "\"!" << std::endl;
		return 0;
	}

	return 0;
}