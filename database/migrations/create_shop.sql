USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS shop_main_categories
(
    id INT UNSIGNED NOT NULL,
    name VARCHAR(64) NOT NULL,
    display_order INT UNSIGNED NOT NULL,
    PRIMARY KEY (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS shop_sub_categories
(
    id INT UNSIGNED NOT NULL,
    main_category_id INT UNSIGNED NOT NULL,
    name VARCHAR(64) NOT NULL,
    display_order INT UNSIGNED NOT NULL,
    page_type INT UNSIGNED NOT NULL,
    PRIMARY KEY (id),
    CONSTRAINT fk_shop_sub_categories_main_category_id FOREIGN KEY (main_category_id) REFERENCES shop_main_categories (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS shop_items
(
    item_id INT UNSIGNED NOT NULL,
    sub_category_id INT UNSIGNED NOT NULL,
    display_order INT UNSIGNED NOT NULL,
    max_duration_days INT UNSIGNED NOT NULL,
    sale_starts_at DATETIME(6) NOT NULL,
    sale_ends_at DATETIME(6) NOT NULL,
    is_on_sale BOOLEAN NOT NULL,
    PRIMARY KEY (item_id),
    CONSTRAINT fk_shop_items_item_id FOREIGN KEY (item_id) REFERENCES item_definitions (id),
    CONSTRAINT fk_shop_items_sub_category_id FOREIGN KEY (sub_category_id) REFERENCES shop_sub_categories (id),
    CONSTRAINT ck_shop_items_max_duration_days CHECK (max_duration_days BETWEEN 1 AND 36500),
    CONSTRAINT ck_shop_items_sale_ends_at CHECK (sale_ends_at > sale_starts_at)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS shop_stocks
(
    id INT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL,
    duration_days INT UNSIGNED NOT NULL,
    show_price INT UNSIGNED NOT NULL,
    sell_price INT UNSIGNED NOT NULL,
    is_on_sale BOOLEAN NOT NULL,
    PRIMARY KEY (id),
    CONSTRAINT fk_shop_stocks_item_id FOREIGN KEY (item_id) REFERENCES shop_items (item_id),
    CONSTRAINT ck_shop_stocks_duration_days CHECK (duration_days BETWEEN 1 AND 36500),
    CONSTRAINT ck_shop_stocks_show_price CHECK (show_price <= 2147483647),
    CONSTRAINT ck_shop_stocks_sell_price CHECK (sell_price BETWEEN 1 AND 2147483647)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS character_item_expirations
(
    character_item_id BIGINT UNSIGNED NOT NULL,
    expires_at DATETIME(6) NOT NULL,
    PRIMARY KEY (character_item_id),
    CONSTRAINT fk_character_item_expirations_character_item_id FOREIGN KEY (character_item_id)
        REFERENCES character_items (id) ON DELETE CASCADE
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS shop_purchases
(
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    character_id INT UNSIGNED NOT NULL,
    stock_id INT UNSIGNED NOT NULL,
    lucci_spent INT UNSIGNED NOT NULL,
    expires_at DATETIME(6) NOT NULL,
    created_at DATETIME(6) NOT NULL,
    PRIMARY KEY (id),
    KEY idx_shop_purchases_character_id (character_id),
    CONSTRAINT fk_shop_purchases_character_id FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE,
    CONSTRAINT fk_shop_purchases_stock_id FOREIGN KEY (stock_id) REFERENCES shop_stocks (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Verified ShopItem2.xml SHA-256: 06cab10e98ae706ae9f00f45c2a1d58e52c302831e40da598e94f977a4521574
-- 42 lucci weapon items / 104 stock options. Original sale dates are preserved.
START TRANSACTION;
INSERT INTO shop_main_categories (id, name, display_order)
SELECT 300, 'Weapons', 1002
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 300, 300, 'All_Weapon', 1300, 2
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 302, 300, 'Weapon_Rifle', 1301, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 303, 300, 'Weapon_ShotGun', 1302, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 304, 300, 'Weapon_Sniper', 1303, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 305, 300, 'SubWeapon', 1304, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 306, 300, 'MeleeWeapon', 1305, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_sub_categories (id, main_category_id, name, display_order, page_type)
SELECT 307, 300, 'SpecialWeapon', 1306, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201013, '棉花糖手枪L3', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201013, 305, 1020, 360, '2011-08-24 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201023, '爆烈手枪L3', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201023, 305, 1007, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201033, '左轮手枪R3', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201033, 305, 1025, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201043, '猛兽手枪L3', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201043, 305, 1003, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201073, '尖锋手枪S3', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201073, 305, 1013, 360, '2011-12-29 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201119, '爆烈手枪L3 MC', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201119, 305, 1003, 360, '2012-11-29 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201120, '棉花糖手枪L3 MC', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201120, 305, 1303, 360, '2015-01-20 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201121, '尖峰手枪S3 MC', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201121, 305, 1103, 360, '2012-11-29 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201122, '猛兽手枪L3 MC', 19, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 201122, 305, 50008, 360, '2014-12-16 00:00:00.000', '2014-12-23 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202014, '棉花糖步枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202014, 302, 1017, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202024, '爆烈步枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202024, 302, 1006, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202033, '尖峰步枪 S3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202033, 302, 1012, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202043, '猛兽步枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202043, 302, 1002, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202072, '左轮步枪R3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202072, 302, 1024, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202329, '爆烈步枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202329, 302, 1002, 360, '2012-11-29 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202330, '棉花糖步枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202330, 302, 1302, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202331, '尖峰步枪S3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202331, 302, 1102, 360, '2012-01-02 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202332, '猛兽步枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202332, 302, 50006, 360, '2011-11-03 00:00:00.000', '2014-12-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202341, '左轮步枪R3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 202341, 302, 1402, 360, '2013-06-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203015, '棉花糖散弹枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203015, 303, 1016, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203023, '爆烈散弹枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203023, 303, 1005, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203033, '尖锋散弹枪 S3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203033, 303, 1011, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203043, '猛兽散弹枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203043, 303, 1001, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203072, '左轮散弹枪R3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203072, 303, 1023, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203223, '爆烈散弹枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203223, 303, 1001, 360, '2015-02-03 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203224, '棉花糖散弹枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203224, 303, 1301, 360, '2011-08-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203225, '尖峰散弹枪S3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203225, 303, 1101, 360, '2012-01-12 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203226, '猛兽散弹枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203226, 303, 50005, 360, '2014-12-02 00:00:00.000', '2014-12-08 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203341, '左轮散弹枪R3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 203341, 303, 1401, 1000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204014, '棉花糖狙击枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204014, 304, 1015, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204025, '爆烈狙击枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204025, 304, 1004, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204033, '尖峰狙击枪 S3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204033, 304, 1010, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204043, '猛兽狙击枪L3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204043, 304, 1000, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204053, '左轮狙击枪R3', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204053, 304, 1022, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204123, '爆烈狙击枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204123, 304, 1000, 360, '2015-02-10 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204124, '棉花糖狙击枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204124, 304, 1300, 360, '2015-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204125, '尖峰狙击枪S3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204125, 304, 1100, 360, '2012-01-16 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204126, '猛兽狙击枪L3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204126, 304, 50007, 360, '2011-11-21 00:00:00.000', '2014-12-15 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204341, '左轮狙击枪R3 MC', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 204341, 304, 1400, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 207013, '砰砰大锤', 20, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 207013, 306, 502, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 207016, '钢铁力量之锤', 20, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 207016, 306, 503, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 207026, '棒棒大锤', 20, FALSE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale)
SELECT 207026, 306, 510, 360, '2011-11-02 00:00:00.000', '2060-01-01 00:00:00.000', TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011247, 207013, 7, 300, 300, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011248, 207013, 30, 600, 600, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011253, 207016, 7, 600, 600, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011254, 207016, 30, 1200, 1200, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012302, 207026, 7, 600, 600, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012303, 207026, 30, 1200, 1200, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014244, 201043, 1, 1000, 1000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014245, 202043, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014246, 203043, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014247, 204043, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014248, 201043, 7, 3000, 3000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014249, 202043, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014250, 203043, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014251, 204043, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014252, 201043, 30, 6000, 6000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014253, 202043, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014254, 203043, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014255, 204043, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014268, 201023, 1, 1000, 1000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014269, 202024, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014270, 203023, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014271, 204025, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014272, 201023, 7, 3000, 3000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014273, 202024, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014274, 203023, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014275, 204025, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014276, 201023, 30, 6000, 6000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014277, 202024, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014278, 203023, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014279, 204025, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014292, 201073, 1, 1000, 1000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014293, 202033, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014294, 203033, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014295, 204033, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014296, 201073, 7, 3000, 3000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014297, 202033, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014298, 203033, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014299, 204033, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014300, 201073, 30, 6000, 6000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014301, 202033, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014302, 203033, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014303, 204033, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014316, 201013, 1, 1000, 1000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014317, 202014, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014318, 203015, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014319, 204014, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014320, 201013, 7, 3000, 3000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014321, 202014, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014322, 203015, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014323, 204014, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014324, 201013, 30, 6000, 6000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014325, 202014, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014326, 203015, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014327, 204014, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014937, 201033, 1, 1000, 1000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014938, 202072, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014939, 203072, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014940, 204053, 1, 1500, 1500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014941, 201033, 7, 3000, 3000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014942, 202072, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014943, 203072, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014944, 204053, 7, 4500, 4500, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014945, 201033, 30, 6000, 6000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014946, 202072, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014947, 203072, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014948, 204053, 30, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016379, 203226, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016380, 203226, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016381, 202332, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016382, 202332, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016417, 204126, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016418, 204126, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016419, 201122, 1, 6400, 6400, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016420, 201122, 7, 15200, 15200, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016440, 202330, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016441, 202330, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016442, 204124, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016443, 204124, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016502, 201120, 1, 6400, 6400, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016503, 201120, 7, 15200, 15200, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016504, 203224, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016505, 203224, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016594, 202329, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016595, 202329, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016596, 203223, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016597, 203223, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016598, 204123, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016599, 204123, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016694, 201119, 1, 6400, 6400, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016695, 201119, 7, 15200, 15200, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016696, 202331, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016697, 202331, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016701, 203225, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016702, 203225, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016805, 204125, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016806, 204125, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016818, 201121, 1, 6400, 6400, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016819, 201121, 7, 15200, 15200, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016820, 202341, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016821, 202341, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016822, 203341, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016823, 203341, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016824, 204341, 1, 9000, 9000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016825, 204341, 7, 22000, 22000, TRUE
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 6, 'CreateShop', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 6);
COMMIT;
