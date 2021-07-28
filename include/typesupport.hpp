#ifndef TYPESUPPORT
#define TYPESUPPORT

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace eul{
	
	using bitvector = std::vector<bool>;
	
	template <typename T>
	inline constexpr bool is_integral_v = std::is_integral_v<T>;
	
	template <typename T>
	inline constexpr bool is_arithmetic_v = std::is_floating_point_v<T> || is_integral_v<T>;
	
	template <typename T>
	concept integral = is_integral_v<T>;
	
	template <typename T>
	concept arithmetic = is_arithmetic_v<T>;
	
	template <typename T>
	concept std_arithmetic = std::is_arithmetic_v<T>;
	
}

#endif