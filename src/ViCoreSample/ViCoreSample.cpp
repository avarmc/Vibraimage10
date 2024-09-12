// ViCoreSample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "CVICoreApp.h"
#include <atlimage.h>

std::wstring GetDefaultPath()
{
	WCHAR fileName[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, fileName, MAX_PATH + 1);

	std::wstring path = std::filesystem::absolute(fileName).c_str();

	size_t pos = path.rfind('\\');
	if (pos != std::wstring::npos)
		return path.substr(0,pos + 1);
	return path;
}

void ListFiles(std::map<double, std::wstring>& files) {
	files.clear();

	auto path = GetDefaultPath() + L"dbg";
	if (!std::filesystem::exists(path))
		return;

	for (const auto& file : std::filesystem::directory_iterator(path)) {
		if (file.is_directory())
			continue;
		std::wstring fp = file.path();
		auto ext = fp.substr(fp.rfind('.'));
		auto fn = fp.substr(fp.rfind('\\') + 1);

		if (ext != L".jpg")
			continue;

		double t = _wtof(fn.substr(1).c_str());
		files[t] = fp;
	}
}

void AddFrame(CVICoreApp* pApp, double t, CImage& img) {
	int w = img.GetWidth();
	int h = img.GetHeight();
	int bpp = img.GetBPP();
	int pitch = img.GetPitch();
	if (bpp != 24)
		return;

	void* p = ((BYTE*)img.GetBits()) + (h - 1) * pitch;
	pApp->AddFrame(t, p, w, h);
}

void AddFrame(CVICoreApp* pApp, double t, std::wstring fp) {
	CImage img;
	if (img.Load(fp.c_str()) == S_OK)
		AddFrame(pApp, t, img);
}

static void VideoInput(CVICoreApp* pApp) {
	std::map<double, std::wstring> files;
	ListFiles(files); 


	while (pApp->IsStarted()) {
		auto t0 = GetTickCount64();
		for (auto& f : files) {
			double t;
			while ( (t = (GetTickCount64() - t0) / 1000.0) < f.first && pApp->IsStarted())
				Sleep(1);
			AddFrame(pApp, f.first, f.second);
			
		}

	}
}

int wmain(int argc, wchar_t* argv[])
{
	CVICoreApp app(argc,argv);
	if (!app.StartEngine()) {
		std::wcout << L"ERROR: app.StartEngine()" << std::endl;
		return -1;
	}

	std::thread video_thread(VideoInput, &app);



	system("pause");
	app.StopEngine();
	video_thread.join();

	return 0;
}
