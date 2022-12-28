#pragma once

#include <cstdint>

#include "yarnbox/ue/common.hpp"

namespace ue {

struct FArchive
{
	struct VTable
	{
		yarn::UnknownFunction scalarDeletingDestructor;
		void(__thiscall* Serialize)(FArchive* self, void* data, int64_t storageSize);
		yarn::UnknownFunction SerializeBits;
		yarn::UnknownFunction SerializeInt;
		yarn::UnknownFunction unknown1;
		yarn::UnknownFunction unknown2;
		yarn::UnknownFunction operator_leftShift1;
		yarn::UnknownFunction operator_leftShift2;
		yarn::UnknownFunction GetArchiveName;
		yarn::UnknownFunction unknown3;
		yarn::UnknownFunction Tell1;
		yarn::UnknownFunction Tell2;
		yarn::UnknownFunction AtEnd;
		yarn::UnknownFunction unknown4;
		yarn::UnknownFunction unknown5;
		yarn::UnknownFunction unknown6;
		yarn::UnknownFunction Precache;
		yarn::UnknownFunction unknown7;
		yarn::UnknownFunction unknown8;
		yarn::UnknownFunction unknown9;
		yarn::UnknownFunction Close;
		yarn::UnknownFunction GetError;
		yarn::UnknownFunction unknown10;
		yarn::UnknownFunction unknown11;
		yarn::UnknownFunction unknown12;
		yarn::UnknownFunction IsCloseComplete;
		yarn::UnknownFunction IsFilterEditorOnly;
		yarn::UnknownFunction unknown13;
	}* vtable;

	int32_t version;
	char unknown[64];
	bool dataOnly;
};

}