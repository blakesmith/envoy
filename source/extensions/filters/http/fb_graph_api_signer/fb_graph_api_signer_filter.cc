#include "source/extensions/filters/http/fb_graph_api_signer/fb_graph_api_signer_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

Http::FilterHeadersStatus Filter::decodeHeaders(__attribute__((unused)) Http::RequestHeaderMap& headers, __attribute__((unused)) bool end_stream) {
    ENVOY_LOG(debug, "fb graph api signer, decode headers");
    return Http::FilterHeadersStatus::Continue;
}

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy


