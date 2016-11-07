#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv)
{
	cout <<  "Heap location=" << sbrk(0) << endl;
	int count  =  100000;
	int blockSize = 4096;

	char *data[count];
	for(int i = 0 ; i < count ; ++i) {
			data[i] = (char*) malloc(blockSize);
			sprintf(data[i],"%d",i);
	}
	cout <<  "Heap location after memory allocation=" << sbrk(0) << endl;

	cout << "Freeing data in the range 10000-40000"  << endl;
	for (int i = 10000; i < 40000; ++i) {
		free(data[i]);
	}

	cout <<  "Heap location after memory free from middle=" << sbrk(0) << endl;

	cout << "Now clearing to the end of heap " << endl;

	for(int i = 60000; i  < count ; ++i) {
			free(data[i]);
	}
	cout <<  "Heap location after memory free=" << sbrk(0) << endl;
	return 0;
}
