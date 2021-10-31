#ifndef PROPOSITION
#define PROPOSITION

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include "eulalgorithm.hpp"
#include "typesupport.hpp"

namespace eul{
	
	class proposition {
		
		inline static std::vector<proposition> truthcache{};
		
		mutable bitvector truthtable;
		
	public:
		
		// Public members
		const std::string expression;
		
		const std::vector<char> leaves;
		
		// Constructor
		
		// Other
		void computetruthtable() const;
		
	};
	
	inline std::size_t truthmask(const std::vector<char> &smallmask, const std::vector<char> &largemask, std::size_t largeindex){
		std::size_t smallindex=0;
		std::size_t j=0;
		for(std::size_t k=0; k<largemask.size(); k++){
			// TODO: Change ull to uz when c++23 will be used
			if(smallmask[j]==largemask[k]){
				if((largeindex >> k) & 1ull){
					smallindex |= 1ull << j;
				}
				j++;
			}
		}
		return smallindex;
	}
	
}

#endif