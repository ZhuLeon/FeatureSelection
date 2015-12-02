#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip> //set precision
#include <math.h>
#include <algorithm>
#include <iterator> //back_inserter

using namespace std;

void printset(vector<int> current_set_of_features)
{
	for (int t = 0; t < current_set_of_features.size(); t++)
	{
		if (t + 1 == current_set_of_features.size())
		{
			cout << current_set_of_features.at(t) << "} ";
		}
		else
		{
			cout << current_set_of_features.at(t) << ", ";
		}
	}
}

float leave_one_out_cross_validation(vector<vector<float>> data, vector<int> current_set_of_features, int x2, int choice, int &wrong)
{
	float accuracy = 0;
	int curr_wrong = 0;
	//Takes one row
	for (int y = 0; y < data.size(); y++)
	{
		float distance = 0;
		float nearest_neighbor = INFINITY;
		int y_pos, y2_pos;
		//Takes another row
		for (int y2 = 0; y2 < data.size(); y2++)
		{
			if (y2 != y) //dont calculate distance if both rows are the same
			{
				distance = 0;
				//compute distance for each feature in each row
				for (int g = 0; g < current_set_of_features.size(); g++)
				{
					distance = distance + pow(data[y][current_set_of_features.at(g)] - data[y2][current_set_of_features.at(g)], 2);
				}
				if (choice != 2)
				{
					distance = sqrt(distance + pow(data[y][x2] - data[y2][x2], 2));
				}
				else
				{
					distance = sqrt(distance - pow(data[y][x2] - data[y2][x2], 2));
				}
				nearest_neighbor = min(nearest_neighbor, distance);
				if (nearest_neighbor == distance)
				{
					y_pos = y;
					y2_pos = y2;
				}
			}
		}
		if (data[y_pos][0] == data[y2_pos][0])
		{
			accuracy++;
		}
		else 
		{
			if (choice == 3)
				curr_wrong++;
		}
		if (curr_wrong > wrong && choice == 3)
		{
			return 0;
		}
	}
	wrong = curr_wrong;
	return ((accuracy / data.size()) * 100);
}

void feature_search(vector<vector<float>> data, int choice)
{
	vector<int> current_set_of_features; //current set of features being considered
	vector<int> high_set; //set that gives the highest accuracy
	float high_acc = 0; //highest accuracy
	//loops through the number of columns (feature#)
	if (choice == 2)
	{
		for (int i = 1; i < data.at(0).size(); i++)
		{
			current_set_of_features.push_back(i);
		}
		//cout << "Running nearest neighbor with all features, I get an accuracy of ";
		//int filler = INFINITY;
		//high_acc = leave_one_out_cross_validation(data, current_set_of_features, 0, choice, filler);
		//cout << high_acc << "%\n" << endl;
	}
	for (int x = 1; x < data.at(0).size(); x++)
	{
		int feature_to_add_at_this_level;
		float best_so_far_accuracy = 0;
		int wrong = INFINITY;
		//loops through the number of columns (feature#)
		for (int x2 = 1; x2 < data.at(0).size(); x2++)
		{
			//first check if the current feature we are looking at has already been considered
			vector<int> intersections; //stores what intersects
			vector<int> current_feature; //store current feature#
			current_feature.push_back(x2);
			sort(current_set_of_features.begin(), current_set_of_features.end());
			set_intersection(current_set_of_features.begin(),current_set_of_features.end(),
				current_feature.begin(), current_feature.end(), 
				back_inserter(intersections)); //current feature intersects set

			if ((intersections.empty() && (choice == 1 || choice == 3)) || (choice == 2 && !intersections.empty())) //Only consider adding, if not already added
			{
				cout << "\tUsing feature(s) {";
				current_set_of_features.push_back(x2);
				printset(current_set_of_features);
				current_set_of_features.pop_back();
				float accuracy = leave_one_out_cross_validation(data, current_set_of_features, x2, choice, wrong);
				cout << " accuracy is " << accuracy << "%" << endl;
				//store only the best accuracy and best feature#
				if (accuracy > best_so_far_accuracy)
				{
					best_so_far_accuracy = accuracy;
					feature_to_add_at_this_level = x2;
				}
			}

		}
		if (choice == 2)
		{
			current_set_of_features.erase(remove(current_set_of_features.begin(), current_set_of_features.end(), feature_to_add_at_this_level), current_set_of_features.end());
			cout << "Feature set {";
			printset(current_set_of_features); 
			cout << " was best, accuracy is " << best_so_far_accuracy << "%\n" << endl;
		}
		else
		{
			current_set_of_features.push_back(feature_to_add_at_this_level);
			cout << "Feature set {";
			printset(current_set_of_features); 
			cout << " was best, accuracy is " << best_so_far_accuracy << "%\n" << endl;
		}

		//after each iteration determine if the accracy increased
		high_acc = max(high_acc, best_so_far_accuracy);
		if (high_acc == best_so_far_accuracy)
		{
			high_set = current_set_of_features;
		}
	}

	cout << "Finished search!! The best feature subset is {";
	printset(high_set);
	cout << ", which has an accuracy of " << high_acc << "%" << endl;
}

int main()
{
	string file;
	cout << "Type the name of the file to test: ";
	cin >> file;
	ifstream data_file(file);
	if (!data_file.is_open())
	{
		cout << "Unable to open file" << endl;
		return 1;
	}
	vector<vector<float>> data; //nxn matrix to store data 
	//if end of the file has not been reached, store data into matrix
	if (data_file.good())
	{
		string str;
		while (getline(data_file, str))
		{
			vector<float> row;
			istringstream ss(str);
			float num;
			while (ss >> num)
			{
				row.push_back(num);
			}
			data.push_back(row);
			row.clear();
		}


	}
	////display matrix
	//for (int y = 0; y < data.size(); y++)
	//{
	//	for (int x = 0; x < data.at(y).size(); x++)
	//	{
	//		cout << data[y][x] << " ";
	//	}
	//	cout << setprecision(8) << endl;
	//}
	int choice = 0;
	cout << "Type the number of algorithm you want to run." << endl
		<< "\t1) Forward Selection" << endl
		<< "\t2) Backward Elmination" << endl
		<< "\t3) Custom Algorithm" << endl;
	cin >> choice;
	cout << "This dataset has " << data.at(0).size() - 1 << " features (not including the class attribute), with " << data.size() << " instances." << endl;
	cout << "\nBeginning search.\n" << endl;
	feature_search(data, choice);

	return 0;
}
