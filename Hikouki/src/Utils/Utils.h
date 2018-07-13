#pragma once

#ifndef ___UTILS_H

#include <vector>
#include <random>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace trau
{
	namespace utils
	{
		static std::random_device seedGen;
		static boost::uuids::random_generator randomGen;
		static std::mt19937 randomEngine(seedGen());

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

		static std::string generateUUID()
		{
			return boost::lexical_cast<std::string>(randomGen());
		}
	}
}

#define ___UTILS_H
#endif