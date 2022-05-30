
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
    string type;
	vector<string> nets;
	int position;
    bool group;    // true= group A ......... flase= group B
    bool fixed = false; //true= fixed ........false= can move
	bool up;
	bool right;
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
	int found1 = Elements[num].name.find("(");
	string temm = Elements[num].name.substr(0, found1-1);
    found1 = temm.find(" ");
    Elements[num].block = temm.substr(found1+1);
    Elements[num].type = temm.substr(0, found1);
	int found = Elements[num].name.find(",");
	while(found != string::npos){
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
	for (int i=0; i< Elements[num].type.length(); i++){
		if (Elements[num].type[i] ==' '){
			Elements[num].type.erase(i,1);
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

int cost(vector<bool> group, vector <vector<int>> vect, int num){
	int D=0;
	for(int i=0; i< vect[num].size(); i++){
		if(group[num] ==group[i]){
			D = D - vect[num][i];
		}
		else {
			D = D + vect[num][i];
		}
	}
	return D;
}


int KL(vector <vector<int>> con_mat, int n, vector<bool>& group ){
	vector <int> Gain; // array of states gain for comp
	vector <vector<int>> state; // vec of states
	vector <int> state0;
	vector<bool> fixed;
	for(int i=0; i<n ; i++){
		fixed.push_back(0);
	}
	int G = 0; //state gain 
	long deltaG; //delta g of each state
	int temp_var; // dummy var
	int gate1=0;
	int gate2=0;
	bool continu = true;
	while(continu){
		deltaG = -10000; //less than any practical value   //group[] fixed[]
		temp_var = -10000;
		for(int i=0; i< n; i++){
			for(int j = 0; j < n; j++){
				if (!fixed[i]  && group[i]!=group[j] && i!=j && !fixed[j]){
					temp_var =  cost(group, con_mat,i) +  cost(group, con_mat,j) - 2*con_mat[i][j];
				}
				if (temp_var > deltaG){
					deltaG = temp_var;
					gate1 = i;
					gate2 = j;
				}
			}
		}
		
		group[gate1] = !group[gate1];
		group[gate2] = !group[gate2];
		fixed[gate1] = true;
		fixed[gate2]= true;
		G = G + deltaG;
		Gain.push_back(G);
		for(int i= 0; i<n; i++){
			state0.push_back(group[i]);
		}
		state.push_back(state0);
		state0.clear();
		continu = false;
		for (int i=0; i<n; i++){
			continu = continu || !fixed[i];
		}
		//this_thread::sleep_for(1s);  //for debuging 
	}

	int neededG = Gain[0]; 
	int neededi= 0;
	for(int i=0; i<n/2; i++){
		if(Gain[i]>neededG){
			neededG = Gain[i];
			neededi = i;
		}
	}
	for(int i=0; i< n; i++){
		group[i] =  state[neededi][i];
		fixed[i] = false;
	}
	return neededG;
}



int main()
{
	ifstream file;
	file.open("netlist_small.txt");
	int n = temp(file);
    Element Elements[n+1];  
	file.open("netlist_small.txt");
	Get_tada(file,Elements, n);
	for(int i=0; i<n; i++){ // adjust && remove spaces
		Adjust_data(Elements,i);
		Remove_trash(Elements,i);
	}
	vector <vector<int>>con_mat(n,vector<int>(n,0));
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
    
    




	vector<bool> group = {0, 0, 0, 0, 1, 1, 1, 1};
	for (int i=0; i<n; i++){ // get 2 partitions
 		Elements[i].group= group[i];
    }
	
	int G = KL(con_mat, n, group);
	while(G > 0){
		G = KL(con_mat, n, group);
	}
	for(int i =0; i<n; i++){
		Elements[i].group= group[i];
	}


	for(int i=0; i<n ; i++){ //sort groups
		if(Elements[i].group){
			Elements[n] = Elements[i];
			for(int j = i; j< n ; j++){
				if(!Elements[j].group){
					Elements[i] = Elements[j];
					Elements[j]= Elements[n];
					break;
				}
			}
		}
	}

	


	vector <vector<int>>con_mat_mod(n,vector<int>(n,0));
	vector <vector<int>>con_mat1(n/2,vector<int>(n/2,0));
	vector <vector<int>>con_mat2(n/2,vector<int>(n/2,0));

	for(int i=0; i<n; i++){  // generate the mod connec mat
		for(int j=0; j<n; j++){
			if(i!=j){
				for(int k=0; k< Elements[i].nets.size(); k++){
					for(int m=0; m < Elements[j].nets.size(); m++){
						if(Elements[i].nets[k]==Elements[j].nets[m]){
							con_mat_mod[i][j]++;
						}
					}
				}
			}
		}
	}

	
	for(int i=0; i<n/2; i++){
		for(int j = 0; j< n/2; j++){
			con_mat1[i][j]= con_mat_mod[i][j];
		}
	}
	for(int i=0; i<n/2; i++){
		for(int j = 0; j< n/2; j++){
			con_mat2[i][j]= con_mat_mod[i+4][j+4];
		}
	}
	
	
	vector<bool> up1 = {0, 0,1, 1};
	vector<bool> up2 = {0, 0,1, 1};
	for (int i=0; i<n/2; i++){ // get 2 partitions
		Elements[i].up= up1[i];
		 
    }
	for (int i=n/2; i<n; i++){ // get 2 partitions
		Elements[i].up= up2[i-4];	 
    }
	

	G = KL(con_mat1, n/2, up1);
	while(G > 0){
		G = KL(con_mat1, n/2, up1);
	}
	for(int i =0; i<n/2;i++){
		Elements[i].up= up1[i];
	}


	G = KL(con_mat2, n/2, up2);
	while(G > 0){
		G = KL(con_mat2, n/2, up2);
	}

	for(int i =n/2; i<n;i++){
		Elements[i].up= up2[i-4];
	}

	//////////// make them 4 groups,  done UP and Down
	////// place the up and down related to cost 



	for(int i=0; i<n/2 ; i++){ //sort first group up and down
		if(Elements[i].up){
			Elements[n] = Elements[i];
			for(int j = i; j< n/2 ; j++){
				if(!Elements[j].up){
					Elements[i] = Elements[j];
					Elements[j]= Elements[n];
					break;
				}
			}
		}
	}
	for(int i=n/2; i<n ; i++){ //sort second group up and down
		if(Elements[i].up){
			Elements[n] = Elements[i];
			for(int j = i; j< n ; j++){
				if(!Elements[j].up){
					Elements[i] = Elements[j];
					Elements[j]= Elements[n];
					break;
				}
			}
		}
	}


	vector <vector<int>>con_mat_mod2(n,vector<int>(n,0));

	for(int i=0; i<n; i++){  // generate the mod connec mat
		for(int j=0; j<n; j++){
			if(i!=j){
				for(int k=0; k< Elements[i].nets.size(); k++){
					for(int m=0; m < Elements[j].nets.size(); m++){
						if(Elements[i].nets[k]==Elements[j].nets[m]){
							con_mat_mod2[i][j]++;
						}
					}
				}
			}
		}
	}
	//  check if need to swap up and down gates within same group

	int pay1= con_mat_mod2[6][0] + con_mat_mod2[6][1] + con_mat_mod2[7][0] + con_mat_mod2[7][1];
	int pay2= con_mat_mod2[4][2] + con_mat_mod2[4][3] + con_mat_mod2[5][2] + con_mat_mod2[5][3];
	if(pay1 > pay2){
		Elements[n] = Elements[6];
		Elements[6] = Elements[4];
		Elements[4] = Elements[n];

		Elements[n] = Elements[7];
		Elements[7] = Elements[5];
		Elements[5] = Elements[n];

	}
	// conn mat with elements sorted up and down
	vector <vector<int>>con_mat_mod3(n,vector<int>(n,0));
	for(int i=0; i<n; i++){  // generate the mod connec mat
		for(int j=0; j<n; j++){
			if(i!=j){
				for(int k=0; k< Elements[i].nets.size(); k++){
					for(int m=0; m < Elements[j].nets.size(); m++){
						if(Elements[i].nets[k]==Elements[j].nets[m]){
							con_mat_mod3[i][j]++;
						}
					}
				}
			}
		}
	}
	// replce each group wrt being right or lleft depending on pay function 
	vector<bool> right = {0,1,0,1,0,1,0,1};
	for(int i=0; i<n; i++){
		Elements[i].right = right[i];
	}

	for(int i=0; i<n/2;i++){
		if(i < n/4){
			pay1 =0;
			for(int j=4; j< n;j++){
				pay1 = pay1+ con_mat_mod3[2*i][j];
			}
			pay2 =0;
			for(int j=4; j< n;j++){
				pay2 = pay2+ con_mat_mod3[2*i+1][j];
			}
			
			if(pay1> pay2){
				Elements[2*i].right=!Elements[2*i].right;
				Elements[2*i+1].right=!Elements[2*i+1].right;
				Elements[n] = Elements[2*i];
				Elements[2*i] = Elements[2*i+1];
				Elements[2*i+1] =Elements[n];
			}	
	
		}
		else{
			pay1 =0;
			for(int j=0; j< n/2 ;j++){
				pay1 = pay1+ con_mat_mod3[2*i][j];
			}
			pay2 =0;
			for(int j=4; j< n;j++){
				pay2 = pay2+ con_mat_mod3[2*i+1][j];
			}
			if(pay1> pay2){
				Elements[2*i].right=!Elements[2*i].right;
				Elements[2*i+1].right=!Elements[2*i+1].right;
				Elements[n] = Elements[2*i];
				Elements[2*i] = Elements[2*i+1];
				Elements[2*i+1] =Elements[n];
			}	
		}
	}
	//cout the gates relative to each other.
	cout<< endl;
	for(int i=0; i< n; i++){
		if(Elements[i].up){
			cout << Elements[i].block<< "  ";
		}
	}
	cout << endl;
	for(int i=0; i< n; i++){
		if(!Elements[i].up){
			cout << Elements[i].block<< "  ";
		}
	}

	//cout << Elements[2].block <<"  "<<Elements[3].block <<"  "<<Elements[6].block <<"  "<<Elements[7].block <<endl;
	//cout << Elements[0].block <<"  "<<Elements[1].block <<"  "<<Elements[4].block <<"  "<<Elements[5].block <<endl;
}






