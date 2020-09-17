//#include<iostream>
#include<bits/stdc++.h>
#include<limits.h>
//#include<fstream>

using namespace std;

class BHT_2bit
{
	public:
	long long branch_addr,branch_dest;
	int prediction;
}hist_tab[8];


/*int prediction_change(char sym,int index)
{
	int prev,next;
	prev=hist_tab[index].prediction;
	if(sym=='+')
		if(hist_tab[index].prediction!=3)
			hist_tab[index].prediction++;
	else
		if(hist_tab[index].prediction!=0)
			hist_tab[index].prediction--;
	next=hist_tab[index].prediction;
	if((prev==2 && next==1)||(prev==1 && next==2))
		return 0;
	else
		return 1;
	
}
*/
int main()
{
	ifstream trace_file;
	trace_file.open("itrace.out");
	if (trace_file.fail()) {
		cerr << "Error: Could not open output file\n";
		return 1;
	}
	else
	{
		long long a1,a2;
		int flag_buffer=0,table_index=0, freq[8]={};
		int check,min,min_index;
		long long branches=0,correct_pred=0,incorrect_pred=0,buffer_miss=0;
		string addr1,addr2;
		getline(trace_file,addr1);
		stringstream convert1;
		convert1 << hex << addr1;
		convert1 >> a1;
/*		getline(trace_file,addr2);
			convert << hex << addr2;
			convert >> a2;
			cout<<a1<<"\n";
			cout << a2<<"\n";
	*/		
		while(!trace_file.eof())
		{
			getline(trace_file,addr2);
			stringstream convert1;
			convert1 << hex << addr2;
			convert1 >> a2;
			//cout<<a1<<"\n";
			//cout << a2<<"\n";
			
			for(int i=0;i<16;i++)
			{
				int prev,next;
				if(hist_tab[i].branch_addr==a1)
				{
					branches++;
					flag_buffer=1;
					prev=hist_tab[i].prediction;
					if(hist_tab[i].branch_dest==a2)
					{
						//correct_pred++;
						//predict_flag=prediction_change('+',i);
						if(hist_tab[i].prediction!=3)
							hist_tab[i].prediction++;
						next=hist_tab[i].prediction;
						//if(predict_flag==1)
						if(prev<next && next<3)
							incorrect_pred++;
						else
							correct_pred++;
						break;
					}
					else{
						//incorrect_pred++;
						//predict_flag=prediction_change('-',i);
						//hist_tab[i].branch_dest=a2;
						prev=hist_tab[i].prediction;
						if(hist_tab[i].prediction!=0)
							hist_tab[i].prediction--;
						next=hist_tab[i].prediction;
						//if(predict_flag==1)
						if(prev>next && next>2)
							incorrect_pred++;
						else
							correct_pred++;
						//(hist_tab[i].prediction==1)?(hist_tab[i].prediction=0):(hist_tab[i].prediction=1);
						break;
					}
				} 
			}
			if(((a2-a1)>15)||(a2<a1)&&(flag_buffer==0))
			{
				branches++;
				buffer_miss++;
				hist_tab[table_index].branch_addr=a1;
				hist_tab[table_index].branch_dest=a2;
				hist_tab[table_index].prediction=1;
				freq[table_index]++;
				check=0;
				while((freq[check++]!=0)&&(check<8));
				if(check==8)
				{
					min=INT_MAX;
					for(int i=0;i<7;i++)
					{
						if(freq[i]<min)
							min_index=i;
					}
					table_index=min_index;
				}
				else
					table_index++;
			}
			a1=a2;
			flag_buffer=0;
		}
		cout<<"Branches: "<<branches;
		cout<<"\nCorrect Predictions: "<<correct_pred;
		cout<<"\nIncorrect Predictions: "<<incorrect_pred;
		cout<<"\nBuffer misses: "<<buffer_miss; 
	}
}
