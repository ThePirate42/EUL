#ifndef EULALGORITHM
#define EULALGORITHM

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>
#include "typesupport.hpp"

namespace eul{
	
	template <std::integral T>
	constexpr auto abs(T n){
		return n<0 ? static_cast<std::make_unsigned_t<T>>(static_cast<std::make_unsigned_t<T>>(-(n+1))+1) : static_cast<std::make_unsigned_t<T>>(n);
	}
	
	template <integral T,integral U>
	constexpr auto gcd(T m,U n){
		auto a=abs(m)>abs(n)?abs(m):abs(n);
		auto b=abs(m)>abs(n)?abs(n):abs(m);
		while(b>0){
			auto c=a%b;
			a=b;
			b=c;
		}
		return a;
	}
	
	template <integral L, integral T, integral U>
	constexpr L ipow(const T& base, U exp){
		if(base == 0 && exp == 0){
			throw std::domain_error("ipow: Base and exponent are 0");
		}
		if(exp < 0){
			throw std::domain_error("ipow: Exponent is negative");
		}
		if(exp == 0){
			return 1;
		}
		L y = 1;
		L x = base;
		while(exp > 1){
			if(!(exp % 2)){
				x *= x;
				exp /= 2;
			} else {
				y *= x;
				x *= x;
				exp = (exp - 1) / 2;
			}
		}
		return x * y;
	}
	
	template <class T,std::size_t N,class U>
	constexpr auto arrayinitializer(U f){
		std::array<T,N> array;
		for(std::size_t i=0; i<N; i++){
			array[i]=f(i);
		}
		return array;
	}
	
	template <std::integral T>
	constexpr std::size_t num10Digits(T n){
		constexpr auto lookuptable = arrayinitializer<T,std::numeric_limits<T>::digits10>([](std::size_t exp)constexpr{return ipow<T>(10,exp+1);});
		auto k=abs(n);
		auto mag = std::upper_bound(lookuptable.begin(), lookuptable.end(), k);
		return (mag-lookuptable.begin())+1;
	}
	
	template <integral T>
	#ifdef ALTISQRT
	constexpr T isqrt(T n){
	#else
	constexpr T isqrtalt(T n){
	#endif
		if(n<0){
			throw std::domain_error("isqrt: Input is negative");
		}
		if(n==1){
			return 1;
		}
		T min=0;
		T max=n/2;
		while(max-min>1){
			T mid=min+(max-min)/2;
			if(mid*mid>n){
				max=mid-1;
			} else {
				min=mid;
			}
		}
		if(max*max<=n){
			return max;
		} else {
			return min;
		}
	}
	
	template <integral T>
	#ifdef ALTISQRT
	T isqrtalt(T n){
	#else
	T isqrt(T n){
	#endif
		if(n<0){
			throw std::domain_error("isqrtalt: Input is negative");
		}
		std::vector<T> c;
		for(T d = n; d>0; d/=4){
			c.push_back(d);
		}
		T res = 0;
		for(std::size_t con=c.size(); con>0; con--){
			T a = 2*res+1;
			res = c[con-1]<(a*a) ? a-1 : a;
		}
		return res;
	}
	
	template <integral T>
	bool isPrime(T cand, const std::vector<T> &primes){
		if(cand<3){
			throw std::invalid_argument("isPrime: Candidate is less than 3");
		}
		bool prime=true;
		T bound=isqrt(cand);
		for(std::size_t con=0;; con++){
			if(con>=primes.size()){
				throw std::invalid_argument("isPrime: The list is too short");
			}
			if(primes[con]>bound){
				break;
			}
			if(cand%primes[con]==0){
				prime=false;
				break;
			}
		}
		return prime;
	}
	
	template <integral T>
	void generatePrimesLimit(T limit, std::vector<T> &primes){
		if(primes.size()<3){
			primes={2,3,5};
		}
		if(limit-primes.back()<=1){
			return;
		}
		T cand;
		if((primes.back()+2)%3==0){
			cand=primes.back()+4;
		} else {
			if(isPrime(primes.back()+2,primes)){
				primes.push_back(primes.back()+2);
				cand=primes.back()+4;
			} else {
				cand=primes.back()+6;
			}
		}
		for(; cand<=limit; cand+=4){
			if(isPrime(cand,primes)){
				primes.push_back(cand);
			}
			cand+=2;
			if(cand<=limit){
				if(isPrime(cand,primes)){
					primes.push_back(cand);
				}
			}
		}
	}
	
	template <integral T>
	void generatePrimesIndex(std::size_t index, std::vector<T> &primes){
		if(primes.size()<3){
			primes={2,3,5};
		}
		if(index<=primes.size()){
			return;
		}
		T cand;
		if((primes.back()+2)%3==0){
			cand=primes.back()+4;
		} else {
			if(isPrime(primes.back()+2,primes)){
				primes.push_back(primes.back()+2);
				cand=primes.back()+4;
			} else {
				cand=primes.back()+6;
			}
		}
		for(; primes.size()<index; cand+=4){
			if(isPrime(cand,primes)){
				primes.push_back(cand);
			}
			cand+=2;
			if(primes.size()<index){
				if(isPrime(cand,primes)){
					primes.push_back(cand);
				}
			}
		}
	}
	
	template <integral U,integral T>
	auto primeFactors(T n, const std::vector<T> &primes){
		if(n<2){
			throw std::domain_error("primeFactors: Input is less than 2");
		}
		std::vector<std::tuple<T,U>> pf;
		for(std::size_t con=0; n!=1; con++){
			std::tuple<T,U> cand(primes[con],0);
			auto& [base,exp] = cand;
			while(n%base==0){
				n/=base;
				exp++;
			}
			if(exp>0){
				pf.push_back(cand);
			}
		}
		return pf;
	}
	
}

#endif
