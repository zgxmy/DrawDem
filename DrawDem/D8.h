#pragma once
#include "Demdata.h"

class D8
{
public:
	int ncols;
	int nrows;
	float nodata_value;
	unsigned char** dirData;
	int** NIP;
	int** Water;
	//const char dir[10] = {0,1,2,4,8,16,32,64,128,255};
	D8(const DemData&);
	D8(const D8& d);
	~D8();
	void PrintData(int t);
	void PrintDataNIP(int t);
private:
	const float sqrt2 = 1.414f;
	void CalWater(int r, int c,D8 &d);
};

