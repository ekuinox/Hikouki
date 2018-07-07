#pragma once

#ifndef ___UTILS_H

#include <vector>
#include <random>

namespace trau
{
	namespace utils
	{
		std::random_device seedGen;
		std::mt19937 randomEngine(seedGen());

		template <class T>
		const T& choice(const T& container)
		{
			return container[randomEngine() % container.size()];
		}

		// for vector
		template <class T>
		const T& choice(const std::vector<T> &container)
		{
			return container[randomEngine() % container.size()];
		}
	}
	
}

#define ___UTILS_H
#endif