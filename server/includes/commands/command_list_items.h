#include "command.h"    

class ListItemsCommand : public Command {
private:
    Id npc_id;
public:
    explicit ListItemsCommand(Id id, Id npc_id);
    void execute(Gameloop& gameloop) override;
};