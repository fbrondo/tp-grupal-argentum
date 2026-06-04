#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <tuple>
#include <vector>

#include "server/includes/core/instances.h"

class Equipment {
private:
    std::vector<ItemInstace*> equipment_container;

    void equipHandItem(ItemInstace* item_inst);
    void equipItemDefensive(ItemInstace* item_inst);

public:
    Equipment(const Equipment&) = delete;             // No permitir copias
    Equipment& operator=(const Equipment&) = delete;  // No permitir igualaciones

    Equipment(Equipment&& other) noexcept = default;
    Equipment& operator=(Equipment&& other) noexcept = default;

    Equipment(/* args */);
    ~Equipment();

    void equipItem(ItemInstace* item_inst);

    /*Vendio, solto el item o lo quito del equipo*/
    void removeItem(Id id_inst_item);
    /*Vendio o solto el item*/
    std::unique_ptr<ItemInstance> removeItem(Id id_inst_item);

    /*Necesario para calcular los puntos de defensa*/
    std::vector<std::tuple<Id, TypeItem>> getEquipment();

    /* Utilizo su arma para atacar o con un objeto magico lanzo algun hechizo, con el TypeItem el
     * server sabra que hacer */
    TypeItem getHandItem();
};


#endif
