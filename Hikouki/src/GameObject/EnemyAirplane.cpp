#include "EnemyAirplane.h"
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "../Utils/MathUtil.h"
#include "../Utils/Utils.h"

using trau::utils::choice;

EnemyAirplane::EnemyAirplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, const char * file)
	: Airplane(_xfile, device), randomEngine(std::mt19937(std::random_device()()))
	, rotationTimer(std::unique_ptr<trau::Timer>(new trau::Timer()))
{
	trans.z = initSpeed;

	// json load

	std::ifstream fin(file);

	if (!fin) throw std::runtime_error("[EnemyAirplane] Read JSON File Error");
	
	std::stringstream strstream;
	strstream << fin.rdbuf();
	fin.close();

	auto data = nlohmann::json::parse(strstream.str());

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

	moveTimelineIndex = trau::utils::randomEngine() % moveTimeline.size();

	const auto& coord = choice(data["defaultPositions"]);

	D3DXMatrixTranslation(&mat, coord["x"], coord["y"], coord["z"]);
}

void EnemyAirplane::update(const UpdateDetail& detail)
{
	if (!active) return;

	if (state == State::Explosion)
	{
		explosion->update(detail);
		explosionTimer.end();
		if (explosionTimer.getSeconds() > explosionTimeSeconds) state = State::EXIT;

		return;
	}
	else
	{
		if (detail.input->getTrigger(KeyCode::F5)) drawingBBox = !drawingBBox;

		rotationTimer->end();
		if (rotationTimer->getSeconds() > moveTimeline[moveTimelineIndex].span)
		{
			rotationTimer->start();
			trans += moveTimeline[moveTimelineIndex].trans;
			angle += moveTimeline[moveTimelineIndex].angle;
			
			moveTimelineIndex++;
			if (moveTimeline.size() == moveTimelineIndex) moveTimelineIndex = 0;
		}

		mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
		bbox->updatePosition(mat);

	}
}
