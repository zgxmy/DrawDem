#include "DemData.h"

DemData::DemData(string dataPath)
{
	float sum = 0;
	ifstream infile;
	infile.open(dataPath);
	assert(infile.is_open());
	string s;
	getline(infile, s);
	this->ncols = atoi(s.substr(13).c_str());
	getline(infile, s);
	this->nrows = atoi(s.substr(14).c_str());
	getline(infile, s);
	this->xllcorner = atof(s.substr(14).c_str());
	getline(infile, s);
	this->yllcorner = atof(s.substr(14).c_str());
	getline(infile, s);
	this->cellsize = atof(s.substr(14).c_str());
	getline(infile, s);
	this->nodata_value = atof(s.substr(14).c_str());
	this->data = new float *[nrows];
	for (int i = 0; i < nrows; i++)
		data[i] = new float[ncols];
	for (int r = 0; r < nrows; r++)
		for (int c = 0; c < ncols; c++)
		{
			infile >> data[r][c];

			averHeight += data[r][c];
			if (r == 0 && c == 0) {
				minHeight = data[r][c];
				maxHeight = data[r][c];
			}
			else
			{
				minHeight = minHeight < data[r][c] ? minHeight : data[r][c];
				maxHeight = maxHeight > data[r][c] ? maxHeight : data[r][c];
			}	
			
		}
	averHeight /= ncols * nrows;
	deltaHeight = maxHeight - minHeight;
	infile.close();
}

DemData::DemData(const DemData &demData)
{
	ncols = demData.ncols;
	nrows = demData.nrows;
	xllcorner = demData.xllcorner;
	yllcorner = demData.yllcorner;
	cellsize = demData.cellsize;
	nodata_value = demData.nodata_value;
	averHeight = demData.averHeight;
	deltaHeight = demData.deltaHeight;
	this->data = new float *[nrows];
	for (int i = 0; i < nrows; i++)
		data[i] = new float[ncols];
	for (int r = 0; r < nrows; r++)
		for (int c = 0; c < ncols; c++)
		{
			data[r][c] = demData.data[r][c];
		}
	
}



void DemData::Smooth(DemData &d)
{
	for (int r = 1; r < d.nrows - 1; r++)
		for (int c = 1; c < d.ncols - 1; c++)
		{
			d.data[r][c] = 0.25f * d.data[r][c]
				+ 0.125f*(d.data[r][c + 1] + d.data[r][c - 1] + d.data[r - 1][c] + d.data[r + 1][c])
				+ 0.0625f*(d.data[r+1][c + 1]+ d.data[r-1][c + 1]+ d.data[r+1][c - 1]+d.data[r-1][c -1]);
		}
}


DemData::~DemData()
{
	for (int i = 0; i < nrows; i++)
		delete[] data[i];
	delete []data;
}
