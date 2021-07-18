#include "source/extensions/filters/http/fb_graph_api_signer/config.h"

#include "envoy/extensions/filters/http/fb_graph_api_signer/v3/fb_graph_api_signer.pb.h"
#include "envoy/extensions/filters/http/fb_graph_api_signer/v3/fb_graph_api_signer.pb.validate.h"
#include "envoy/registry/registry.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

Http::FilterFactoryCb FbGraphApiSignerFilterFactory::createFilterFactoryFromProtoTyped(
    __attribute__((unused)) const envoy::extensions::filters::http::fb_graph_api_signer::v3::FbGraphApiSigner& config,
    __attribute__((unused)) const std::string& stats_prefix, __attribute__((unused)) Server::Configuration::FactoryContext& context) {

    return nullptr;
}

REGISTER_FACTORY(FbGraphApiSignerFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
