#include "Website.h"


Website::Website(string name, int imp, int cl) {
	//index is the index of the website in the vector of websites in the source.cpp file
	//the default value of impressions is 1
	//as putting it with 0 might cause run time error when we Calculate CTR
	this->name = name;
	index = count++;
	if (imp > 0)
		impressions = imp;//as we will divide by it in getCTR
	else
		impressions = 1;

	if (cl >= 0)
		clicks = cl;
	else
		clicks = 0;

	PR = 0;//Inital value
}
void Website::incrementImp() {
	impressions++;
}
void Website::incrementClick() {
	clicks++;
}

double Website::getCTR() {
	return (double)clicks / impressions;
}

int Website::getImp() {
	return impressions;
}
int Website::getClick() {
	return clicks;
}
int Website::getIndex() {
	return index;
}
double Website::getPR() {
	return PR;
}
string Website::getName() {
	return name;
}

double Website::getScore() {
	double frac = (0.1 * impressions) / (1.0 + 0.1 * impressions);

	return 0.4 * PR + ((1 - frac) * PR + frac * getCTR()) * 0.6;
}


void Website::setImpressions(int impressions) {
	if(impressions>0)//as we will divide by it in getCTR
		this->impressions = impressions;		
}
void Website::setClicks(int clicks) {
	if(clicks>=0)
		this->clicks = clicks;
}
void Website::setPR(double PR){
	this->PR = PR;
}


int Website::count = 0;