# Facebook Graph API Signer

This Envoy HTTP filter can be used to generate `appsecret_proof`
signed values
for
[secure graph API requests](https://developers.facebook.com/docs/graph-api/securing-requests/). The
Graph API will validate the `appsecret_proof`, even if your Facebook
application does not require the appsecret proof in your application's
API settings.

The filter will look for access tokens in the following places, and
use the access token for signing:

1. A request `access_token` query parameter.
2. The `Authorization` header, in the form: `Authorization: Bearer <ACCESS_TOKEN>`.

The algorithm is basically:

`appsecret_proof = hex(hmacSha256(message=$access_token_value, key=$app_secret));`

appsecret_proof will be appended to the request query parameters.

NOTE: If the incoming request already contains an `appsecret_proof`
query parameter, the filter will pass the request through without signing.

# Usage

Setup a separate envoy listener for the graph API, so you can
configure the filter. You must specify the `app_secret` configuration
value, to give envoy the ability to sign requests.

```yaml
static_resources:
  - name: facebook_listener
    address:
      socket_address: { address: 127.0.0.1, port_value: 10001 }
    filter_chains:
    - filters:
      - name: envoy.filters.network.http_connection_manager
        typed_config:
          "@type": type.googleapis.com/envoy.extensions.filters.network.http_connection_manager.v3.HttpConnectionManager
          stat_prefix: ingress_http
          codec_type: AUTO
          access_log:
            name: envoy.access_loggers.stdout
            typed_config:
              "@type": type.googleapis.com/envoy.extensions.access_loggers.stream.v3.StdoutAccessLog
          route_config:
            name: local_route
            virtual_hosts:
            - name: fb_graph
              domains: ["*"]
              routes:
              - match: { prefix: "/" }
                route:
                  cluster: fb_graph
                  auto_host_rewrite: true # This is critical, so FB doesn't redirect you
          http_filters:
          - name: envoy.filters.http.fb_graph_api_signer
            typed_config:
              "@type": type.googleapis.com/envoy.extensions.filters.http.fb_graph_api_signer.v3.FbGraphApiSigner
              app_secret: YOUR_APP_SECRET
          - name: envoy.filters.http.router
```

For reference, you can setup your FB graph cluster like so:

```yaml
  clusters:
  - name: fb_graph
    connect_timeout: 0.25s
    type: LOGICAL_DNS
    dns_lookup_family: V4_ONLY
    lb_policy: ROUND_ROBIN
    load_assignment:
      cluster_name: fb_graph
      endpoints:
      - lb_endpoints:
        - endpoint:
            address:
              socket_address:
                address: graph.facebook.com
                port_value: 443
    transport_socket:
      name: envoy.transport_sockets.tls
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.transport_sockets.tls.v3.UpstreamTlsContext
        sni: graph.facebook.com
```
