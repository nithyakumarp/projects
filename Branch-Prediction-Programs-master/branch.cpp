#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

int main()
{
	long long current_hex, next_hex; 
	int index=0, buffer=0, correct=0, wrong=0, branch_count;
	long long branch[32]={0};
	int  predictor[32]={0};
	ifstream in;
	stringstream str;
	in.open("itrace.txt");
	string current, next;
	getline(in , current);

	while(!(in.eof()))
	{
		stringstream str1;
		str1<<hex<<current;
		str1>>current_hex;
		stringstream str2;
		getline(in, next);
		str2<<hex<<next;
		str2>>next_hex;
	
//		cout<<"\n"<<current_hex<<"  "<<next_hex<<"  "<<next_hex - current_hex;

		for(int i=0; i<32; i++)
		{
			int check;
			if (current_hex == branch[i])      // If the current address is already present in the table
			{
//			cout<<"\n adddress match found "<<current_hex;
			if(((next_hex - current_hex)>15) || ((next_hex - current_hex)<0))// check if the current address is now taken or not
				{
					check = 1;
//					cout<<"   and it is taken"; 
				}
				else{
//					cout<<"  but it is not taken";
					check = 0;
				}
				if (predictor[i] == check)     // check if the current state of the branch matches with the predicted value. if yes, increment correct
				{
					correct+=1;     
//					cout<<"\n correct is incremented";
					break;       
				}
				else{
					wrong+=1;
//					cout<<"\n wrong is incremented";
					predictor[i] = 1-predictor[i];  // since predicted value is wrong, change the value in the predictor.
					break;
				}
			}
		//	else{
		//		continue;
		//	}	
		}
		
		if(((next_hex - current_hex)>15) || ((next_hex - current_hex)<0x0)) // if the current address is a branch
		{
//			cout<<"\n branch found: "<<current_hex; 
			int hit;
			
			for(int i=0; i<32; i++)
			{
				if(current_hex == branch[i])  //check if the current branch is a hit or not. 
				{
//					cout<<"  and it is already there ";
					predictor[i] = 1; 
					hit = 1;
					break;
				}		
				else
				{
//					cout<<"  and it is not present";
					hit = 0;
				      // if buffer miss, count it.
				}
					
			}
			if(hit == 0)
			{
				if(index == 31)
					index = 0;      // if the end of the branch table is reached, move index to 0
				else
					index+=1;
				
				buffer+=1;
//				cout<<"\n making a new "<<current_hex<<" branch entry at "<<index;
				branch[index] = current_hex; // if buffer miss, make the branch table entry.
				predictor[index] = 1;		
			}
		}
		current = next;
	}
for(int i=0;i<32;i++)
{
cout<<"\n"<<branch[i]<<"  "<<predictor[i];
}
cout<<"\n The number of correct predictions = "<<correct;
cout<<"\n The number of wrong predictions = "<<wrong;
cout<<"\n The number of buffer misses = "<<buffer;
return(0);
}
	
