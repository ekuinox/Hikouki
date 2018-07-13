#include "GameObject.h"

GameObject::GameObject()
	: GameObjectInterface()
{
}

GameObject::~GameObject()
{

}

void GameObject::beforeDraw(const LPDIRECT3DDEVICE9 &)
{
}

void GameObject::draw(const LPDIRECT3DDEVICE9&) const
{
	if (!drawing) return;
}

void GameObject::update(const UpdateDetail & detail)
{
	if (!active) return;
}

void GameObject::afterUpdate(const UpdateDetail &)
{
}

void GameObject::show()
{
	drawing = true;
}

void GameObject::hide()
{
	drawing = false;
}

void GameObject::enable()
{
	active = true;
}

void GameObject::disable()
{
	active = false;
}

unsigned int GameObject::getLayer() const
{
	return layer;
}

unsigned int GameObject::getPriority() const
{
	return priority;
}
