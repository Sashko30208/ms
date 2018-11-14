#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h> //memset()
#include <stdlib.h>  //exit()
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <unistd.h>//close()
#include <netinet/in.h>
#include <sys/select.h> //fd_set/zero etc..


#define BUFLEN 512 //max Buffer length
#define PORT 8888
#define REPLY " Is_Reply\n"

void die (char *s)
{
perror(s);
exit(1);
}


char *globalReply (char *cr)
{
    char *pointer;
    if((pointer =malloc(strlen(cr)+strlen(REPLY)))==NULL)
    {
        
        perror ("malloc");
        exit (1);
        return NULL;
    }    
    sprintf(*(&pointer),"%s%s\n",cr,REPLY);
    //printf ("\nptr:%s\n",*(&pointer));
    return *(&pointer);
}



int main()
{
    char buf[BUFLEN];
    struct timeval tv;
    int tsock; /* Ведущий сокет TCP */
    int usock; /* Сокет UDP */
    int ssock; /* Ведомый сокет TCP */
    fd_set rfds; /* Дескрипторы, готовые к чтению */
    /* инициализация сокетов tsock и usock */
    //nfds = MAX(tsock, usock) +1; /* Длина битовой маски для набора дескрипторов */
    int nfds= 3;
    //create a UDP socket
    if ((usock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket(udp)");
    }
    /*create a TCP socket
    (AF_INET(internet-домен),SOCK_STREAM(для TCP),протокол по умолчанию)*/
	if ((tsock = socket(AF_INET, SOCK_STREAM, 0))==-1) //создание сокета
	{
        die("socket(tcp)");
    }
    tv.tv_sec = 5;
    tv.tv_usec = 0;

        struct sockaddr_in si;
        bzero(&si, sizeof(si));
        /*заполнение сткутуры*/
    	si.sin_family = AF_INET; //семейство адресов
    	//Host TO Network Short - преобразование числа порядка хоста в сетевой
	    si.sin_port = htons(PORT); //номер порта
    	//Host TO Network Long - реобразование числа порядка хоста в сетевой
	    // INADDR_ANY - соединения с клиентами через любой IP
	    si.sin_addr.s_addr = htonl(INADDR_ANY); //IP-адрес
	
    while (1) 
    {
        FD_ZERO(&rfds);
        FD_SET(tsock, &rfds); 
        FD_SET(usock, &rfds);
       
        printf("i'm waiting 4 u..\n");
        if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv) <0) 
        {
            /* ошибка */
            printf("an error in select statement\n");
            continue;
        }
        if (FD_ISSET(tsock, &rfds)) 
        {
            socklen_t len;
            len=sizeof(si);
            ssock = accept(tsock, (struct sockaddr *)&si,&len);
            if(ssock <0)
            {
                /* ошибка */
                printf("error in send\n");
                close(ssock);
            }
            else
            {
                //globalReply(buf);
                if(send(ssock,globalReply(buf), strlen(globalReply(buf)),0)<0) 
                {
                    /* ошибка */
                    printf("error in send\n");
                }
                close(ssock);
                memset(buf, 0,BUFLEN);
            }
        }
        if (FD_ISSET(usock, &rfds)) 
        {
            socklen_t len;
            len=sizeof(si);
            if (recvfrom(usock, buf, sizeof(buf), 0, (struct sockaddr *)&si, &len) <0) 
            {
            
                /*ошибка*/
                printf("error in recvfrom\n");
                //close(usock);
            }
            else
            {
                //globalReply(buf);
                if(sendto(usock, globalReply(buf), strlen(globalReply(buf)), 0, (struct sockaddr *)&si, sizeof(si))<0)
                {
                    /*ошибка*/
                    printf("error in sendto\n");
                }
                //close(usock);
                memset(buf,0,BUFLEN);
            }
        }
    } 
    close(usock);
    close(tsock);
    return 0;
}