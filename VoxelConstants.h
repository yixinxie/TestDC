#pragma once
class VoxelConstants{
public:
	static const int UsableRangeShift = 1; // # of bit shift
	static const int UsableRange = 1 << UsableRangeShift;
	// for array based chunk storage, we set DataRange = UsableRange + 2, TraverseRange = UsableRage + 1
	// but for octree based storage, we set DataRange = UsableRange + 1, TraverseRange = UsableRange
	//static const int TraverseRange = UsableRange;
	static const int DataRange = UsableRange + 1;
};