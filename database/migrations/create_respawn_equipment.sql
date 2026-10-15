USE bubblefighter_cn0261;

-- Original cashCN prices, dates and character compatibility.
-- No cash is granted. Only the verified ordinary quick respawn necklace is enabled.
START TRANSACTION;
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100241, '快速复活项链', 14, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 100241, 16384, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100241, 204, 149, 360, '2011-10-25 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011974, 100241, 1, 70, 70, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011975, 100241, 7, 210, 210, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011976, 100241, 30, 420, 420, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016073, 100241, 3, 420, 420, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
INSERT INTO schema_migrations (version, name, applied_at)
SELECT 11, 'CreateRespawnEquipment', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 11);
COMMIT;
