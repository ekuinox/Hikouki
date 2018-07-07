#include "EnemyAirplane.h"
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "../Utils/MathUtil.h"

using json = nlohmann::json;

EnemyAirplane::EnemyAirplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, trau::Timer *timer, const char * file)
	: Airplane(_xfile, device, timer)
{
	std::random_device seed_gen;
	std::mt19937 _engine(seed_gen());
	randomEngine = _engine;
	trans.z = 20.0f;
	rotationTimer = std::unique_ptr<trau::Timer>(new trau::Timer());

	// json load

	std::ifstream fin(file);

	if (!fin) throw "read file error";
	
	std::stringstream strstream;
	strstream << fin.rdbuf();
	fin.close();

	auto data = json::parse(strstream.str());

	// JSONのエラーチェックを行わない
	for (const auto& moves : data["moveTimeline"])
	{
		moveTimeline.emplace_back(
			Moves{ 
				moves["span"],
				{ moves["trans"]["x"], moves["trans"]["x"], moves["trans"]["z"] },
				{ moves["angle"]["x"], moves["angle"]["x"], moves["angle"]["z"] }
			}
		);
	}

	moveTimelineIndex = 0;

	D3DXMatrixTranslation(&mat, data["defaultPositions"][0]["x"], data["defaultPositions"][0]["y"], data["defaultPositions"][0]["z"]);
}

void EnemyAirplane::update()
{
	if (!active) return;

	if (explosion_flag)
	{
		explosion->Update();
	}
	else
	{
		rotationTimer->end();
		if (rotationTimer->getSeconds() > moveTimeline[moveTimelineIndex].span)
		{
			rotationTimer->start();
			trans += moveTimeline[moveTimelineIndex].trans;
			angle += moveTimeline[moveTimelineIndex].angle;
			
			moveTimelineIndex++;
			if (moveTimeline.size() == moveTimelineIndex) moveTimelineIndex = 0;
		}

		
		D3DXMATRIX mx;
		MakeWorldMatrix(mx, mat, angle * timer->getSeconds(), trans * timer->getSeconds());
		bbox->updatePosition(mat);
	}
}
