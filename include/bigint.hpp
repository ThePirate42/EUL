#ifndef BIGINT
#define BIGINT

#include <algorithm>
#include <cctype>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "eulalgorithm.hpp"
#include "typesupport.hpp"

namespace eul{
	
	class bigint {
		
		using size_type = std::uint_least64_t;
		
		static const std::uint_least64_t base=static_cast<std::uint_least64_t>(std::numeric_limits<std::uint32_t>::max())+1;
		static const std::uint_least64_t digits10base=static_cast<std::uint_least64_t>(std::numeric_limits<std::uint32_t>::digits10)+1;
		
		bool negative=false;
		std::vector<std::uint32_t> container;
		
		std::uint_least64_t getDigit(size_type k) const{
			if(k>=container.size()){
				return 0;
			}
			return container[k];
		}
		
		void normalize(){
			while(!container.empty() && container.back()==0){
				container.pop_back();
			}
			if(container.empty()){
				container.push_back(0);
				negative=false;
			}
			container.shrink_to_fit();
		}
		
	public:
	
		// Constructors
		bigint() : container{0} {}
		
		template<std::integral T>
		bigint(T n){
			if(n==0){
				container.push_back(0);
				return;
			}
			if(n<0){
				negative=true;
			}
			std::uintmax_t l=abs(n);
			while(l>0){
				container.push_back(l%base);
				l/=base;
			}
		}
		
		template<std::floating_point T>
		explicit bigint(T n){
			if(n>-1 && n<1){
				container.push_back(0);
				return;
			}
			if(n<0){
				negative=true;
				n=-n;
			}
			n=std::floor(n);
			while(n>=1){
				container.push_back(std::fmod(n,base));
				n=std::floor(n/base);
			}
		}
	
		// Comparison operators
		friend bool operator==(const bigint &a,const bigint &b) = default;
		friend std::strong_ordering operator<=>(const bigint &a,const bigint &b);
	
		// Unary arithmetic operators
		bigint operator+() const {return *this;}
		bigint operator-() const{
			bigint flip=*this;
			if(flip!=0){
				flip.negative=!(flip.negative);
			}
			return flip;
		}
		
		// Compound assignment operators
		bigint& operator+=(const bigint &b);
		bigint& operator-=(const bigint &b);
		bigint& operator*=(const bigint &b);
		bigint& operator/=(const bigint &b);
		bigint& operator%=(const bigint &b);
		
		// Increment/decrement
		bigint& operator++() {*this+=1; return *this;}		
		bigint& operator--() {*this-=1; return *this;}
		bigint operator++(int) {bigint old=*this; ++*this; return old;}
		bigint operator--(int) {bigint old=*this; --*this; return old;}
		
		// Conversion functions
		explicit operator bool() const {return *this!=0;}
		explicit operator std::string() const;
		
		friend bigint stobi(const std::string &n);
		
		// Math friend functions
		friend inline bigint abs(bigint n) {n.negative=false; return n;}
		friend std::size_t num10Digits (bigint n);
		
		// Debug
		void dump() const{
			if(negative){
				std::cout << "-_";
			}
			for(size_type con=container.size(); con>0; con--){
				std::cout << container[con-1] << "_";
			}
			std::cout << "[" << container.size() << "," << base << "," << digits10base << "]" << "\n" << std::flush;
		}
	};
	
	template<>
	inline constexpr bool is_integral_v<bigint> = true;
	
	bigint stobi(const std::string &str){
		bigint res;
		
		std::string::const_iterator msd=std::find_if_not(str.begin(),str.end(),[](char d){return std::isspace(d);});
		if(*msd=='+'){
			msd++;
		} else if(*msd=='-'){
			res.negative=true;
			msd++;
		}
		if(!std::isdigit(*msd)){
			throw std::invalid_argument("stobi");
		}
		msd=std::find_if(msd,str.end(),[](char d){return d!='0';});
		if(!std::isdigit(*msd)){
			res.negative=false;
			return res;
		}
		std::string::const_iterator alsd=std::find_if_not(msd,str.end(),[](char d){return std::isdigit(d);});
		
		res.container.clear();
		std::string n(msd,alsd);
		while(n.size()>bigint::digits10base || std::stoull(std::string(n,0,bigint::digits10base))>=bigint::base){
			std::string quot;
			bigint::size_type con=bigint::digits10base;
			std::uint_least64_t partdivid=std::stoull(std::string(n,0,bigint::digits10base));
			if(partdivid<bigint::base){
				partdivid=partdivid*10+(n[con]-'0');
				con++;
			}
			while(con<n.size()){
				quot+=partdivid/bigint::base+'0';
				partdivid=(partdivid%bigint::base)*10+(n[con]-'0');
				con++;
			}
			quot+=partdivid/bigint::base+'0';
			partdivid%=bigint::base;
			res.container.push_back(partdivid);
			n=quot;
		}
		res.container.push_back(std::stoull(n));
		
		return res;
	}
	
	inline namespace literals{
		
		inline bigint operator"" _bi (const char *n){
			std::string str=n;
			if(str.size()<=std::numeric_limits<unsigned long long>::digits10){
				return bigint(std::stoull(str));
			}
			return stobi(str);
		}
		
	}
		
	std::strong_ordering operator<=>(const bigint &a,const bigint &b){
		if(a.negative!=b.negative){
			return b.negative<=>a.negative;
		}
		if(a.negative==true){
			if(a.container.size()!=b.container.size()){
				return b.container.size()<=>a.container.size();
			}
			return std::lexicographical_compare_three_way(b.container.rbegin(),b.container.rend(),a.container.rbegin(),a.container.rend());
		}
		if(a.container.size()!=b.container.size()){
			return a.container.size()<=>b.container.size();
		}
		return std::lexicographical_compare_three_way(a.container.rbegin(),a.container.rend(),b.container.rbegin(),b.container.rend());
	}
	
	inline bigint operator+(bigint a,const bigint &b){
		a+=b;
		return a;
	}
	
	inline bigint operator-(bigint a,const bigint &b){
		a-=b;
		return a;
	}
	
	inline bigint operator*(bigint a,const bigint &b){
		a*=b;
		return a;
	}
	
	inline bigint operator/(bigint a,const bigint &b){
		a/=b;
		return a;
	}
	
	inline bigint operator%(bigint a,const bigint &b){
		a%=b;
		return a;
	}
	
	bigint& bigint::operator+=(const bigint &b){
		if(this==&b){
			*this*=2;
			return *this;
		}
		if(b==0){
			return *this;
		}
		if(negative!=b.negative){
			*this-=-b;
			return *this;
		}
		size_type digits=container.size();
		if(digits<b.container.size()){
			digits=b.container.size();
		}
		std::uint_least64_t rem=0;
		for(size_type k=0; k<digits; k++){
			std::uint_least64_t sum=rem+getDigit(k)+b.getDigit(k);
			rem=sum/base;
			sum%=base;
			if(k<container.size()){
				container[k]=sum;
			} else {
				container.push_back(sum);
			}
		}
		if(rem!=0){
			container.push_back(rem);
		}
		return *this;
	}
	
	bigint& bigint::operator-=(const bigint &b){
		if(this==&b){
			*this=0;
			return *this;
		}
		if(b==0){
			return *this;
		}
		if(negative!=b.negative){
			*this+=-b;
			return *this;
		}
		if(abs(*this)<abs(b)){
			*this=-(b-*this);
			return *this;
		}
		size_type digits=container.size();
		std::uint_least64_t rem=0;
		for(size_type k=0; k<digits; k++){
			std::uint_least64_t diff=base+getDigit(k)-b.getDigit(k)-rem;
			rem=1;
			if(diff>=base){
				diff-=base;
				rem=0;
			}
			container[k]=diff;
		}
		normalize();
		return *this;
	}
	
	bigint& bigint::operator*=(const bigint &b){
		if(*this==0){
			return *this;
		}
		if(b==0){
			*this=0;
			return *this;
		}
		bool sign=(negative!=b.negative);
		bigint sum=0;
		for(size_type k=0; k<b.container.size(); k++){
			bigint part;
			part.container=std::vector<std::uint32_t>(k,0);
			std::uint_least64_t rem=0;
			for(size_type j=0; j<container.size() || rem!=0; j++){
				std::uint_least64_t prod=(b.getDigit(k)*getDigit(j))+rem;
				rem=prod/base;
				prod%=base;
				part.container.push_back(prod);
			}
			part.normalize();
			sum+=part;
		}
		*this=sum;
		negative=sign;
		return *this;
	}
	
	bigint& bigint::operator/=(const bigint &b){
		if(b==0){
			throw std::domain_error("bigint: Division by zero");
		}
		if(abs(*this)<abs(b)){
			*this=0;
			return *this;
		}
		bool sign=(negative!=b.negative);
		bigint quot,partdivid;
		size_type con=b.container.size();
		quot.container.clear();
		partdivid.container=std::vector<std::uint32_t>(container.end()-con,container.end());
		con++;
		if(partdivid<b){
			partdivid.container.insert(partdivid.container.begin(),*(container.end()-con));
			con++;
		}
		while(con<=container.size()){
			std::uint_least64_t min=0;
			std::uint_least64_t max=base-1;
			while(max-min>1){
				std::uint_least64_t mid=min+(max-min)/2;
				if(partdivid-b*mid<0){
					max=mid-1;
				} else {
					min=mid;
				}
			}
			std::uint_least64_t partquot;
			if(partdivid-b*max<0){
				partquot=min;
			} else {
				partquot=max;
			}
			partdivid-=b*partquot;
			quot.container.push_back(partquot);
			partdivid.container.insert(partdivid.container.begin(),*(container.end()-con));
			partdivid.normalize();
			con++;
		}
		std::uint_least64_t min=0;
		std::uint_least64_t max=base-1;
		while(max-min>1){
			std::uint_least64_t mid=min+(max-min)/2;
			if(partdivid-b*mid<0){
				max=mid-1;
			} else {
				min=mid;
			}
		}
		std::uint_least64_t partquot;
		if(partdivid-b*max<0){
			partquot=min;
		} else {
			partquot=max;
		}
		quot.container.push_back(partquot);
		std::reverse(quot.container.begin(),quot.container.end());
		*this=quot;
		negative=sign;
		return *this;
	}
	
	bigint& bigint::operator%=(const bigint &b){
		*this=*this-(*this/b)*b;
		return *this;
	}
	
	bigint::operator std::string() const{
		std::string str;
		if(*this==0){
			str+='0';
			return str;
		}
		bigint n=*this;
		n.negative=false;
		while(n>0){
			bigint a=n/10;
			str+=(n-a*10).container[0]+'0';
			n=a;
		}
		if(negative){
			str+='-';
		}
		std::reverse(str.begin(),str.end());
		return str;
	}
	
	std::ostream& operator<<(std::ostream &os, const bigint &n){
		os << static_cast<std::string>(n);
		return os;
	}
	
	std::istream& operator>>(std::istream &is, bigint &n){
		std::string str;
		is >> str;
		try{
			n=stobi(str);
		} catch(std::invalid_argument&){
			is.setstate(std::ios::failbit);
		}
		return is;
	}
	
	std::size_t num10Digits(bigint n){
		static std::vector<bigint> lookuptable;
		bigint::size_type maxmag=((32*n.container.size()*std::log(2))/(std::log(2)+std::log(5)))+1;
		if(maxmag>lookuptable.size()){
			for(bigint::size_type con=lookuptable.size(); con<maxmag; con++){
				lookuptable.push_back(ipow<bigint>(10,con+1));
			}
		}
		n=abs(n);
		auto mag = std::upper_bound(lookuptable.begin(), lookuptable.end(), n);
		return (mag-lookuptable.begin())+1;
	}
	
}

#endif