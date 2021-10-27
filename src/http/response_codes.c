#include "response_codes.h"

#include "../utils/Errors.h"

#include <stdlib.h>

String *response_code_as_string(HttpResponseCode response_code) {
    switch (response_code) {
    case HttpContinue:
        return string_from_cstr("Continue");
    case HttpSwitchingProtocols:
        return string_from_cstr("SwitchingProtocols");
    case HttpProcessing:
        return string_from_cstr("Processing");
    case HttpEarlyHints:
        return string_from_cstr("EarlyHints");

    case HttpOk:
        return string_from_cstr("Ok");
    case HttpCreated:
        return string_from_cstr("Created");
    case HttpAccepted:
        return string_from_cstr("Accepted");
    case HttpNonAuthorativeInformation:
        return string_from_cstr("NonAuthorativeInformation");
    case HttpNoContent:
        return string_from_cstr("NoContent");
    case HttpResetContent:
        return string_from_cstr("ResetContent");
    case HttpPartialContent:
        return string_from_cstr("PartialContent");
    case HttpMultiStatus:
        return string_from_cstr("MultiStatus");
    case HttpAlreadyReported:
        return string_from_cstr("AlreadyReported");
    case HttpIMUsed:
        return string_from_cstr("IMUsed");

    case HttpMultipleChoice:
        return string_from_cstr("MultipleChoice");
    case HttpMovedPermanently:
        return string_from_cstr("MovedPermanently");
    case HttpFound:
        return string_from_cstr("Found");
    case HttpSeeOther:
        return string_from_cstr("SeeOther");
    case HttpNotModified:
        return string_from_cstr("NotModified");
    case HttpTemporaryRedirect:
        return string_from_cstr("TemporaryRedirect");
    case HttpPermanentRedirect:
        return string_from_cstr("PermanentRedirect");

    case HttpBadRequest:
        return string_from_cstr("BadRequest");
    case HttpUnathorized:
        return string_from_cstr("Unathorized");
    case HttpPaymentRequired:
        return string_from_cstr("PaymentRequired");
    case HttpForbidden:
        return string_from_cstr("Forbidden");
    case HttpNotFound:
        return string_from_cstr("NotFound");
    case HttpMethodNotAllowed:
        return string_from_cstr("MethodNotAllowed");
    case HttpNotAcceptable:
        return string_from_cstr("NotAcceptable");
    case HttpProxyAuthenticationRequired:
        return string_from_cstr("ProxyAuthenticationRequired");
    case HttpRequestTimeout:
        return string_from_cstr("RequestTimeout");
    case HttpConflict:
        return string_from_cstr("Conflict");
    case HttpGone:
        return string_from_cstr("Gone");
    case HttpLengthRequired:
        return string_from_cstr("LengthRequired");
    case HttpPreconditionFailed:
        return string_from_cstr("PreconditionFailed");
    case HttpPayloadTooLarge:
        return string_from_cstr("PayloadTooLarge");
    case HttpURITooLong:
        return string_from_cstr("URITooLong");
    case HttpUnsupportedMediatype:
        return string_from_cstr("UnsupportedMediatype");
    case HttpRangeNotSatisfiable:
        return string_from_cstr("RangeNotSatisfiable");
    case HttpExpectationFailed:
        return string_from_cstr("ExpectationFailed");
    case HttpImATeapot:
        return string_from_cstr("ImATeapot");
    case HttpMisdirectedRequest:
        return string_from_cstr("MisdirectedRequest");
    case HttpUnprocessableEntity:
        return string_from_cstr("UnprocessableEntity");
    case HttpLocked:
        return string_from_cstr("Locked");
    case HttpFailedDependency:
        return string_from_cstr("FailedDependency");
    case HttpTooEarly:
        return string_from_cstr("TooEarly");
    case HttpUpgradeRequired:
        return string_from_cstr("UpgradeRequired");
    case HttpPreconditionRequired:
        return string_from_cstr("PreconditionRequired");
    case HttpTooManyConflicts:
        return string_from_cstr("TooManyConflicts");
    case HttpRequestHeaderFieldsTooLarge:
        return string_from_cstr("RequestHeaderFieldsTooLarge");
    case HttpUnavailableForLegalReasons:
        return string_from_cstr("UnavailableForLegalReasons");

    case HttpInternalServerError:
        return string_from_cstr("InternalServerError");
    case HttpNotImplemented:
        return string_from_cstr("NotImplemented");
    case HttpBadGateway:
        return string_from_cstr("BadGateway");
    case HttpServiceUnavailable:
        return string_from_cstr("ServiceUnavailable");
    case HttpGatewayTimeout:
        return string_from_cstr("GatewayTimeout");
    case HttpHTTPVersionNotSupported:
        return string_from_cstr("HTTPVersionNotSupported");
    case HttpVariantAlsoNegotiates:
        return string_from_cstr("VariantAlsoNegotiates");
    case HttpInsufficientStorage:
        return string_from_cstr("InsufficientStorage");
    case HttpLoopDetected:
        return string_from_cstr("LoopDetected");
    case HttpNotExtended:
        return string_from_cstr("NotExtended");
    case HttpNetworkAuthenticationRequired:
        return string_from_cstr("NetworkAuthenticationRequired");
    default:
        eprintf("Not a valid HTTP Response! (value: %u)", response_code);
        exit(-1);
    }
}
