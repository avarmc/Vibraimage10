#include "StdAfx.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp" 
#include "VIMT.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include "safe_delete.h"
#include <cstdlib>
#include <memory>

#include "../../inc/OCV_lib.h"

#define pd ((OCV_FaceData*)_pd)

using namespace cv;


typedef struct tagOCV_FaceData
{
	std::shared_ptr<CascadeClassifier>	pccCPU;

	Mat	image,smallImage;
	std::vector<Rect>	faces;
	std::vector<int>	faceID;

    float scaleFactor,scale;
	int qual, dsize, msize;

	Size sMin,sMax;

	WCHAR	path[MAX_PATH+1];

	CMTCriticalSection lock;
} OCV_FaceData;

std::string toupper(const std::string & s)
{
	std::string ret(s.size(), char());
	for (unsigned int i = 0; i < s.size(); ++i)
		ret[i] = (s[i] <= 'z' && s[i] >= 'a') ? s[i] - ('a' - 'A') : s[i];
	return ret;
}

// Get the size that is needed for strutf16_utf8 applied to all s characters
static size_t strutf16_utf8_size(const wchar_t* s)
{
	size_t length = 0;

	for (; *s; ++s)
	{
		unsigned int ch = *s;

		if (ch < 0x80) length += 1;
		else if (ch < 0x800) length += 2;
		else if (ch < 0x10000) length += 3;
		else if (ch < 0x200000) length += 4;
	}

	return length;
}


static char* strutf16_utf8(char* s, unsigned int ch)
{
	static const unsigned char UTF8_BYTE_MASK = 0xBF;
	static const unsigned char UTF8_BYTE_MARK = 0x80;
	static const unsigned char UTF8_BYTE_MASK_READ = 0x3F;
	static const unsigned char UTF8_FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	unsigned int length;

	if (ch < 0x80) length = 1;
	else if (ch < 0x800) length = 2;
	else if (ch < 0x10000) length = 3;
	else if (ch < 0x200000) length = 4;
	else return s;

	s += length;

	// Scary scary fall throughs.
	switch (length)
	{
	case 4:
		*--s = (char)((ch | UTF8_BYTE_MARK) & UTF8_BYTE_MASK);
		ch >>= 6;
	case 3:
		*--s = (char)((ch | UTF8_BYTE_MARK) & UTF8_BYTE_MASK);
		ch >>= 6;
	case 2:
		*--s = (char)((ch | UTF8_BYTE_MARK) & UTF8_BYTE_MASK);
		ch >>= 6;
	case 1:
		*--s = (char)(ch | UTF8_FIRST_BYTE_MARK[length]);
	}

	return s + length;
}

static std::string as_utf8(const wchar_t* str)
{
	std::string result;
	result.reserve(strutf16_utf8_size(str));

	for (; *str; ++str)
	{
		char buffer[6];

		result.append(buffer, strutf16_utf8(buffer, *str));
	}

	return result;
}

std::string w2c(const WCHAR* str)
{
	return as_utf8(str);
}

COCV_Face::COCV_Face(void)
{
	_pd = new OCV_FaceData;
	pd->pccCPU = 0;

	setUseOptimized(true);


	memset(pd->path,0,sizeof(pd->path));

	memset(errMsg,0,sizeof(errMsg));

}

COCV_Face::~COCV_Face(void)
{
	CMTSingleLock lock(&(pd->lock), TRUE);
	pd->pccCPU = 0;
	lock.Unlock();

	delete pd;
}

bool	COCV_Face::SetPath(LPCWSTR path)
{
	if (wcscmp(path, pd->path) == 0)
		return true;

	CMTSingleLock lock(&(pd->lock), TRUE);

	
	bool bUnicode = false;
	for (int i = 0; path[i]; ++i) {
		if (path[i] & 0xFF00) {
			bUnicode = true;
			break;
		}
	}

	std::string spath;

	if(!bUnicode)
		spath = w2c(path);
	else {
		WCHAR tmp[MAX_PATH * 4];
		GetShortPathNameW(path, tmp, MAX_PATH * 4);
		spath = w2c(tmp);
	}

	int gpuCnt = 0;

	bool ok = false;


	bool bGPU = (gpuCnt > 0);

	if (bGPU)
	{
		pd->pccCPU = 0;
	}else
	{
		pd->pccCPU = std::make_shared<CascadeClassifier>();
	}

	if (pd->pccCPU)
	{
		try {
			errMsg[0] = 0;
			ok = pd->pccCPU->load(spath);
			if (ok)
				wcscpy_s(pd->path,MAX_PATH+1, path);
			else
				pd->path[0] = 0;
		}
		catch (Exception& e)
		{
			strcpy_s(errMsg, sizeof(errMsg),e.msg.c_str());
			pd->path[0] = 0;

			return false;
		}


		return ok;
	}



	return ok;
}

void	COCV_Face::Configure(float scale,int qual,int dsize,int msize)
{
	pd->scaleFactor = 1.2f;
	pd->scale = scale;
	pd->qual = qual;
	pd->dsize = dsize;
	pd->msize = msize;

}


bool	COCV_Face::SetImage(BYTE *pBI, int w, int h,int bpp)
{
	CMTSingleLock lock(&(pd->lock),TRUE);


	if(pd->image.cols != w || pd->image.rows != h)
	{
		pd->image.release();
		pd->image.create(h,w,CV_8UC1);
	}

	int s = (int)pd->image.elemSize();
	for(int y = 0; y < h; ++y)
	{
		BYTE *pD = (BYTE *)(pd->image.ptr(h-1-y));
		BYTE *pS = pBI+y*w, *pE = pS+w;
		memcpy(pD,pS,w);
	}
		

	if(pd->scale <= 1)
	{
		if(pd->smallImage.cols != w || pd->smallImage.rows != h)
		{
			pd->smallImage.release();
			pd->smallImage.create(h,w,CV_8UC1); 
		}
	}
	else
	{
		int nw = cvRound(h/pd->scale), nh = cvRound(w/pd->scale);
		if(pd->smallImage.cols != nw || pd->smallImage.rows != nh)
		{
			pd->smallImage.release();
			pd->smallImage.create(nw,nh,CV_8UC1);
		}
	}

	try {
		resize( pd->image, pd->smallImage, pd->smallImage.size(), 0, 0, INTER_LINEAR );
	} catch( Exception& e )
	{
		strcpy_s(errMsg,sizeof(errMsg),e.msg.c_str());
		return false;
	}

	return true;
}

int	COCV_Face::Process(int bOne)
{
	CMTSingleLock lock(&(pd->lock),TRUE);


	WCHAR *path = pd->path;
	if (*path == 0)
		return 0;


	try {
		equalizeHist( pd->smallImage,pd->smallImage );
	} catch( Exception& e )
	{
		strcpy_s(errMsg, sizeof(errMsg), e.msg.c_str());
		return 0;
	}
	
	int qual = pd->qual;
	int dsize = pd->dsize;
	int msize = pd->msize;
	int flags = 0; // qual ? CV_HAAR_SCALE_IMAGE : 0;

	dsize = dsize < 8 ? 8 : dsize;

	double step = 1.2;


	if (bOne)
		flags |= CASCADE_FIND_BIGGEST_OBJECT;

	if(qual == 0)
		qual = 2, flags |= CASCADE_DO_ROUGH_SEARCH;
	else
	if(qual < 1) qual = 1;
	else
	if(qual > 4) qual = 4;

	
	pd->sMin.width = pd->sMin.height = dsize;
	pd->sMax.width = pd->sMax.height = msize;

	pd->faces.clear();
	pd->faceID.clear();

	try {
		pd->pccCPU->detectMultiScale( pd->smallImage, pd->faces, pd->scaleFactor, pd->qual, flags, pd->sMin, pd->sMax );
	} catch( Exception& e )
	{
		strcpy_s(errMsg, sizeof(errMsg), e.msg.c_str());
		return 0;
	}

	size_t cnt = pd->faces.size();
	pd->faceID.resize(cnt);

	for (size_t i = 0; i < cnt; ++i)
		pd->faceID[i] = 0;

	return (int)cnt;
}


bool	COCV_Face::GetFace(int i, int& x, int& y, int& w, int& h, int& faceID)
{
	CMTSingleLock lock(&(pd->lock),TRUE);
	if(i < 0 || i >= (int)pd->faces.size() )
		return false;

	if( pd->faces[i].width < pd->dsize*2/3)
		return false;

	x = pd->faces[i].x*pd->image.cols/pd->smallImage.cols;
	y = pd->faces[i].y*pd->image.rows/pd->smallImage.rows;
	w = pd->faces[i].width*pd->image.cols/pd->smallImage.cols;
	h = pd->faces[i].height*pd->image.rows/pd->smallImage.rows;
	faceID = pd->faceID[i];

	if (h < w)
	{
		y -= (w - h) / 2;
		h = w;
	}

	return true;
}