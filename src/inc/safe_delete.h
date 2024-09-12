#pragma once

#ifndef SAFE_DELETE
	#define SAFE_DELETE(x) { if(x) { delete (x); (x) = 0; } }
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x) = 0; } }
#endif
