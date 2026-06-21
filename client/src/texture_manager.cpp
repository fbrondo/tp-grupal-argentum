#include "client/includes/texture_manager.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include <SDL2/SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/core/texture.h"
#include "common/includes/direction.h"
#include "common/includes/types.h"

TextureManager::TextureManager(SDL2pp::Renderer& renderer_, WindowSDL& window_):
        renderer(renderer_), window(window_) {
    try {
        const SDL2pp::Surface icon(ARGENTUM_SHARE_PATH "/client/assets/icon.png");
        window.setIcon(icon);
    } catch (const std::exception& e) {
        std::cerr << "No se pudo cargar el icono: " << e.what() << std::endl;
    }
    this->textures = load_all_game_assets();
}

void TextureManager::register_spritesheet(const std::string& base_id, int frame_width,
                                          int frame_height, uint8_t frames_per_row,
                                          uint32_t speed_ms) {
    // Fila 0 (y = 0 * h) -> Sur (DOWN)
    // Fila 1 (y = 1 * h) -> Norte (UP)
    // Fila 2 (y = 2 * h) -> Oeste (LEFT)
    // Fila 3 (y = 3 * h) -> Este (RIGHT)

    // Registro las animaciones de caminata
    register_grid_animation(base_id + "_walk_" + std::to_string(DOWN), 0, 0 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);
    register_grid_animation(base_id + "_walk_" + std::to_string(UP), 0, 1 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);
    register_grid_animation(base_id + "_walk_" + std::to_string(LEFT), 0, 2 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);
    register_grid_animation(base_id + "_walk_" + std::to_string(RIGHT), 0, 3 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);

    // Registro las animaciones "idle" (usando el primer cuadro de cada fila)
    register_grid_animation(base_id + "_idle_" + std::to_string(DOWN), 0, 0 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
    register_grid_animation(base_id + "_idle_" + std::to_string(UP), 0, 1 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
    register_grid_animation(base_id + "_idle_" + std::to_string(LEFT), 0, 2 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
    register_grid_animation(base_id + "_idle_" + std::to_string(RIGHT), 0, 3 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
}
void TextureManager::load_heads_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    // --- HUMAN HEADS --- //
    load_texture(textures_aux, "head_" + std::to_string(BROWN_SPIKY_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3060.png");
    register_spritesheet("head_" + std::to_string(BROWN_SPIKY_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BROWN_LONG_GOATEE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3061.png");
    register_spritesheet("head_" + std::to_string(BROWN_LONG_GOATEE), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(DARK_RED_FULL_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3062.png");
    register_spritesheet("head_" + std::to_string(DARK_RED_FULL_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BLUE_PONYTAIL),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3063.png");
    register_spritesheet("head_" + std::to_string(BLUE_PONYTAIL), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BROWN_MESSY_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3064.png");
    register_spritesheet("head_" + std::to_string(BROWN_MESSY_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BLONDE_WILD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3065.png");
    register_spritesheet("head_" + std::to_string(BLONDE_WILD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(RED_SPIKY_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/humanos/3066.png");
    register_spritesheet("head_" + std::to_string(RED_SPIKY_BEARD), 27, 64, 1);
    // --- ELF HEADS --- //
    load_texture(textures_aux, "head_" + std::to_string(BLONDE_LONG),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/elfos/3000.png");
    register_spritesheet("head_" + std::to_string(BLONDE_LONG), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(GREY_LONG),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/elfos/3001.png");
    register_spritesheet("head_" + std::to_string(GREY_LONG), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BLUE_LONG),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/elfos/3002.png");
    register_spritesheet("head_" + std::to_string(BLUE_LONG), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(SILVER_LONG),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/elfos/3003.png");
    register_spritesheet("head_" + std::to_string(SILVER_LONG), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(RED_SPIKY),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/elfos/3004.png");
    register_spritesheet("head_" + std::to_string(RED_SPIKY), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(GREEN_SPIKY),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/elfos/3005.png");
    register_spritesheet("head_" + std::to_string(GREEN_SPIKY), 27, 64, 1);
    // --- DWARF HEADS --- //
    load_texture(textures_aux, "head_" + std::to_string(BROWN_SHORT_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/enanos/3020.png");
    register_spritesheet("head_" + std::to_string(BROWN_SHORT_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(GREY_SHORT_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/enanos/3021.png");
    register_spritesheet("head_" + std::to_string(GREY_SHORT_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BROWN_FULL_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/enanos/3022.png");
    register_spritesheet("head_" + std::to_string(BROWN_FULL_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(PURPLE_MOUSTACHE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/enanos/3023.png");
    register_spritesheet("head_" + std::to_string(PURPLE_MOUSTACHE), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(LIGHT_BROWN_LINE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/enanos/3024.png");
    register_spritesheet("head_" + std::to_string(LIGHT_BROWN_LINE), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(SILVER_BRAIDED),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/enanos/3025.png");
    register_spritesheet("head_" + std::to_string(SILVER_BRAIDED), 27, 64, 1);
    // --- GNOME HEADS --- //
    load_texture(textures_aux, "head_" + std::to_string(BROWN_WILD_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/gnomos/3040.png");
    register_spritesheet("head_" + std::to_string(BROWN_WILD_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BROWN_LONG_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/gnomos/3041.png");
    register_spritesheet("head_" + std::to_string(BROWN_LONG_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BROWN_CLEAN_SHAVEN),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/gnomos/3042.png");
    register_spritesheet("head_" + std::to_string(BROWN_CLEAN_SHAVEN), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(GREY_CLEAN_SHAVEN),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/gnomos/3043.png");
    register_spritesheet("head_" + std::to_string(GREY_CLEAN_SHAVEN), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(GREY_WILD_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/gnomos/3044.png");
    register_spritesheet("head_" + std::to_string(GREY_WILD_BEARD), 27, 64, 1);
    load_texture(textures_aux, "head_" + std::to_string(BALD_GREY_BEARD),
                 ARGENTUM_SHARE_PATH "/common/assets/units/heads/gnomos/3045.png");
    register_spritesheet("head_" + std::to_string(BALD_GREY_BEARD), 27, 64, 1);
}
void TextureManager::load_bodies_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    // --- HUMAN BODIES --- //
    load_texture(textures_aux, "body_" + std::to_string(CITIZEN_LEATHER),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/humanos/1036.png");
    register_spritesheet("body_" + std::to_string(CITIZEN_LEATHER), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(CITIZEN_BLUE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/humanos/1056.png");
    register_spritesheet("body_" + std::to_string(CITIZEN_BLUE), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(CITIZEN_RED),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/humanos/1070.png");
    register_spritesheet("body_" + std::to_string(CITIZEN_RED), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(PALADIN_PLATE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/humanos/1194.png");
    register_spritesheet("body_" + std::to_string(PALADIN_PLATE), 27, 47, 6);
    // --- ELF BODIES --- //
    load_texture(textures_aux, "body_" + std::to_string(MAGE_ROBE_NAVY),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/elfos/1689.png");
    register_spritesheet("body_" + std::to_string(MAGE_ROBE_NAVY), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(NOBLE_COAT_BLUE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/elfos/1896.png");
    register_spritesheet("body_" + std::to_string(NOBLE_COAT_BLUE), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(DRUID_ROBE_TEAL),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/elfos/1897.png");
    register_spritesheet("body_" + std::to_string(DRUID_ROBE_TEAL), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(WIZARD_STARS_ROBE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/elfos/1900.png");
    register_spritesheet("body_" + std::to_string(WIZARD_STARS_ROBE), 27, 47, 6);
    // --- DWARF BODIES --- //
    load_texture(textures_aux, "body_" + std::to_string(WORKER_RED),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/enanos/1681.png");
    register_spritesheet("body_" + std::to_string(WORKER_RED), 25, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(ROYAL_TUNIC),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/enanos/1718.png");
    register_spritesheet("body_" + std::to_string(ROYAL_TUNIC), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(CHAINMAIL_ARMOR),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/enanos/1837.png");
    register_spritesheet("body_" + std::to_string(CHAINMAIL_ARMOR), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(ADVENTURER_ORANGE),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/enanos/1921.png");
    register_spritesheet("body_" + std::to_string(ADVENTURER_ORANGE), 27, 47, 6);
    // --- GNOME BODIES --- //
    load_texture(textures_aux, "body_" + std::to_string(FORESTER_LEAVES),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/gnomos/1700.png");
    register_spritesheet("body_" + std::to_string(FORESTER_LEAVES), 25, 45, 6);
    load_texture(textures_aux, "body_" + std::to_string(PIRATE_PANTS),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/gnomos/1710.png");
    register_spritesheet("body_" + std::to_string(PIRATE_PANTS), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(ASSASSIN_LEATHER),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/gnomos/1841.png");
    register_spritesheet("body_" + std::to_string(ASSASSIN_LEATHER), 27, 47, 6);
    load_texture(textures_aux, "body_" + std::to_string(HUNTER_FUR),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/gnomos/1861.png");
    register_spritesheet("body_" + std::to_string(HUNTER_FUR), 27, 47, 6);
    // --- GHOST TEXTURE --- //
    load_texture(textures_aux, "body_" + std::to_string(GHOST),
                 ARGENTUM_SHARE_PATH "/common/assets/units/bodies/2200.png");
    register_spritesheet("body_" + std::to_string(GHOST), 29, 32, 3);
}
void TextureManager::load_npcs_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    load_texture(textures_aux, "npc_" + std::to_string(FIELD_GOBLIN),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/300.png");
    register_spritesheet("npc_" + std::to_string(FIELD_GOBLIN), 25, 32, 8);
    load_texture(textures_aux, "npc_" + std::to_string(FOREST_GOBLIN),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/301.png");
    register_spritesheet("npc_" + std::to_string(FOREST_GOBLIN), 25, 32, 8);
    load_texture(textures_aux, "npc_" + std::to_string(SKELETON),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/302.png");
    register_spritesheet("npc_" + std::to_string(SKELETON), 25, 52, 6);
    load_texture(textures_aux, "npc_" + std::to_string(WARRIOR_SKELETON),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/303.png");
    register_spritesheet("npc_" + std::to_string(WARRIOR_SKELETON), 25, 52, 6);
    load_texture(textures_aux, "npc_" + std::to_string(ZOMBIE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/304.png");
    register_spritesheet("npc_" + std::to_string(ZOMBIE), 24, 47, 8);
    load_texture(textures_aux, "npc_" + std::to_string(DUNGEON_SPIDER),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/307.png");
    register_spritesheet("npc_" + std::to_string(DUNGEON_SPIDER), 192, 200, 5);
    load_texture(textures_aux, "npc_" + std::to_string(CAVE_SPIDER),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/306.png");
    register_spritesheet("npc_" + std::to_string(CAVE_SPIDER), 97, 97, 5);
    load_texture(textures_aux, "npc_" + std::to_string(FOREST_SPIDER),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/305.png");
    register_spritesheet("npc_" + std::to_string(FOREST_SPIDER), 97, 97, 5);
    load_texture(textures_aux, "npc_" + std::to_string(ORC),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/308.png");
    register_spritesheet("npc_" + std::to_string(ORC), 24, 51, 6);
    load_texture(textures_aux, "npc_" + std::to_string(GOLEM),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/309.png");
    register_spritesheet("npc_" + std::to_string(GOLEM), 162, 162, 6);
    load_texture(textures_aux, "npc_" + std::to_string(INFERNAL_GOLEM),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/criatures/310.png");
    register_spritesheet("npc_" + std::to_string(INFERNAL_GOLEM), 162, 162, 6);

    load_texture(textures_aux, "npc_" + std::to_string(PRIEST),
                 ARGENTUM_SHARE_PATH "/client/assets/NPCs/4.png");
    load_texture(textures_aux, "npc_" + std::to_string(BANKER),
                 ARGENTUM_SHARE_PATH "/client/assets/NPCs/0.png");
    load_texture(textures_aux, "npc_" + std::to_string(MERCHANT),
                 ARGENTUM_SHARE_PATH "/client/assets/NPCs/2.png");
    load_texture(textures_aux, "npc_" + std::to_string(15),
                 ARGENTUM_SHARE_PATH "/client/assets/NPCs/1.png");  // Banquero seleccionado
    load_texture(textures_aux, "npc_" + std::to_string(16),
                 ARGENTUM_SHARE_PATH "/client/assets/NPCs/3.png");  // Comerciante seleccionado
    load_texture(textures_aux, "npc_" + std::to_string(17),
                 ARGENTUM_SHARE_PATH "/client/assets/NPCs/5.png");  // Sacerdote seleccionado
}
void TextureManager::load_items_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {

    const std::string base_path = ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/";
    const std::vector<ItemTextureInfo> items_to_load = {
            {SWORD, "1000.png"},         {AXE, "1001.png"},         {HAMMER, "1002.png"},
            {ASH_STAFF, "1100.png"},     {ELVEN_FLUTE, "1101.png"}, {KNOTTED_STAFF, "1102.png"},
            {INLAID_STAFF, "1103.png"},  {SIMPLE_BOW, "1003.png"},  {COMPOUND_BOW, "1004.png"},
            {LEATHER_ARMOR, "1400.png"}, {PLATE_AMOR, "1401.png"},  {BLUE_TUNIC, "1402.png"},
            {HOOD, "1300.png"},          {IRON_HELMET, "1301.png"}, {TORTOISE_SHIELD, "1500.png"},
            {IRON_SHIELD, "1501.png"},   {MAGIC_HAT, "1302.png"},   {LIFE_POTION, "1600.png"},
            {MANA_POTION, "1601.png"},   {GOLD, "2000.png"}};

    for (const auto& item: items_to_load) {
        load_texture(textures_aux, "item_" + std::to_string(item.id), base_path + item.filename);
    }
}
/*void TextureManager::load_items_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    load_texture(textures_aux, "item_" + std::to_string(SWORD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1000.png");
    load_texture(textures_aux, "item_" + std::to_string(AXE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1001.png");
    load_texture(textures_aux, "item_" + std::to_string(HAMMER),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1002.png");
    load_texture(textures_aux, "item_" + std::to_string(ASH_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1100.png");
    load_texture(textures_aux, "item_" + std::to_string(ELVEN_FLUTE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1101.png");
    load_texture(textures_aux, "item_" + std::to_string(KNOTTED_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1102.png");
    load_texture(textures_aux, "item_" + std::to_string(INLAID_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1103.png");
    load_texture(textures_aux, "item_" + std::to_string(SIMPLE_BOW),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1003.png");
    load_texture(textures_aux, "item_" + std::to_string(COMPOUND_BOW),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1004.png");
    load_texture(textures_aux, "item_" + std::to_string(LEATHER_ARMOR),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1400.png");
    load_texture(textures_aux, "item_" + std::to_string(PLATE_AMOR),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1401.png");
    load_texture(textures_aux, "item_" + std::to_string(BLUE_TUNIC),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1402.png");
    load_texture(textures_aux, "item_" + std::to_string(HOOD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1300.png");
    load_texture(textures_aux, "item_" + std::to_string(IRON_HELMET),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1301.png");
    load_texture(textures_aux, "item_" + std::to_string(TORTOISE_SHIELD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1500.png");
    load_texture(textures_aux, "item_" + std::to_string(IRON_SHIELD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1501.png");
    load_texture(textures_aux, "item_" + std::to_string(MAGIC_HAT),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1302.png");
    load_texture(textures_aux, "item_" + std::to_string(LIFE_POTION),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1600.png");
    load_texture(textures_aux, "item_" + std::to_string(MANA_POTION),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/1601.png");
    load_texture(textures_aux, "item_" + std::to_string(GOLD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Items/2000.png");
}*/
void TextureManager::load_animation_items(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    // --- HUMAN/ELF ITEMS ANIMATION --- //
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(SWORD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1000.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(SWORD), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(AXE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1001.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(AXE), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(HAMMER),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1002.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(HAMMER), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(ASH_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1100.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(ASH_STAFF), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(ELVEN_FLUTE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1101.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(ELVEN_FLUTE), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(KNOTTED_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1102.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(KNOTTED_STAFF), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(INLAID_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1103.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(INLAID_STAFF), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(SIMPLE_BOW),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1003.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(SIMPLE_BOW), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(COMPOUND_BOW),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1004.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(COMPOUND_BOW), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(LEATHER_ARMOR),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1400.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(LEATHER_ARMOR), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(PLATE_AMOR),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1401.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(PLATE_AMOR), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(BLUE_TUNIC),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1402.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(BLUE_TUNIC), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(HOOD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1300.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(HOOD), 27, 63, 1);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(IRON_HELMET),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1301.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(IRON_HELMET), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(TORTOISE_SHIELD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1500.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(TORTOISE_SHIELD), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(IRON_SHIELD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1501.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(IRON_SHIELD), 25, 45, 6);
    load_texture(textures_aux, "anim_hum_elf_" + std::to_string(MAGIC_HAT),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/humanos-elfos/1302.png");
    register_spritesheet("anim_hum_elf_" + std::to_string(MAGIC_HAT), 27, 63, 1);

    // --- DWARF/GNOME ITEMS ANIMATION --- //
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(SWORD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11000.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(SWORD), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(AXE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11001.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(AXE), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(HAMMER),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11002.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(HAMMER), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(ASH_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11100.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(ASH_STAFF), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(ELVEN_FLUTE),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11101.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(ELVEN_FLUTE), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(KNOTTED_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11102.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(KNOTTED_STAFF), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(INLAID_STAFF),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11103.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(INLAID_STAFF), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(SIMPLE_BOW),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11003.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(SIMPLE_BOW), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(COMPOUND_BOW),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11004.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(COMPOUND_BOW), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(LEATHER_ARMOR),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11400.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(LEATHER_ARMOR), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(PLATE_AMOR),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11401.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(PLATE_AMOR), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(BLUE_TUNIC),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11402.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(BLUE_TUNIC), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(HOOD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11300.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(HOOD), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(IRON_HELMET),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11301.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(IRON_HELMET), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(TORTOISE_SHIELD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11500.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(TORTOISE_SHIELD), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(IRON_SHIELD),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11501.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(IRON_SHIELD), 25, 46, 6);
    load_texture(textures_aux, "anim_drf_gnm_" + std::to_string(MAGIC_HAT),
                 ARGENTUM_SHARE_PATH "/client/assets/Sprites/Units/items/gnomos-enanos/11302.png");
    register_spritesheet("anim_drf_gnm_" + std::to_string(MAGIC_HAT), 25, 46, 6);
}

void TextureManager::load_tile_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    const std::array<std::pair<std::string, std::string>, 4> layers = {{
            {ARGENTUM_SHARE_PATH "/common/assets/maps/background/", "tile_bg_"},
            {ARGENTUM_SHARE_PATH "/common/assets/maps/details/", "tile_det_"},
            {ARGENTUM_SHARE_PATH "/common/assets/maps/objets/", "tile_obj_"},
            {ARGENTUM_SHARE_PATH "/common/assets/maps/roof/", "tile_roof_"},
    }};
    // int total = 0;
    for (const auto& [dir, prefix]: layers) {
        if (!std::filesystem::exists(dir)) {
            std::cout << "[TextureManager] DIRECTORIO NO ENCONTRADO: " << dir << std::endl;
            continue;
        }
        // int count = 0;
        for (const auto& entry: std::filesystem::directory_iterator(dir)) {
            if (entry.path().extension() != ".png")
                continue;
            const std::string id = entry.path().stem().string();
            load_texture(textures_aux, prefix + id, entry.path().string());
            // count++;
        }
        /*std::cout << "[TextureManager] " << prefix << "* : " << count << " tiles cargados."
                  << std::endl;*/
        // total += count;
    }
    // std::cout << "[TextureManager] Total tiles: " << total << std::endl;
}

void TextureManager::load_effect_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    const std::array<std::pair<VisualEffectID, std::string>, 7> effects = {{
            {VisualEffectID::EXPLOSION, "Explosion"},
            {VisualEffectID::FIRE, "Fire"},
            {VisualEffectID::DEATH, "Death"},
            {VisualEffectID::LIGHTNING, "Lightning"},
            {VisualEffectID::BE_HEALED, "BeHealed"},
            {VisualEffectID::BE_ATTACKED, "BeAttacked"},
            {VisualEffectID::DMG, "Dmg"},
    }};

    for (const auto& [effect_id, folder]: effects) {
        const std::filesystem::path dir = ARGENTUM_SHARE_PATH "/client/assets/Effects/" + folder;
        if (!std::filesystem::exists(dir)) {
            std::cout << "[TextureManager] DIRECTORIO NO ENCONTRADO: " << dir << std::endl;
            continue;
        }

        std::vector<std::pair<int, std::filesystem::path>> frames;
        for (const auto& entry: std::filesystem::directory_iterator(dir)) {
            if (entry.path().extension() != ".png")
                continue;
            try {
                frames.emplace_back(std::stoi(entry.path().stem().string()), entry.path());
            } catch (const std::exception&) {}
        }

        std::sort(frames.begin(), frames.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

        VisualEffectClip clip;
        clip.frame_rate_ms = 80;
        for (const auto& [frame_number, path]: frames) {
            const std::string texture_id = "effect_" +
                                           std::to_string(static_cast<uint16_t>(effect_id)) + "_" +
                                           std::to_string(frame_number);
            if (load_texture(textures_aux, texture_id, path.string())) {
                clip.frame_texture_ids.push_back(texture_id);
            }
        }

        if (!clip.frame_texture_ids.empty()) {
            this->visual_effects[effect_id] = std::move(clip);
        }
    }
}

void TextureManager::load_HUD_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    // --- HUD BASE --- //
    load_texture(textures_aux, "hud_upper_layer", "client/assets/HUD/UpperLayer.png");

    // --- CONSOLA --- //
    load_texture(textures_aux, "hud_console_base", "client/assets/HUD/Console/base.png");

    // --- STATS & INFO --- //
    load_texture(textures_aux, "hud_stats_base", "client/assets/HUD/UserStats/base.png");
    load_texture(textures_aux, "hud_user_info_base", "client/assets/HUD/UserInfo/base.png");

    // --- INVENTARIO --- //
    load_texture(textures_aux, "hud_inventory_base", "client/assets/HUD/UserInventory/base.png");
    load_texture(textures_aux, "hud_inventory_selected",
                 "client/assets/HUD/UserInventory/selected.png");

    // --- BARRAS DE PROGRESO (Vida, Maná, Exp) --- //
    load_texture(textures_aux, "hud_pb_base", "client/assets/HUD/ProgressBar/base.png");
    load_texture(textures_aux, "hud_pb_bisel", "client/assets/HUD/ProgressBar/bisel.png");
    load_texture(textures_aux, "hud_pb_black_border",
                 "client/assets/HUD/ProgressBar/black_border.png");
    load_texture(textures_aux, "hud_pb_white_border",
                 "client/assets/HUD/ProgressBar/white_border.png");
    load_texture(textures_aux, "hud_pb_old_base", "client/assets/HUD/ProgressBar/old_base.png");
}

std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>
        TextureManager::load_all_game_assets() {
    // std::cout << "[TextureManager] Cargando assets..." << std::endl;
    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> textures_aux;
    // --- ITEMS --- //
    load_items_textures(textures_aux);
    load_animation_items(textures_aux);
    // --- HEADS --- //
    load_heads_textures(textures_aux);
    // --- BODIES --- //
    load_bodies_textures(textures_aux);
    // --- NPCS --- //
    load_npcs_textures(textures_aux);
    // --- TILES --- //
    load_tile_textures(textures_aux);
    // --- EFFECTS --- //
    load_effect_textures(textures_aux);
    // --- HUD --- //
    load_HUD_textures(textures_aux);

    // std::cout << "[TextureManager] Carga finalizada." << std::endl;
    return textures_aux;
}

bool TextureManager::load_texture(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_,
        const std::string& id, const std::string& filename) {
    if (textures_.contains(id)) {
        return true;
    }
    try {
        SDL2pp::Surface surface(filename);
        surface.SetColorKey(true, 0);
        textures_[id] = std::make_unique<SDL2pp::Texture>(renderer, surface);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error al cargar la textura [" << id << "] desde " << filename << ": "
                  << e.what() << std::endl;
        return false;
    }
}

void TextureManager::register_grid_animation(const std::string& anim_id, int start_x, int start_y,
                                             int frame_width, int frame_height, int frame_count,
                                             uint32_t speed_ms) {
    AnimationClip clip;
    clip.frame_rate_ms = speed_ms;

    for (int i = 0; i < frame_count; ++i) {
        SDL_Rect rect;
        rect.x = start_x + (i * frame_width);
        rect.y = start_y;
        rect.w = frame_width;
        rect.h = frame_height;

        clip.frames.push_back(rect);
    }
    animations[anim_id] = clip;
}

SDL2pp::Texture& TextureManager::get_texture(const std::string& id) const {
    const auto it = textures.find(id);
    if (it == textures.end()) {
        throw std::runtime_error("Textura no encontrada: " + id);
    }
    return *(it->second);
}

const AnimationClip& TextureManager::get_animation(const std::string& anim_id) const {
    const auto it = animations.find(anim_id);
    if (it == animations.end()) {
        throw std::runtime_error("Animacion no encontrada: " + anim_id);
    }
    return it->second;
}

const VisualEffectClip& TextureManager::get_visual_effect(VisualEffectID effect_id) const {
    const auto it = visual_effects.find(effect_id);
    if (it == visual_effects.end()) {
        throw std::runtime_error("Efecto visual no encontrado: " +
                                 std::to_string(static_cast<uint16_t>(effect_id)));
    }
    return it->second;
}

uint32_t TextureManager::get_current_animation_frame(const AnimationState& state,
                                                     const AnimationClip& clip) {
    // 1. Tiempo transcurrido (siempre positivo)
    uint32_t elapsed_time = SDL_GetTicks() - state.start_time;
    // 2. Cuadros transcurridos
    uint32_t total_frames_elapsed = elapsed_time / clip.frame_rate_ms;
    // 3. Cantidad de cuadros (clip.frames.size() devuelve un size_t, lo casteamos a uint32_t)
    uint32_t num_frames = static_cast<uint32_t>(clip.frames.size());
    if (num_frames == 0)
        return 0;
    if (state.is_looping) {
        return total_frames_elapsed % num_frames;
    }
    if (total_frames_elapsed >= num_frames) {
        return num_frames - 1;
    }
    return total_frames_elapsed;
}
