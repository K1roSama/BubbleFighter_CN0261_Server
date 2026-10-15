USE bubblefighter_cn0261;

-- Original cashCN prices, dates and character compatibility.
-- No cash is granted. Functional accessories and quantity-based weapons remain excluded.
START TRANSACTION;
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100004, '企鹅上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100004, 8761733283840, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100004, 110, 2, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011278, 100004, 1, 133, 133, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011279, 100004, 7, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011280, 100004, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100007, '企鹅帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100007, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100007, 102, 4, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011275, 100007, 1, 167, 167, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011276, 100007, 7, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011277, 100007, 30, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100047, '花纹宽边帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100047, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100047, 102, 13, 360, '2011-09-13 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011550, 100047, 1, 218, 218, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011551, 100047, 7, 750, 750, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011552, 100047, 30, 1300, 1300, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100048, '花纹无袖连衣裙', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100048, 549755813888, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100048, 108, 14, 360, '2011-09-13 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011553, 100048, 1, 168, 168, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011554, 100048, 7, 480, 480, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011555, 100048, 30, 960, 960, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100049, '花纹鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100049, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100049, 106, 15, 360, '2011-09-13 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011556, 100049, 1, 88, 88, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011557, 100049, 7, 268, 268, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011558, 100049, 30, 540, 540, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100074, '夏威夷西瓜帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100074, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100074, 102, 27, 360, '2011-08-26 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011574, 100074, 1, 218, 218, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011575, 100074, 7, 750, 750, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011576, 100074, 30, 1300, 1300, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100105, '万圣节魔法师风帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100105, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100105, 102, 44, 360, '2011-10-28 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011929, 100105, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011930, 100105, 7, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011931, 100105, 30, 1240, 1240, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100108, '万圣节骷髅手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100108, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100108, 105, 46, 360, '2011-10-28 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011938, 100108, 1, 95, 95, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011939, 100108, 7, 280, 280, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011940, 100108, 30, 590, 590, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100109, '万圣节骷髅靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100109, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100109, 106, 47, 360, '2011-10-28 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011941, 100109, 1, 95, 95, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011942, 100109, 7, 280, 280, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011943, 100109, 30, 590, 590, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100117, '泡泡棒球队棒球服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100117, 652835028992, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100117, 108, 54, 360, '2011-10-25 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011959, 100117, 1, 133, 133, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011960, 100117, 7, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011961, 100117, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100118, '泡泡棒球队棒球裤', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100118, 3298534883328, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100118, 109, 55, 360, '2011-10-25 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011962, 100118, 1, 83, 83, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011963, 100118, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011964, 100118, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100119, '泡泡棒球队手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100119, 412316860416, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100119, 105, 56, 360, '2011-10-25 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011965, 100119, 1, 83, 83, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011966, 100119, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011967, 100119, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100120, '泡泡棒球队靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100120, 4398046511104, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100120, 106, 57, 360, '2011-10-25 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011968, 100120, 1, 83, 83, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011969, 100120, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011970, 100120, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100121, '夏威夷沙滩T恤', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100121, 1065151889408, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100121, 108, 58, 360, '2011-08-26 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011577, 100121, 1, 168, 168, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011578, 100121, 7, 480, 480, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011579, 100121, 30, 960, 960, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100122, '夏威夷沙滩短裤', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100122, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100122, 109, 59, 360, '2011-08-26 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011580, 100122, 1, 88, 88, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011581, 100122, 7, 268, 268, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011582, 100122, 30, 540, 540, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100124, '夏威夷沙滩凉鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100124, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100124, 106, 60, 360, '2011-08-26 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011583, 100124, 1, 88, 88, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011584, 100124, 7, 268, 268, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011585, 100124, 30, 540, 540, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100133, '万圣节骷髅上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100133, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100133, 108, 66, 360, '2011-10-28 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011932, 100133, 1, 175, 175, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011933, 100133, 7, 530, 530, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011934, 100133, 30, 1150, 1150, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100134, '万圣节骷髅裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100134, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100134, 109, 67, 360, '2011-10-28 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011935, 100134, 1, 95, 95, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011936, 100134, 7, 280, 280, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011937, 100134, 30, 590, 590, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100147, '爱斯基摩棉帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100147, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100147, 102, 77, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011980, 100147, 1, 188, 188, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011981, 100147, 7, 560, 560, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011982, 100147, 30, 1120, 1120, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100148, '爱斯基摩棉外套', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100148, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100148, 108, 78, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011983, 100148, 1, 168, 168, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011984, 100148, 7, 480, 480, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011985, 100148, 30, 960, 960, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100149, '爱斯基摩棉裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100149, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100149, 109, 79, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011986, 100149, 1, 88, 88, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011987, 100149, 7, 268, 268, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011988, 100149, 30, 540, 540, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100150, '爱斯基摩皮手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100150, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100150, 105, 80, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011989, 100150, 1, 88, 88, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011990, 100150, 7, 268, 268, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011991, 100150, 30, 540, 540, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100151, '爱斯基摩雪地靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100151, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100151, 106, 81, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011992, 100151, 1, 88, 88, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011993, 100151, 7, 268, 268, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011994, 100151, 30, 540, 540, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100181, '恶魔角发带', 1, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100181, 2, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100181, 202, 300555, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014415, 100181, 1, 190, 190, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014416, 100181, 7, 490, 490, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014417, 100181, 30, 990, 990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100240, '泡泡棒球队棒球帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100240, 8589934592, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100240, 102, 148, 360, '2011-10-25 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011956, 100240, 1, 183, 183, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011957, 100240, 7, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011958, 100240, 30, 1100, 1100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100413, '飞影头盔HC', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100413, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100413, 102, 228, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012230, 100413, 1, 240, 240, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012235, 100413, 7, 720, 720, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012240, 100413, 30, 1450, 1450, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016086, 100413, 15, 1450, 1450, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100414, '飞影夹克HC', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100414, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100414, 108, 229, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012231, 100414, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012236, 100414, 7, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012241, 100414, 30, 1250, 1250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016087, 100414, 15, 1250, 1250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100415, '飞影炫裤HC', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100415, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100415, 109, 230, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012232, 100415, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012237, 100415, 7, 320, 320, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012242, 100415, 30, 650, 650, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016088, 100415, 15, 650, 650, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100416, '飞影手套HC', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100416, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100416, 105, 231, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012233, 100416, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012238, 100416, 7, 320, 320, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012243, 100416, 30, 650, 650, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016089, 100416, 15, 650, 650, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100417, '飞影靴HC', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100417, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100417, 106, 232, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012234, 100417, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012239, 100417, 7, 320, 320, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012244, 100417, 30, 650, 650, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016090, 100417, 15, 650, 650, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100418, '红玫瑰HC头带', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100418, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100418, 102, 233, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012551, 100418, 1, 240, 240, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012561, 100418, 7, 720, 720, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012566, 100418, 30, 1250, 1250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015769, 100418, 15, 720, 720, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100419, '红玫瑰HC上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100419, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100419, 108, 234, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012552, 100419, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012562, 100419, 7, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012567, 100419, 30, 1050, 1050, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015770, 100419, 15, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100420, '红玫瑰HC裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100420, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100420, 109, 235, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012553, 100420, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012563, 100420, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012568, 100420, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015771, 100420, 15, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100421, '红玫瑰HC手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100421, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100421, 105, 236, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012554, 100421, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012564, 100421, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012569, 100421, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015772, 100421, 15, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100422, '红玫瑰HC鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100422, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100422, 106, 237, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012555, 100422, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012565, 100422, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012570, 100422, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015773, 100422, 15, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100433, 'BF冒险王帽HC', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100433, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100433, 102, 242, 360, '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012833, 100433, 1, 240, 240, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012843, 100433, 7, 720, 720, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012848, 100433, 30, 1250, 1250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100434, 'BF冒险王上衣HC', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100434, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100434, 108, 243, 360, '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012834, 100434, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012844, 100434, 7, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012849, 100434, 30, 1050, 1050, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100435, 'BF冒险王裤子HC', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100435, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100435, 109, 244, 360, '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012835, 100435, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012845, 100435, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012850, 100435, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100436, 'BF冒险王手套HC', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100436, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100436, 105, 245, 360, '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012836, 100436, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012846, 100436, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012851, 100436, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100437, 'BF冒险王靴HC', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100437, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100437, 106, 246, 360, '2011-03-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012837, 100437, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012847, 100437, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012852, 100437, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100449, '超级明星☆帽HC', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100449, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100449, 102, 1008, 360, '2009-01-01 00:00:00.000', '2060-01-07 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012958, 100449, 1, 240, 240, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012968, 100449, 7, 720, 720, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012973, 100449, 30, 1250, 1250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100450, '超级明星☆上衣HC', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100450, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100450, 108, 1009, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012959, 100450, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012969, 100450, 7, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012974, 100450, 30, 1050, 1050, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100451, '超级明星☆下装HC', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100451, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100451, 109, 1010, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012960, 100451, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012970, 100451, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012975, 100451, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100452, '超级明星☆手套HC', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100452, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100452, 105, 1011, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012961, 100452, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012971, 100452, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012976, 100452, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100453, '超级明星☆鞋HC', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100453, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100453, 106, 1012, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012962, 100453, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012972, 100453, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012977, 100453, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100510, 'BF学校霸王HC头型', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100510, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100510, 102, 285, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012598, 100510, 1, 220, 220, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012608, 100510, 7, 660, 660, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012613, 100510, 30, 1320, 1320, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100511, 'BF学校霸王HC上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100511, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100511, 108, 286, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012599, 100511, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012609, 100511, 7, 630, 630, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012614, 100511, 30, 1260, 1260, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100512, 'BF学校霸王HC裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100512, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100512, 109, 287, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012600, 100512, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012610, 100512, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012615, 100512, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100513, 'BF学校霸王HC手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100513, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100513, 105, 288, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012601, 100513, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012611, 100513, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012616, 100513, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100514, 'BF学校霸王HC鞋子', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100514, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100514, 106, 289, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012602, 100514, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012612, 100514, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012617, 100514, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100530, '泡泡自由HC发型', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100530, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100530, 102, 290, 360, '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012660, 100530, 1, 240, 240, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012670, 100530, 7, 720, 720, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012675, 100530, 30, 1250, 1250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100531, '泡泡自由HC上衣', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100531, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100531, 108, 291, 360, '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012661, 100531, 1, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012671, 100531, 7, 620, 620, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012676, 100531, 30, 1050, 1050, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100532, '泡泡自由HC裤子', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100532, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100532, 109, 292, 360, '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012662, 100532, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012672, 100532, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012677, 100532, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100533, '泡泡自由HC手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100533, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100533, 105, 293, 360, '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012663, 100533, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012673, 100533, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012678, 100533, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100534, '泡泡自由HC靴', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100534, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100534, 106, 294, 360, '2012-03-12 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012664, 100534, 1, 110, 110, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012674, 100534, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012679, 100534, 30, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 110072, '朋克熊头套', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 110072, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 110072, 102, 295, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011281, 110072, 1, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011282, 110072, 7, 750, 750, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011283, 110072, 30, 1300, 1300, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 110073, '朋克熊头服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 110073, 8761733283840, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 110073, 110, 296, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011284, 110073, 1, 200, 200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011285, 110073, 7, 600, 600, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011286, 110073, 30, 1200, 1200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 110101, '金星训练帽', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 110101, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 110101, 102, 297, 360, '2011-08-10 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011287, 110101, 1, 183, 183, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011288, 110101, 7, 550, 550, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011289, 110101, 30, 1100, 1100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 110103, '金星训练鞋', 42, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 110103, 4398046511104, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 110103, 106, 298, 360, '2011-08-10 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011296, 110103, 1, 83, 83, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011297, 110103, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011298, 110103, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 110129, '金星训练衣服', 39, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 110129, 652835028992, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 110129, 108, 299, 360, '2011-08-10 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011290, 110129, 1, 133, 133, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011291, 110129, 7, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011292, 110129, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 110130, '金星训练裤', 40, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 110130, 3298534883328, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 110130, 109, 300, 360, '2011-08-10 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011293, 110130, 1, 83, 83, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011294, 110130, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011295, 110130, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 119999, '闪闪五星头巾', 33, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 119999, 8589934592, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 119999, 102, 318, 360, '2011-10-11 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011759, 119999, 1, 170, 170, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011760, 119999, 7, 490, 490, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011761, 119999, 30, 990, 990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 120335, '金星训练手套', 38, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 120335, 412316860416, 703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 120335, 105, 339, 360, '2011-08-26 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011363, 120335, 1, 83, 83, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011364, 120335, 7, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011365, 120335, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201014, '棉花糖手枪L3 HC', 19, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 201014, 524288, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 201014, 305, 1120, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014332, 201014, 7, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014336, 201014, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201024, '爆烈手枪L3-HC', 19, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 201024, 524288, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 201024, 305, 1085, 360, '2013-04-30 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014284, 201024, 7, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014288, 201024, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201044, '猛兽手枪L3 HC', 19, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 201044, 524288, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 201044, 305, 1061, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014260, 201044, 7, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014264, 201044, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201075, '尖锋手枪 S3 HC', 19, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 201075, 524288, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 201075, 305, 1103, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014308, 201075, 7, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014312, 201075, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201127, '左轮手枪R3 HC', 19, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 201127, 524288, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 201127, 305, 34100, 360, '2013-06-27 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014609, 201127, 7, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014611, 201127, 30, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202016, '棉花糖步枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202016, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202016, 302, 1115, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014333, 202016, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014337, 202016, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202025, '爆烈步枪L3-HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202025, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202025, 302, 1080, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014285, 202025, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014289, 202025, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202034, '尖锋步枪S3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202034, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202034, 302, 1102, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014309, 202034, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014313, 202034, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202044, '猛兽步枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202044, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202044, 302, 1060, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014261, 202044, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014265, 202044, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202127, '左轮步枪R3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202127, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202127, 302, 2030, 360, '2013-07-16 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014484, 202127, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014485, 202127, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202321, '力之轮回步枪 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202321, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202321, 302, 2020, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013913, 202321, 7, 1880, 1880, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013914, 202321, 30, 2980, 2980, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202324, '熊猫步枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 202324, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 202324, 302, 2009, 360, '2012-11-04 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013476, 202324, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013479, 202324, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203016, '棉花糖散弹枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203016, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203016, 303, 1111, 360, '2011-08-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014334, 203016, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014338, 203016, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203024, '爆烈散弹枪L3-HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203024, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203024, 303, 1071, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014286, 203024, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014290, 203024, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203034, '尖锋散弹枪S3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203034, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203034, 303, 1101, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014310, 203034, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014314, 203034, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203044, '猛兽散弹枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203044, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203044, 303, 1051, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014262, 203044, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014266, 203044, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203127, '左轮散弹枪R3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203127, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203127, 303, 34101, 360, '2013-06-27 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014608, 203127, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014610, 203127, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203216, '熊猫散弹枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203216, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203216, 303, 2005, 360, '2012-11-04 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013477, 203216, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013480, 203216, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203336, '凯旋V-87散弹枪 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 203336, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 203336, 303, 2015, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013915, 203336, 7, 1880, 1880, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013916, 203336, 30, 2980, 2980, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204015, '棉花糖狙击枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204015, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204015, 304, 1110, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014335, 204015, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014339, 204015, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204026, '爆烈狙击枪L3-HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204026, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204026, 304, 1070, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014287, 204026, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014291, 204026, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204034, '尖锋狙击枪S3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204034, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204034, 304, 1100, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014311, 204034, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014315, 204034, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204044, '猛兽狙击枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204044, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204044, 304, 1050, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014263, 204044, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014267, 204044, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204120, '熊猫狙击枪L3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204120, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204120, 304, 2001, 360, '2012-11-04 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013478, 204120, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013481, 204120, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204127, '左轮狙击枪R3 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204127, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204127, 304, 2031, 360, '2013-07-16 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014486, 204127, 7, 1380, 1380, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014487, 204127, 30, 2580, 2580, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204336, '凯旋G-1狙击枪 HC', 18, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 204336, 262144, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 204336, 304, 2010, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013917, 204336, 7, 1880, 1880, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013918, 204336, 30, 2980, 2980, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 207028, '尖锋冲击大锤', 20, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 207028, 1048576, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 207028, 306, 599, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013198, 207028, 7, 880, 880, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013199, 207028, 30, 1980, 1980, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
INSERT INTO schema_migrations (version, name, applied_at)
SELECT 10, 'ExpandShopEquipment', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 10);
COMMIT;
