C = gcc
all: multthread multthreadmutex

multthread : multthreadmutex.c
	$(C) -o multthread multthreadmutex.c

multthreadmutex : multthreadmutex.c
	$(C) -DPTHREAD_SYNC -o multthreadmutex multthreadmutex.c

clean:
	rm multthread multthreadmutex

