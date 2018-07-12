#include "XFileManager.h"

XFileManager::XFileManager(LPDIRECT3DDEVICE9 _device)
	: device(_device)
{
}

XFileManager::~XFileManager()
{
	for (const auto& xfile : xfiles)
	{
		delete xfile.second.xfile;
	}
}

void XFileManager::add(const char * key, const char * filename)
{
	if (!xfiles.count(key)) xfiles[key].xfile = new CDirect3DXFile(filename, device);
}

void XFileManager::add(std::vector<std::pair<const char*, const char*>> list)
{
	for (const auto& value : list)
	{
		add(value.first, value.second);
	}
}

CDirect3DXFile * XFileManager::get(const char * key)
{
	return xfiles[key].xfile;
}