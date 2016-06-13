#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


#define MAX_MTEXT 4096
struct mbuf
{
	long mtype; // Message type
	char mtext[MAX_MTEXT]; //Message body
};

void help(char * temp)
{
	printf("Usage: %s\n",temp);
	printf("The process waits for user input.\n");
	printf("Usage: %s -h\n",temp );
	printf("h: Prints help information\n");
	exit(EXIT_FAILURE);
}

void* receivemessages()
{
	while(1)
	{
		key_t key = (int)getpid();
		int msqid;
		msqid = msgget(key,IPC_CREAT | 0666);
		if(msqid == -1)
		{
			perror("msgget");
			exit(EXIT_FAILURE);
		}
		struct mbuf recmsg;
		int msgrec;
		msgrec = msgrcv(msqid, &recmsg, sizeof recmsg.mtext, 0,0);
		if(msgrec == -1)
		{
			if(errno == EIDRM)
			{
				return EXIT_SUCCESS;
			}
			perror("msgrec");
			exit(EXIT_FAILURE);
		}
		printf("%s\n",recmsg.mtext );
	}	
}

int main(int argc, char * argv[])
{
	char opt;
	int hcount=0;
	while((opt = getopt(argc, argv, "h")) != -1)
	{
		switch(opt)
		{
			case 'h':
				hcount=1;
				break;
			default:
				return -1;
		}
	}
	if(hcount == 1)
	{
		help(argv[0]);
	}
	pthread_t receivethread; //receiving messages on different thread
	if((pthread_create(&receivethread, NULL, receivemessages, (void *)NULL)) != 0)  // run on other thread
	{
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	key_t key = (int)getpid();
	int msqid;
	msqid = msgget(key,IPC_CREAT | 0666);
	if(msqid == -1)
	{
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	char * pid = malloc(16);
	snprintf(pid, 16, "%i", (int)getpid());
	char * sentstring = malloc(32);
	sentstring[0] = '\0';
	sentstring = strcat(sentstring, "Process ");
	sentstring = strcat(sentstring, pid);
	sentstring = strcat(sentstring, " sent: ");
	while(1)
	{
		char * line = NULL;
		size_t len = 0;
		getline(&line, &len, stdin);
		line[strlen(line) -1] =0;
		line[len] = '\0';
		if(strncmp(line,"exit",4) == 0)
		{
			break;
		}
		char * fullsentstring;
		if((fullsentstring = malloc(strlen(sentstring) + strlen(line) + 1)) != NULL)
		{
			fullsentstring[0] = '\0';
			fullsentstring = strcat(fullsentstring, sentstring);
			fullsentstring = strcat(fullsentstring, line);
		}
		struct mbuf msg;
		msg.mtype = 100;
		strcpy(msg.mtext, fullsentstring);
		int length = strlen(msg.mtext);
		msg.mtext[length] = '\0';
		int maxind,ind;
		struct msqid_ds ds;
		struct msginfo info;
		int msqidint;
		maxind = msgctl(0, MSG_INFO,(struct msqid_ds*)&info);
		if(maxind == -1)
		{
			perror("msgctl");
			exit(EXIT_FAILURE);
		}
		for(ind = 0; ind <= maxind; ind++)
		{
			msqidint = msgctl(ind, MSG_STAT, &ds);
			if(msqidint == -1)
			{
				if(errno != EINVAL && errno != EACCES)
				{
					continue;
				}
			}
			if((int)ds.msg_perm.__key == (int)getpid())
			{

			}
			else
			{
				int tempmsqid;
				tempmsqid = msgget(ds.msg_perm.__key,IPC_CREAT | 0666);
				if(msgsnd(tempmsqid, &msg, length+1,0) == -1)
				{
					perror("msgsnd");
					exit(EXIT_FAILURE);
				}
			}

		}
	}
	if(msgctl(msqid, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(EXIT_FAILURE);
	}
	if(pthread_join(receivethread, NULL) != 0)
	{
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}