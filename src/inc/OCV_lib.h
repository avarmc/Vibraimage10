#pragma once


#ifdef OCV_LIB_EXPORTS
#define OCV_LIB_API __declspec(dllexport)
#else
#define OCV_LIB_API __declspec(dllimport)
#endif

class OCV_LIB_API COCV_Face
{
private:
	void *_pd;
public:
	COCV_Face(void);
	~COCV_Face(void);
public:
	char errMsg[512];
public:
	void	Configure(float scale,int qual,int dsize,int msize);
	bool	SetPath(LPCWSTR path);
	bool	SetImage(BYTE *pBI, int w, int h,int bpp);
	int		Process(int bOne=0);
	bool	GetFace(int i, int& x, int& y, int& w, int& h,int& faceID);
};
