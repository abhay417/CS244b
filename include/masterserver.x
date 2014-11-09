
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

program masterserver_api {
  version api_v1 {
	Result remove(longstring) = 1;
	Result put(kvpair) = 2;	
	Result get(longstring) = 3;
	Result list(longstring) = 4;
  } = 1;
} = 0x40048086;

