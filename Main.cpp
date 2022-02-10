#include<iostream>
#include<fstream>
#include<cmath>
#include<string>
#include<vector>
#include "passive.h"
#include "active.h"
#include<complex>
#include <Eigen>
#include <fstream>
#include<algorithm>
#include<iomanip>
using namespace std;
using namespace Eigen;
#define signedComponent pair<Component* ,bool>
enum CountList
{
	maxCount = 20,
};
enum AllComponents
{
	VoltageSource,
	CurrentSource,
	DependantVolSource,
	DependantCurSource,
	Passive,
	CountAllComponentsTypes
};
vector<int> nodes[maxCount];
bool SeriesNodes[maxCount];
bool ifValidNode[maxCount];
bool ifSuperNodeMade[maxCount][maxCount];
bool superNodeFinished[maxCount];
int rowNum = 0;
Matrix<complex<double>, Dynamic, Dynamic> equations;
Matrix <complex<double>, Dynamic, 1> result;
Matrix<complex<double>, Dynamic, 1> solution;
Matrix<complex<double>, Dynamic, Dynamic> equations2;
Matrix <complex<double>, Dynamic, 1> result2;
Matrix<complex<double>, Dynamic, 1> solution2;

vector<signedComponent> Comp[maxCount][maxCount];
double resultEquations[maxCount];
int CompCount = 0;
int nodeCount = 0;
double w;
void simplifyParrarel();
void mergeComponents(vector<signedComponent> vr);
bool superNodes[maxCount][maxCount];
void setSuperNodes();
void setEquationsForNode(int x);
void setOrdinaryEquationsBetweenTwoNodes(int x, int y, int setIn);
void setSuperNodeEquationsBetweenTwoNodes(int x, int y, int setIn);
void solveEquations();
/*void simplifySeries();
Component* ConvertFromVolSrcToCurrSrc(Component* vol, Component* res);
Component* ConvertFromCurrSrcToVolSrc(Component* curr, Component* res);
int catchTheBeginning(int x);
pair<Component*, Component*> merge2Series(pair<Component*, Component*>, pair<Component*, Component*>);
pair<Component* , Component*> makeSeriesCompination(int x , int e );*/
int main()
{
	ifstream f;
	f.open("Circuits.txt");
	string s;
	int N1, N2;
	double value , phase;
	nodeCount = 0;
	while (!f.eof()) {
		f >> s;
		if (s == "w") {

			f >> w;
		}
		else if (s == "res") {
			f >> s;
			f >> N1 >> N2 >> value;
			Component* temp = new Res(value);
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp , 1 });
		}
		else if (s == "vsrc") {
			f >> s;
			f >> N1 >> N2 >> value >> phase;
			Component* temp = new Vsrc(value, phase);
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp , 0 });
		}
		else if (s == "cccs") {
			f >> s;
			int N3, N4;
			string l;
			Component* c = NULL;
			f >> N1 >> N2 >> N3 >> N4 >> l >> value;
			for (int i = 0; i < nodeCount; i++)
			{
				for (int j = 0; j < nodeCount; j++)
				{
					for (auto k : Comp[i][j])
					{
						if (k.first->getLabel() == l)
							c = k.first;
					}
				}
			}
			Component* temp = new Cccs(value, 0, N3, N4, c);
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp ,0 });
		}
		else if (s == "cap") {
			f >> s;
			f >> N1 >> N2 >> value;
			Component* temp = new Cap(1 /(value * w));
			cout << 1 / (value * w) << endl;
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp , 1 });
		}
		else if (s == "ind") {
			f >> s;
			f >> N1 >> N2 >> value;
			Component* temp = new Ind(value * w);
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp , 1 });
		}
		else if (s == "isrc") {

			f >> s;
			f >> N1 >> N2 >> value >> phase;
			Component* temp = new Isrc(value, phase);
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp , 0 });
		}
		else if (s == "vcvs") {
			int N3, N4;
			f >> s;
			f >> N1 >> N2 >> N3 >> N4 >> value;
			Component* temp = new Vcvs(value, 0, N3, N4);
			temp->setLabel(s);
			Comp[N1][N2].push_back({ temp , 1 });
			Comp[N2][N1].push_back({ temp , 0 });
		}
		nodeCount = max(nodeCount, max(N1, N2));
	}
	nodeCount++;
	equations.resize(nodeCount, nodeCount);
	result.resize(nodeCount, 1);
	solution.resize(nodeCount, 1);
	equations2.resize(nodeCount - 1, nodeCount - 1);
	result2.resize(nodeCount - 1, 1);
	solution2.resize(nodeCount - 1, 1);
	for (int i = 0; i < nodeCount; i++)
	{
		result(i, 0) = polar(0, 0);
		for (int j = 0; j < nodeCount; j++)
		{
			equations(i, j) = polar(0 , 0);
		}
	}
	cout << equations.size() << endl << endl;
	setSuperNodes();
	simplifyParrarel();
	for (int i = 1; i < nodeCount; i++)
	{
		if(!superNodeFinished[i])
			setEquationsForNode(i);
	}
	for (int i = 1; i < nodeCount; i++)
	{
		result2(i - 1, 0) = result(i  , 0);
		cout << "result " << i << "  " << result(i, 0) << endl;
		for (int j = 1; j < nodeCount; j++)
		{
			cout << "{ " << i << " , " << j << " } " << equations(i, j) << endl;
			equations2(i - 1, j - 1) = equations(i, j);
		}
	}
	solveEquations();
	cout << equations2 << endl;
	cout << result2 << endl;
	cout << "Output\n";
	cout << "--------------------------------------------------------------------------------------------------\n";
	cout << solution2 << endl;
	cout << "--------------------------------------------------------------------------------------------------\n";
	for (int i = 0; i < nodeCount - 1; i++)
	{
		cout << fixed << setprecision(4) <<  "V" << i + 1 << " : " << abs(solution2(i, 0)) << ", " << arg(solution2(i, 0)) * 180 / acos(-1) << endl;
	}
	return 0;
}
void setOrdinaryEquationsBetweenTwoNodes(int x, int y, int setIn)
{
	for (auto i : Comp[x][y])
	{
		if (!i.first->isWaste) {
			if (dynamic_cast<passive*>(i.first) != NULL)
			{
				if (y != 0)
					equations(setIn, y) = equations(setIn, y) - polar(1.0, 0.0) / i.first->GetValue(); //edit1
				equations(setIn, x) = equations(setIn, x) + polar(1.0, 0.0) / i.first->GetValue();
			}
			else
			{
				if (dynamic_cast<Cccs*>(i.first) != NULL)
				{
					if (i.first->getP1() != 0)
						equations(setIn, i.first->getP1()) = equations(setIn, i.first->getP1()) - (-1.0 + (i.second / 1 * 2)) * i.first->GetValue() / i.first->getComponent();
					if (i.first->getP2() != 0)
						equations(setIn, i.first->getP2()) = equations(setIn, i.first->getP2()) + (-1.0 + (i.second / 1 * 2)) * i.first->GetValue() / i.first->getComponent();
				}
				else
				{
					result(setIn, 0) = result(setIn, 0) + (-1.0 + (i.second / 1 * 2)) * i.first->GetValue();
				}
			}
		}
		cout << setIn << " " << y << " " << equations(setIn, y).real() << " " << equations(setIn, y).imag() << endl << setIn << " " << x << " " << equations(setIn, x).real() << " " << equations(setIn, x).imag() << endl;
		cout << equations(setIn, i.first->getP1()) << " " << equations(setIn, i.first->getP2()) << endl;
	}
}
void setSuperNodeEquationsBetweenTwoNodes(int x, int y, int setIn)
{
	if (ifSuperNodeMade[x][y]) return;
	superNodeFinished[x] = 1;
	superNodeFinished[y] = 1;
	ifSuperNodeMade[x][y] = 1;
	ifSuperNodeMade[y][x] = 1;
	if (y == 0)
	{
		for (int i = 0; i < nodeCount; i++)
			equations(setIn, i) = polar(0, 0);
		equations(setIn, x) = polar(1.0, 0.0);
		for (auto k : Comp[x][y])
		{
			if (dynamic_cast<Vsrc*> (k.first) != NULL)
			{
				result(setIn, 0) = (-1.0 + ((k.second / 1) * 2)) * k.first->GetValue();
				cout << result(setIn, 0) << endl;
			}
			else if(dynamic_cast<Vcvs*>(k.first) != NULL)
			{
				equations(setIn, k.first->getP1()) = equations(setIn, k.first->getP1()) - (-1.0 + ((k.second / 1) * 2)) * k.first->GetValue();
				equations(setIn, k.first->getP2()) = equations(setIn, k.first->getP2()) + (-1.0 + ((k.second / 1) * 2)) * k.first->GetValue();			
			}
		}
		return;
	}
	for (auto k : Comp[x][y])
	{
		if (dynamic_cast<Vsrc*> (k.first) != NULL)
		{
			equations(y, x) = equations(y, x) + polar(1.0, 0.0);
			equations(y, y) = equations(y, y) - polar(1.0, 0.0);
			result(y, 0) = result(y, 0) + (-1.0 + (k.second / 1 * 2)) * k.first->GetValue();
			cout << y << " " << y << " " << equations(y, x).real() << " " << equations(y, x).imag() << endl << y << " " << y << " " << equations(y, y).real() << " " << equations(y, y).imag() << endl;
			for (int i = 0; i < nodeCount; i++)
			{
				if (!superNodes[y][i])
				{
					setOrdinaryEquationsBetweenTwoNodes(y, i, setIn);
				}
			}
			for (int i = 0; i < nodeCount; i++)
			{
				if (superNodes[y][i])
					setSuperNodeEquationsBetweenTwoNodes(y, i, setIn);
			}
		}
		else if (dynamic_cast<Vcvs*>(k.first) != NULL)
		{
			equations(y, x) = equations(y, x) + polar(1.0, 0.0);
			equations(y, y) = equations(y, y) - polar(1.0, 0.0);
			equations(y, k.first->getP1()) = equations(y, k.first->getP1()) - (-1.0 + ((k.second / 1) * 2)) * k.first->GetValue();
			equations(y, k.first->getP2()) = equations(y, k.first->getP2()) + (-1.0 + ((k.second / 1) * 2)) * k.first->GetValue();
			for (int i = 0; i < nodeCount; i++)
			{
				if (!superNodes[y][i])
				{
					setOrdinaryEquationsBetweenTwoNodes(y, i, setIn);
				}
			}
			for (int i = 0; i < nodeCount; i++)
			{
				if (superNodes[y][i])
					setSuperNodeEquationsBetweenTwoNodes(y, i, setIn);
			}
		}
	}
}
void solveEquations()
{
	solution2 = equations2.inverse() * result2;
}
void setSuperNodes()
{
	for (int i = 0; i < nodeCount; i++)
	{
		for (int j = 0; j < nodeCount; j++)
		{
			for (auto k : Comp[i][j])
			{
				if (dynamic_cast<Voltage*>(k.first) != NULL)
				{
					superNodes[i][j] = 1;
				}
			}
		}
	}
}
void setEquationsForNode(int x)
{
	//cout << "iam here\n";
	for (int i = 0; i < nodeCount; i++)
	{
		if (!superNodes[x][i])
		{
			setOrdinaryEquationsBetweenTwoNodes(x, i, x);
		}
	}
	for (int i = 0; i < nodeCount; i++)
	{
		if (superNodes[x][i])
			setSuperNodeEquationsBetweenTwoNodes(x, i, x);
	}
}
void simplifyParrarel()
{
	for (int i = 0; i < nodeCount; i++)
	{
		for (int j = i + 1; j < nodeCount; j++)
		{
			vector<signedComponent> All3Component[CountAllComponentsTypes];
			for (auto k : Comp[i][j])
			{
				if (dynamic_cast<Voltage*> (k.first) != NULL)
				{
					All3Component[VoltageSource].push_back(k);
				}
				else if (dynamic_cast<passive*>(k.first) != NULL)
				{
					All3Component[Passive].push_back(k);
				}
				else if (dynamic_cast<Isrc*>(k.first) != NULL)
				{
					All3Component[CurrentSource].push_back(k);
				}
			}
			for (int i = 0; i < CountAllComponentsTypes; i++)
			{
				if (All3Component[i].size() > 1)
				{
					mergeComponents(All3Component[i]);
				}
			}
		}
	}
}
void mergeComponents(vector<signedComponent> vr)
{
	signedComponent merged = vr[0];
	for (int i = 1; i < vr.size(); i++)
	{
		merged.first->mergeWithParrarel(vr[i].first , merged.second ,  vr[i].second);
		vr[i].first->isWaste = 1;
	}
}
/*
void simplifySeries()
{
	for (int i = 0; i < nodeCount; i++)
		SeriesNodes[i] = 0;
	for (int i = 0; i < nodeCount; i++)
	{
		if (nodes[i].size() == 2)
		{
			if (!SeriesNodes[i]) {
				SeriesNodes[i] = 1;
				int beginning = catchTheBeginning(nodes[i][0]);
				int end = catchTheBeginning(nodes[i][1]);
				makeSeriesCompination(beginning, end);
			}
		}
	}
}
int catchTheBeginning(int x)
{
	SeriesNodes[x] = 1;
	if (nodes[x].size() != 2)
		return x;
	else
	{
		if (!SeriesNodes[nodes[x][0]])
		{
			return catchTheBeginning(nodes[x][0]);
		}
		else if (!SeriesNodes[nodes[x][1]])
		{
			return catchTheBeginning(nodes[x][1]);
		}
		return x;
	}
}
pair<Component*, Component*> merge2Series(pair<Component*, Component*> p1, pair<Component*, Component*> p2)
{
	pair<Component*, Component*> result;
	if (dynamic_cast<passive*>(p1.first) != NULL)
	{
		if (dynamic_cast<Current*>(p1.second) != NULL)
		{
			p1.second = ConvertFromCurrSrcToVolSrc(p1.first, p1.second);
		}
	}
	if (dynamic_cast<passive*>(p2.first) != NULL)
	{
		if (dynamic_cast<Current*>(p2.second) != NULL)
		{
			p2.second = ConvertFromCurrSrcToVolSrc(p2.first, p2.second);
		}
	}
	if (p1.first != NULL)
	{
		if (p2.first != NULL)
		{
			p1.first->mergeWithSeries(p2.first);
		}
	}
	else if(p2.first )
	{
		if()
	}
}
pair<Component*, Component*> makeSeriesCompination(int x, int e)
{
	if (x == e) {
		Component* pas = new passive(0, 0);
		Component* act = new active(0, 0);
		return { pas , act };
	}
	SeriesNodes[x] = 0;
	if (SeriesNodes[nodes[x][0]])
	{

	}
	else if (SeriesNodes[nodes[x][1]])
	{

	}
}
Component* ConvertFromVolSrcToCurrSrc(Component * vol, Component* res)
{
	Component* curr;
	curr->setTheta(vol->GetTheta() - res->GetTheta());
	curr->setValue(vol->GetValue() / res->GetValue());
	// you must handle the dependant case
	return curr;
}

Component* ConvertFromCurrSrcToVolSrc(Component * curr, Component * res)
{
	Component* vol;
	vol->setTheta(res->GetTheta() + curr->GetTheta());
	vol->setValue(res->GetValue() * curr->GetValue());
	// you must handle the dependant case
	return vol;
}
/*
parrarel
series
convertFromVoltageSourcToCurrentSource;
convertFromCurrentSourceToVoltageSource;
*/