#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include <climits>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <functional>
#include <numeric>
#include <ostream>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// A complexity class that orders like O(1) < O(log n) < O(n) < ... and prints like a string.
//
//   Complexity c = estimate_complexity("vector", merge_sort);
//   cout << c;                        // O(n log n)
//   if (c <= "O(n log n)") ...        // compare with a name ("nlogn", "n^2", "O(n)" all work)
//   if (c == Complexity::Linear) ...  // or with a named constant
//   string s = c;                     // converts to std::string
//
// If estimation failed, ok() is false, printing shows the reason, and it compares
// below every real complexity.
class Complexity
{
public:
	enum Kind { Invalid=-1, Constant, Log, Linear, NLogN, Quadratic, Cubic };

	Complexity(Kind k=Invalid):k_(k){}

	// From a name. Throws std::invalid_argument on an unknown name so typos don't
	// silently compare false.
	Complexity(const std::string& name):k_(parse(name))
	{
		if(k_==Invalid) throw std::invalid_argument("unknown complexity \""+name+"\"");
	}
	Complexity(const char* name):Complexity(std::string(name)){}

	static Complexity error(std::string reason)
	{
		Complexity c;
		c.msg_=std::move(reason);
		return c;
	}

	Kind kind() const { return k_; }
	bool ok() const { return k_!=Invalid; }

	std::string str() const
	{
		static const char* names[]={"O(1)","O(log n)","O(n)","O(n log n)","O(n^2)","O(n^3)"};
		if(!ok()) return msg_.empty()?"unknown":msg_;
		return names[k_];
	}
	operator std::string() const { return str(); }

	friend bool operator==(const Complexity& a,const Complexity& b){ return a.k_==b.k_; }
	friend bool operator!=(const Complexity& a,const Complexity& b){ return a.k_!=b.k_; }
	friend bool operator< (const Complexity& a,const Complexity& b){ return a.k_< b.k_; }
	friend bool operator<=(const Complexity& a,const Complexity& b){ return a.k_<=b.k_; }
	friend bool operator> (const Complexity& a,const Complexity& b){ return a.k_> b.k_; }
	friend bool operator>=(const Complexity& a,const Complexity& b){ return a.k_>=b.k_; }

	friend std::ostream& operator<<(std::ostream& os,const Complexity& c){ return os<<c.str(); }
	friend std::string operator+(const std::string& s,const Complexity& c){ return s+c.str(); }
	friend std::string operator+(const Complexity& c,const std::string& s){ return c.str()+s; }

private:
	Kind k_;
	std::string msg_;

	// Accepts "O(n log n)", "nlogn", "n log n", "O(N^2)", "n2", "1", "logn", ...
	static Kind parse(const std::string& raw)
	{
		std::string s;
		for(char ch:raw)
			if(!std::isspace((unsigned char)ch) && ch!='*')
				s+=(char)std::tolower((unsigned char)ch);
		if(s.size()>=3 && s[0]=='o' && s[1]=='(' && s.back()==')')
			s=s.substr(2,s.size()-3);

		if(s=="1") return Constant;
		if(s=="logn"||s=="lgn") return Log;
		if(s=="n") return Linear;
		if(s=="nlogn"||s=="nlgn") return NLogN;
		if(s=="n^2"||s=="n2"||s=="nn") return Quadratic;
		if(s=="n^3"||s=="n3"||s=="nnn") return Cubic;
		return Invalid;
	}
};

// Guesses the time complexity of `algo` by timing it on inputs of doubling size.
// gen(n)       -> builds an input of size n (not timed)
// algo(input&) -> runs the algorithm on that input (timed)
// Sizes double from startN up to maxN, or until one run exceeds timeLimitMs.
template<class Gen,class Algo>
Complexity estimate_complexity(Gen gen,Algo algo,int startN=1000,int maxN=1<<20,
                               int reps=5,double timeLimitMs=500)
{
	std::vector<double> ns,ts;

	for(long long n=startN;n<=maxN;n*=2)
	{
		auto input=gen((int)n);
		std::vector<double> times;
		for(int r=0;r<reps;r++)
		{
			auto copy=input;
			auto st=std::chrono::steady_clock::now();
			algo(copy);
			auto en=std::chrono::steady_clock::now();
			times.push_back(std::chrono::duration<double,std::milli>(en-st).count());
		}
		std::sort(times.begin(),times.end());
		double med=times[reps/2];     // median is robust to outliers

		ns.push_back((double)n);
		ts.push_back(med);
		if(med>timeLimitMs) break;    // stop before slow algorithms take forever
	}

	if(ns.size()<3) return Complexity::error("not enough data points");

	std::vector<std::pair<Complexity::Kind,std::function<double(double)>>> cands={
		{Complexity::Constant,  [](double){return 1.0;}},
		{Complexity::Log,       [](double n){return std::log2(n);}},
		{Complexity::Linear,    [](double n){return n;}},
		{Complexity::NLogN,     [](double n){return n*std::log2(n);}},
		{Complexity::Quadratic, [](double n){return n*n;}},
		{Complexity::Cubic,     [](double n){return n*n*n;}}
	};

	// Drop the smallest sizes: they're dominated by timer noise and cache effects.
	int skip=ns.size()>=6?2:0;

	// For the right complexity f, T(n)/f(n) is roughly constant.
	// Measure that with the coefficient of variation and pick the smallest.
	Complexity::Kind best=Complexity::Invalid;
	double bestCV=1e18;
	for(auto &cand:cands)
	{
		std::vector<double> r;
		for(int i=skip;i<(int)ns.size();i++)
			r.push_back(ts[i]/cand.second(ns[i]));

		double mean=std::accumulate(r.begin(),r.end(),0.0)/r.size();
		double var=0;
		for(double x:r) var+=(x-mean)*(x-mean);
		double cv=std::sqrt(var/r.size())/mean;

		if(cv<bestCV) bestCV=cv,best=cand.first;
	}
	return Complexity(best);
}

// Convenience generator: vector of n random ints in [-1e6, 1e6].
inline std::vector<int> random_int_vector(int n)
{
	static std::mt19937 rng(696967);
	std::vector<int> a(n);
	std::uniform_int_distribution<int> dist(-1000000,1000000);
	for(int &x:a) x=dist(rng);
	return a;
}

// Convenience generator: string of n random lowercase letters.
inline std::string random_string(int n)
{
	static std::mt19937 rng(424242);
	std::string s(n,'a');
	std::uniform_int_distribution<int> dist(0,25);
	for(char &c:s) c=char('a'+dist(rng));
	return s;
}

// Shorthand: name the input type instead of writing a generator.
//   "vector" -> random std::vector<int> of size n  (algo takes std::vector<int>&)
//   "string" -> random lowercase std::string of length n  (algo takes std::string&)
//   "int"    -> the number n itself  (algo takes int&), e.g. loops to n, sieves
// Example: estimate_complexity("vector", merge_sort);
template<class Algo>
Complexity estimate_complexity(const char* kind,Algo algo,int startN=1000,int maxN=1<<20,
                               int reps=5,double timeLimitMs=500)
{
	std::string k=kind;

	if(k=="vector")
	{
		if constexpr(std::is_invocable_v<Algo&,std::vector<int>&>)
			return estimate_complexity(random_int_vector,algo,startN,maxN,reps,timeLimitMs);
		else
			return Complexity::error("error: \"vector\" needs a function taking std::vector<int>&");
	}
	if(k=="string")
	{
		if constexpr(std::is_invocable_v<Algo&,std::string&>)
			return estimate_complexity(random_string,algo,startN,maxN,reps,timeLimitMs);
		else
			return Complexity::error("error: \"string\" needs a function taking std::string&");
	}
	if(k=="int")
	{
		if constexpr(std::is_invocable_v<Algo&,int&>)
			return estimate_complexity([](int n){return n;},algo,startN,maxN,reps,timeLimitMs);
		else
			return Complexity::error("error: \"int\" needs a function taking int&");
	}
	return Complexity::error("error: unknown input type \""+k+"\" (use \"vector\", \"string\" or \"int\")");
}

// Short names:  estimate("vector", merge_sort) < complexity("n^2")
using complexity=Complexity;

template<class... Args>
Complexity estimate(Args&&... args)
{
	return estimate_complexity(std::forward<Args>(args)...);
}

// Returns true if the sort function passes both correctness and complexity checks
// Returns true if the sort function passes both correctness and complexity checks
inline bool test_sort(void (*sort_fn)(std::vector<int>&), const std::string &desired_comp)
{
	std::mt19937 rng(12345);

	// Correctness tests
	std::cout<<"Checking Correctness...."<<std::endl;
	std::vector<std::vector<int>> tests={
		{}, {1}, {2,1}, {1,2}, {1,1,1,1},
		{1,2,3,4,5}, {5,4,3,2,1},
		{5,1,5,2,5,3,5},
		{-5,-1,-3,-2,-4},
		{INT_MIN,0,INT_MAX,-1,1}
	};

	std::uniform_int_distribution<int> dist(-100000,100000);
	for(int t=0;t<100;t++)
	{
		int n=rng()%100;
		std::vector<int> a(n);
		for(int &x:a)
			x=dist(rng);
		tests.push_back(a);
	}

	for(int tc=0;tc<(int)tests.size();tc++)
	{
		std::vector<int> a=tests[tc],b=a;
		std::sort(b.begin(),b.end());
		sort_fn(a);

		if(a!=b)
		{
			std::cout<<"FAILED test "<<tc<<std::endl;
			return false;
		}
	}
	std::cout<<"All correctness tests passed!\n\n";

	// Time complexity test
	std::cout<<"Checking time complexity...."<<std::endl;
	Complexity cmp=estimate_complexity("vector",sort_fn);
	Complexity desired=Complexity(desired_comp);
	if(cmp<=desired)
	{
		std::cout<<"Time complexity correct!"<<std::endl;
		return true;
	}
	std::cout<<"Failed Time Complexity test: found to be "<<cmp<<std::endl;
	return false;
}

#endif // COMPLEXITY_H
