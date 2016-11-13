---
layout: post
title:  "Basics of Malloc Magiq"
date:   2016-11-04 23:20:37 -0400
categories: jekyll update
---

Basics
-----------
Memory allocation/deallocation is one of interesting problem in operating system. It is one of the fundamental things that every process needs. Mostly in a standard C program, we use malloc/calloc functions (seldom realloc)  for that purpose. The following snippet shows one such typical example. 
{% highlight c %}
int main()
{
	void *p = malloc(sizeof(int));
	memset(p,0, sizeof(int));	
	// Memory Allocation is done. The above two lines
	// can be achieved with a single line by calloc.
	// We will do a comparison of these two ways of
	// memory allocation in a separate topic.
	/**************
	* Do the work with the memory
	**/
	// Now free the memory
	free(p);
	return 0;
}
{% endhighlight %}
What is interesting here, is that, while allocation, we are passing the size of the memory but during free, we are just sending the pointer to the memory itself. Then the question naturally comes, that where does the free function getting the size information. Well, the magic lies in the  memory allocated by malloc itself. During memory allocation, malloc stores the size of the block at the beginning of it and returns the pointer to the memory, which points to a location of after the size storing block. A typical block allocated  by malloc looks like (a simplified view from malloc.c implementation),

{%highlight bash%}

+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
| Size of previous chunk
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
| Size of chunk
mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-                                         
| User data starts here... .
. .
. (malloc_usable_size() bytes) .
. .
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
| (size of chunk, but used for application data)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
| Size of next chunk, in bytes
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

{%endhighlight%} 

For malloc caller, the memory would point to the location pointed by _mem->_.

Usable Size
-----------------

The second interesting thing about malloc is,  when we are requesting certain amount of memory from the system, most of them time malloc allocates more memory than that, due to alignment and padding. The following code snippet shows, that a call to malloc requesting to allocate 4 bytes of memory leads to a  memory allocation of bigger size, which can be seen in the output of the subsequent call to `malloc_usable_size` (which returns the size of the memory,that can be used by prog), that returns 24 on Linux x86\_64 machine. Though the actual memory allocated by malloc is even more than that. 

{% highlight cpp %}
#include <iostream>
#include <unistd.h>
#include <malloc.h>
using namespace std;

int main() {
	void *ch =  malloc(sizeof(int)); 
	cout  << "Usable size : " << malloc_usable_size(ch) << endl;
	return 0;
}

////////////////
// output
////////////////
// Usable size : 24

{% endhighlight %}

The output of a similar prog ( I guess by now you can guess the code for this, so I am not posting it here), could bolster this padding logic even more. 

{%highlight bash%}
Requsted= 22 Usable= 24
Requsted= 23 Usable= 24 
Requsted= 24 Usable= 24 
.........
Requsted= 39 Usable= 40
Requsted= 40 Usable= 40
Requsted= 41 Usable= 56
{% endhighlight %}


Top of the Heap
-------------------

Now we are moving towards the final part of the blog, where we will try to show the  movement of the top of the heap with memory allocation and de-allocation. `malloc` internally calls `brk` or `mmap` for memory allocation from the system. As we know, system calls are quite expensive, so malloc put many optimization techniques, to minimize that system calls and re-use already allcoated memory from the system. For sake of simplicity, let's consider that malloc is not using `mmap` system calls(because mmapped regions can not be coalesced into one). 

{% highlight cpp %}

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <malloc.h>
using namespace std; 


int main()
{

	cout << "Heap location before memory allocation=" 
		<< sbrk(0) << endl;
	int count = 1000000; 
	char *data[count];
	const int blockSize = 1024; 

	for (int  i= 0 ; i < count ; ++i ) {
		data[i] = (char*) malloc(blockSize);		
	}

	cout << "Heap location after memory allocation=" 
		<< sbrk(0) << endl;
	//Trying to trim right after deletion. 
	//This is will trim a bit
    malloc_trim(0);
	cout << "Heap Location after trim= " 
		<< sbrk(0) << endl;
	//Futher call to trim has no effect.
    malloc_trim(0);
	cout << "Heap Location after trim= " 
		<< sbrk(0) << endl;

	//Removing some spaces from the middle. 
	for(int i = 10000; i < 40000; ++i) {
		free(data[i]);
	}

	cout << 
		"Heap location after massive free from the middle=" 
		<< sbrk(0) << endl;
	//No effect of trimp after freeing memory from the middle. 
	//Because trim only trims from the top.
	malloc_trim(0) ; 
	cout << "Effect of trimp after free from the middle= " 
		<< sbrk(0) << endl;

	//Now removing 10 blocks of memory from the top 
	//and top won't change. 
	for(int  i = count - 10 ; i< count ; ++i) {
		free(data[i]);
	}

	//No affect can be seen after free
	cout << "Heap location after removing 10 blocks from the top= " 
		<< sbrk(0) << endl;

	//However if we can trim now, we can see the result
	malloc_trim(0);
	cout << "Heap location after trim= " << sbrk(0) << endl;


	//Nnow free a huge block of memory from top. 
	//This will lower the top
	for (int i = 40000; i < count -10 ; ++i) {
		free(data[i]);
	}

	cout << "Heap location after huge free to the top " << sbrk(0) << endl;


	return 0;
	
}

{% endhighlight %}

{% highlight bash %}
Heap location before memory allocation=0x229d000
Heap location after memory allocation=0x4028c000
Heap Location after trim= 0x40270000
Heap Location after trim= 0x40270000
Heap location after massive free from the middle=0x40270000
Effect of trimp after free from the middle= 0x40270000
Heap location after removing 10 blocks from the top= 0x40270000
Heap location after trim= 0x4026d000
Heap location after huge free to the top 0x2ca9000


{% endhighlight %}

Let's analyze the output of the program.`sbrk(0)` returns the current location of the heap top(0x229d000). After we allocated 1000000 blocks of memory, where each block is again 1024 bytes, then the heap moves to 0x4028c000. However now it allocates some extra memory on the top, which can be removed by calling `malloc_trim`. This function calls, `sbrk` system call with -ve number, to return the allocated memory to the the system. That's why we see the top gets reduced to 0x40270000. However subsequent call to `malloc_trim` could not reduce it any further, as we have already returned any extra memory that malloc has allocated, to the system already. 
Then the program deallocates some memory from the middle of the chunk. However this memory, does not get returned to the system, rather malloc recycles that for future memory allocation requests. That's why even after freeing the memory, the top of heap does not reduced. Hence even `malloc_trim` could not make any difference to the top either. 
Then the prog frees from memory from the top. Surprisingly even then the top did not get lowered itself. 
We can see that from this, `Heap location after removing 10 blocks from the top= 0x40270000`. But now when called trim, then the expected result come out. 
Finally when we deallocate huge memory block from the top, malloc automatically frees that memory to the system. 
`Heap location after huge free to the top 0x2ca9000`.



Reference
1. [The Linux Programming Interface](http://man7.org/tlpi/)
2. [Malloc Source Code](https://code.woboq.org/userspace/glibc/malloc/malloc.c.html)

