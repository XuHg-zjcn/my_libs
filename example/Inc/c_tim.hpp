#ifndef C_TIM_HPP
#define C_TIM_HPP

typedef uint64_t Type_ns;
typedef float    Type_Hz;
typedef uint64_t TypeDiv;

class C_TIM{
public:
  	void set_Hz(Type_Hz Hz);
	void set_ns(Type_ns ns);
}
