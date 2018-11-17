// Extract.cpp : Defines the exported functions for the DLL application.
//

#include "Core.h"
#include "UnCore.h"
#include "UnPackage.h"
#include "GameDatabase.h"

#define MAKE_DIRS		1
//#define DISABLE_WRITE	1		// for quick testing of extraction

#define HOMEPAGE		"http://www.gildor.org/" //this is all a port of Gildor's PackageExtract tool from UEModel

#if UNREAL4

int UE4UnversionedPackage(int verMin, int verMax)
{
	appError("Unversioned UE4 packages are not supported. Please restart UModel and select UE4 version in range %d-%d using UI or command line.", verMin, verMax);
	return -1;
}

bool UE4EncryptedPak()
{
	return false;
}

#endif // UNREAL4

/*-----------------------------------------------------------------------------
	Service functions
-----------------------------------------------------------------------------*/

static void GetFullExportFileName(const FObjectExport &Exp, UnPackage *Package, char *buf, int bufSize)
{
	// get full path
	Package->GetFullExportName(Exp, buf, bufSize);
	char *dst;
	// replace '.' with '/' and find the string end
	for (dst = buf; *dst; dst++)
		if (*dst == '.') *dst = '/';
	// and append class name as file extension
	assert(*dst == 0);
	const char *ClassName = Package->GetObjectName(Exp.ClassIndex);
	appSprintf(dst, bufSize - (dst - buf), ".%s", ClassName);
}

static void GetFullExportName(const FObjectExport &Exp, UnPackage *Package, char *buf, int bufSize)
{
	// put class name
	const char *ClassName = Package->GetObjectName(Exp.ClassIndex);
	appSprintf(buf, bufSize, "%s'", ClassName);
	// get full path
	char *dst = strchr(buf, 0);
	Package->GetFullExportName(Exp, dst, bufSize - (dst - buf));
	// append "'"
	dst = strchr(buf, 0);
	appSprintf(dst, bufSize - (dst - buf), "'");
}

static TArray<FString> filters;

static bool FilterClass(const char *ClassName)	//?? check logic: filter = pass or reject
{
	if (!filters.Num()) return true;

	bool filter = false;
	for (int fidx = 0; fidx < filters.Num(); fidx++)
	{
		if (!stricmp(*filters[fidx], ClassName))
			return true;
	}
	return false;
}