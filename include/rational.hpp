#ifndef RATIONAL
#define RATIONAL

#include "bigint.hpp"
#include "eulalgorithm.hpp"

namespace eul{
	
	class rational {
		
		bigint numerator;
		bigint denominator;
		
		void normalize(){
			bigint d=gcd(numerator,denominator);
			numerator/=d;
			denominator/=d;
			numerator = (numerator>0 && denominator>=0)||(numerator<0 && denominator<0) ? abs(numerator) : -abs(numerator);
			denominator=abs(denominator);
		}
		
	public:
	
		// Constructors
		rational() : numerator{0}, denominator{1} {}
		
		template<integral T>
		rational(T n){
			if(n==0){
				container.push_back(0);
				return;
			}
			uintmax_t l;
			if(n<0){
				negative=true;
				l=static_cast<uintmax_t>(-(n+1))+1;
			} else {
				l=n;
			}
			while(l>0){
				container.push_back(l%base);
				l/=base;
			}
		}
		
		template<std::floating_point T>
		explicit rational(T n){
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
	
		// Unary arithmetic operators
		rational operator+() const {return *this;}
		rational operator-() const {
			rational flip=*this;
			flip.numerator=-flip.numerator;
			return flip;
		}
		friend inline rational abs(rational n) {n.numerator=abs(n.numerator); return n;}
		
		// Comparison operators
		friend bool operator==(const rational &a,const rational &b) = default;
		friend std::strong_ordering operator<=>(const rational &a,const rational &b);
		
		// Compound assignment operators
		rational& operator+=(const rational &b);
		rational& operator-=(const rational &b);
		rational& operator*=(const rational &b);
		rational& operator/=(const rational &b);
		rational& operator%=(const rational &b);
		
		// Increment/decrement
		rational& operator++(){*this+=1; return *this;}
		rational& operator--(){*this-=1; return *this;}
		rational operator++(int) {rational old=*this; ++*this; return old;}
		rational operator--(int) {rational old=*this; --*this; return old;}
		
		// Conversion functions
		inline explicit operator bool() const;
		explicit operator std::string() const;
		
		friend rational stobi(const std::string &n);
		
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
	
	rational stobi(const std::string &str){
		rational res;
		
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
		while(n.size()>rational::digits10base || std::stoull(std::string(n,0,rational::digits10base))>=rational::base){
			std::string quot;
			rational::size_type con=rational::digits10base;
			rational::uint_least64_t partdivid=std::stoull(std::string(n,0,rational::digits10base));
			if(partdivid<rational::base){
				partdivid=partdivid*10+(n[con]-'0');
				con++;
			}
			while(con<n.size()){
				quot+=partdivid/rational::base+'0';
				partdivid=(partdivid%rational::base)*10+(n[con]-'0');
				con++;
			}
			quot+=partdivid/rational::base+'0';
			partdivid%=rational::base;
			res.container.push_back(partdivid);
			n=quot;
		}
		res.container.push_back(std::stoull(n));
		
		return res;
	}
	
	inline rational operator"" _bi (const char *n){
		std::string str=n;
		if(str.size()<=std::numeric_limits<unsigned long long>::digits10){
			return rational(std::stoull(str));
		}
		return stobi(str);
	}
		
	std::strong_ordering operator<=>(const rational &a,const rational &b){
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
	
	inline rational::operator bool() const{
		return *this!=0_bi;
	}
	
	inline rational operator+(rational a,const rational &b){
		a+=b;
		return a;
	}
	
	inline rational operator-(rational a,const rational &b){
		a-=b;
		return a;
	}
	
	inline rational operator*(rational a,const rational &b){
		a*=b;
		return a;
	}
	
	inline rational operator/(rational a,const rational &b){
		a/=b;
		return a;
	}
	
	inline rational operator%(rational a,const rational &b){
		a%=b;
		return a;
	}
	
	rational& rational::operator+=(const rational &b){
		if(this==&b){
			*this*=2_bi;
			return *this;
		}
		if(b==0_bi){
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
		uint_least64_t rem=0;
		for(size_type k=0; k<digits; k++){
			uint_least64_t sum=rem+getDigit(k)+b.getDigit(k);
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
	
	rational& rational::operator-=(const rational &b){
		if(this==&b){
			*this=0_bi;
			return *this;
		}
		if(b==0_bi){
			return *this;
		}
		if(negative!=b.negative){
			*this+=-b;
			return *this;
		}
		if(biabs(*this)<biabs(b)){
			*this=-(b-*this);
			return *this;
		}
		size_type digits=container.size();
		uint_least64_t rem=0;
		for(size_type k=0; k<digits; k++){
			uint_least64_t diff=base+getDigit(k)-b.getDigit(k)-rem;
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
	
	rational& rational::operator*=(const rational &b){
		if(*this==0_bi){
			return *this;
		}
		if(b==0_bi){
			*this=0_bi;
			return *this;
		}
		bool sign=(negative!=b.negative);
		rational sum=0_bi;
		for(size_type k=0; k<b.container.size(); k++){
			rational part;
			part.container=std::vector<uint32_t>(k,0);
			uint_least64_t rem=0;
			for(size_type j=0; j<container.size() || rem!=0; j++){
				uint_least64_t prod=(b.getDigit(k)*getDigit(j))+rem;
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
	
	rational& rational::operator/=(const rational &b){
		if(b==0_bi){
			throw std::domain_error("rational: Division by zero");
		}
		if(biabs(*this)<biabs(b)){
			*this=0_bi;
			return *this;
		}
		bool sign=(negative!=b.negative);
		rational quot,partdivid;
		size_type con=b.container.size();
		quot.container.clear();
		partdivid.container=std::vector<uint32_t>(container.end()-con,container.end());
		con++;
		if(partdivid<b){
			partdivid.container.insert(partdivid.container.begin(),*(container.end()-con));
			con++;
		}
		while(con<=container.size()){
			uint_least64_t min=0;
			uint_least64_t max=base-1;
			while(max-min>1){
				uint_least64_t mid=min+(max-min)/2;
				if(partdivid-b*mid<0_bi){
					max=mid-1;
				} else {
					min=mid;
				}
			}
			uint_least64_t partquot;
			if(partdivid-b*max<0_bi){
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
		uint_least64_t min=0;
		uint_least64_t max=base-1;
		while(max-min>1){
			uint_least64_t mid=min+(max-min)/2;
			if(partdivid-b*mid<0_bi){
				max=mid-1;
			} else {
				min=mid;
			}
		}
		uint_least64_t partquot;
		if(partdivid-b*max<0_bi){
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
	
	rational& rational::operator%=(const rational &b){
		*this=*this-(*this/b)*b;
		return *this;
	}
	
	rational::operator std::string() const{
		std::string str;
		if(*this==0_bi){
			str+='0';
			return str;
		}
		rational n=*this;
		n.negative=false;
		while(n>0_bi){
			str+=(n%10_bi).container[0]+'0';
			n/=10_bi;
		}
		if(negative){
			str+='-';
		}
		std::reverse(str.begin(),str.end());
		return str;
	}
	
	std::ostream& operator<<(std::ostream &os, const rational &n){
		os << static_cast<std::string>(n);
		return os;
	}
	
	std::istream& operator>>(std::istream &is, rational &n){
		std::string str;
		is >> str;
		try{
			n=stobi(str);
		} catch(std::invalid_argument&){
			is.setstate(std::ios::failbit);
		}
		return is;
	}
	
}

#endif