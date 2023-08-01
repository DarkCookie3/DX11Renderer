#pragma once

enum class OpacityMeshGroup : int
{
	Opaque = 0b0001,
	Translucent = 0b0010
};

inline bool operator | (OpacityMeshGroup arg1, OpacityMeshGroup arg2)
{
	return static_cast<uint32_t>(arg1) | static_cast<uint32_t>(arg2);
}

inline bool operator & (OpacityMeshGroup arg1, OpacityMeshGroup arg2)
{
	return static_cast<uint32_t>(arg1) & static_cast<uint32_t>(arg2);
}

inline uint32_t GetIndexForOpacityMeshGroup(OpacityMeshGroup arg1)
{
	return static_cast<uint32_t>(arg1) / 2;
}