//============================================================================
// Name        : parse.cpp
// Author      : Daryl
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<algorithm>
#include <vector>
#include <set>
#include <map>
using namespace std;

string clean(string s, char chars[]){

	//remove desired characters from string

	// https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
	for (unsigned int i = 0; i < strlen(chars); ++i){
		s.erase(remove(s.begin(), s.end(), chars[i]),s.end());
	}
	return s;

}

string upper(string s) {
	//make input all caps
	//https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

bool vecContains(string keyword, vector<string> v) {
	//https://stackoverflow.com/questions/6277646/in-c-check-if-stdvectorstring-contains-a-certain-value
	if (std::find(v.begin(), v.end(), keyword) != v.end())
	{
	  return true;
	} else {
		return false;
	}

}

void printStars(string starCount) {
	bool halfstar = false;
	//if last character of string is 5 there is a half star (ex:4.5 = ****(halfstar)

	int lastChar = starCount.back() - '0';

	if (lastChar == 5) {
		halfstar = true;
	}

	for (int i = 0; i < stoi(starCount); i++) {
		//★
		cout<<"\u2605";
	}

	if (halfstar) {
		cout << ".5";
	}

	cout << endl;

}

void printBusiness(map<string, string> m) {

	try {
		cout << m.at("name") << endl;
		cout << m.at("address") << endl;
		cout << m.at("city") << endl;
		cout << m.at("state") << endl;
		cout << m.at("postal_code") << endl;
		printStars(m.at("stars"));
		cout << m.at("categories") << endl << endl;
	}
	catch (exception& e) {
		//while testing, there was a key not found error; this subverts it
		if (e.what() == "map::at:  key not found") {
			;
		}
	}

}

void printCategories(string path) {
	string line;
	//open file
	ifstream myfile;
	myfile.open (path);
	//holds categories that have been printed
	set<string> printed;

	//while not at end of file
	while (! myfile.eof() ) {

		//one line of the JSON file represents one business
		//get line, convert to string and assign it to 'line' variable
		getline(myfile, line);

		//remove all {, }, and " from string to make it easier to parse
		line = clean(line, "{}\"");

		string comma = ",";
		//holds index of comma in line
		size_t pos = 0;
		string token;

		// split line by commas
		// from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
		while ((pos = line.find(",")) != std::string::npos) {
			//token becomes string that holds both key and value of json object; separated by ':'
			//token will become a string of json attributes that looks like "key:value"
			token = line.substr(0, pos);

			// index of where colon is. colon divides the key and the value
			int split = token.find(":");


			string key = token.substr(0, split);
			string category = token.substr(split+1);

			//if the key of the json attribute is equal to any of the fields we are looking for
			if (key == "categories") {
				//make string uppercase
				category = upper(category);

				//check if category is in set that holds strings that have been printed
				//https://stackoverflow.com/questions/1701067/how-to-check-that-an-element-is-in-a-stdset
				bool inSet = printed.find(category) != printed.end();

				//if not printed yet
				if (!inSet) {
					printed.insert(category);
					cout << category << endl;
				}


			}

			//delete part of line that has been parsed already
			line.erase(0, pos + comma.length());

		}


	}
	myfile.close();
}

//if value of map is equal to what we are looking for, return true
bool validate(string value, string s) {
	try {
		if (value.find(s) != string::npos) { //if contsins
			return true;
		}
		else {
			return false;
		}
	}
	catch (exception& e) {
		//while testing, there was a key not found error; this subverts it
		if (e.what() == "map::at:  key not found") {
			;
		}
		return false;
	}
}


map<string, string> createMap(string line) {
	map<string, string> m;

	string comma = ",";
	//holds index of comma in line
	size_t pos = 0;
	string token;

	// split line by commas
	// from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
	while ((pos = line.find(",")) != std::string::npos) {
		//token becomes string that holds both key and value of json object; separated by ':'
		//token will become a string of json attributes that looks like "key:value"
		token = line.substr(0, pos);

		// index of where colon is. colon divides the key and the value
		int split = token.find(":");

		string key = token.substr(0, split);
		string value = token.substr(split+1);

		//if the key of the json attribute is equal to any of the fields we are looking for
		if (key == "name" || key == "address" || key == "city" ||
				key == "state" || key == "postal_code" || key == "stars"
						|| key == "categories") {
			//make keys uppercase
			transform(value.begin(), value.end(), value.begin(), ::toupper);

			//add to map
			m[key] = value;

		}

		//delete part of line that has been parsed already
		line.erase(0, pos + comma.length());

	}

	return m;
}

int main() {
	string path = "yelp_academic_dataset_business.json";
	//array of characters that will be deleted from every line we parse
	char chars[] = "{}\"\/";
	bool businessFound = false;
	string filteringBy;
	string match;
	string negative; //what string will say if no matches found

	string in;

	cout << "How would you like to filter the file?" << endl;
	cout << "1) City" << endl << "2) State" << endl
			<< "3) Zip Code" << endl << "4) Rating" << endl << "5) Category" <<endl;

	getline(cin,in);

	cout << endl;
	//convert user input to int; when you use cin alongside getline(), things get funky
	int input = stoi(in);


	switch(input) {
	//set filter to string that equals the json attribute we will be looking at for each object
		case 1: // city
			filteringBy = "city";
			cout << "Enter a city: ";
			getline(cin, match);
			match = upper(match);
			negative = "No businesses found in " + match;
			cout << "Searching for businesses in " << match << endl;
			break;
		case 2: //state
			filteringBy = "state";
			cout << "Enter a state (ex: CA, NY, TX): ";
			getline(cin, match);
			match = upper(match);
			negative = "No businesses found in " + match;
			cout << "Searching for businesses in " << match << endl;
			break;
		case 3: //zip code
			filteringBy = "postal_code";
			cout << "Enter a zip code: ";
			getline(cin, match);
			negative = "No businesses found with zip code: " + match;
			cout << "Searching for businesses with zip code: " << match << endl;
			break;
		case 4: // rating
			filteringBy = "stars";
			cout << "Enter desired rating (ex: 1.0, 1.5 - 5.0): ";
			getline(cin, match);
			negative = "No businesses found with " + match + " stars.";
			cout << "Searching for businesses with " << match << " stars." << endl;
			break;
		case 5:
			filteringBy = "categories";
			printCategories(path);
			cout << "Enter one of the categories from above: ";
			getline(cin, match);
			negative = "No businesses found with category: " + match;
			cout << "Searching for businesses with category: " << match << endl;
			break;

	}


	string line;
	//open file
	ifstream myfile;
	myfile.open (path);

	//while not at end of file
	while (! myfile.eof() ) {
		//map to hold all the attriubtes of business that we actually need
		map<string, string> businessMap;

		//one line of the JSON file represents one business
		//get line, convert to string and assign it to 'line' variable
		getline(myfile, line);

		//remove all {, }, and " from string to make it easier to parse
		line = clean(line, chars);

		//create a hashmap of the business that holds all the properties we want
		businessMap = createMap(line);


		//if filteringBy:match is inside map
		if (validate(businessMap.at(filteringBy), match)) {
			businessFound = true;
			cout << "Found a business!" << endl << endl;
			printBusiness(businessMap);
		}

	}

	//close file
	myfile.close();

	if (businessFound) {
		cout << "Done" << endl;
	} else {
		cout << "Done" << endl << "No businesses found in " << match;
	}



	return 0;
}




