/*****************************************************************************

$Id$

File:     em.h
Date:     06Apr06

Copyright (C) 2006-07 by Francis Cianfrocca. All Rights Reserved.
Gmail: blackhedd

This program is free software; you can redistribute it and/or modify
it under the terms of either: 1) the GNU General Public License
as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version; or 2) Ruby's License.

See the file COPYING for complete licensing information.

*****************************************************************************/

#ifndef __EventMachine__H_
#define __EventMachine__H_


#include "rubyisms.h"
#define EmSelect rb_thread_select

class EventableDescriptor;
class InotifyDescriptor;


/********************
class EventMachine_t
********************/

class EventMachine_t
{
	public:
		static int GetMaxTimerCount();
		static void SetMaxTimerCount (int);

	public:
		EventMachine_t (EMCallback);
		virtual ~EventMachine_t();

		void Run();
		void ScheduleHalt();
		void SignalLoopBreaker();
		unsigned long InstallOneshotTimer (int);
		unsigned long ConnectToServer (const char *, int, const char *, int);
		unsigned long ConnectToUnixServer (const char *);

		unsigned long CreateTcpServer (const char *, int);
		unsigned long OpenDatagramSocket (const char *, int);
		unsigned long CreateUnixDomainServer (const char*);
		unsigned long OpenKeyboard();
		//const char *Popen (const char*, const char*);
		unsigned long Socketpair (char* const*);

		void Add (EventableDescriptor*);
		void Modify (EventableDescriptor*);
		void Deregister (EventableDescriptor*);

		unsigned long AttachFD (int, bool);
		int DetachFD (EventableDescriptor*);

		void ArmKqueueWriter (EventableDescriptor*);
		void ArmKqueueReader (EventableDescriptor*);

		void SetTimerQuantum (int);
		static void SetuidString (const char*);
		static int SetRlimitNofile (int);

		pid_t SubprocessPid;
		int SubprocessExitStatus;

		int GetConnectionCount();
		double GetHeartbeatInterval();
		int SetHeartbeatInterval(double);

		unsigned long WatchFile (const char*);
		void UnwatchFile (int);
		void UnwatchFile (const unsigned long);

		#if defined(HAVE_SYS_EVENT_H) && defined(HAVE_SYS_QUEUE_H)
		void _HandleKqueueFileEvent (struct kevent*);
		void _RegisterKqueueFileEvent(int);
		#endif

		unsigned long WatchPid (int);
		void UnwatchPid (int);
		void UnwatchPid (const unsigned long);

		#if defined(HAVE_SYS_EVENT_H) && defined(HAVE_SYS_QUEUE_H)
		void _HandleKqueuePidEvent (struct kevent*);
		#endif

		uint64_t GetCurrentLoopTime() { return MyCurrentLoopTime; }

		// Temporary:
		void _UseEpoll();
		void _UseKqueue();

		bool UsingKqueue() { return bKqueue; }
		bool UsingEpoll() { return bEpoll; }

		void QueueHeartbeat(EventableDescriptor*);
		void ClearHeartbeat(uint64_t, EventableDescriptor*);

		uint64_t GetRealTime();

	private:
		bool _RunOnce();
		void _RunTimers();
		void _UpdateTime();
		void _AddNewDescriptors();
		void _ModifyDescriptors();
		void _InitializeLoopBreaker();
		void _CleanupSockets();

		bool _RunSelectOnce();
		bool _RunEpollOnce();
		bool _RunKqueueOnce();

		void _ModifyEpollEvent (EventableDescriptor*);
		void _DispatchHeartbeats();
		timeval _TimeTilNextEvent();
		void _CleanBadDescriptors();

	public:
		void _ReadLoopBreaker();
		void _ReadInotifyEvents();
        int NumCloseScheduled;

	private:
		enum {
			MaxEpollDescriptors = 64*1024,
			MaxEvents = 4096
		};
		int HeartbeatInterval;
		EMCallback EventCallback;

		class Timer_t: public Bindable_t {
		};

		std::multimap<uint64_t, Timer_t> Timers;
		std::multimap<uint64_t, EventableDescriptor*> Heartbeats;
		std::map<int, Bindable_t*> Files;
		std::map<int, Bindable_t*> Pids;
		std::vector<EventableDescriptor*> Descriptors;
		std::vector<EventableDescriptor*> NewDescriptors;
		std::set<EventableDescriptor*> ModifiedDescriptors;

		uint64_t NextHeartbeatTime;

		int LoopBreakerReader;
		int LoopBreakerWriter;
		#ifdef OS_WIN32
		struct sockaddr_in LoopBreakerTarget;
		#endif

		timeval Quantum;

		uint64_t MyCurrentLoopTime;

		#ifdef OS_WIN32
		unsigned TickCountTickover;
		unsigned LastTickCount;
		#endif

	private:
		bool bTerminateSignalReceived;

		bool bEpoll;
		int epfd; // Epoll file-descriptor
		#if defined(HAVE_EPOLL_CREATE)
		struct epoll_event epoll_events [MaxEvents];
		#endif

		bool bKqueue;
		int kqfd; // Kqueue file-descriptor
		#if defined(HAVE_SYS_EVENT_H) && defined(HAVE_SYS_QUEUE_H)
		struct kevent Karray [MaxEvents];
		#endif

		InotifyDescriptor *inotify; // pollable descriptor for our inotify instance
};


/*******************
struct SelectData_t
*******************/

struct SelectData_t
{
	SelectData_t();

	int _Select();

	int maxsocket;
	fd_set fdreads;
	fd_set fdwrites;
	fd_set fderrors;
	timeval tv;
	int nSockets;
};

#endif // __EventMachine__H_
