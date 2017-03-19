# Unix Named Pipe
Pipe file system is one of the beatiful feature, which serves mutiple different purpose. The most amazing of them is the IPC. Writing local server(or process), which does time consuming job, can be communicated using unix pipe. So the all the clients(or communicating process) will open up the named pipe, which the server is listening and write into that pipe without waiting for any ack back from the server. On the other hand, the server will gradually read each request one by one from the file and serve the requested job. That way the client process can asychronously do the work, without delegating the job in another thread. 

In the following sections I will try to describe, how we can setup the link between two communicating process using unix named pipes.

## Creating Pipe
Creating a pipe is fairly simple. From the shell we can use,
```bash
mknod  /tmp/arijit_fifo c
```
or a little more handy way of doing this , 
```bash
mkfifo  /tmp/arijit_fifo 
```
From C/C++ program, we can achieve this by,
```C++
// S_FIFO  -> This is the fifo file
// S_IRUSR -> Reading permission
// S_IWUSR -> Writing Permission.
// 0       -> Asking to ignore this parameter
int rc  = mknod(fileName.c_str(),S_IFIFO | S_IRUSR | S_IWUSR, 0);
```

Now once this pipe creation is done, so we will go forward with reading from the file. Reading from the file completely(or in other words, tailing the file), is just like reading any other file. 
```bash
while true
  do 
	  read line < $reader_pipe;
	  echo $line
  done
```
Doing same from the C++
```C
fstream fs; 
fs.open(fileName.c_str(),fstream::app| fstream::in | fstream::out);
if(fs.is_open()){
   fs<< "Writing into pipe" << endl;
   fs.close();
 }
```
Putting all the things together, I wrote a small server prograim using bash shell script, which is going to read from the pipe and just simple echo them and only on receiving "exit" the proggram exits.  The code is  as below,
```bash
#!/bin/bash
reader_pipe=/tmp/arijit_reader;

function init 
{
  if [[ ! -p  $reader_pipe ]]; 
  then 
    mkfifo $reader_pipe
    echo "Pipe created" 
	fi
}

function reader 
{
	while true
	do 
		read line < $reader_pipe;
		echo $line
		if [[ $line == "exit" ]];
		then 
			break;
    fi
  done
}
```

I also wrote another C++ client code, which will open this pipe and write into this with tunable number of threads(tunable coming as a part of the argumnet to program).
```C
int threadHandler(int id)
{
	string fileName("/tmp/arijit_reader");
	int rc  = 
		mknod(fileName.c_str(),S_IFIFO | S_IRUSR | S_IWUSR, 0);
    fstream fs; 
	fs.open(fileName.c_str(),fstream::app| fstream::in | fstream::out);
	if(fs.is_open()){
		for(int counter = 0;  counter < 10000; ++counter) {
		    stringstream ss;
			ss<< "From thread " <<  id << ":" << counter ;
			fs << ss.str() << endl;
			ss.flush(); // just to make sure we are flushing the data 
			//this_thread::sleep_for(chrono::duration<int,milli>(200));
		}
		fs.close();
	} else {
		cerr << "Unable to open the file" << endl;
		return -1;
	}
	return 0;
}

int main(int argc, char **argv) 
{
	int counter =  atoi(argv[1]);
	vector<thread> tList; 
	for (int i = 0 ; i <  counter; ++i ) {
		tList.push_back(move(thread(threadHandler,i)));
	}	
	for_each(tList.begin(), tList.end(), [](thread& t) {t.join();});
	return 0;
}

```
## Experiment Results:

If we gradually increase the number of threads, we will see the gradual increase in the load using htop utility on a terminal. Since each thread is writing 10k times, I assume it is a good load to test with.
![img](https://arijitvt.github.io/images/general.png)
I was runnint this program in a laptop, with 8 Gigs of ram , 2.6Ghz I7 prorcessor with 8 HT cores, running ubuntu 14.04. Though this is entire io bound process, but I could see as I was increasing the number of threads, the shell server started consuming more and more CPU and the virtual memory usage by each thread in C++ program increased, keeping the cpu profile fairly constant, that also make sense, since each thread is doing  the same work. The high cpu usage of the server program, is due to managing the buffer the pipe file. I want to do more investigation on this. This is still an open question to me.

After I increased the thread count to 1022, one thread was not able to open the pipe. All further increase of the number of threads, that failure increased fairly linearly. After little bit  of investigation, I checked the limit using `/proc/<pid>/limits` in my ubuntu system and I found the max number of open file descriptor from a process allowed is 1024. With 1022 threads, we are going to open 1022+3 fd, which is exceeding the maximum permitted limit. 

