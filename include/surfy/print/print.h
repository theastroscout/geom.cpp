#ifndef SURFY_PRINT_HPP
#define SURFY_PRINT_HPP

#include <iostream>
#include <string>
#include "../../json.h"


namespace surfy {
	using json = nlohmann::ordered_json;

	// Overload for printing JSON objects
	inline void print(const json& arg) {
		std::cout << arg.dump(1, '\t') << std::endl;
	}

	template<typename T, typename... Args>
	inline void print(const std::vector<T>& arg, const Args&... args) {
		json j = arg;
		std::cout << j.dump();
		if constexpr(sizeof...(args) > 0) {
			std::cout << " ";
			print(args...);
		} else {
			std::cout << std::endl;
		}
	}
	
	template<typename T, typename... Args>
	inline void print(const T& arg, const Args&... args) {
		std::cout << arg;
		
		if constexpr(sizeof...(args) > 0) {
			std::cout << " ";
			print(args...);
		} else {
			std::cout << std::endl;
		}
	}
	
}

#endif