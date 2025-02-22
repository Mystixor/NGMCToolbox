#pragma once

#include "stdafx.h"

#include "utility/Utility.h"

namespace NGMC
{
	namespace Chunk
	{
		struct Version
		{
			uint8_t v[4];

			Version() : v({}) {}
			Version(uint32_t pv)
			{
				*(uint32_t*)v = pv;
			}

			operator uint32_t() const { return *(uint32_t*)v; }
		};

		constexpr inline uint32_t vSIGMA1 = 0x00010000;
		constexpr inline uint32_t vSIGMA2 = 0x01010000;

		struct Header
		{
			char magic[8];
			Version version;
			int32_t dat_0C;
			uint32_t chunkSize;
			int32_t childCount;
			int32_t dat_18;
			int32_t dat_1C;
			int32_t childOffsetsOffset;
			int32_t extraDataOffset;
			int32_t dat_28;
			int32_t dat_2C;
		};

		template<typename T>
		struct Node
		{
			Header header;

			T* GetChild(uint32_t index)
			{
				bool isBigEndian = header.version.v[0] == 255;

				int32_t childCount = SwapBytesIf((unsigned long)header.childCount, isBigEndian);
				if (index < childCount)
				{
					uint32_t* childOffsets = (uint32_t*)((uintptr_t)this + SwapBytesIf((unsigned long)header.childOffsetsOffset, isBigEndian));
					return (T*)((uintptr_t)this + SwapBytesIf((unsigned long)childOffsets[index], isBigEndian));
				}
				return nullptr;
			}
		};

		template<typename T>
		struct SizedChildrenNode : Node<T>
		{
			int32_t GetChildSize(uint32_t index)
			{
				bool isBigEndian = this->header.version.v[0] == 255;

				int32_t childCount = SwapBytesIf((unsigned long)this->header.childCount, isBigEndian);
				if (index < childCount)
				{
					uint32_t* sizeOffsets = (uint32_t*)((uintptr_t)this + SwapBytesIf((unsigned long)this->header.extraDataOffset, isBigEndian));
					return (int32_t)SwapBytesIf(*(unsigned long*)((uintptr_t)this + SwapBytesIf(sizeOffsets[index], isBigEndian)), isBigEndian);
				}
				return -1;
			}
		};
	}
}