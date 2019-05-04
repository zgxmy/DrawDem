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


DemData::~DemData()
{
	for (int i = 0; i < nrows; i++)
		delete[] data[i];
	delete []data;
}
