#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <memory>
#include <vector>
#include "server/includes/core/instances.h"

class Equipment {
private:
    std::vector<std::unique_ptr<ItemInstace>> equipment_container;
    void equipHandItem(std::unique_ptr<ItemInstace> item_inst);
    void equipItemDefensive(std::unique_ptr<ItemInstace> item_inst);

public:
    Equipment(const Equipment&) = delete;            // No permitir copias
    Equipment& operator=(const Equipment&) = delete; // No permitir igualaciones

    Equipment(Equipment&& other) noexcept = default;
    Equipment& operator=(Equipment&& other) noexcept = default;
    
    Equipment(/* args */);
    ~Equipment();

    void equipItem(std::unique_ptr<ItemInstace> item_inst);

    /*Vendio o solto el item*/
    std::unique_ptr<ItemInstace> removeItem(Id id_inst_item);

    /*Necesario para calcular los puntos de defensa*/
    std::vector<const ItemInstace*> getEquipmentDefensive();

    /* Utilizo su arma para atacar o con un objeto magico lanzo algun hechizo, con el TypeItem el
     * server sabra que hacer */
    TypeItem getHandItem();
};

#endif
