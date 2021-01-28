#pragma once

#include <stdexcept>
#include <cmath>
#include <type_traits>
#include <string_view>

#include "types.hpp"

template<typename T> 
T StrToNum(std::string_view str) requires std::is_same_v<T,u32> || std::is_same_v<T,f32>
{
	T value = static_cast<T>(0); // initalize to 0, be it float or uint
	if constexpr(std::is_same_v<T,u32>)
	{
		u32 digitMultiplier = 1;
		for(auto it = str.rbegin();it != str.rend();it++)
		{
			if(*it >= '0' && *it <= '9') // the curreent caracter is a digit
			{
				if(*it != '0') // if digit is 0, we needn't do anything
					value += (*it - '0') * digitMultiplier;
			}
			else
			{
				std::string msg = "Character \' \' cannot be used in representation of an unisgend integer!";
				msg[11] = *it;
				throw std::runtime_error(msg);
			}

			digitMultiplier *= 10;
		}
	}
	else
	{
		f32 sign = 1.f;
		if(str[0] == '-')
		{
			sign = -1.f;
			str.remove_prefix(1);
		}

		u64 dotInx = str.find('.');

		f32 digitMultiplier;
		if(dotInx == std::string_view::npos) // number doesn't contain a decimal point
			digitMultiplier = std::pow(10,str.length() - 1);
		else
			digitMultiplier = std::pow(10,static_cast<i32>(dotInx) - 1);
		
		u32 digitsProcessed = 0; // float has a precioion of about 7 digits, so there is no point of processing any following digits
		for(auto it = str.begin();it != str.end();it++)
		{
			if(digitsProcessed == 7)
				break;
				
			if((*it >= '0' && *it <= '9') || *it == '.') // the curreent caracter is a digit
			{
				if(*it == '.')
					continue;
				else if(*it != '0') // if digit is 0, we needn't do anything
					value += (*it - '0') * digitMultiplier * sign;
			}
			else
			{
				std::string msg = "Character \' \' cannot be used in representation of a floating point number!";
				msg[11] = *it;
				throw std::runtime_error(msg);
			}

			digitsProcessed++;
			digitMultiplier /= 10;
		}
	}

	return value;
}