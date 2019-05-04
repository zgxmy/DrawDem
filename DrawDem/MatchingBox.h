#pragma once
#include"DemData.h"
class MatchingBox
{
public:
	MatchingBox();
	MatchingBox(float height,DemData* d);
	~MatchingBox();
	void CountLineSize(unsigned int i);
	unsigned int Cell(char a, char b, char c, char d);
	void AddPoint(unsigned int i, int r, int c);
	void AddLine(unsigned int i, int r, int c);
	float* lines;
	unsigned int ncols;
	unsigned int nrows;
	unsigned int lineSize;
	float height;
private:
	char** sign;
	float* linePtr;
	DemData* d;
};

