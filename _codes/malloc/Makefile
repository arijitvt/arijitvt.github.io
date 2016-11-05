
cc=g++
del=rm -rf

target=output


cflags=-g -c -std=c++0x -I$(boost_inc) -I.
lflags=-L$(boost_lib) -lboost_system -lboost_thread

objs= \
	  malloc_test.o


all:$(objs) 
	$(cc) $(objs) -o $(target)  $(lflags)


%.o:%.cpp
	$(cc) $(cflags) -o $@ $<


clean:
	$(del) $(objs)
	$(del) $(target)
