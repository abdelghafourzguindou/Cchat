#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(void)
{
    const int WORD = 1024;
    const int port = 100;
    const char* ip = "127.0.0.1";
    int dss;
    int dsc;
    int size_of_clt_addr;
    int clt_number = 0;
    int maxDS = 0;
    int i, j;
    int lstSocket[20];

    char msg_send[WORD];
    char msg_recv[WORD];

    struct sockaddr_in addr_ser;
    struct sockaddr_in addr_clt;

    socklen_t lgaClt = sizeof(addr_clt);

    fd_set lstDS;

    addr_ser.sin_family = AF_INET;
    addr_ser.sin_port   = htons(port);
    inet_aton(ip, &(addr_ser.sin_addr));
    memset(&addr_ser.sin_zero, '0', 8);

    dss = socket(PF_INET, SOCK_STREAM, 6);
    if(dss == -1)
    {
        perror("error craete socket\n");
        exit(-1);
    }

    int bind_result = bind(dss, (struct sockaddr*)& addr_ser, sizeof(struct sockaddr));
    if(bind_result == -1)
    {
        perror("error bind\n");
        exit(-1);
    }
    puts("bind done\n");

    int listen_res = listen(dss, 5);
    if(listen_res == -1)
    {
        perror("error : listen error");
        exit(-1);
    }
    printf("listen...\n");

    lstSocket[0] = dss;
    maxDS        = dss;
    clt_number++;

    while(1)
    {
        FD_ZERO(&lstDS);

        for(i = 0; i < clt_number; i++)
        {
            FD_SET(lstSocket[i], &lstDS);
            maxDS = lstSocket[i] > maxDS ? lstSocket[i] : maxDS;
        }

        select(maxDS + 1, &lstDS, NULL, NULL, NULL);

        if(FD_ISSET(dss, &lstDS))
        {
            dsc = accept(dss, (struct sockaddr*)& addr_clt, (socklen_t*)& size_of_clt_addr);
            if(dsc == -1)
            {
                perror("accept error");
            }
            puts("connection accepted\n");

            maxDS = dsc > maxDS ? dsc : maxDS;
            FD_SET(dsc, &lstDS);
            lstSocket[clt_number] = dsc;
            clt_number++;
        }
        else
        {
            for(i = 0; i < clt_number; i++)
            {
                if(FD_ISSET(lstSocket[i], &lstDS))
                {
                    memset(msg_recv, '\0', WORD);
                    if(recv(lstSocket[i], msg_recv, WORD, 0) == -1)
                    {
                        close(lstSocket[i]);
                        for(j = i; j < clt_number; j++)
                        {
                            lstSocket[j] = lstSocket[j+1];
                        }
                        clt_number--;
                    }
                    else 
                    {
                        memset(msg_send, '\0', WORD);
                        strcpy(msg_send, msg_recv);
    
                        for(j = 0; j < clt_number; j++)
                        {
                            if(lstSocket[j] != dss && lstSocket[j] != lstSocket[i])
                            {
                                if(send(lstSocket[j], msg_send, strlen(msg_send), 0) == -1)
                                {
                                    perror("can't sending");
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}