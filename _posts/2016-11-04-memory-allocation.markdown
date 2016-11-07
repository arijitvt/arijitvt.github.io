---
layout: post
title:  "Memory Allocation"
date:   2016-11-04 23:20:37 -0400
categories: jekyll update
---
Memory allocation/deallocation is one of interesting problem in operating systems. It is one of the fundamental thing that every process needs. Mostly in a standard C program, we use m
What is interesting here is that while allocation, we are passing the size of the memory but while deallocating, we are just sending the pointer to the memory itself. Then the question naturally comes, that where does the free functions getting the size information. Well, the magic lies in the  memory allocated by malloc itself. In the first place malloc stores the size of the memory it has allocated and free determines the size from that predefined location. This is one of the reason behind **Free the memory by free, if it is allocated by malloc.calloc.** .alloc  group of functions like malloc, calloc, which allocates memory of specified size, where malloc just allocates memory and returns the first location of the memory and calloc initalizes that memory with NULL. A sample program looks like this, 
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

The system calls that are actually interacting with the kernel is brk/sbrk. The nice thing about free is that if we free  a block of memory, it does not go to operating system directly, rather malloca maintains a list of free memory and mark this block free and append this to the freelist. In future if it gets any request of allocating memory it re-uses this one.




