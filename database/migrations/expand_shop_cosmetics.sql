USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS item_equipment_rules
(
    item_id INT UNSIGNED NOT NULL,
    occupied_slots BIGINT UNSIGNED NOT NULL,
    character_mask INT UNSIGNED NOT NULL,
    PRIMARY KEY (item_id),
    CONSTRAINT fk_item_equipment_rules_item_id FOREIGN KEY (item_id) REFERENCES item_definitions (id),
    CONSTRAINT ck_item_equipment_rules_occupied_slots CHECK (occupied_slots > 0)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Original SlotLocation IDs occupy bits 1..31; PartLocation IDs use a separate domain at 32 + id.
-- The server treats primary and secondary clothing parts as exclusive for consistent snapshots.
-- These internal slot IDs are never sent on the wire; S805 contains item resource IDs.
-- Verified ShopItem2.xml SHA-256: 06cab10e98ae706ae9f00f45c2a1d58e52c302831e40da598e94f977a4521574
START TRANSACTION;
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT id, CAST(1 AS UNSIGNED) << equipment_slot, 3 FROM item_definitions
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_main_categories (id, name, display_order)
SELECT 100, 'Clothes', 1004
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 100, 100, 'All_Clothes', 1100, 2
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 102, 100, 'Hats', 1102, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 105, 100, 'Gloves', 1106, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 106, 100, 'Shoes', 1107, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 107, 100, 'SetClothes', 1101, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 108, 100, 'Shirts', 1103, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 109, 100, 'Pants', 1104, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 110, 100, 'Costumes', 1105, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_main_categories (id, name, display_order)
SELECT 200, 'Accessories', 1005
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 200, 200, 'All_Accesory', 1200, 2
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 202, 200, 'HeadAccesory', 1201, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 203, 200, 'FaceAccesory', 1202, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 204, 200, 'FrontAccesory', 1203, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 205, 200, 'BackAccesory', 1204, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100043, '绿色瞄准眼镜', 10, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100043, 1536, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100043, 203, 10, 360,
    '2011-08-05 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011303, 100043, 7, 1200, 1200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011304, 100043, 30, 2400, 2400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100044, '粉色瞄准眼镜', 10, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100044, 1536, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100044, 203, 11, 360,
    '2010-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011305, 100044, 7, 1200, 1200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011306, 100044, 30, 2400, 2400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100046, '水弹储备弹匣500ml', 17, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100046, 131072, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100046, 205, 12, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011301, 100046, 7, 1900, 1900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011302, 100046, 30, 3800, 3800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100057, '龟仙人背壳 S1', 16, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100057, 65536, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100057, 205, 17, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011309, 100057, 7, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011310, 100057, 30, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100062, '信天翁羽翼', 16, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100062, 65536, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100062, 205, 18, 360,
    '2011-09-06 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011633, 100062, 1, 600, 600, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011634, 100062, 7, 2400, 2400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011635, 100062, 30, 4800, 4800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100063, '幸运道具挂饰', 14, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100063, 16384, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100063, 204, 19, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011348, 100063, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011349, 100063, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100072, '小熊套装帽子', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100072, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100072, 102, 25, 360,
    '2011-08-05 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011341, 100072, 7, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011342, 100072, 30, 10000, 10000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100073, '小熊套装衣服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100073, 8761733283840, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100073, 110, 26, 360,
    '2011-08-05 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011344, 100073, 7, 3000, 3000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011345, 100073, 30, 7000, 7000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100076, '多彩五角海星', 1, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100076, 2, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100076, 202, 28, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011307, 100076, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011308, 100076, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100077, '趴趴章鱼', 1, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100077, 2, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100077, 202, 29, 360,
    '2011-10-18 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011927, 100077, 7, 1500, 1500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011928, 100077, 30, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100078, '短跑家潜水眼镜', 10, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100078, 1536, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100078, 203, 30, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011428, 100078, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011429, 100078, 7, 1600, 1600, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011430, 100078, 30, 3200, 3200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100086, '生命臂章', 13, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100086, 8192, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100086, 204, 37, 360,
    '2011-10-18 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011299, 100086, 7, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011300, 100086, 30, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100096, '中秋喜兔帽子', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100096, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100096, 102, 40, 360,
    '2011-09-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011608, 100096, 7, 6000, 6000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011609, 100096, 30, 12000, 12000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100106, '万圣节南瓜幽灵帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100106, 25769803776, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100106, 102, 45, 360,
    '2011-11-08 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011945, 100106, 7, 6000, 6000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011946, 100106, 30, 12000, 12000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100111, '水弹储备弹匣1L', 17, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100111, 131072, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100111, 205, 49, 360,
    '2011-08-26 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011562, 100111, 1, 600, 600, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011563, 100111, 7, 2400, 2400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011564, 100111, 30, 4800, 4800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100125, '中秋喜兔衣服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100125, 1065151889408, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100125, 108, 61, 360,
    '2011-09-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011611, 100125, 7, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011612, 100125, 30, 10000, 10000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100126, '中秋喜兔裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100126, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100126, 109, 62, 360,
    '2011-09-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011614, 100126, 7, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011615, 100126, 30, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100128, '中秋喜兔鞋子', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100128, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100128, 106, 63, 360,
    '2011-09-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011617, 100128, 7, 1700, 1700, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011618, 100128, 30, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100140, '液体推进器 P01', 16, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100140, 65536, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100140, 205, 71, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011320, 100140, 1, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011321, 100140, 7, 3200, 3200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011322, 100140, 30, 6400, 6400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100141, '医疗救急箱M1', 15, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100141, 32768, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100141, 204, 72, 360,
    '2011-10-18 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011925, 100141, 7, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011926, 100141, 30, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016083, 100141, 15, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100143, '营救臂章', 13, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100143, 8192, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100143, 204, 74, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011356, 100143, 7, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011357, 100143, 30, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100144, '信天翁护腕', 24, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100144, 16777216, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100144, 204, 75, 360,
    '2011-08-30 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011565, 100144, 1, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011566, 100144, 7, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011567, 100144, 30, 4000, 4000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100145, '隐蔽达人的挂饰', 14, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100145, 16384, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100145, 204, 76, 360,
    '2011-12-08 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012049, 100145, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012050, 100145, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100173, '飞影头盔', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100173, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100173, 102, 92, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012203, 100173, 7, 4000, 4000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012208, 100173, 30, 8000, 8000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013149, 100173, 3, 4000, 4000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100174, '飞影夹克', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100174, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100174, 108, 93, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012204, 100174, 7, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012209, 100174, 30, 4000, 4000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013150, 100174, 3, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100175, '飞影炫裤', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100175, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100175, 109, 94, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012205, 100175, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012210, 100175, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013151, 100175, 3, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100176, '飞影手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100176, 412316860416, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100176, 105, 95, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012206, 100176, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012211, 100176, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013152, 100176, 3, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100177, '飞影靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100177, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100177, 106, 96, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012207, 100177, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012212, 100177, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013153, 100177, 3, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100182, '香蕉腰带', 17, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100182, 131072, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100182, 205, 101, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012915, 100182, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012916, 100182, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100242, '手雷腰带L1', 15, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100242, 32768, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100242, 204, 150, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012251, 100242, 1, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012252, 100242, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012253, 100242, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100290, 'BF冒险王帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100290, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100290, 102, 179, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012818, 100290, 7, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012823, 100290, 30, 7000, 7000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100291, 'BF冒险王上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100291, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100291, 108, 180, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012819, 100291, 7, 1500, 1500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012824, 100291, 30, 3000, 3000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100292, 'BF冒险王裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100292, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100292, 109, 181, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012820, 100292, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012825, 100292, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100293, 'BF冒险王手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100293, 412316860416, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100293, 105, 182, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012821, 100293, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012826, 100293, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100294, 'BF冒险王靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100294, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100294, 106, 183, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012822, 100294, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012827, 100294, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100330, '超级明星☆帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100330, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100330, 102, 1001, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012943, 100330, 7, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012948, 100330, 30, 7000, 7000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100331, '超级明星☆上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100331, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100331, 108, 1002, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012944, 100331, 7, 1500, 1500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012949, 100331, 30, 3000, 3000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100332, '超级明星☆下装', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100332, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100332, 109, 1003, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012945, 100332, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012950, 100332, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100333, '超级明星☆手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100333, 412316860416, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100333, 105, 1004, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012946, 100333, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012951, 100333, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100334, '超级明星☆鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100334, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 100334, 106, 1005, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012947, 100334, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012952, 100334, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120101, '风帽训练帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120101, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120101, 102, 319, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011263, 120101, 7, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011264, 120101, 30, 10000, 10000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120103, '风帽训练鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120103, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120103, 106, 320, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011269, 120103, 7, 1500, 1500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011270, 120103, 30, 3000, 3000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120129, '风帽训练衣服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120129, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120129, 108, 321, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011265, 120129, 7, 4000, 4000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011266, 120129, 30, 8000, 8000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120130, '风帽训练裤', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120130, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120130, 109, 322, 360,
    '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011267, 120130, 7, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011268, 120130, 30, 4000, 4000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120156, '红玫瑰头带', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120156, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120156, 102, 328, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012536, 120156, 7, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012541, 120156, 30, 7000, 7000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120157, '红玫瑰上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120157, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120157, 108, 329, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012537, 120157, 7, 1500, 1500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012542, 120157, 30, 3000, 3000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120158, '红玫瑰裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120158, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120158, 109, 330, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012538, 120158, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012543, 120158, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120159, '红玫瑰手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120159, 412316860416, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120159, 105, 331, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012539, 120159, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012544, 120159, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120160, '红玫瑰鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120160, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120160, 106, 332, 360,
    '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012540, 120160, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012545, 120160, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120232, '军帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120232, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120232, 102, 334, 360,
    '2011-10-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011730, 120232, 7, 6000, 6000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011731, 120232, 30, 12000, 12000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120233, '军服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120233, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120233, 108, 335, 360,
    '2011-10-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011733, 120233, 7, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011734, 120233, 30, 10000, 10000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120234, '军裤', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120234, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120234, 109, 336, 360,
    '2011-10-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011736, 120234, 7, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011737, 120234, 30, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120235, '军服手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120235, 412316860416, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120235, 105, 337, 360,
    '2011-10-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011739, 120235, 7, 1700, 1700, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011740, 120235, 30, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120236, '军靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120236, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120236, 106, 338, 360,
    '2011-10-11 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011742, 120236, 7, 1700, 1700, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011743, 120236, 30, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120237, '泡泡自由发型', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120237, 8589934592, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120237, 102, 340, 360,
    '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012645, 120237, 7, 3500, 3500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012650, 120237, 30, 7000, 7000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120238, '泡泡自由上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120238, 652835028992, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120238, 108, 341, 360,
    '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012646, 120238, 7, 1500, 1500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012651, 120238, 30, 3000, 3000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120239, '泡泡自由裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120239, 3298534883328, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120239, 109, 342, 360,
    '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012647, 120239, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012652, 120239, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120240, '泡泡自由手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120240, 412316860416, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120240, 105, 343, 360,
    '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012648, 120240, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012653, 120240, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120241, '泡泡自由靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120241, 4398046511104, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days,
     sale_starts_at, sale_ends_at, is_on_sale)
SELECT 120241, 106, 344, 360,
    '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012649, 120241, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012654, 120241, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 8, 'ExpandShopCosmetics', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 8);
COMMIT;
