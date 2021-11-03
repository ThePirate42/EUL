#ifndef PROPOSITION
#define PROPOSITION

#include <algorithm>
#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>
#include "eulalgorithm.hpp"
#include "typesupport.hpp"

// Foward declarations necessary to use std::unordered_set correctly
namespace eul{
	
	class proposition;
	
	bool operator==(const proposition &a,const proposition &b);
	
}

template<>
struct std::hash<eul::proposition>{
	std::size_t operator()(const eul::proposition& x) const;
};

// Class body
namespace eul{
	
	class proposition {
		
		static std::unordered_set<proposition> truthcache;
		
		std::string expression;
		
		std::vector<char> leaves;
		
		mutable bitvector truthtable;
		
	public:
		
		// Constructor
		proposition(const std::string& expre){
			expression=expre;
			for(std::size_t con=0; con<expre.size(); con++){
				if(expre[con]>='A' && expre[con]<='Z'){
					leaves.push_back(expre[con]);
				}
			}
			std::sort(leaves.begin(),leaves.end());
			std::vector<char>::const_iterator newend = std::unique(leaves.begin(),leaves.end());
			leaves.resize(newend-leaves.begin());
			leaves.shrink_to_fit();
		}
		
		// Getters
		const std::string& getExpression() const {return expression;}
		const std::vector<char>& getLeaves() const {return leaves;}
		
		// Comparison operator
		friend bool operator==(const proposition &a,const proposition &b) {return a.expression==b.expression;}
		
		// Other
		void computetruthtable() const;
		
		static void clearcache(){
			truthcache.clear();
		}
		
	};
	
}
	
inline std::size_t std::hash<eul::proposition>::operator()(const eul::proposition& prop) const{
	return std::hash<std::string>{}(prop.getExpression());
}
	
namespace eul{
	
	inline std::unordered_set<proposition> proposition::truthcache{};
	
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
	
	inline void proposition::computetruthtable() const{
		std::unordered_set<proposition>::const_iterator cachecopy = truthcache.find(*this);
		if(cachecopy!=truthcache.end()){
			truthtable=(*cachecopy).truthtable;
			return;
		}
	}
	
}

#endif