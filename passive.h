#pragma once
#include "Component.h"
class passive : public Component
{
public:
	passive(double v, double t) : Component(v, t) {}
	virtual void mergeWithParrarel(Component* c , bool signOfThis , bool signOfC)
	{
		/*double val2 = c->GetValue(), t2 = c->GetTheta();
		double val1 = this->value, t1 = this->theta;
		convertFromPolarToCar(val1, t1);
		convertFromPolarToCar(val2, t2);
		double val3 = val1 + val2;
		double t3 = t1 + t2;
		convertFromCarToPolar(val3, t3);
		this->setValue(c->GetValue() * this->GetValue() / val3);
		this->setTheta(c->GetTheta() + this->GetTheta() - t3);*/
		value = (this->value * c->GetValue()) / (this->value + c->GetValue());
	}
	virtual void mergeWithSeries(Component* c)
	{
		/*double val2 = c->GetValue(), t2 = c->GetTheta();
		double val1 = this->value, t1 = this->theta;
		convertFromPolarToCar(val1, t1);
		convertFromPolarToCar(val2, t2);
		double val3 = val1 + val2;
		double t3 = t1 + t2;
		convertFromCarToPolar(val3, t3);
		this->setValue(val3);
		this->setTheta(t3);*/
		this->value += c->GetValue();
	}
};
class Cap : public passive
{
public:
	Cap(double v) : passive(v, -90) {}
};
class Res : public passive
{
public:
	Res(double v) :passive(v, 0) {}
};
class Ind : public passive
{
public:
	Ind(double v) : passive(v, 90) {}
};

