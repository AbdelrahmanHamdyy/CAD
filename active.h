#pragma once
#include "Component.h"
#include<iostream>
using namespace std;
class active : public Component
{
public:
	active(double v, double t) :Component(v, t) {}
};
class Voltage : public active
{
public:
	Voltage(double v, double t) : active(v, t) {}
};
class Current : public active
{
public:
	Current(double v, double t) :active(v, t)
	{
	}
};
class Isrc : public Current
{
public:
	Isrc(double v, double t) : Current(v, t) {}
	void mergeWithParrarel(Component* c , bool signOfThis , bool signOfC)
	{
		if (signOfThis == signOfC)
			this->value += c->GetValue();
		else
			this->value -= c->GetValue();
	}
};
class Vsrc : public Voltage
{
public:
	Vsrc(double v, double t) :Voltage(v, t) {}
	void mergeWithParrarel(Component* c, bool x, bool y) { cout << "wrong combination\n"; }
};
class Vcvs : public Voltage
{
	int p1, p2;
public:
	Vcvs(double v, double t, int x, int y) :Voltage(v, t), p1(x), p2(y) {}
	void mergeWithParrarel(Component* c , bool x , bool y) { cout << "wrong combination\n"; }
	int getP1() { return p1; }
	int getP2() { return p2; }
};
class Cccs : public Current
{
	complex<double> Comp;
	int p1, p2;
public:
	Cccs(double v, double t, int x, int y, Component* c) :Current(v, t), Comp(c->GetValue()), p1(x), p2(y)
	{
	}
	void mergeWithParrarel(Component* c , bool signOfThis , bool signOfC)
	{

	}
	complex<double> getComponent() { return Comp; }
	int getP1() { return p1; }
	int getP2() { return p2; }
};
