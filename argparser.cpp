/**
 *
 * @file argparser.cpp
 * @brief TFTP Client
 * @author Nikita Zhukov xzhuko01@vutbr.cz
 * @date 15.11.2021
 */
#include <iostream>
#include <getopt.h>
#include <string.h>

#include "tftp-client.hh"

using namespace std;

typedef struct flags
{
    int readFlag;
    int writeFlag;
    int dFlag;
} Flags;

/**
 * @brief Parses CLI arguments
 * 
 * @param argc 
 * @param argv 
 * @return int return value
 */
int TftpClient::argparse(int argc, char **argv)
{
    int returnValue = 0;
    int opt, tmp;
    Flags flags = {
        .readFlag = 0,
        .writeFlag = 0,
        .dFlag = 0};
    static const char *sOptions = "RWd:t:s:mc:a:";

    while ((opt = getopt(argc, argv, sOptions)) != -1)
    {
        switch (opt)
        {
        case 'R':
            this->opOperation = OP_RRQ;
            flags.readFlag = 1;
            break;
        case 'W':
            this->opOperation = OP_WRQ;
            flags.writeFlag = 1;
            break;
        case 'd':
            flags.dFlag = 1;
            this->file = optarg;
            break;
        case 't':
            this->timeout = atoi(optarg);
            break;
        case 's':
            if (atoi(optarg) > 512)
                this->maxSizeOfBlock = atoi(optarg);
            break;
        case 'm':
            this->isMulticast = atoi(optarg);
            break;
        case 'c':
            if (strcmp(optarg, "netascii") == 0 || strcmp(optarg, "octet") == 0)
            {
                this->mode = optarg;
            }
            break;
        case 'a':
            break;
        default:
            break;
        }
    }

    if ((flags.readFlag && flags.writeFlag) || (!flags.readFlag && !flags.writeFlag))
    {
        cerr << "[ERROR] It shoud be read or write." << endl;
        returnValue = 1;
    }

    if (!flags.dFlag)
    {
        cerr << "[ERROR] File parameter is missing." << endl;
        returnValue = 1;
    }

    // cout << this->mode << endl;
    return returnValue;
}