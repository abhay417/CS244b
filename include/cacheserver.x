
typedef string longstring<>;

struct kvpair {
	string key<512>;
	string val<>;
};

struct Result {
	int code;
	bool success;
	string val<>;
};

program cacheserver_api {
  version api_v1 {
	Result create(kvpair) = 1;
	Result remove(longstring) = 2;
	Result set(kvpair) = 3;	
	Result list(longstring) = 4;
	Result get(longstring) = 5;
  } = 1;
} = 0x40048086;

