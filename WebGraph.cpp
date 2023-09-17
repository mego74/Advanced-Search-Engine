#include "WebGraph.h"


WebGraph::WebGraph(int N) {
	this->N = N;
	adjlist.resize(N);//the graph is zero indexed
	adjlistTranspose.resize(N);
}

void WebGraph::addEdge(int src, int dest) {
	adjlist[src].push_back(dest);//the graph is directed
	adjlistTranspose[dest].push_back(src);

}

void WebGraph::calculatePageRank() {
	PageRank.resize(N);
	for (int i = 0; i < N; i++)//Initilizaiton
	{
		PageRank[i].push_back(1.0 / N);
		PageRank[i].push_back(0);
	}
	double MIN = 1e9;//I will use it for normalization
	double MAX = 0;

	for (int i = 1; i < 101; i++) {//I will stop if I made 100 iteration OR if the diffenece between current calculation
		//and previous one (margin of error) is less than 0.01.
		//This is choice is arbitrary but it guarantees good results according to
		//http://cs.brown.edu/courses/cs016/static/files/assignments/projects/GraphHelpSession.pdf
		//I choose the damping factor to be 0.85 according to the same above file

		bool achieved = true;
		
		for (int webs = 0; webs < N; webs++) {

			//for each webs we will determine its rank

			const double d = 0.85;//damping factor
			double cur = 0;
			for (int j : adjlistTranspose[webs]) {//all websites poinitng at webs
				cur += (PageRank[j][(i - 1) % 2] / adjlist[j].size());
			}
			cur *= d;
			cur += (1 - d) / N;
			PageRank[webs][i % 2] = cur;
			MIN = min(cur, MIN);
			MAX = max(cur, MAX);
			achieved &= (abs(cur - PageRank[webs][(i - 1) % 2]) < 0.01);
		}
	
		
		if (achieved) {//page rank have stabilized, so we don't need more iterations
			//To be consistent, I will always make sure the final answer is the first column
			//of PageRank matrix
			if (i % 2==1) {//the answer in the second column, so we put it in the first column as well
				for (int i = 0; i < N; i++)
				{
					if (MIN != MAX) {//that would be prbably always the case as we used damping factor
						PageRank[i][0] = (PageRank[i][1]-MIN) / (MAX-MIN);//normalizing usin Min-Max Normalizaition
					}

					
				}
			}

			return;//terminate the function
		}
		
	}
	//Here, we finidhed 100 iterations witout reaching achieved (unlikely to happen), so we normalize here
	for (int i = 0; i < N && MIN!=MAX; i++)
	{	
		PageRank[i][0] = PageRank[i][1]-MIN / MAX-MIN;//normalizing
	}
}
	

void WebGraph::setPageRank(vector<Website>& websites) {
	calculatePageRank();
	for (int i = 0; i < N; i++)
	{
		websites[i].setPR(PageRank[i][0]);
	}
}