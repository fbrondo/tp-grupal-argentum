#include "command.h"

class HealCommand : public Command {
public:
    explicit HealCommand(Id id);
    void execute(Gameloop& gameloop) override;
};