#include "command.h"

class MeditateCommand : public Command {
public:
    explicit MeditateCommand(Id id);
    void execute(Gameloop& gameloop) override;
};