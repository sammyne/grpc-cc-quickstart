# grpc-cc-quickstart

## Environment
- sammyne/grpc-cc:1.48.0-ubuntu20.04

## Quickstart

Source codes go as
- [server.cc](./src/server.cc)
- [client.cc](./src/client.cc)

Demo is run within the container backed by the image described in Environment section.

```bash
docker run -it --rm                   \
  -v $PWD:/hello-world                \
  sammyne/grpc-cc:1.48.0-ubuntu20.04  \
  bash
```

After getting into the container, build and start the server

```bash
bash serve.sh
```

Open another terminal, exec into the container, and run the client

```bash
bash check.sh
```

Finally, sample errors (reporting `PEER_DID_NOT_RETURN_A_CERTIFICATE`) go as 

```bash
I0823 11:39:50.068360577  142781 ev_epoll1_linux.cc:121]     grpc epoll fd: 3
D0823 11:39:50.068419568  142781 ev_posix.cc:141]            Using polling engine: epoll1
D0823 11:39:50.068463931  142781 lb_policy_registry.cc:48]   registering LB policy factory for "grpclb"
D0823 11:39:50.068479781  142781 lb_policy_registry.cc:48]   registering LB policy factory for "rls_experimental"
D0823 11:39:50.068490170  142781 lb_policy_registry.cc:48]   registering LB policy factory for "priority_experimental"
D0823 11:39:50.068500560  142781 lb_policy_registry.cc:48]   registering LB policy factory for "weighted_target_experimental"
D0823 11:39:50.068510238  142781 lb_policy_registry.cc:48]   registering LB policy factory for "pick_first"
D0823 11:39:50.068518833  142781 lb_policy_registry.cc:48]   registering LB policy factory for "round_robin"
D0823 11:39:50.068530365  142781 lb_policy_registry.cc:48]   registering LB policy factory for "ring_hash_experimental"
D0823 11:39:50.068541215  142781 dns_resolver_ares.cc:545]   Using ares dns resolver
D0823 11:39:50.068624932  142781 certificate_provider_registry.cc:39] registering certificate provider factory for "file_watcher"
D0823 11:39:50.068648396  142781 lb_policy_registry.cc:48]   registering LB policy factory for "cds_experimental"
D0823 11:39:50.068694532  142781 lb_policy_registry.cc:48]   registering LB policy factory for "xds_cluster_impl_experimental"
D0823 11:39:50.068703519  142781 lb_policy_registry.cc:48]   registering LB policy factory for "xds_cluster_resolver_experimental"
D0823 11:39:50.068713007  142781 lb_policy_registry.cc:48]   registering LB policy factory for "xds_cluster_manager_experimental"
I0823 11:39:50.068798096  142781 server_builder.cc:348]      Synchronous server. Num CQs: 1, Min pollers: 1, Max Pollers: 2, CQ timeout (msec): 10000
I0823 11:39:50.069476626  142781 ssl_transport_security.cc:283] Could not get common name of subject from certificate.
I0823 11:39:50.069780285  142781 socket_utils_common_posix.cc:429] Disabling AF_INET6 sockets because ::1 is not available.
I0823 11:39:50.069806123  142781 socket_utils_common_posix.cc:353] TCP_USER_TIMEOUT is available. TCP_USER_TIMEOUT will be used thereafter
I0823 11:39:50.070068915  142781 chttp2_server.cc:985]       WARNING: UNKNOWN:Only 1 addresses added out of total 2 resolved {created_time:"2022-08-23T11:39:50.069954611+00:00", file_line:983, file:"/github.com/grpc/grpc/src/core/ext/transport/chttp2/server/chttp2_server.cc", children:[UNKNOWN:Address family not supported by protocol {file:"/github.com/grpc/grpc/src/core/lib/iomgr/socket_utils_common_posix.cc", file_line:446, created_time:"2022-08-23T11:39:50.06990652+00:00", errno:97, os_error:"Address family not supported by protocol", syscall:"socket", target_address:"[::1]:50051"}]}
Server listening on localhost:50051


E0823 11:39:51.621816329  142786 ssl_transport_security.cc:1495] Handshake failed with fatal error SSL_ERROR_SSL: error:100000f7:SSL routines:OPENSSL_internal:WRONG_VERSION_NUMBER.
D0823 11:39:51.621928779  142786 security_handshaker.cc:206] Security handshake failed: UNKNOWN:Tls handshake failed {file:"/github.com/grpc/grpc/src/core/lib/security/transport/security_handshaker.cc", file_line:408, created_time:"2022-08-23T11:39:51.621864578+00:00", tsi_error:"TSI_PROTOCOL_FAILURE", tsi_code:10}
D0823 11:39:51.622025741  142786 chttp2_server.cc:451]       Handshaking failed: UNKNOWN:Tls handshake failed {file:"/github.com/grpc/grpc/src/core/lib/security/transport/security_handshaker.cc", file_line:408, created_time:"2022-08-23T11:39:51.621864578+00:00", tsi_error:"TSI_PROTOCOL_FAILURE", tsi_code:10}


E0823 11:39:58.040644176  142786 ssl_transport_security.cc:1910] No match found for server name: 0.0.0.0.
E0823 11:39:58.041471564  142786 ssl_transport_security.cc:1495] Handshake failed with fatal error SSL_ERROR_SSL: error:100000c0:SSL routines:OPENSSL_internal:PEER_DID_NOT_RETURN_A_CERTIFICATE.
D0823 11:39:58.041564839  142786 security_handshaker.cc:206] Security handshake failed: UNKNOWN:Tls handshake failed {tsi_code:10, tsi_error:"TSI_PROTOCOL_FAILURE", created_time:"2022-08-23T11:39:58.041503254+00:00", file_line:408, file:"/github.com/grpc/grpc/src/core/lib/security/transport/security_handshaker.cc"}
D0823 11:39:58.041651260  142786 chttp2_server.cc:451]       Handshaking failed: UNKNOWN:Tls handshake failed {tsi_code:10, tsi_error:"TSI_PROTOCOL_FAILURE", created_time:"2022-08-23T11:39:58.041503254+00:00", file_line:408, file:"/github.com/grpc/grpc/src/core/lib/security/transport/security_handshaker.cc"}
```

## Reference
- https://github.com/grpc/grpc/issues/12146#issuecomment-321627641
- [gRPC SSL加密传输数据实例（C++版）](https://blog.csdn.net/chenwr2018/article/details/105708168)
