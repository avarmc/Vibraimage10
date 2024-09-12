// ViConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>
#include <iostream>
#include "CViEngineExAPI.h"
#include "CViEngineExConsole.h"



int wmain(int argc, wchar_t* argv[])
{
	if (argc <= 4) {
		std::wcout << L"Usage: ViConsole.exe <video file path> <output xml> <start time> <measurement length>" << std::endl;
		std::wcout << L"  Example:\r\n    ViConsole.exe \"c:\\AviBase\\test.avi\" \"c:\\AviBase\\test.xml\" 20 60" << std::endl;
		return 0;
	}

	CViEngineExConsole vi;
	
	vi.OutputTo(argv[2]);
	vi.StartFrom((float)_wtof(argv[3]));

	if (vi.Start()) {

		vi.PutVar((float)_wtof(argv[4]), L"VI_INFO_M_PERIOD");
		vi.OpenDocument(argv[1]);

		while (vi.IsReady()) {
			if (!vi.Check())
				break;
			std::this_thread::sleep_for(std::chrono::microseconds(100000));
		}
	}

	vi.Stop();
	return 0;
}

