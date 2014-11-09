
#ifndef __SERVERDB_H__
#define __SERVERDB_H__

#include <set>

class ServerDB {
public:
    /*
     * Constructor for ServerDB
     *
     * Creates an instance backed by the given file name
     */
    ServerDB();
    /*
     * Destructor for ServerDB
     */
    ~ServerDB();
    /*
     * Checks if a key exists.
     *
     * Return Value: true if the key exists, false otherwise
     */
    bool find(const std::string &key);
    /*
     * Get a value
     *
     * Return Value: value of the key or throws an exception
     */
    std::string get(const std::string &key);
    /*
     * List all sub-keys
     *
     * Return Value: A set of all sub-keys, returns empty set if it does not 
     * exist or has no children.
     * leaving this for debug purposes
     */
    std::set<std::string> list(const std::string &path);
private:
    /*
     * Set or Update a key-value pair
     *
     * Return Value: true if the key already exists, false otherwise
     */
    bool put(const std::string &path, const std::string &val);

};

#endif /* __SERVERDB_H__ */

