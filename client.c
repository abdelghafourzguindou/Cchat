#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void main(void)
{
    const int WORD = 1024;
    const int port = 100;
    const char* ip = "127.0.0.1";

    struct sockaddr_in addser;

    char* msg_send = calloc(WORD, sizeof(char));
    char* msg_recv = calloc(WORD, sizeof(char));

    addser.sin_family   =   AF_INET;
    addser.sin_port     =   htons(port);
    inet_aton(ip, &(addser.sin_addr));
    memset(&(addser.sin_zero), '0', 8);

    int ds = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ds == -1)
    {
        perror("error creating socket");
        exit(-1);
    }

    int conn_result = connect(ds, (struct sockaddr*)& addser, sizeof(struct sockaddr));
    if(conn_result == -1)
    {
        perror("error invalide connection");
        exit(-1);
    }

    while(1)
    {
        if(fork() == 0)
        {
            memset(msg_recv, '\0', WORD);
            if(recv(ds, msg_recv, WORD, 0) == -1)
            {
                perror("Cant recv");
            }
            printf("\n%s\n", msg_recv);   
        }
        printf("\nmsg# ");
        memset(msg_send, '\0', WORD);
        gets(msg_send);
        if(send(ds, msg_send, strlen(msg_send), 0) == -1)
        {
            perror("cant sending");
        }
    }
    close(ds);
}