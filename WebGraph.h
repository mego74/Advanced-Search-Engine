#pragma once
#include"Website.cpp"


class WebGraph
{
private:
	vector<vector<int>> adjlist;
	vector<vector<int>> adjlistTranspose;//adjlist of the transpose of the graph to help us calculate the page rank
	int N;//num of vertices (Websites). vertices indexed from 0 to N-1
	vector<vector<double>> PageRank;//2D matrix of size N*2 that saves the previous and current iteration of calculating Page Rank.
	void calculatePageRank();

public:

	WebGraph(int N);
	void addEdge(int src, int dest);
	void setPageRank(vector<Website> &websites);

};