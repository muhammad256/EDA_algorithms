
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
#include<algorithm>
using namespace std;
struct Element
{
	string name;
	string block;
    string type; 
	vector<string> nets;
    int x;
    int y;
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
void fillArea(Element Elements [],int num){
    if(Elements[num].type=="Flipflop"){
        Elements[num].x =8;
        Elements[num].y =1;
    }
    else if(Elements[num].type=="Inv"){
        Elements[num].x =2;
        Elements[num].y =1;
    }
    else if(Elements[num].type=="And"){
        int comp = Elements[num].nets.size() -1;
        Elements[num].x =comp+1;
        Elements[num].y =comp+1;
    }
    else if(Elements[num].type=="Or"){
        int comp = Elements[num].nets.size() -1;
        Elements[num].x =comp+1;
        Elements[num].y =comp+1;
    }
    else if(Elements[num].type=="Xor"){
        int comp = Elements[num].nets.size() -1;
        Elements[num].x =2*comp;
        Elements[num].y =comp+2;
    }
    else if(Elements[num].type=="Nand"){
        int comp = Elements[num].nets.size() -1;
        Elements[num].x =comp;
        Elements[num].y =comp;
    }
    else if(Elements[num].type=="Nor"){
        int comp = Elements[num].nets.size() -1;
        Elements[num].x =comp;
        Elements[num].y =comp;
    }
    else if(Elements[num].type=="Xnor"){
        int comp = Elements[num].nets.size() -1;
        Elements[num].x =2*comp;
        Elements[num].y =comp;
    }
    else{
         Elements[num].x=1;
         Elements[num].y= 1;
    }

}

void merge2(Element Elements [],int num1, int num2,int n){
    int temp = (Elements[num1].x + Elements[num2].x)*max(Elements[num1].y, Elements[num2].y);
    int A = temp;
    int x = Elements[num1].x + Elements[num2].x;
    int y = max(Elements[num1].y, Elements[num2].y);
    temp = (Elements[num1].x + Elements[num2].y)*max(Elements[num1].y,Elements[num2].x);
    if(A > temp){
        A = temp;
        x = (Elements[num1].x + Elements[num2].y);
        y = max(Elements[num1].y,Elements[num2].x);
    }
    temp = (Elements[num1].y + Elements[num2].x)*max(Elements[num1].x, Elements[num2].y);
    if(A > temp){
        A = temp;
        x = (Elements[num1].y + Elements[num2].x);
        y = max(Elements[num1].x, Elements[num2].y);
    }
    temp = (Elements[num1].y + Elements[num2].y)*max(Elements[num1].x, Elements[num2].x);
    if(A > temp){
        A = temp;
        x = (Elements[num1].y + Elements[num2].y);
        y = max(Elements[num1].x, Elements[num2].x);
    }
    Elements[n].x = x;
    Elements[n].y = y;
}


int findGCD(int a, int b) {
   if (b == 0)
      return a;
    return findGCD(b, a % b);
}
void lowestFraction(int num1, int num2){
      int denom;
      denom = findGCD(num1,num2);
      num1/=denom;
      num2/=denom;
      cout<< "aspect ration is: "<<num1<<"/"<<num2<<endl;
}

int main()
{
	ifstream file;
	file.open("netlist.txt");
	int n = temp(file);
    Element Elements[n+1];  // total object
	file.open("netlist.txt");
	Get_tada(file,Elements, n);
	for(int i=0; i<n; i++){ // adjust && remove spaces
		Adjust_data(Elements,i);
		Remove_trash(Elements,i);
        fillArea(Elements,i);
	}
    for(int i=0; i<n; i++){
        //cout << Elements[i].name;
        //cout << endl;
        //cout << Elements[i].x<<"*"<<Elements[i].y<<endl;
        for(int j = 0; j < Elements[i].nets.size(); j++){
           // cout<< Elements[i].nets[j];
        }
        //cout << endl;
    }
    Elements[n].x = Elements[0].x;
    Elements[n].y = Elements[0].y;
    
    merge2(Elements,0,3,n);
    for(int i=1; i< n; i++){
        merge2(Elements,n,i,n);
    }
    double sum = 0; 

    for(int i=0; i< n; i++){
        sum = sum + Elements[i].x*Elements[i].y;
        cout <<"gate" <<i<<": " <<Elements[i].x << "*"<< Elements[i].y<<"  ";
    }
    cout<<endl;
    cout <<"min area is: "<< Elements[n].x*Elements[n].y<< endl;
    lowestFraction( Elements[n].x,Elements[n].y);
    double wasted = (1-(sum/(Elements[n].x*Elements[n].y)))*100;
    cout<< "percentage of wasted area is: "<< wasted<< "%";
}




