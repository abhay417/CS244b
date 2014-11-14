// -*- C++ -*-
// Automatically generated from include/server.x.
// DO NOT EDIT or your changes may be overwritten

#ifndef __XDR_INCLUDE_SERVER_HH_INCLUDED__
#define __XDR_INCLUDE_SERVER_HH_INCLUDED__ 1

#include <xdrpp/types.h>

using longstring = xdr::xstring<>;
using cache_data = longstring;

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
    using res_type = cache_data;
    using res_wire_type = cache_data;
    
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

  template<typename T, typename...A> static bool
  call_dispatch(T &&t, std::uint32_t proc, A &&...a) {
    switch(proc) {
    case 1:
      t.template dispatch<getCacheContents_t>(std::forward<A>(a)...);
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
  };
};

#endif // !__XDR_INCLUDE_SERVER_HH_INCLUDED__
