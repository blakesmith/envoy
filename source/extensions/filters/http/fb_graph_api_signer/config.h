#pragma once

#include "envoy/extensions/filters/http/fb_graph_api_signer/v3/fb_graph_api_signer.pb.h"
#include "envoy/extensions/filters/http/fb_graph_api_signer/v3/fb_graph_api_signer.pb.validate.h"

#include "source/extensions/filters/http/common/factory_base.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

class FbGraphApiSignerFilterFactory : public Common::FactoryBase<
    envoy::extensions::filters::http::fb_graph_api_signer::v3::FbGraphApiSigner> {

public:
    FbGraphApiSignerFilterFactory() : FactoryBase("envoy.filters.http.fb_graph_api_signer") {}

    Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
        const envoy::extensions::filters::http::fb_graph_api_signer::v3::FbGraphApiSigner& proto_config,
        const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
