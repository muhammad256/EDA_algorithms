
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
using namespace std;
struct Element
{
	string name;
	string block;
	vector<string> nets;
    bool group;    // true= group A ......... flase= group B
    bool fixed = false; //true= fixed ........false= can move
};
int temp(ifstream &file) {
	int n = -1;
	string temp_str;
	while (file) {
		getline(file, temp_str);
		n++;
	}
	file.close();
	return n;
}
void Get_tada(ifstream &file, Element Elements [], int n) {
	for (int i = 0; i < n; i++)
	{
		getline(file, Elements[i].name);
	}
	file.close();
};
void Adjust_data(Element Elements [],int num){
	int found0 = Elements[num].name.find(":");
	int found1 = Elements[num].name.find("(");
	Elements[num].block = Elements[num].name.substr(found1+1, found0-found1-1);
	int found = Elements[num].name.find(",");
	int i = 0;
	while(found != string::npos){
		i++;
		if (i>5){ // just to avoid problems
			break;
		}
		Elements[num].nets.push_back(Elements[num].name.substr(found-3,3));
		found = Elements[num].name.find(",", found+1);
	}
	found = Elements[num].name.find(")");
	Elements[num].nets.push_back(Elements[num].name.substr(found-3,3));

}; 
void Remove_trash(Element Elements [],int num){
	for (int i=0; i< Elements[num].block.length(); i++){
		if (Elements[num].block[i] ==' '){
			Elements[num].block.erase(i,1);
			i--; //
		}
	}
	
	for (int i = 0; i<Elements[num].nets.size(); i++ ){
		for(int j = 0; j< Elements[num].nets[i].size(); j++){
			if (Elements[num].nets[i][j] ==' '){
				Elements[num].nets[i].erase(j,1);
				j--;
			}
		}	
	}
};
int cost(Element Elements [], vector <vector<int>> vect, int num){
	int D=0;
	for(int i=0; i< vect[num].size(); i++){
		if(Elements[num].group ==Elements[i].group){
			D = D - vect[num][i];
		}
		else {
			D = D + vect[num][i];
		}
	}
	return D;
}

int main()
{
	ifstream file;
	file.open("pin-oriented.txt");
	int n = temp(file);
    Element Elements[n+1];  // free one for dummy if needed;
	file.open("pin-oriented.txt");
	Get_tada(file,Elements, n);
	for(int i=0; i<n; i++){ // adjust && remove spaces
		Adjust_data(Elements,i);
		Remove_trash(Elements,i);
	}
	vector <vector<int>>con_mat(n+1,vector<int>(n+1,0));
 	for(int i=0; i<n; i++){  // generate the connec mat
		for(int j=0; j<n; j++){
			if(i!=j){
				for(int k=0; k< Elements[i].nets.size(); k++){
					for(int m=0; m < Elements[j].nets.size(); m++){
						if(Elements[i].nets[k]==Elements[j].nets[m]){
							con_mat[i][j]++;
						}
					}
				}
			}
		}
	}

    if (n % 2 !=0){  // handle odd netlists 
        Elements[n].name="this is dummy"; 
        Elements[n].block="D";  
        Elements[n].group=false;  
		n++;
    }
    for (int i=0; i<n; i++){ // get 2 partitions
        if (i < n/2){  
        Elements[i].group=true; 
        }
        else {
            Elements[i].group=false;
        } 
    }

	vector <int> Gain; // array of states gain for comp
	vector <vector<int>> state; // vec of states
	vector <int> state0;
	int G = 0; //state gain 
	long deltaG; //delta g of each state
	int temp_var; // dummy var
	int gate1=0;
	int gate2=0;
	bool continu = true;
	while(continu){
		deltaG = -10000; //less than any practical value
		temp_var = -10000;
		for(int i=0; i< n; i++){
			for(int j = 0; j < n; j++){
				if (!Elements[i].fixed  && Elements[i].group!=Elements[j].group && i!=j && !Elements[j].fixed){
					temp_var =  cost(Elements, con_mat,i) +  cost(Elements, con_mat,j) - 2*con_mat[i][j];
				}
				if (temp_var > deltaG){
					deltaG = temp_var;
					gate1 = i;
					gate2 = j;
				}
			}
		}
		Elements[gate1].group = !Elements[gate1].group;
		Elements[gate2].group = !Elements[gate2].group;
		Elements[gate1].fixed = true;
		Elements[gate2].fixed = true;
		G = G + deltaG;
		Gain.push_back(G);
		for(int i= 0; i<n; i++){
			state0.push_back(Elements[i].group);
		}
		state.push_back(state0);
		state0.clear();
		continu = false;
		for (int i=0; i<n; i++){
			continu = continu || !Elements[i].fixed;
		}
		//this_thread::sleep_for(1s);  //for debuging 
	}
	int neededG = -1; 
	int neededi;
	for(int i=0; i<n/2; i++){
		if(Gain[i]>neededG){
			neededG = Gain[i];
			neededi = i;
		}
	}
	for(int i=0; i< n; i++){
		Elements[i].group =  state[neededi][i];
	}

	ofstream MyFile("partitioned_netlist.txt");
	MyFile << "Group A:"<<endl;
	for(int i=0; i<n; i++){
		if (!Elements[i].group && Elements[i].name!="this is dummy"){
			MyFile << Elements[i].name<<endl;
		}
	}
	MyFile << "Group B:"<< endl;
	for(int i=0; i<n; i++){
		if (Elements[i].group && Elements[i].name!="this is dummy"){
			MyFile << Elements[i].name<< endl;
		}
	}
}




