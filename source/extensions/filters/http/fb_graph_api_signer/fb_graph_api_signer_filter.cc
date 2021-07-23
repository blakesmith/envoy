#include "source/extensions/filters/http/fb_graph_api_signer/fb_graph_api_signer_filter.h"

#include "source/common/http/utility.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

const std::string ACCESS_TOKEN_QUERY_PARAM = "access_token";

Http::FilterHeadersStatus Filter::decodeHeaders(__attribute__((unused)) Http::RequestHeaderMap& headers, __attribute__((unused)) bool end_stream) {
    ENVOY_LOG(debug, "fb graph api signer, decode headers");

    if (headers.Path() == nullptr) {
        ENVOY_LOG(debug, "no HTTP URL path found. Skip signing.");
        return Http::FilterHeadersStatus::Continue;
    }

    const auto& params = Http::Utility::parseAndDecodeQueryString(headers.getPathValue());
    const auto& access_token_it = params.find(ACCESS_TOKEN_QUERY_PARAM);

    if (access_token_it == params.end()) {
        ENVOY_LOG(debug, "no access token found. skip signing");
    } else {
        auto access_token = access_token_it->second;
        ENVOY_LOG(debug, "Found access token: {}", access_token);
    }

    return Http::FilterHeadersStatus::Continue;
}

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy


