load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_extension",
    "envoy_cc_library",
    "envoy_extension_package",
)

licenses(["notice"])  # Apache 2

envoy_extension_package()

envoy_cc_library(
    name = "fb_graph_api_signer_lib",
    srcs = ["fb_graph_api_signer_filter.cc"],
    hdrs = ["fb_graph_api_signer_filter.h"],
    deps = [
        "//envoy/http:filter_interface",
        "//source/extensions/filters/http/common:pass_through_filter_lib",
        "//source/common/common:hex_lib",
        "//source/common/crypto:utility_lib",
        "//source/common/http:header_utility_lib",
        "@envoy_api//envoy/extensions/filters/http/fb_graph_api_signer/v3:pkg_cc_proto",
    ],
)

envoy_cc_extension(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":fb_graph_api_signer_lib",
        "//envoy/registry",
        "//source/extensions/filters/http/common:factory_base_lib",
        "@envoy_api//envoy/extensions/filters/http/fb_graph_api_signer/v3:pkg_cc_proto",
    ],
)
