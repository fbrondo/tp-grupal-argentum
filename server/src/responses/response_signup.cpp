#include "server/includes/responses/response_signup.h"

ResponseSignup::ResponseSignup(bool success_): success(success_) {}

ResponseSignup::ResponseSignup(bool success_, const std::string& message_):
        success(success_), message(message_) {}

void ResponseSignup::execute(ServerProtocol& protocol) {
    protocol.sendSignupResponse(this->success, this->message);
}
