#include "Rader.h"
#include "EnemyAirplane.h"
#include "PlayerAirplane.h"

Rader::Rader(const std::shared_ptr<XFileObjectBase>& _target, D3DXVECTOR2 _center, float _radius)
	: target(_target), center(_center), radius(_radius)
{
}

void Rader::draw(const LPDIRECT3DDEVICE9 & device) const
{
	// ライティング計算はしない & 深度バッファに書き込まない
	device->SetRenderState(D3DRS_LIGHTING, false);
	device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	device->SetTexture(0, nullptr);
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	drawCircle(center, radius, D3DCOLOR_ARGB(64, 0, 0, 128), device);
	drawTriangle(center, D3DXVECTOR2(10, 10), D3DCOLOR_ARGB(255, 255, 0, 0), device);
	for (const auto& dot : dots) drawQuad(dot.coord, dot.size, dot.color, device);

	// 戻す
	device->SetRenderState(D3DRS_LIGHTING, true);
	device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void Rader::update(const UpdateDetail & detail)
{
	dots.clear();

	D3DXMATRIX inversedTargetMatrix = target->getMat();
	float det;
	D3DXMatrixInverse(&inversedTargetMatrix, &det, &target->getMat());

	for (const auto& gameObject : detail.gameObjects)
	{
		if (gameObject->getId() == EnemyAirplane::id)
		{
			const auto& enemyAirplane = std::static_pointer_cast<EnemyAirplane>(gameObject);
			if (enemyAirplane->getState() == EnemyAirplane::State::ALIVE)
			{
				const auto& fixedCoord = fixCoordPlayer(enemyAirplane->getPos(), inversedTargetMatrix);
				if (isInsideRange(fixedCoord))
				{
					dots.emplace_back(Dot{
						D3DXVECTOR2{ center.x + (fixedCoord.x / SCOPE_RATE), center.y + (fixedCoord.y / SCOPE_RATE) },
						D3DXVECTOR2{ 4, 4 },
						D3DCOLOR_ARGB(255, 255, 0, 0)
					});
				}
			}
		}
	}

	if (target->getId() == PlayerAirplane::id)
	{
		for (const auto& homingMissilePosition : std::static_pointer_cast<PlayerAirplane>(target)->getHomingMissilePositions())
		{
			const auto& fixedCoord = fixCoordPlayer(homingMissilePosition, inversedTargetMatrix);
			if (isInsideRange(fixedCoord))
			{
				dots.emplace_back(Dot{
					D3DXVECTOR2{ center.x + (fixedCoord.x / SCOPE_RATE), center.y + (fixedCoord.y / SCOPE_RATE) },
					D3DXVECTOR2{ 4, 4 },
					D3DCOLOR_ARGB(255, 0, 255, 0)
				});
			}
		}
	}
}

void Rader::drawTriangle(const D3DXVECTOR2 & frontVertex, const D3DXVECTOR2 & size, const DWORD & color, LPDIRECT3DDEVICE9 device) const
{
	VERTEX vertexes[3] = {
		VERTEX{ frontVertex.x, frontVertex.y, 0.0f, 0.0f, color },
		VERTEX{ frontVertex.x + size.x / 2.0f, frontVertex.y + size.y, 0.0f, 0.0f, color },
		VERTEX{ frontVertex.x - size.x / 2.0f, frontVertex.y + size.y, 0.0f, 0.0f, color },
	};

	device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, vertexes, sizeof VERTEX);
}

void Rader::drawQuad(const D3DXVECTOR2 & frontVertex, const D3DXVECTOR2 & size, const DWORD & color, LPDIRECT3DDEVICE9 device) const
{
	VERTEX vertexes[4] = {
		VERTEX{ frontVertex.x - size.x / 2.0f, frontVertex.y - size.y / 2.0f, 0.0f, 0.0f, color },
		VERTEX{ frontVertex.x + size.x / 2.0f, frontVertex.y - size.y / 2.0f, 0.0f, 0.0f, color },
		VERTEX{ frontVertex.x + size.x / 2.0f, frontVertex.y + size.y / 2.0f, 0.0f, 0.0f, color },
		VERTEX{ frontVertex.x - size.x / 2.0f, frontVertex.y + size.y / 2.0f, 0.0f, 0.0f, color },
	};

	device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertexes, sizeof VERTEX);
}

void Rader::drawCircle(const D3DXVECTOR2 & frontVertex, const float & radius, const DWORD & color, LPDIRECT3DDEVICE9 device) const
{
	VERTEX vertexes[CIRCLED_VERTEXES_COUNT];
	constexpr auto angle = (D3DX_PI * 2) / CIRCLED_VERTEXES_COUNT;

	for (auto i = 0; i < CIRCLED_VERTEXES_COUNT; ++i)
	{
		vertexes[i] = {
			cosf(angle * i) * radius + frontVertex.x,
			sinf(angle * i) * radius + frontVertex.y,
			0.0f,
			1.0f,
			color
		};
	}

	device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, CIRCLED_VERTEXES_COUNT - 2, vertexes, sizeof VERTEX);
}

D3DXVECTOR3 Rader::fixCoordPlayer(const D3DXVECTOR3 & wpos, const D3DXMATRIX & playerMatrix) const
{
	D3DXVECTOR3 result;

	D3DXVec3TransformCoord(&result, &wpos, &playerMatrix);

	return result;
}

bool Rader::isInsideRange(const D3DXVECTOR3 & coord) const
{
	return !(powf(coord.x / SCOPE_RATE, 2) + powf(coord.y / SCOPE_RATE, 2) > powf(radius, 2));
}
