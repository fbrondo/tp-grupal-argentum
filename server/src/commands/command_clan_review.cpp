#include "server/includes/commands/command_clan_review.h"

#include "server/includes/gameloop.h"

ClanReviewCommand::ClanReviewCommand(Id id): Command(id) {}

void ClanReviewCommand::execute(Gameloop& gameloop) {
    gameloop.processClanReview(this->getIdPlayer());
}
