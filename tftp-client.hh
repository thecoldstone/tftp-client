/**
 *
 * @file tftp-client.hh
 * @brief TFTP Client
 * @author Nikita Zhukov xzhuko01@vutbr.cz
 * @date 15.11.2021
 */
#ifndef _TFTP_CLIENT_H
#define _TFTP_CLIENT_H

#include <iostream>

#define OP_RRQ 1 // TFTP Opcodes
#define OP_WRQ 2
#define OP_DATA 3
#define OP_ACK 4
#define OP_ERROR 5

using namespace std;

class TftpClient
{
public:
    string file;
    int timeout;
    int maxSizeOfBlock;
    int isMulticast;
    string mode;
    string address;
    int port;

    int opOperation;
    int isIPV4;

    char fileBuffer[512];
    int fileSize;
    FILE *fp;
    int cntRespondBlock;
    int sendTotal;

    TftpClient();
    int listenTo(string server, string port);
    int run();
    int argparse(int argc, char **argv);

private:
    int nPackets;
    void *log(string msg, int type);
    void sendData(int sock, struct sockaddr_in server, int blockNumber);
};

#endif