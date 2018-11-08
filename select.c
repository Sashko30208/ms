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

#define BUFLEN 512 //max Buffer length
#define PORT 8888
#define REPLY " Is_Reply\n"

void die (char *s)
{
perror(s);
exit(1);
}
char handler[BUFLEN] (char[] cr)
{
    char ret[BUFLEN];
    memset (ret,'\0',BUFLEN);
    sprintf(*(&ret),"%s%s\n",cr," Is reply");
    return ret;
}

int main()
{
    char buf[BUFLEN];
    int tsock; /* Ведущий сокет TCP */
    int usock; /* Сокет UDP */
    int ssock; /* Ведомый сокет TCP */
    fd_set rfds; /* Дескрипторы, готовые к чтению */
    /* инициализация сокетов tsock и usock */
    //nfds = MAX(tsock, usock) +1; /* Длина битовой маски для набора дескрипторов */
    int nfds= 3;
    

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
       

        if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) <0) 
        {
            /* ошибка */
            printf("an error in select statement");
            continue;
        }
        if (FD_ISSET(tsock, &rfds)) 
        {
            len=sizeof(si);
            ssock = accept(tsock, (struct sockaddr *)&si,&len);
            if(ssock <0)
            {
                /* ошибка */
                close(ssock);
            }
            else
            {
                buf = handler(buf);
                if(send(ssock,buf, sizeof(buf))<0) 
                {
                    /* ошибка */
                    printf("error in send");
                }
                close(ssock);
            }
        }
        if (FD_ISSET(usock, &rfds)) 
        {
            len = sizeof(si);
            if (recvfrom(usock, buf, sizeof(buf), 0, (struct sockaddr *)&si, &len) <0) 
            {
            
                /*ошибка*/
                close(usock);
            }
            else
            {
                buf = handler(buf);
                if(sendto(usock, buf, strlen(buf), 0, (struct sockaddr *)&si, sizeof(si))<0)
                {
                    /*ошибка*/
                    printf("error in send");
                }
                close(usock);
            }
        }
    } 
    return 0;
}