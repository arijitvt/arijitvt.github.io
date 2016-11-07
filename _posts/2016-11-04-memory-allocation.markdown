---
layout: post
title:  "Memory Allocation"
date:   2016-11-04 23:20:37 -0400
categories: jekyll update
---
Memory allocation/deallocation is one of interesting problem in operating systems. It is one of the fundamental thing that every process needs. Mostly in a standard C program, we use m
What is interesting here is that while allocation, we are passing the size of the memory but while deallocating, we are just sending the pointer to the memory itself. Then the question naturally comes, that where does the free functions getting the size information. Well, the magic lies in the  memory allocated by malloc itself. In the first place malloc stores the siz	e of the memory it has allocated and free determines the size from that predefined location. This is one of the reason behind **Free the memory by free, if it is allocated by malloc.calloc.** .alloc  group of functions like malloc, calloc, which allocates memory of specified size, where malloc just allocates memory and returns the first location of the memory and calloc initalizes that memory with NULL. A sample program looks like this,
{% highlight c %}
int main()
{
	void *p = malloc(sizeof(int));
	memset(p,0, sizeof(int));
	// Memory Allocation is done. The above two lines
	// can beachieved with a single line by calloc.
	// We will do a comparison of these two ways of
	// memory allocation in a separate topic.
	// Now free the memory
	free(p);
	return 0;
}
{% endhighlight %}

What is interesting here is that while allocation, we are passing the size of the memory but while deallocation, we are just sending the pointer to the memory itself. Then the question naturally comes, that where does the free functions getting the size information. Well, the magic lies in the  memory allocated by malloc itself. In the first place malloc stores the size of the memory it has allocated and free determines the size from that predefined location. This is one of the reason behind **Free the memory by free, if it is allocated by malloc/calloc.** .

The system calls that are actually interacting with the kernel is brk/sbrk. The nice thing about free is that if we free  a block of memory, it does not go to operating system directly, rather malloc maintains a list of free memory and mark this block free and append this to the freelist. In future if it gets any request of allocating memory it re-uses this one. The following program  will show that,

{% highlight cpp %}


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

{% endhighlight %}

If we analyze the output of the program, we can see after allocating 100k blocks each being of 4096 bytes of size, we got our heap pointer increased. If we stop the execution of the program after the memory allocation using `getchar` or something similar and check the /proc/<pid>/maps file, we will be able to determine the heap location of the program. Things will get interesting, after it de-allocates the memory region, which lies in between 10000 to 40000 range. We can see the heap location did not alter even after freeing the memory. Because free just marked this section as deleted and will be used for future allocations. However, when we are freeing the memory from middle to the end, the free function smartly decrease the end of the heap.

{% highlight bash %}
Heap location=0x1c82000
Heap location after memory allocation=0x1a4c1000
Freeing data in the range 10000-40000
Heap location after memory free from middle=0x1a4c1000
Now clearing to the end of heap 
Heap location after memory free=0x107ed000
{% endhighlight %}



Reference
1. [Book] (www.google.com)
2. [Link] (https://sploitfun.wordpress.com/2015/02/10/understanding-glibc-malloc/)

