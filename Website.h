#pragma once

#include<bits\stdc++.h>
#include<unordered_map>
using namespace std;
class Website
{
private:
	string name;
	int impressions;
	int clicks;
	int index;
	double PR;
public:
	Website(string name, int imp=1, int cl=0);
	//updates
	void incrementImp();
	void incrementClick();

	//getters
	double getCTR();
	int getImp();
	int getClick();
	int getIndex();
	double getPR();
	double getScore();
	string getName();

	//setters
	void setImpressions(int impressions);
	void setClicks(int clicks);
	void setPR(double PR);

	static int count;//count of the websites
};

