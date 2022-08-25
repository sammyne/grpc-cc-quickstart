# grpc-cc-quickstart

## Environment
- sammyne/grpc-cc:1.48.0-ubuntu20.04
- libssl-dev 1.1.1f

## Quickstart

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

Finally, sample errors go as 

```bash
E0823 07:10:48.114715075  763508 grpc_tls_certificate_distributor.cc:184] assertion failed: root_cert_name.has_value() || identity_cert_name.has_value()
serve.sh: line 15: 763508 Aborted                 (core dumped) ./build/server $pki_dir/key.pem $pki_dir/cert.pem
```

## Reference
- https://github.com/grpc/grpc/issues/12146#issuecomment-321627641
- [gRPC SSL加密传输数据实例（C++版）](https://blog.csdn.net/chenwr2018/article/details/105708168)
- [gRPC SSL/TLS双向认证--SSL/TLS 工作原理](https://blog.csdn.net/qq_35487883/article/details/120864586)
