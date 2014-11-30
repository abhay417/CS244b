typedef string longstring<>;
typedef opaque bytestream<>;

struct heartbeat {
	longstring nodeIP;
  //int capacity;
	int timeStamp;
};

struct newCacheServerInfo {
  int64_t fromLow;
  int64_t fromHigh;
  int64_t toLow;
  int64_t toHigh;
  longstring newNodeIP;
};

struct cacheTransfer {
  int64_t lowDigest;
  int64_t highDigest;
  bytestream cacheData;
  longstring sourceNodeIP;
};

struct cacheRequest {
  longstring host;
  bool isHeadRequest;
  longstring requestUrl;
  longstring request;
};

program server_api {
  version api_v1 {
    void sendHeartbeat(heartbeat) = 1;
    longstring getCacheServer(longstring) = 2; 
  } = 1;
} = 0x40048086;

program cache_server_api {
  version cache_api_v1 {
    bytestream getCacheContents(longstring) = 1;
    bytestream getCacheContents2(cacheRequest) = 2;
    void newCacheserverAdded(newCacheServerInfo) = 3;
    void sendCachedData(cacheTransfer) = 4;
  } = 1;
} = 0x40048082;

