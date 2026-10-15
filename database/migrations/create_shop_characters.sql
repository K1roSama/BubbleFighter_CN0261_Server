USE bubblefighter_cn0261;

SET @migration_statement = IF(
    EXISTS (SELECT 1 FROM information_schema.columns
        WHERE table_schema = DATABASE() AND table_name = 'shop_items' AND column_name = 'currency'),
    'SELECT 1', 'ALTER TABLE shop_items ADD COLUMN currency VARCHAR(8) NOT NULL DEFAULT ''lucci''');
PREPARE migration_command FROM @migration_statement;
EXECUTE migration_command;
DEALLOCATE PREPARE migration_command;

SET @migration_statement = IF(
    EXISTS (SELECT 1 FROM information_schema.columns
        WHERE table_schema = DATABASE() AND table_name = 'shop_purchases' AND column_name = 'cash_spent'),
    'SELECT 1', 'ALTER TABLE shop_purchases ADD COLUMN cash_spent INT UNSIGNED NOT NULL DEFAULT 0');
PREPARE migration_command FROM @migration_statement;
EXECUTE migration_command;
DEALLOCATE PREPARE migration_command;

SET @migration_statement = IF(
    EXISTS (SELECT 1 FROM information_schema.columns
        WHERE table_schema = DATABASE() AND table_name = 'shop_sub_categories' AND column_name = 'is_visible'),
    'SELECT 1', 'ALTER TABLE shop_sub_categories ADD COLUMN is_visible BOOLEAN NOT NULL DEFAULT TRUE');
PREPARE migration_command FROM @migration_statement;
EXECUTE migration_command;
DEALLOCATE PREPARE migration_command;

CREATE TABLE IF NOT EXISTS character_appearances
(
    item_id INT UNSIGNED NOT NULL,
    character_type INT UNSIGNED NOT NULL,
    PRIMARY KEY (item_id),
    UNIQUE KEY uk_character_appearances_character_type (character_type),
    CONSTRAINT fk_character_appearances_item_id FOREIGN KEY (item_id) REFERENCES item_definitions (id),
    CONSTRAINT ck_character_appearances_character_type CHECK (character_type BETWEEN 1 AND 32)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Slot 63 is internal ownership state, outside original socket/part domains; never sent as a slot.
-- Original cashCN prices and dates are retained. This migration does not grant cash.
START TRANSACTION;
INSERT INTO shop_main_categories (id, name, display_order)
SELECT 400, 'Characters', 1003
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 401, 400, 'All_Characters', 1307, 2, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 402, 400, 'Characters', 1308, 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300001, '皮蛋', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300001, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300001, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300002, '黑妞', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300002, 2
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300002, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300003, '宝宝', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300003, 3
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300003, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300004, '胖墩', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300004, 4
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300004, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300005, '小乖', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300005, 5
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300005, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300006, '小强', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300006, 6
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300006, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300008, '蓝蓝', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300008, 8
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300008, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300010, '蓝妹妹', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300010, 10
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300010, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300011, '雷欧', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300011, 11
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300011, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300012, '拉比', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300012, 12
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300012, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 300014, '杰克将军', 63, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO character_appearances (item_id, character_type)
SELECT 300014, 14
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO item_equipment_rules (item_id, occupied_slots, character_mask)
SELECT 300014, 9223372036854775808, 11967
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300003, 402, 525, 360, '2011-09-27 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011587, 300003, 1, 300, 300, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011588, 300003, 7, 900, 900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011589, 300003, 30, 1800, 1800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011641, 300003, 3, 300, 300, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300004, 402, 526, 360, '2011-11-02 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012304, 300004, 1, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012305, 300004, 7, 1200, 1200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012306, 300004, 30, 2400, 2400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300005, 402, 527, 360, '2011-11-29 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011780, 300005, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011781, 300005, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011782, 300005, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300006, 402, 528, 360, '2011-12-02 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012078, 300006, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012079, 300006, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012080, 300006, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300008, 402, 530, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012576, 300008, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012578, 300008, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012579, 300008, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300010, 402, 1000, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013226, 300010, 1, 800, 800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013227, 300010, 7, 1900, 1900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013228, 300010, 30, 3900, 3900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300011, 402, 2001, 360, '2013-08-20 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014744, 300011, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014745, 300011, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014746, 300011, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300012, 402, 1500, 360, '2013-02-08 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013696, 300012, 1, 990, 990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013697, 300012, 7, 2390, 2390, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013698, 300012, 30, 4990, 4990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 300014, 402, 2000, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014091, 300014, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014092, 300014, 7, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014093, 300014, 30, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100043
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100044
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100046
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100057
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100062
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100063
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100072
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100073
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100076
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100077
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100078
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100086
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100096
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100106
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100111
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100125
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100126
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100128
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100140
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100141
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100143
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100144
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100145
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100173
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100174
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100175
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100176
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100177
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100182
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 100242
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 1727 WHERE item_id = 100290
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 1727 WHERE item_id = 100291
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 1727 WHERE item_id = 100292
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 1727 WHERE item_id = 100293
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 1727 WHERE item_id = 100294
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100330
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100331
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100332
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100333
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 100334
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120101
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120103
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120129
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120130
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120156
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120157
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120158
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120159
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120160
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120232
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120233
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120234
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120235
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120236
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120237
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120238
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120239
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120240
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 703 WHERE item_id = 120241
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201001
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201013
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201023
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201033
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201043
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201073
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201119
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201120
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201121
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 201122
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202001
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202014
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202024
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202033
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202043
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202072
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202329
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202330
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202331
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202332
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 202341
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203001
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203015
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203023
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203033
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203043
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203072
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203223
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203224
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203225
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203226
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 203341
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204001
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204014
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204025
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204033
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204043
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204053
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204123
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204124
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204125
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204126
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 204341
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 207001
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 207013
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 207016
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
UPDATE item_equipment_rules SET character_mask = 11967 WHERE item_id = 207026
AND NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);

-- Preserve the creation choice as the permanent fallback; do not grant the other base character.
INSERT INTO character_items (character_id, item_id, quantity, equipped_slot, created_at, updated_at)
SELECT characters.id, character_appearances.item_id, 1, 63, UTC_TIMESTAMP(6), UTC_TIMESTAMP(6)
FROM characters JOIN character_appearances
ON character_appearances.character_type = characters.character_type
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 9, 'CreateShopCharacters', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 9);
COMMIT;
