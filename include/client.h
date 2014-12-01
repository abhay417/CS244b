
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <set>
#include <vector>
#include <string>

enum ClientError {
    /*
     * Set, Get or Remove failed because they key was not found.
     */
    KEY_NOT_FOUND,
    /*
     * Create operation failed because the key has no parent.
     */
    NO_PARENT,
    /*
     * The key has children so it cannot be deleted.
     */
    HAS_CHILDREN,
    /*
     * The key path violates our formatting rules
     */
    MALFORMED_KEY,
};

/*
 * For server and/or library errors that must delivered to the client
 */
class ClientException : public std::exception {
public:
    ClientException(enum ClientError err) { errcode = err; }
    virtual ~ClientException() { }
    ClientError code() const { return errcode; }
    const char *what() {
        switch (errcode) {
            case KEY_NOT_FOUND:
                return "KEY NOT FOUND";
            case NO_PARENT:
                return "NO PARENT";
            case HAS_CHILDREN:
                return "HAS CHILDREN";
            case MALFORMED_KEY:
                return "MALFORMED KEY";
        }
        return "UNKNOWN ERROR CODE";
    }
private:
    enum ClientError errcode;
};

/*
 * For all the remote calls in the Client class you should use the ClientError 
 * class to throw exceptions that the client's can understand.
 */
class Client {
public:
    Client();
    ~Client();
    /*
     * Connect to a server
     */
    void open(const std::string &host);
    /*
     * Disconnect from a server
     */
    void close();
    /*
     * Test if still connected to a server
     *
     * Return Value: true if connected
     */
    bool isConnected();
    /*
     * Sends heartbeat to the hash ring server
     *
     * Return Value: void
     */
    void heartbeat(const std::string& ip);
    /*
     * Gets the IP address of the Cache server
     *
     * Return Value: IP address of the cache server
     */
    std::string getCacheServer(const std::string& url);
    /*
     * Gets the IP address of the Cache server
     *
     * Return Value: IP address of the cache server
     */
    std::vector<uint8_t> getCacheContents(const std::string& cacheHost,
                                          const std::string& url);
    /*
     * Gets the cache content from the appropriate cache server
     *
     * Return Value: Response as a vector of uint8_t
     */
    std::vector<uint8_t> getCacheContents2(const std::string& url);
private:
    xdr::srpc_client<api_v1> *client;
    //xdr::srpc_client<cache_api_v1> *cacheClient;
};

#endif /* __CLIENT_H__ */

