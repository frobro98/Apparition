// Copyright 2020, Nathan Blane

#pragma once

// TODO - Should probably wrap this up in its own file
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDICAPMASKS
#define NOATOM
#define NODRAWTEXT
#define NOKERNAL
#define NOMMMGR
#define NOMETAFILE
#define NOSOUND
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWSPOS
#define NOMCX
#define NOCRYPT
#define NOTAPE
#define NOIMAGE
#define NOPROXYSTUB
#define NORPC
#include <windows.h>

#include "Platform/PlatformDefinitions.h"
#include "Application/Musa.hpp"

extern MusaApp* CreateMusaApplication(char* cmdlineArgs);

int WINAPI WinMain(HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR lpCmdLine,
	int /*nCmdShow*/)

{
	//int argc = 0;
	//char* argv = CommandLineToArgvW(lpCmdLine, &argc);
	gApp = CreateMusaApplication(lpCmdLine);
	gApp->LaunchApplication();

	return 0;
}
