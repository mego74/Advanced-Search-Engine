#include "WebGraph.cpp"
#include "Website.h"

//I will ignore links from a page to itself
//I will assume that there is no more than one link from A to B

//"A valid query is one of the following:
//1. A single statment inside a quotation.\n";
//"2. Words Seperated by AND, i.e. word1 AND word2 AND word3... (words can be inside quotations\n";
//"3. Words Seperated by OR, i.e. word1 OR word2 OR word3... (words can be inside quotations\n";
//"4. Plain Text: words seperated by spaces.\n\n";


//global variables
vector<Website> websites;//vector of all websites
unordered_map<string, vector<int>> inSensitiveKeyWords;//map between keyword name and indicies of its websites in websites vector
unordered_map<string, vector<int>> SensitiveKeyWords;
unordered_map<string, int> WebsitesByName;//map between website name and its index
bool TwoChoice = 0;//if the output instructions consists of two choices only, it would be true. i.e. when we ask the user to choose 1. new search 2. Exit
bool ViewPageMode = 0;//true if we are in view a specific page mode
vector<int> retrieved;//indices of retrieved websites after each search


//five helping functions
void Lower(string &s) {//makes all letters of string lower 
	for (int i = 0; i < (int) s.size(); i++)
	{
		s[i] = tolower(s[i]);
	}
}
bool IsFileEmpty(string s) {
	ifstream Input(s);
	string k;
	if (getline(Input, k))
		return false;
	return true;
}

vector<int> vectorIntersection(vector<int> &a, vector<int> &b) {
	//find intersection between two sorted arrays
	//returns  a sorted array as well
	int i = 0, j = 0;
	vector<int> ans;
	while (i < a.size() && j < b.size())
	{
		if (a[i] == b[j]) {
			ans.push_back(a[i]);
			i++; j++;
		}
		else if (a[i] < b[j])
			i++;
		else
			j++;
	}
	return ans;
}

vector<int> vectorUnion(vector<int>& a, vector<int>& b) {//find union between two sorted arrays
	//returns  a sorted array as well
	int i = 0, j = 0;
	vector<int> ans;
	while (i < a.size() && j < b.size())
	{
		if (a[i] == b[j]) {
			ans.push_back(a[i]);
			i++; j++;
		}
		else if (a[i] < b[j]) {
			ans.push_back(a[i]);
			i++;
		}
		else {
			ans.push_back(b[j]);
			j++;
		}
	}
	while (i < a.size())
	{
		ans.push_back(a[i]);
		i++;
	}

	while (j < b.size())
	{
		ans.push_back(b[j]);
		j++;
	}

	return ans;
}

//to be used by std::sort to sort websites according to their scores descindigly in the ViewResults Function
bool Compare(int i, int j) {
	return websites[i].getScore() > websites[j].getScore();
}


//Initilaization of files
void InitializeKeywords() {
	ifstream  KeyInput;
	KeyInput.open("keywords.csv");
	
	string name, word, line;
	while (getline(KeyInput, line))
	{
		stringstream s(line);
		bool first = true;
		while (getline(s, word, ','))
		{
			if (first) {
				websites.push_back(Website(word));
				WebsitesByName[word] = websites.back().getIndex();
				first = false;
			}
			else {
				SensitiveKeyWords[word].push_back(websites.back().getIndex());
				Lower(word);//makes its characters lower for the not case sensitive search
				inSensitiveKeyWords[word].push_back(websites.back().getIndex());
			}
		}
		
	}
	KeyInput.close();
}

void InitializeImpressions() {
	ifstream ImpInput;
	ImpInput.open("impressions.csv");
	string word1, line, word2;

	while (getline(ImpInput, line)) {
		stringstream s(line);

		getline(s, word1, ',');
		getline(s, word2, ',');
		if(WebsitesByName.count(word1))//the website already exists
			websites[WebsitesByName[word1]].setImpressions(stoi(word2));
		else {
			websites.push_back(Website(word1, stoi(word2)));
			WebsitesByName[word1] = websites.back().getIndex();
		}

	}
	ImpInput.close();
}

void InitializeClicks() {
	ifstream ClickInput;
	ClickInput.open("clicks.csv");//A new file I created to save the clicks of each site, to update CTR
	
	string word1, line, word2;

	while (getline(ClickInput, line)) {//at the first run the file would be empty, so this loop will not be entered
		//but, after that the file would have some info
		//Anyway, clicks is initialized by 0 in the consturctor of each website.
		stringstream s(line);
		getline(s, word1, ',');
		getline(s, word2, ',');


		if (WebsitesByName.count(word1))//the website already exists
			websites[WebsitesByName[word1]].setClicks(stoi(word2));
		else {
			websites.push_back(Website(word1));
			websites.back().setClicks(stoi(word2));
			WebsitesByName[word1] = websites.back().getIndex();
		}
		
	}
	ClickInput.close();
}

void AssignPageRank() {//builds the graph and assings PR to each website
	
	
	if (!IsFileEmpty("pagerank.csv")) {
		string line, web, prS;
		double PR;
		ifstream PRInput("pagerank.csv");
		
		while (getline(PRInput, line))
		{
			stringstream s(line);
			getline(s, web, ',');
			getline(s, prS, ',');
			PR = stod(prS);
			websites[WebsitesByName[web]].setPR(PR);
		}
		PRInput.close();
	}
	else {
		string line, web1, web2;
		ifstream GraphInput;
		GraphInput.open("webgraph.csv");
		while (getline(GraphInput, line))//this loop just to make sure all webstistes exist, and add those which don't exist
		{
			//I am making that to avoid corrupt data input where some website is mentioned in one file but not in another one. 
			stringstream s(line);
			getline(s, web1, ',');
			getline(s, web2, ',');

			if (!WebsitesByName.count(web1)) {
				websites.push_back(Website(web1));
				WebsitesByName[web1] = websites.back().getIndex();
			}
			if (!WebsitesByName.count(web2)) {
				websites.push_back(Website(web2));
				WebsitesByName[web2] = websites.back().getIndex();
			}

			//G.addEdge(WebsitesByName[web1], WebsitesByName[web2]);//add the edge
		}
		GraphInput.close();

		GraphInput.open("webgraph.csv");//open the file again

		WebGraph G(Website::count);
		//this loop will actually construct the graph

		while (getline(GraphInput, line))
		{
			stringstream s(line);
			getline(s, web1, ',');
			getline(s, web2, ',');
			G.addEdge(WebsitesByName[web1], WebsitesByName[web2]);//add the edge
		}

		G.setPageRank(websites);//assign PR to each website

		GraphInput.close();
	}

	

	

}







vector<int> SearchWord(string word) {//return the search result of a single word or a statement inside a quotation.
	vector<int> empty;
	if (word[0] == '"') {
		word = word.substr(1);
		word.pop_back();
		return SensitiveKeyWords.count(word) ? SensitiveKeyWords[word] : empty;
	}
	else {
		Lower(word);
		return inSensitiveKeyWords.count(word) ? inSensitiveKeyWords[word] : empty;
	}
		
}

vector<int> Search() {
	//takes the search query, and returns the result
	//result would be vector {-1} if the query is not valid
	cout << "\nEnter your valid search query: ";
	TwoChoice = 0;
	string line;
	cin.ignore();
	getline(cin, line);
	vector<int> ans;
	
	bool onlyQuotation = false;
	if (line[0] == '"' && line.back()=='"') {//single sentence quotation case-> "        "
		onlyQuotation = true;
		for (int i = 1; i < line.size()-1; i++)//check if there is another quotation in the middle
			//case like "wordA" AND "wordB". We want to detect tthat using that loop
		{
			if (line[i] == '"')
				onlyQuotation = false;
		}
		if(onlyQuotation)
			ans = SearchWord(line);
	}

	if(!onlyQuotation) {//the query is not a singly quotation
		stringstream ss(line);
		bool AND = 0, OR = 0;
		string word;
		vector<string> words;
		int k = -1;
		while (ss>>word)
		{
			
			k++;
			if (k== 1) {
				if (word == "AND")
					AND = 1;
				else if (word == "OR")
					OR = 1;
			}
			if (AND && k % 2) {
				if (word != "AND") {
					return ans = { -1 };//-1 measns not valid
					break;
				}
				else {
					continue;
				}
					
			}
			else if (OR && k % 2) {
				if (word != "OR")
					return ans = {-1};
				else {
					continue;
				}
			}

			words.push_back(word);
		
		}
		

		vector<int> empty;
		if (!words.empty()) {
			if (AND) {
				ans = SearchWord(words[0]);
				if (ans.empty())//first word doesn't exist, and they are all AND
					return ans;

				for (int i = 1; i < words.size(); i++){
					vector<int> vec = SearchWord(words[i]);
					ans=vectorIntersection(ans, vec);
					if (ans.empty())
						return ans;
				}
			}
			else {
				for (int i = 0; i < words.size(); i++) {
					
					vector<int> vec = SearchWord(words[i]);
					ans = vectorUnion(ans, vec);
				}
			}
		}
	}

	
	return ans;//if ans remains empty then the query is invalid
}

void NewSeach() {
	TwoChoice = 1;
	ViewPageMode = 0;
	cout << "What would you like to do?\n";
	cout << "1. New Seach\n2. Exit\n\nTyper in your choice: ";
}


void viewResults() {
	//res is not empty and it is not -1
	TwoChoice = 0;
	ViewPageMode = 0;
	if (retrieved.size() == 0) {
		cout << "\nNo websites found for that query\n\n";
		NewSeach();
	}
	else if (retrieved[0] == -1) {
		cout << "\nYou enterend and invalid query\n\n";
		NewSeach();
	}
	else {
		sort(retrieved.begin(), retrieved.end(), Compare);
		cout << "\nSearch Results:\n";
		for (int i = 0; i < retrieved.size(); i++)
		{
			cout << i + 1 << ".  " << websites[retrieved[i]].getName() << "\n";
			websites[retrieved[i]].incrementImp();
		}
		cout << "\nWhat would you like to do?\n";
		cout << "1. Choose a webpage to open\n";
		cout << "2. New search\n";
		cout << "3. Exit\n\n";
		cout << "Type in your choice: ";
	}
}

void ViewWebsite(Website &w) {
	TwoChoice = 0;
	ViewPageMode = 1;
	cout << "\nYou are now viewing " << w.getName() << "\n\n";
	cout << "What would you like to do?\n";
	cout << "1. Back to search results.\n";
	cout << "2. New search\n";
	cout << "3. Exit\n\n";
	cout << "Type in your choice: ";
	w.incrementClick();
}


void EXIT() {//saves the data and exit

	//I save results in new then remove the old ones then rename the new files to be like the old ones
	//I do so to accomodate to the case when the program crashes while updating the data
	//I don't want to lose the old data if that happens.

	ofstream Output;
	if (IsFileEmpty("pagerank.csv")){//if the page rank is empty, we save it for the first time.
		//After that, we don't save it since the webgraph does not change
		Output.open("newpagerank.csv");
		for (auto w : websites) {
			Output << w.getName() << "," << w.getPR() << "\n";
		}
		Output.close();
		remove("pagerank.csv");
		rename("newpagerank.csv", "pagerank.csv");
	}

	//updating impressions.csv
	Output.open("newimpressions.csv");
	for (auto w : websites) {
		Output << w.getName() << "," << w.getImp() << "\n";
	}
	Output.close();
	remove("impressions.csv");
	rename("newimpressions.csv", "impressions.csv");


	//updating clicks.csv
	Output.open("newclicks.csv");
	for (auto w : websites) {
		Output << w.getName() << "," << w.getClick() << "\n";
	}
	Output.close();
	remove("clicks.csv");
	rename("newclicks.csv","clicks.csv");



	//exit the program successfully
	exit(0);
}


bool Interact(int choice) {
	if (choice <= 0 || choice >= 4 || (TwoChoice && choice == 1)) {
		cout << "Invalid Input\n\n";
		NewSeach();
		return true;
	}

	if (choice == 2) {
		retrieved = Search();
		viewResults();
		return true;
	}
	else if(choice == 3) {
		EXIT();
		return false;
	}
	//when choice is 1, we have two cases
	if (!ViewPageMode) {
		cout << "\nEnter The number of the website you want to open: ";	
		cin >> choice;
		while (choice <= 0 || choice > retrieved.size()) {
			cout << "Invalid Input\n";
			cout << "Enter The number of the website you want to open: ";
			cin >> choice;
		}
		ViewWebsite(websites[retrieved[choice - 1]]);
	}
	else {
		viewResults();
		return true;
	}
}



void InitalPrint() {
	cout << "Welcome!\n";
	cout << "A valid query is one of the following:\n1. A single statment inside a quotation.\n";
	cout << "2. Words Seperated by AND, i.e. word1 AND word2 AND word3... (words can be inside quotations\n";
	cout << "3. Words Seperated by OR, i.e. word1 OR word2 OR word3... (words can be inside quotations\n";
	cout << "4. Plain Text: words seperated by spaces.\n\n";


}


int main() {
	
	InitializeKeywords();//reads the file keywords, amd initialize the three data strucutres.
	InitializeImpressions();//reads the file impressions, and then changes the value of impression of each website accordingly
	InitializeClicks();//reads the file clicks, and then changes the value of clicks of each website accordingly

	//At first, there is no clicks data given, but I will save the clicks information in that file in order for CTR to be 
	//more reflective in the future.
	
	
	AssignPageRank();
	//If it is the first run of the program ever (the pagerank file is empty or does not exist), then it will construct the graph
	//and sets the pagerank
	//otherwise, it will retrieve the page rank from the file
	

	TwoChoice = true;//at first we have twoChoice mode
	InitalPrint();
	NewSeach();

	int option;
	cin >> option;
	option++;//the first mode is two choice mode
	while (Interact(option))
	{
		cin >> option;
		if (TwoChoice)
			option++;	
	}


	return 0;
}