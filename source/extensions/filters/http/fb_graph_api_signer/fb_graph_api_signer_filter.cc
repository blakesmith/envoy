#include "source/extensions/filters/http/fb_graph_api_signer/fb_graph_api_signer_filter.h"

#include "source/common/common/hex.h"
#include "source/common/crypto/utility.h"
#include "source/common/http/utility.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

const std::string ACCESS_TOKEN_QUERY_PARAM = "access_token";
const std::string APPSECRET_PROOF_QUERY_PARAM = "appsecret_proof";

Filter::Filter(const std::shared_ptr<std::string>& app_secret) : app_secret_(app_secret) { }

Http::FilterHeadersStatus Filter::decodeHeaders(Http::RequestHeaderMap& headers, __attribute__((unused)) bool end_stream) {
    ENVOY_LOG(debug, "fb graph api signer, decode headers");

    if (headers.Path() == nullptr) {
        ENVOY_LOG(debug, "no HTTP URL path found. Skip signing.");
        return Http::FilterHeadersStatus::Continue;
    }

    auto params = Http::Utility::parseAndDecodeQueryString(headers.getPathValue());
    const auto& access_token_it = params.find(ACCESS_TOKEN_QUERY_PARAM);

    if (access_token_it == params.end()) {
        ENVOY_LOG(debug, "no access token found. skip signing");
    } else {
        auto access_token = access_token_it->second;
        ENVOY_LOG(debug, "Found access token: {}", access_token);

        auto& hashing_util = Envoy::Common::Crypto::UtilitySingleton::get();
        const std::vector<uint8_t> signing_key(app_secret_->begin(), app_secret_->end());
        const std::string appsecret_proof = Hex::encode(hashing_util.getSha256Hmac(signing_key, access_token));
        ENVOY_LOG(debug, "Computed appsecret_proof: {}", appsecret_proof);

        params[APPSECRET_PROOF_QUERY_PARAM] = appsecret_proof;
        auto newPath = Http::Utility::queryParamsToString(params);
        headers.setPath(newPath);
    }

    return Http::FilterHeadersStatus::Continue;
}

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy


