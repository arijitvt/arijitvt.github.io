#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) 
{
	pid_t pid = getpid();
	cout << "Pid of this process :" << pid << endl;
	char ch; 
	do 
	{

			cout <<  "Heap location=" << sbrk(0) << endl;
			int count  = -1;
			int blockSize = -1;
			cout << "Enter the number of memory blocks to be allocated: " ;
			cin >> count ;
			cout << "Enter Block size " ; 
			cin   >> blockSize;

			char *data[count];
			for(int i = 0 ; i < count ; ++i) {
					data[i] = (char*) malloc(blockSize); 	
					sprintf(data[i],"%d",i);
			}
			cout <<  "Heap location after memory allocation=" << sbrk(0) << endl;

			int location;
			cout << "Enter a location: "; 
			cin >> location; 

			for(int i = location; i  < count ; ++i) {
					free(data[i]);
			}

			cout <<  "Heap location after memory free=" << sbrk(0) << endl;
			cout << "Enter s to stop";
			ch =getchar();

	}while(ch != 's');
	//char ch ;
	//do {
	//	int min , max; 
	//	min = max = 0;
	//	cout << "Enter a range to free:min max " ;
	//	cin >> min >> max;
	//	for (int i= min ; i < max ; ++i) {
	//			//cout  << "Data in this location: " << data[i] << endl;
	//			free(data[i]);
	//	}
	//	cout <<  "Heap location after memory allocation=" << sbrk(0) << endl;
	//	cout << "Press s to stop" ;
	//	ch = getchar();
	//} while(ch != 's');
	return 0;
}
