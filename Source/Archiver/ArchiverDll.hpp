#pragma once

#ifdef ARCHIVE_EXPORT
#define ARCHIVE_API __declspec(dllexport)
#else
#define ARCHIVE_API __declspec(dllimport)
#endif

