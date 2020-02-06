#include <iostream>
#include <sys/types.h>
#include<sys/socket.h>
#include<sys/socketvar.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<thread>
#include<string.h>
#include<errno.h>
#include <unistd.h>
using namespace std;


void recvWork(int mode)
{

    sleep(0.5);
        if(mode == 1)
        {
            int sock = socket(AF_INET,SOCK_DGRAM,0);
            if(sock < 0)
            {
                cout << "error create socket req" <<endl;
            }
            int broadcast = 1;
            if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))<0)
            {
                cout << "error setsockopt req - "<< strerror(errno) <<"  " <<errno <<endl;
            };
            struct sockaddr_in recv;
            int len = sizeof(struct sockaddr_in);
            int bufSize = 256;
            char recvBuf[bufSize] = "";
            recv.sin_family = AF_INET;
            recv.sin_port = htons(8080);
            recv.sin_addr.s_addr = INADDR_ANY;
            if(bind(sock,(sockaddr*)&recv,sizeof(recv))<0)
            {
                cout << "error bind - " << strerror(errno) <<"  " <<errno<<endl;
            };
            while(true)
            {
                memset(recvBuf,0,bufSize);
                recvfrom(sock,recvBuf,bufSize,0,(struct sockaddr*)&recv,&len);
                cout << recvBuf <<endl;
            }
        }
        else
        {
            int len = sizeof(struct sockaddr_in);
            int bufSize = 256;
            char recvBuf[bufSize] = "";
            int sockfd = socket(AF_INET,SOCK_DGRAM,0);
            if(sockfd < 0)
            {
                cout << "error create socket send" <<endl;
            }
            const int optval = 1;
            if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))<0)
            {
                 cout << "error setsockopt send - "<<strerror(errno)  <<"  " <<errno  <<endl;
            }
            struct sockaddr_in addr;
            bzero(&addr, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(8000);
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
            {
                 cout << "error multycast bind - " << strerror(errno) <<"  " <<errno<<endl;

            }
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq))<0)
            {
                 cout << "error setsockopt multicast send - "<<strerror(errno)  <<"  " <<errno  <<endl;
            }
            while(true)
            {
                memset(recvBuf,0,bufSize);
                recvfrom(sockfd,recvBuf,bufSize,0,(struct sockaddr*)&addr,&len);
                cout << recvBuf <<endl;
            }
      }
}

int main()
{
    int pid = getpid();
    cout << "choose mode, 1 - broadcast, 2 - multycast" <<endl;
    int mode;
    cin >> mode;

    std::thread recvThread(recvWork,mode);
    recvThread.detach();

    if(mode == 1)
    {
        int sockS = socket(AF_INET,SOCK_DGRAM,0);
        if(sockS < 0)
        {
            cout << "error create socket send" <<endl;
        }
        int broadcast = 1;
        if(setsockopt(sockS,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))<0)
        {
            cout << "error setsockopt send - "<<strerror(errno)  <<"  " <<errno  <<endl;

        };
        struct sockaddr_in send;
        int len = sizeof(struct sockaddr_in);
        int bufSize = 256;
        char sendBuf[bufSize] = "";
        struct sockaddr s;

        send.sin_family = AF_INET;
        send.sin_port = htons(8080);
        send.sin_addr.s_addr = inet_addr("255.255.255.255");
        string req;
        while(true)
        {
            cout<<"input mes"<<endl;
            cin >>req;
            req += " from - "+std::to_string(pid);
            if(sendto(sockS, req.c_str(),req.length(),0,(struct sockaddr*)&send,sizeof(send)) < 0)
            {
                 cout << "error send" <<endl;
            };
        }
    }
    else
    {
        int sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(sockfd < 0)
        {
           cout << "error create socket send" <<endl;
        }
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8000);
        addr.sin_addr.s_addr = inet_addr("239.255.255.250");
        string req;
        while(true)
        {
            cout<<"input mes"<<endl;
            cin >>req;
            req += " from - "+std::to_string(pid);

            if(sendto(sockfd, req.c_str(),req.length(),0,(struct sockaddr*)&addr,sizeof(addr)) < 0)
            {
                cout << "error send" <<endl;
            };
        }

    }
    return 0;
}
