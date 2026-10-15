USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS character_weapon_presets
(
    character_id INT UNSIGNED NOT NULL,
    active_index TINYINT UNSIGNED NOT NULL DEFAULT 0,
    slot_count TINYINT UNSIGNED NOT NULL DEFAULT 2,
    PRIMARY KEY (character_id),
    CONSTRAINT fk_character_weapon_presets_character_id FOREIGN KEY (character_id)
        REFERENCES characters (id) ON DELETE CASCADE,
    CONSTRAINT ck_character_weapon_presets_slot_count CHECK (slot_count BETWEEN 2 AND 3),
    CONSTRAINT ck_character_weapon_presets_active_index CHECK (active_index < slot_count)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS character_weapon_preset_items
(
    character_id INT UNSIGNED NOT NULL,
    preset_index TINYINT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NULL,
    PRIMARY KEY (character_id, preset_index),
    CONSTRAINT fk_character_weapon_preset_items_character_id FOREIGN KEY (character_id)
        REFERENCES character_weapon_presets (character_id) ON DELETE CASCADE,
    CONSTRAINT fk_character_weapon_preset_items_item_id FOREIGN KEY (item_id) REFERENCES item_definitions (id),
    CONSTRAINT ck_character_weapon_preset_items_preset_index CHECK (preset_index < 3)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS shop_permanent_stocks
(
    stock_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (stock_id),
    CONSTRAINT fk_shop_permanent_stocks_stock_id FOREIGN KEY (stock_id) REFERENCES shop_stocks (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- NULL represents a permanent purchase; existing timed purchase timestamps are preserved.
ALTER TABLE shop_purchases MODIFY COLUMN expires_at DATETIME(6) NULL;

-- Original ShopItem2.xml categories and stock 1013409: type 9, amount -1, cashCN 4800.
-- Operator decision: reopen this originally disabled item after implementing its permanent 2 -> 3 slot effect.
START TRANSACTION;
INSERT INTO shop_main_categories (id, name, display_order)
SELECT 500, 'Etc', 1006 WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 501, 500, 'All_Etc', 1400, 2, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 502, 500, 'Spray', 1401, 0, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 503, 500, 'couple', 1402, 0, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 504, 500, 'synthetic', 1403, 0, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type, is_visible)
SELECT 505, 500, 'Followers', 1404, 0, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 500001, '武器切换扩张卡', 0, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_items
    (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 500001, 501, 300279, 36500, '2009-01-01 00:00:00', '2060-01-01 00:00:00', TRUE, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013409, 500001, 36500, 4800, 4800, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO shop_permanent_stocks (stock_id)
SELECT 1013409 WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
INSERT INTO schema_migrations (version, name, applied_at)
SELECT 13, 'CreateWeaponPresets', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 13);
COMMIT;
