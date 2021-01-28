#pragma once

#include <iostream>
#include <type_traits>
#include <chrono>

using namespace std::chrono;

template<typename U = seconds>
struct Timer
{
	std::string tag;
	time_point<steady_clock> begin;
	Timer(const std::string &tag)
	{
		this->tag = tag;
		this->begin = steady_clock::now();
	}

	~Timer()
	{
		const bool milliSec = std::is_same<U,milliseconds>::value;
		const bool sec      = std::is_same<U,seconds>::value;
		static_assert(milliSec || sec,"Select an appropriate time unit. (std::chrono::seconds or std::chrono::milliseconds)");
		
		if constexpr(milliSec)
		{
			duration<float,std::milli> diff = steady_clock::now() - this->begin;
			std::cout << "Task \"" << this->tag << "\" took: " << diff.count() << "ms" << std::endl;
		}
		else if constexpr(sec)
		{
			duration<float> diff = steady_clock::now() - this->begin;
			std::cout << "Task \"" << this->tag << "\" took: " << diff.count() << 's' << std::endl;
		}
	}
};