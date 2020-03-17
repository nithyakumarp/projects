
#include<iostream>
#include<fstream>
#include<string.h>
#include<limits.h>  // to use INT_MAX
using namespace std;
int c = 0,x,y,w,source,destination;
string source_s, dest_s;
int graph[52][52];  // this 2D array is used to store the weights of the edges. --- adjacecy matrix.
string a,b,s;
string nodes[52];
string names[52];
int answer[52];
int dist[52];
int previous[52];
bool graph_set[52];
// Relax function updates a node with minimum distance if available
void relax(int min_node, int i, int graph[][52], int dist[])
{
    if(dist[i] > (dist[min_node] + graph[min_node][i]))  //to check if there is a possibility of shorter distance
    {
        dist[i] = dist[min_node] + graph[min_node][i]; // the shorter distance is updated
        previous[i] = min_node;        // the previous of the node is also updated.
    }

}
// print the path from source to destination
void print_result(int dist[], int previous[], int source, int destination)
{
    int temp = destination, n=1;
     answer[0] = destination;
    cout<<"\nThe shortest distance from the "<<names[source]<<" to "<<names[destination]<<" is: "<<dist[destination]<<" feet."<<endl;
    cout<<"The path from the "<<names[source]<<" to "<<names[destination]<<" is:"<<endl;
    while(temp != source)    // the previous of the node is recorded until the source is reached
    {
        answer[n] = previous[temp];//the previous of all the nodes from the destination to the source is recorded
        temp = previous[temp];
        n = n+1;
    }
     for(int i=n-1; i>=0; i--) // since the previous loop was run n-1 times, it is clear that there are n-1 vertices between the source and the destination
     {
         cout<<" "<<nodes[answer[i]]<<" ("<<names[answer[i]]<<")";//the path is printed from source to destination
         if(i!=0)          // this condition is just to make sure that the arrow is not printed for the last element
         {
             cout<<"  -->";
         }
     }
}
// Shortest path algorithm
void djikstra(int graph[52][52], int source, int destination)
{
       for(int i=0; i<52; i++)
    {
        dist[i] = INT_MAX;   // initializing the distance of all the elements to infinity (MAX_INT for integers)
        previous[i] = -1;   // initializing the previous of all the elements to -1.
        graph_set[i] = false;
    }
    dist[source] = 0;        // set the source distance alone to zero
    for(int count=0; count<51; count++)
    {
        int min_dis = INT_MAX, min_node;     //initialize min_dis to infinity every time the loop runs.
        for(int i=0; i<52; i++)
        {
            if((graph_set[i] == false) && (dist[i]<=min_dis)) //from all the nodes that are not in the set, the node with the minimum distance is found.
            {
                min_dis = dist[i];
                min_node = i;         // the index of the node with the minimum distance
            }
        }
        graph_set[min_node] = true;  // add the selected node to the set
        if(min_node == destination)
            break;
        for(int i=0; i<52; i++)
        {
            // if the node is not in the set, if it is a neighbour of the minimum node, it is relaxed.
            if((graph_set[i]==false) && (graph[i][min_node]) && (graph[min_node][i]) && (dist[min_node]!=INT_MAX))
            {
                relax(min_node,i,graph,dist);
            }
        }
    }
    print_result(dist, previous, source, destination);
}
int main()
{
    int size;
    for(int i=0; i<52; i++)
    {
        for(int j=0; j<52; j++)
        {
            graph[i][j] = 0;
        }
    }
    ifstream in1, in2;
    in1.open("names.txt");  // open the text files
    if(in1.fail())
    {
	cerr<<"\nError. Could not open the names.txt file ";    // prints error if there is a problem with opening the file
    }		
    in2.open("data.txt");
    if(in2.fail())
    {
	cerr<<"\nError. Could not open the data.txt file ";
    }
    while(!(in1.eof()))      // the loop is run until the end of the file
    {
        in1>>s;
        nodes[c] = s;       // the nodes are read and stored in an array
        getline(in1, names[c]);   // their corresponding building names are stored
        c = c + 1;
    }
    in2>>size;
    while (!(in2.eof()))
    {
        in2>>a>>b;         //a and b are the vertex read from one line of the file
        in2>>w;            // w is the weight between the vertex a and b
        for(int i =0; i<52; i++)
        {
            if(nodes[i] == a)   // identify the array index for the vertex
            {
                x = i;
            }
            if(nodes[i] == b)
            {
                y = i;
            }
        }
        graph[x][y] = w;   //update the 2D array with the weights at the index value found above.
        graph[y][x] = w;
    }
    cout<<"\n These are the list of buildings and their names. Choose the source and destination point from it. ";
    for(int i=0; i<52; i++)
    {
        cout<<"\n"<<nodes[i]<<" "<<names[i];
    }
    cout<<"\n Total number of buildings = "<<size;
    cout<<"\n Enter the source node ";     // The source and destination nodes are obtained from the user
    cin>>source_s;
    cout<<"\n Enter the destination node ";
    cin>> dest_s;
    // Since the vertex number entered by the user can have variable names, for example 23A, 23B and since the vertex values are randomly arranged between 20 and 82 in the map, the values are stored in an array and the ARRAY INDEX value of a particular vertex is considered as the node value now and the algorithm is implemented on this value. Basically, the actual vertex in the graph are mapped to a sequential set of numbers from 0 to 51.
    for(int i=0; i<52; i++)
        {
            if(nodes[i]==source_s)
            {
                source = i;   // for the user entered source vertex value, the integer value source is the mapped value that we are going to use.
            }
            if(nodes[i] == dest_s)
            {
                destination = i;   // integer value destination is the value for the destination vertex
            }
        }
    djikstra(graph, source, destination); // this function will find the shortest path and will call the print function.
    in1.close();
    in2.close();   // close the files.

    return(0);
}
