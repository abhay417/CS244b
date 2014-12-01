
#include <stdbool.h>
#include <stdint.h>

#include <unistd.h>

#include <set>
#include <string>
#include <fstream>
#include <iostream>

#include <xdrpp/srpc.h>

#include <include/server.hh>
#include <include/client.h>

using namespace std;

Client client;

#define DEBUG_MAX_ARGS      5
#define DEBUG_MAX_LINE      256

void
Cmd_Help(int argc, const char *argv[])
{
    cout << "hb send Heartbeat" << endl;
    cout << "cs get cashe server ip" << endl;
    cout << "cc get cashe contents" << endl;
}

void
Cmd_Heartbeat(int argc, const char *argv[])
{
    if (argc != 2) {
        cout << "Usage: hb IP" << endl;
        return;
    }

    try {
        client.heartbeat(argv[1]);
    } catch (ClientException &e) {
        cout << e.what() << endl;
    }
}

void
Cmd_CacheServer(int argc, const char *argv[])
{
    if (argc != 2) {
        cout << "Usage: cs URL" << endl;
        return;
    }

    try {
        string serverIP = client.getCacheServer(argv[1]);
        cout << "Server IP: " << serverIP << endl;
    } catch (ClientException &e) {
        cout << e.what() << endl;
    }
}

void
Cmd_CacheContents(int argc, const char *argv[])
{
    if (argc != 3) {
        cout << "Usage: cc HOST URL" << endl;
        return;
    }

    try {
        vector<uint8_t> ret = client.getCacheContents(argv[1], argv[2]);
        //cout << "Cache return: " << ret << endl;
    } catch (ClientException &e) {
        cout << e.what() << endl;
    }
}

void
Cmd_CacheContents2(int argc, const char *argv[])
{
    if (argc != 2) {
        cout << "Usage: cc URL" << endl;
        return;
    }

    try {
        vector<uint8_t> ret = client.getCacheContents2(argv[1]);
        //cout << "Cache return: " << ret << endl;
    } catch (ClientException &e) {
        cout << e.what() << endl;
    }
}

void
DispatchCommand(char *buf)
{
    int argc;
    char *argv[DEBUG_MAX_ARGS];
    char *nextArg;

    // parse input
    argv[0] = buf;
    for (argc = 1; argc < DEBUG_MAX_ARGS; argc++) {
        nextArg = strchr(argv[argc - 1], ' ');
        if (nextArg == NULL)
        {
            break;
        }

        *nextArg = '\0';
        argv[argc] = nextArg + 1;
    }

    // execute command
    string cmd = argv[0];
    if (cmd == "hb") {
        Cmd_Heartbeat(argc, (const char **)argv);
    } else if (cmd == "cs") {
        Cmd_CacheServer(argc, (const char **)argv);
    } else if (cmd == "cc") {
        Cmd_CacheContents(argc, (const char **)argv);
    } else if (cmd == "cc2") {
        Cmd_CacheContents2(argc, (const char **)argv);
    } else if (cmd != "") {
        printf("Unknown command '%s'\n", argv[0]);
    }
}

void
Prompt()
{
    char buf[DEBUG_MAX_LINE];
    cout << "Client Shell" << endl;

    while (cin.good()) {
        cout << "> ";

        // read input
        cin.getline((char *)&buf, DEBUG_MAX_LINE);

        DispatchCommand(buf);
    }
}

void
RunScript(const char *file)
{
    char buf[DEBUG_MAX_LINE];
    fstream fin {file};

    while (!fin.eof()) {
        // read input
        fin.getline((char *)&buf, DEBUG_MAX_LINE);

        DispatchCommand(buf);
    }
}

int
main(int argc, const char *argv[])
{
    client = Client();

    if (argc != 2 && argc != 3) {
        cout << "Usage: shell HOST [SCRIPT]" << endl;
        return 1;
    }

    // Setup connection
    try {
        client.open(argv[1]);
    } catch (exception &e) {
        cout << "Connection failed!" << endl;
        cout << "Exception: " << e.what() << endl;
        return 1;
    }

    // Either execute script or prompt
    try {
        if (argc == 2) {
            Prompt();
        } else {
            RunScript(argv[2]);
        }
    } catch(exception &e) {
        cout << e.what() << endl;
        return 1;
    }

    return 0;
}

