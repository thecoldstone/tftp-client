/**
 *
 * @file tftp-client.cpp
 * @brief TFTP Client
 * @author Nikita Zhukov xzhuko01@vutbr.cz
 * @date 15.11.2021
 */
#include "tftp-client.hh"
#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <ctime>

#define BSIZE 600
#define MODE "octet"
#define TFTP_PORT 69

/**
 * @brief Construct a new Tftp Client:: Tftp Client object
 * 
 */
TftpClient::TftpClient()
{
    this->mode = "octet";
    this->address = "127.0.0.1";
    this->port = TFTP_PORT;
    this->nPackets = 0;
    this->isIPV4 = 1;
    this->sendTotal = 0;
    this->fileSize = 0;
    this->maxSizeOfBlock = 512;
}

/**
 * @brief Sends data to the server
 * 
 * @param sock
 * @param server 
 * @param blockNumber number of the current block
 */
void TftpClient::sendData(int sock, struct sockaddr_in sserver, int blockNumber)
{
    char buffer[BSIZE], *p;

    int bytes = fread(&this->fileBuffer, sizeof(char), this->maxSizeOfBlock, this->fp);

    *(short *)buffer = htons(OP_DATA);
    p = buffer + 2;
    *(short *)p = htons(blockNumber);
    p += 2;
    strcpy(p, this->fileBuffer);
    sendto(sock, buffer, strlen(this->fileBuffer) + 4, 0, (struct sockaddr *)&sserver, sizeof sserver);

    memset(&fileBuffer, 0, sizeof(this->fileBuffer));

    this->cntRespondBlock = bytes;
    this->sendTotal += bytes;
    string logLine = "Sending DATA ..." + to_string(this->sendTotal) + "B out of " + to_string(this->fileSize) + "B within package #" + to_string(blockNumber);
    this->log(logLine, 0);
}

/**
 * @brief Send tftp request to the tftp server
 * 
 * @return int rc - return value
 */
int TftpClient::run()
{
    int sock, rc;
    struct sockaddr_in sserver;
    struct hostent *host;
    char buffer[BSIZE], *p;

    // Setting up the connection
    if (this->isIPV4)
    {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
    }
    else
    {
    }

    host = gethostbyname(this->address.c_str());
    sserver.sin_family = AF_UNSPEC;
    memcpy(&sserver.sin_addr.s_addr, host->h_addr, host->h_length);

    sserver.sin_port = htons(this->port);

    // Preparing first request body for sending
    *(short *)buffer = htons(this->opOperation); // Operation code
    p = buffer + 2;                              // Filename
    strcpy(p, this->file.c_str());

    p += strlen(this->file.c_str()) + 1; // Mode
    strcpy(p, this->mode.c_str());

    p += strlen(this->mode.c_str()) + 1;

    this->cntRespondBlock = sendto(sock, buffer, p - buffer, 0, (struct sockaddr *)&sserver, sizeof sserver);
    string logString = (this->opOperation == 1 ? "RRQ" : "WRQ");
    this->log(logString, 0);
    this->nPackets++;

    char fileBuffer[512];
    if (this->opOperation == 1)
    {
        this->fp = fopen(this->file.c_str(), "wb+");
    }
    else
    {
        this->fp = fopen(this->file.c_str(), "rb+");
    }
    struct stat st;
    stat(this->file.c_str(), &st);
    this->fileSize = st.st_size;
    int i = 0;

    memset(&buffer, 0, sizeof(buffer));

    // Listenining to the server's respond
    do
    {
        int server_len = sizeof sserver;
        // Getting respond from the server side
        this->cntRespondBlock = recvfrom(sock, buffer, BSIZE, 0, (struct sockaddr *)&sserver, (socklen_t *)&server_len);
        int server_respond = ntohs(*(short *)buffer);
        if (server_respond == OP_ERROR)
        {
            this->log(buffer + 4, 2);
            rc = 2;
            break;
        }
        else if (server_respond == OP_ACK)
        {
            int opAckRespond = ntohs(*(short *)(buffer + 2));
            if (opAckRespond == i)
            {
                i++;
                // Sending OP_DATA to the server
                this->sendData(sock, sserver, i);
            }
            else
            {
                this->log("Server error", 2);
                rc = 2;
                break;
            }
        }
        else if (server_respond == OP_DATA)
        {
            // Write to the file
            fprintf(this->fp, "%s", buffer + 4);

            // Logging
            string logLine = "Receiving DATA ..." + to_string(this->cntRespondBlock - 4) + "B";
            this->log(logLine, 0);

            // Sending OP_ACK to the server
            *(short *)buffer = htons(OP_ACK);
            sendto(sock, buffer, 4, 0, (struct sockaddr *)&sserver, sizeof sserver);
            this->nPackets++;

            memset(&buffer, 0, sizeof(buffer));
        }
        else
        {
            this->log("Internal error", 3);
        }
    } while (this->cntRespondBlock >= 512); // Accepts responses until the last package is less than 512 B

    this->log("Transfer completed without errors", 0);

    fclose(this->fp);

    return rc;
}

/**
 * @brief Get the Current Time object
 * 
 * @return string 
 */
string getCurrentTime()
{
    time_t currentTimeSystem = time(0);
    tm *currentTime = localtime(&currentTimeSystem);

    string output = to_string(currentTime->tm_year) + "-";
    output += to_string(currentTime->tm_mon) + "-";
    output += to_string(currentTime->tm_mday) + " ";
    output += to_string(currentTime->tm_hour) + ":";
    output += to_string(currentTime->tm_min) + ":";

    return output;
}

/**
 * @brief Printing out the log 
 * 
 * @param msg Message text
 * @param type Type of the message to print out
 * @return void* 
 */
void *TftpClient::log(string msg, int type)
{
    if (type >= 2)
    {
        cerr << "[" << getCurrentTime() << "]"
             << msg
             << endl;
    }
    else
    {
        cout << "[" << getCurrentTime() << "]"
             << msg
             << endl;
    }
}