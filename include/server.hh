// -*- C++ -*-
// Automatically generated from include/server.x.
// DO NOT EDIT or your changes may be overwritten

#ifndef __XDR_INCLUDE_SERVER_HH_INCLUDED__
#define __XDR_INCLUDE_SERVER_HH_INCLUDED__ 1

#include <xdrpp/types.h>

using longstring = xdr::xstring<>;
using bytestream = xdr::opaque_vec<>;

struct heartbeat {
  longstring nodeIP{};
  std::int32_t timeStamp{};
};
namespace xdr {
template<> struct xdr_traits<::heartbeat>
  : xdr_struct_base<field_ptr<::heartbeat,
                              decltype(::heartbeat::nodeIP),
                              &::heartbeat::nodeIP>,
                    field_ptr<::heartbeat,
                              decltype(::heartbeat::timeStamp),
                              &::heartbeat::timeStamp>> {
  template<typename Archive> static void
  save(Archive &ar, const ::heartbeat &obj) {
    archive(ar, obj.nodeIP, "nodeIP");
    archive(ar, obj.timeStamp, "timeStamp");
  }
  template<typename Archive> static void
  load(Archive &ar, ::heartbeat &obj) {
    archive(ar, obj.nodeIP, "nodeIP");
    archive(ar, obj.timeStamp, "timeStamp");
  }
};
}

struct newCacheServerInfo {
  int64_t fromLow{};
  int64_t fromHigh{};
  int64_t toLow{};
  int64_t toHigh{};
  longstring newNodeIP{};
};
namespace xdr {
template<> struct xdr_traits<::newCacheServerInfo>
  : xdr_struct_base<field_ptr<::newCacheServerInfo,
                              decltype(::newCacheServerInfo::fromLow),
                              &::newCacheServerInfo::fromLow>,
                    field_ptr<::newCacheServerInfo,
                              decltype(::newCacheServerInfo::fromHigh),
                              &::newCacheServerInfo::fromHigh>,
                    field_ptr<::newCacheServerInfo,
                              decltype(::newCacheServerInfo::toLow),
                              &::newCacheServerInfo::toLow>,
                    field_ptr<::newCacheServerInfo,
                              decltype(::newCacheServerInfo::toHigh),
                              &::newCacheServerInfo::toHigh>,
                    field_ptr<::newCacheServerInfo,
                              decltype(::newCacheServerInfo::newNodeIP),
                              &::newCacheServerInfo::newNodeIP>> {
  template<typename Archive> static void
  save(Archive &ar, const ::newCacheServerInfo &obj) {
    archive(ar, obj.fromLow, "fromLow");
    archive(ar, obj.fromHigh, "fromHigh");
    archive(ar, obj.toLow, "toLow");
    archive(ar, obj.toHigh, "toHigh");
    archive(ar, obj.newNodeIP, "newNodeIP");
  }
  template<typename Archive> static void
  load(Archive &ar, ::newCacheServerInfo &obj) {
    archive(ar, obj.fromLow, "fromLow");
    archive(ar, obj.fromHigh, "fromHigh");
    archive(ar, obj.toLow, "toLow");
    archive(ar, obj.toHigh, "toHigh");
    archive(ar, obj.newNodeIP, "newNodeIP");
  }
};
}

struct cacheTransfer {
  int64_t lowDigest{};
  int64_t highDigest{};
  bytestream cacheData{};
  longstring sourceNodeIP{};
};
namespace xdr {
template<> struct xdr_traits<::cacheTransfer>
  : xdr_struct_base<field_ptr<::cacheTransfer,
                              decltype(::cacheTransfer::lowDigest),
                              &::cacheTransfer::lowDigest>,
                    field_ptr<::cacheTransfer,
                              decltype(::cacheTransfer::highDigest),
                              &::cacheTransfer::highDigest>,
                    field_ptr<::cacheTransfer,
                              decltype(::cacheTransfer::cacheData),
                              &::cacheTransfer::cacheData>,
                    field_ptr<::cacheTransfer,
                              decltype(::cacheTransfer::sourceNodeIP),
                              &::cacheTransfer::sourceNodeIP>> {
  template<typename Archive> static void
  save(Archive &ar, const ::cacheTransfer &obj) {
    archive(ar, obj.lowDigest, "lowDigest");
    archive(ar, obj.highDigest, "highDigest");
    archive(ar, obj.cacheData, "cacheData");
    archive(ar, obj.sourceNodeIP, "sourceNodeIP");
  }
  template<typename Archive> static void
  load(Archive &ar, ::cacheTransfer &obj) {
    archive(ar, obj.lowDigest, "lowDigest");
    archive(ar, obj.highDigest, "highDigest");
    archive(ar, obj.cacheData, "cacheData");
    archive(ar, obj.sourceNodeIP, "sourceNodeIP");
  }
};
}

struct api_v1 {
  static constexpr std::uint32_t program = 1074036870;
  static constexpr const char *program_name = "server_api";
  static constexpr std::uint32_t version = 1;
  static constexpr const char *version_name = "api_v1";

  struct sendHeartbeat_t {
    using interface_type = api_v1;
    static constexpr std::uint32_t proc = 1;
    static constexpr const char *proc_name = "sendHeartbeat";
    using arg_type = heartbeat;
    using arg_wire_type = heartbeat;
    using res_type = void;
    using res_wire_type = xdr::xdr_void;
    
    template<typename C, typename...A> static auto
    dispatch(C &&c, A &&...a) ->
    decltype(c.sendHeartbeat(std::forward<A>(a)...)) {
      return c.sendHeartbeat(std::forward<A>(a)...);
    }
    
    template<typename C, typename DropIfVoid, typename...A> static auto
    dispatch_dropvoid(C &&c, DropIfVoid &&d, A &&...a) ->
    decltype(c.sendHeartbeat(std::forward<DropIfVoid>(d), std::forward<A>(a)...)) {
      return c.sendHeartbeat(std::forward<DropIfVoid>(d), std::forward<A>(a)...);
    }
  };

  struct getCacheServer_t {
    using interface_type = api_v1;
    static constexpr std::uint32_t proc = 2;
    static constexpr const char *proc_name = "getCacheServer";
    using arg_type = longstring;
    using arg_wire_type = longstring;
    using res_type = longstring;
    using res_wire_type = longstring;
    
    template<typename C, typename...A> static auto
    dispatch(C &&c, A &&...a) ->
    decltype(c.getCacheServer(std::forward<A>(a)...)) {
      return c.getCacheServer(std::forward<A>(a)...);
    }
    
    template<typename C, typename DropIfVoid, typename...A> static auto
    dispatch_dropvoid(C &&c, DropIfVoid &&d, A &&...a) ->
    decltype(c.getCacheServer(std::forward<DropIfVoid>(d), std::forward<A>(a)...)) {
      return c.getCacheServer(std::forward<DropIfVoid>(d), std::forward<A>(a)...);
    }
  };

  template<typename T, typename...A> static bool
  call_dispatch(T &&t, std::uint32_t proc, A &&...a) {
    switch(proc) {
    case 1:
      t.template dispatch<sendHeartbeat_t>(std::forward<A>(a)...);
      return true;
    case 2:
      t.template dispatch<getCacheServer_t>(std::forward<A>(a)...);
      return true;
    }
    return false;
  }

  template<typename _XDRBASE> struct client : _XDRBASE {
    using _XDRBASE::_XDRBASE;

    template<typename..._XDRARGS> auto
    sendHeartbeat(_XDRARGS &&..._xdr_args) ->
    decltype(this->_XDRBASE::template invoke<sendHeartbeat_t>(_xdr_args...)) {
      return this->_XDRBASE::template invoke<sendHeartbeat_t>(_xdr_args...);
    }

    template<typename..._XDRARGS> auto
    getCacheServer(_XDRARGS &&..._xdr_args) ->
    decltype(this->_XDRBASE::template invoke<getCacheServer_t>(_xdr_args...)) {
      return this->_XDRBASE::template invoke<getCacheServer_t>(_xdr_args...);
    }
  };
};

struct cache_api_v1 {
  static constexpr std::uint32_t program = 2147778694;
  static constexpr const char *program_name = "cache_server_api";
  static constexpr std::uint32_t version = 1;
  static constexpr const char *version_name = "cache_api_v1";

  struct getCacheContents_t {
    using interface_type = cache_api_v1;
    static constexpr std::uint32_t proc = 1;
    static constexpr const char *proc_name = "getCacheContents";
    using arg_type = longstring;
    using arg_wire_type = longstring;
    using res_type = bytestream;
    using res_wire_type = bytestream;
    
    template<typename C, typename...A> static auto
    dispatch(C &&c, A &&...a) ->
    decltype(c.getCacheContents(std::forward<A>(a)...)) {
      return c.getCacheContents(std::forward<A>(a)...);
    }
    
    template<typename C, typename DropIfVoid, typename...A> static auto
    dispatch_dropvoid(C &&c, DropIfVoid &&d, A &&...a) ->
    decltype(c.getCacheContents(std::forward<DropIfVoid>(d), std::forward<A>(a)...)) {
      return c.getCacheContents(std::forward<DropIfVoid>(d), std::forward<A>(a)...);
    }
  };

  struct newCacheserverAdded_t {
    using interface_type = cache_api_v1;
    static constexpr std::uint32_t proc = 2;
    static constexpr const char *proc_name = "newCacheserverAdded";
    using arg_type = newCacheServerInfo;
    using arg_wire_type = newCacheServerInfo;
    using res_type = void;
    using res_wire_type = xdr::xdr_void;
    
    template<typename C, typename...A> static auto
    dispatch(C &&c, A &&...a) ->
    decltype(c.newCacheserverAdded(std::forward<A>(a)...)) {
      return c.newCacheserverAdded(std::forward<A>(a)...);
    }
    
    template<typename C, typename DropIfVoid, typename...A> static auto
    dispatch_dropvoid(C &&c, DropIfVoid &&d, A &&...a) ->
    decltype(c.newCacheserverAdded(std::forward<DropIfVoid>(d), std::forward<A>(a)...)) {
      return c.newCacheserverAdded(std::forward<DropIfVoid>(d), std::forward<A>(a)...);
    }
  };

  struct sendCachedData_t {
    using interface_type = cache_api_v1;
    static constexpr std::uint32_t proc = 3;
    static constexpr const char *proc_name = "sendCachedData";
    using arg_type = cacheTransfer;
    using arg_wire_type = cacheTransfer;
    using res_type = void;
    using res_wire_type = xdr::xdr_void;
    
    template<typename C, typename...A> static auto
    dispatch(C &&c, A &&...a) ->
    decltype(c.sendCachedData(std::forward<A>(a)...)) {
      return c.sendCachedData(std::forward<A>(a)...);
    }
    
    template<typename C, typename DropIfVoid, typename...A> static auto
    dispatch_dropvoid(C &&c, DropIfVoid &&d, A &&...a) ->
    decltype(c.sendCachedData(std::forward<DropIfVoid>(d), std::forward<A>(a)...)) {
      return c.sendCachedData(std::forward<DropIfVoid>(d), std::forward<A>(a)...);
    }
  };

  template<typename T, typename...A> static bool
  call_dispatch(T &&t, std::uint32_t proc, A &&...a) {
    switch(proc) {
    case 1:
      t.template dispatch<getCacheContents_t>(std::forward<A>(a)...);
      return true;
    case 2:
      t.template dispatch<newCacheserverAdded_t>(std::forward<A>(a)...);
      return true;
    case 3:
      t.template dispatch<sendCachedData_t>(std::forward<A>(a)...);
      return true;
    }
    return false;
  }

  template<typename _XDRBASE> struct client : _XDRBASE {
    using _XDRBASE::_XDRBASE;

    template<typename..._XDRARGS> auto
    getCacheContents(_XDRARGS &&..._xdr_args) ->
    decltype(this->_XDRBASE::template invoke<getCacheContents_t>(_xdr_args...)) {
      return this->_XDRBASE::template invoke<getCacheContents_t>(_xdr_args...);
    }

    template<typename..._XDRARGS> auto
    newCacheserverAdded(_XDRARGS &&..._xdr_args) ->
    decltype(this->_XDRBASE::template invoke<newCacheserverAdded_t>(_xdr_args...)) {
      return this->_XDRBASE::template invoke<newCacheserverAdded_t>(_xdr_args...);
    }

    template<typename..._XDRARGS> auto
    sendCachedData(_XDRARGS &&..._xdr_args) ->
    decltype(this->_XDRBASE::template invoke<sendCachedData_t>(_xdr_args...)) {
      return this->_XDRBASE::template invoke<sendCachedData_t>(_xdr_args...);
    }
  };
};

#endif // !__XDR_INCLUDE_SERVER_HH_INCLUDED__
