#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

int main()
{
	long long current_hex, next_hex;  // long long is used to store the huge branch address value
	bool already_present, taken;
	int index=-1, buffer=0, correct=0, wrong=0, current_index;
	long long branch[16]={0};
	int  prediction[16]={0};
	ifstream in;
	stringstream str;
	in.open("itrace.txt");   // the given trace file was converted into a txt file and accessed.
	string current, next;
	getline(in , current);

	while(!(in.eof()))         // loop until the end of the file
	{
		stringstream str1;
		str1<<hex<<current;
		str1>>current_hex;
		stringstream str2;
		getline(in, next);
		str2<<hex<<next;
		str2>>next_hex;                     // read the file contents as hex to perform calculations
		for(int i=0; i<16; i++)
		{
			if (current_hex == branch[i])      // If the current address is already present in the table, make the already_present variable TRUE. Else, FALSE,
			{
				already_present = true;
				current_index = i;
				break;
			}
			else
			{
				already_present = false;
			}
		}
		
		if(((next_hex - current_hex)>15) || ((next_hex - current_hex)<0x0) || (already_present == true)) // if the current address is a branch
		{
			if(((next_hex - current_hex)>15) || ((next_hex - current_hex)<0))  //check if the branch is taken or not.
			{
				taken = true;                                               // if the branch is taken, set the taken varialble as TRUE.
			}
				else
				{
					taken = false;
				}
	// 0(00)-strongly not taken;  1(01)-weakly not taken;  2(10)-weakly taken;   3(11)-strongly taken			

             // Based on whether the branch is already present or not in the branch history table, and based on it is now talen or not, further steps are carried out.
	
			if(already_present == true)
			{
		// if the branch is alread present, check the correspinding value in the predicton table. 
				if(prediction[current_index] == 0)
				{
					if(taken == true)
					{
						wrong+=1;                      // if the prediction value is strongly not taken and if the branch is currently taken, increment wrong and change the prediction bits to weakly not taken
						prediction[current_index] += 1;
					}
					else
					{
						correct += 1;
					}
				}
				else if(prediction[current_index] == 1)
				{
					if(taken == true)
					{
						wrong+=1;    // if the prediction value is found as weakly not taken and it is actually taken, increment wrong and make it weakly taken
						prediction[current_index] += 1;
					}
					else
					{                            // if the prediction value is found as weakly not taken and it is actually not taken, increment correct and make it strongly not taken
						correct += 1;
						prediction[current_index] -= 1;
					}
				}
				// similarly the other possibilities are also defined
				else if(prediction[current_index] == 2)
				{
					if(taken == true)
					{
						correct+=1;
						prediction[current_index] += 1;
					}
					else
					{
						wrong += 1;
						prediction[current_index] -= 1;
					}
				}
				else if(prediction[current_index] == 3)
				{
					if(taken == true)
					{
						correct+=1;
					}
					else
					{
						wrong += 1;
						prediction[current_index] -= 1;
					}
				}
			}				
			else         // if the branch is not already present, make a new entry
			{
				if(index == 15)
					index = 0;      // if the end of the branch table is reached, move index to 0. If not, increment index.
				else
					index+=1;
				
				buffer+=1;          // count this as a buffer miss
				branch[index] = current_hex; // new entry into branch table
				//The prediction bit for the new entry is set as wealky not taken.
				prediction[index] = 1 ; 
			}
		}
		current = next;
	}// print all the number of correct, wrong predictoins and buffer miss.
cout<<"\n The number of correct predictions = "<<correct;
cout<<"\n The number of wrong predictions = "<<wrong;
cout<<"\n The number of buffer misses = "<<buffer;
return(0);
}
	
