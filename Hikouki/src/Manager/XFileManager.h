#pragma once
#include "../Utils/CDirect3DXFile.h"
#include <vector>
#include <map>

class XFileManager {
private:
	struct _XFILESET { CDirect3DXFile *xfile; };
	std::map<const char*, struct _XFILESET> xfiles;
	LPDIRECT3DDEVICE9 device;
public:
	XFileManager(LPDIRECT3DDEVICE9);
	~XFileManager();
	void add(const char*, const char*);
	void add(std::vector<std::pair<const char*, const char*>>);
	CDirect3DXFile* get(const char*);
};

