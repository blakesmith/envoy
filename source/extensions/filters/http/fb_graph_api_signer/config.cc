#include "source/extensions/filters/http/fb_graph_api_signer/config.h"
#include "source/extensions/filters/http/fb_graph_api_signer/fb_graph_api_signer_filter.h"

#include "envoy/extensions/filters/http/fb_graph_api_signer/v3/fb_graph_api_signer.pb.h"
#include "envoy/extensions/filters/http/fb_graph_api_signer/v3/fb_graph_api_signer.pb.validate.h"
#include "envoy/registry/registry.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

Http::FilterFactoryCb FbGraphApiSignerFilterFactory::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::fb_graph_api_signer::v3::FbGraphApiSigner& config,
    const std::string& stats_prefix, Server::Configuration::FactoryContext& context) {

    auto filter_config = std::make_shared<FilterConfig>(config.app_secret(), stats_prefix, context.scope());

    return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
        auto filter = std::make_shared<Filter>(filter_config);
        callbacks.addStreamDecoderFilter(filter);
    };

}

REGISTER_FACTORY(FbGraphApiSignerFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
