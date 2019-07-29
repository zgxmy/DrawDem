#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
using namespace std;
class DemData
{

public:
	int ncols;
	int nrows;
	float xllcorner;
	float yllcorner;
	float cellsize;
	float nodata_value;
	float** data;

	float averHeight;
	float minHeight;
	float maxHeight;
	float deltaHeight;
	DemData(string s);
	DemData(const DemData& );
	static void Smooth(DemData &);
	~DemData();
};

