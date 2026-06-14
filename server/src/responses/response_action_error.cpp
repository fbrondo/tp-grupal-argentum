#include "server/includes/responses/response_action_error.h"

#include "server/includes/server_protocol.h"

ResponseActionError::ResponseActionError(std::string message): message(std::move(message)) {}

void ResponseActionError::execute(ServerProtocol& protocol) {
    protocol.sendActionError(this->message);
}
