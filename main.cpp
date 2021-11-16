/**
 *
 * @file main.c
 * @brief TFTP Client
 * @author Nikita Zhukov xzhuko01@vutbr.cz
 * @date 15.11.2021
 */
#include "tftp-client.hh"

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#define STDOUT 1

using namespace std;

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    int rc;
    TftpClient *tftpClient = new TftpClient();

    // Init tftp client based on passed arguments
    if ((rc = tftpClient->argparse(argc, argv)) == 1)
    {
        return rc;
    }

    // Run tfpt client
    tftpClient->run();
    return 0;
}
