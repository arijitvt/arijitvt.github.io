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
	cout <<  "Heap location=" << sbrk(0) << endl;
	int count  = -1;
	int blockSize = -1;
	cout << "Enter the  size of memory to allocate" ;
	cin >> count ;
	cout << "Enter Block size " ; 
	cin   >> blockSize;

	char *data[count];
    for(int i = 0 ; i < count ; ++i) {
		data[i] = (char*) malloc(blockSize); 	
	}
	cout <<  "Heap location after memory allocation=" << sbrk(0) << endl;

	char ch ;
	do {
		int location;
		cout << "Enter the location to free" << endl;
		cin >> location;
		free(data[location]);
		cout <<  "Heap location after memory allocation=" << sbrk(0) << endl;
		cout << "Press s to stop" ;
		cin >> ch;
	} while(ch != 's');
	return 0;
}
