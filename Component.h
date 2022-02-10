#pragma once
#include<complex>
using namespace std;
class Component
{
protected:
	complex<double> value;
	string label;
public:
	bool isWaste;
	const double pi;
	virtual int getP1() { return 0; }
	virtual int getP2() { return 0; }
	virtual complex<double> getComponent() { return NULL; }
	void setLabel(string s) { label = s; }
	string getLabel() { return label; }
	Component(double v, double t) : pi(acos(-1))
	{
		value = polar(v, t * pi / 180);
		isWaste = 0;
	}
	virtual void divide2Components(Component* x)
	{
		double val = this->GetAbs() / x->GetAbs();
		double theta = this->GetTheta() - x->GetTheta();
		this->value = polar(val, theta);
	}
	virtual double GetTheta()
	{
		return arg(value);
	}
	virtual double GetAbs() { return abs(value); }
	virtual complex<double> GetValue() { return value; }
	virtual void setPolar(double x, double y)
	{
		value = polar(x, y);
	}
	void convertFromPolarToCar(double& x, double& y)
	{
		double z = x * cos(y);
		y = x * sin(y);
		x = z;
	}
	void convertFromCarToPolar(double& x, double& y)
	{
		if (x == 0) {
			x = abs(y);
			y = (y > 0 ? 1 : -1) * 90 * pi / 180;
			return;
		}
		double z = sqrt(x * x + y * y);
		y = atan(y / x);
		x = z;
	}
	virtual void mergeWithParrarel(Component* c , bool signOfThis , bool signOfC) {};
	virtual void mergeWithSeries(Component* c) {};
};

