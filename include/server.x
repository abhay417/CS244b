typedef string longstring<>;
typedef opaque bytestream<>;

struct heartbeat {
	longstring nodeIP;
	int timeStamp;
};

program server_api {
  version api_v1 {
    void sendHeartbeat(heartbeat) = 1;
    longstring getCacheServer(longstring) = 2; 
  } = 1;
} = 0x40048086;

program cache_server_api {
  version cache_api_v1 {
    longstring getCacheContents(longstring) = 1;
  } = 1;
} = 0x80048086;

