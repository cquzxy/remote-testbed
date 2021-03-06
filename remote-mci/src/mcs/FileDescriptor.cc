#include "protocols/MMSException.h"
#include "mcs/FileDescriptor.h"

namespace remote { namespace mcs {

bool FileDescriptor::serviceLoop()
{
	unsigned int i, instanceCount;
	int f;
	filedescriptorsbyfd_t::iterator fileDescriptorIterator;
	// register the timeout handler
	signal( SIGALRM, timeoutHandler );
	installSignalHandlers();

	do
	{
		instanceCount = instances.size();
		pollfd fdmap[instanceCount];
		FileDescriptor::buildPollMap(fdmap);
		
		if ( poll(fdmap,instanceCount,-1) < 0)
		{
			if (errno == EINTR)
				continue;
			__THROW__("Error in poll!");
		}

		try
		{
			// look for activity on any sockets
			for ( i = 0; i < instanceCount; i++ )
			{
				if ( fdmap[i].revents )
				{
					f = fdmap[i].fd;
					fileDescriptorIterator = instances.find(f);
					if (fileDescriptorIterator != instances.end())
					{
						fileDescriptorIterator->second->handleEvent(fdmap[i].revents);
					}
				}
			}
		}
		catch (remote::protocols::MMSException e)
		{
			clearTimeout();
			Log::error("Exception: %s", e.what());
			fileDescriptorIterator->second->clearTimeout();
			fileDescriptorIterator->second->destroy(false);
		}
	}
	while (!exitSignal);

	return exitSignal == 0;
}

FileDescriptor::FileDescriptor(int p_fd)
{
	clearTimeout();
	fd = p_fd;
	Log::info("Opened fd %i",fd);
	instances[fd] = this;
}

void FileDescriptor::buildPollMap(pollfd* map)
{
	int j = 0;
	filedescriptorsbyfd_t::iterator i;
	for (i = instances.begin(); i != instances.end(); i++)
	{
		map[j].fd = i->second->fd;
		map[j].events = POLLMSG|POLLRDBAND|POLLRDNORM|POLLIN|POLLPRI|POLLERR|POLLHUP|POLLNVAL;
		map[j].revents = 0;
		j++;
	}
}


FileDescriptor::~FileDescriptor()
{
	if (close(fd) != 0)
		Log::error("close(%i) - %s", fd, strerror(errno));
	else
		Log::info("Closed fd %i",fd);
	instances.erase(fd);
}

void FileDescriptor::destroy(bool silent)
{
	delete this;
}


void FileDescriptor::setTimeout(int secs)
{
		itimerval timeout;
		// stop the timer
		timeout.it_interval.tv_sec = 0;
		timeout.it_interval.tv_usec = 0;
		timeout.it_value.tv_sec = secs;
		timeout.it_value.tv_usec = 0;
		if (setitimer(ITIMER_REAL, &timeout, NULL))
		{
			__THROW__ ("Timer set error\n");
		};
		currentFd = fd;
}

void FileDescriptor::clearTimeout()
{
		itimerval timeout;
		// stop the timer
		timeout.it_interval.tv_sec = 0;
		timeout.it_interval.tv_usec = 0;
		timeout.it_value.tv_sec = 0;
		timeout.it_value.tv_usec = 0;
		if (setitimer(ITIMER_REAL, &timeout, NULL))
		{
			__THROW__ ("Timer clear error\n");
		};
		currentFd = -1;
}


RETSIGTYPE FileDescriptor::timeoutHandler(int sig)
{
	Log::warn("Timeout occured!");
	close(currentFd); // this should generate an exception
}

void FileDescriptor::exitSignalHandler(int signo)
{
	exitSignal = signo;
}

void FileDescriptor::installSignalHandlers()
{
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;

	sigaction(SIGPIPE, &act, NULL);

	memset(&act, 0, sizeof(act));
	act.sa_handler = exitSignalHandler;
	sigfillset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGSEGV, &act, NULL);
	sigaction(SIGKILL, &act, NULL);

}

filedescriptorsbyfd_t FileDescriptor::instances;
int FileDescriptor::currentFd = -1;
int FileDescriptor::exitSignal = 0;
}}
