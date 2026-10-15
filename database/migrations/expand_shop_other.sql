USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS shop_preview_items
(
    item_id INT UNSIGNED NOT NULL,
    quantity_unit ENUM('day', 'count') NOT NULL,
    PRIMARY KEY (item_id),
    CONSTRAINT fk_shop_preview_items_item_id FOREIGN KEY (item_id) REFERENCES shop_items (item_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Display-only items: original prices and quantities; no purchase or gameplay effect.
-- For these entries duration_days carries the original amount; resource unit controls the UI.
START TRANSACTION;
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100064, '基本蓝色喷剂', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100064, 502, 20, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'lucci'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100064, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011411, 100064, 1, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100065, '基本红色喷剂', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100065, 502, 21, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'lucci'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100065, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011412, 100065, 1, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100080, '鲜橙黄色喷剂', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100080, 502, 30, 10, '2011-08-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100080, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011332, 100080, 1, 900, 900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100082, '暗影灰色喷剂', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100082, 502, 32, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100082, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011346, 100082, 1, 900, 900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100083, '经验值+50% 奖励卡片', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100083, 501, 40, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100083, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011337, 100083, 1, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011338, 100083, 7, 900, 900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011339, 100083, 30, 1800, 1800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011646, 100083, 3, 400, 400, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100090, '战绩初始化卡片', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100090, 501, 41, 10, '2011-09-06 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100090, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011636, 100090, 1, 1200, 1200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100091, '逃跑初始化卡片', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100091, 501, 42, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100091, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015237, 100091, 1, 3990, 3990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100104, '泡泡战士更名卡片', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100104, 501, 43, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100104, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1012215, 100104, 1, 2800, 2800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100226, '金币 +50% 奖励卡', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100226, 501, 44, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100226, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014099, 100226, 1, 100, 100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014100, 100226, 7, 200, 200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014101, 100226, 30, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100251, '小喇叭', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100251, 501, 50, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100251, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011431, 100251, 1, 50, 50, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011432, 100251, 50, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100252, '大喇叭', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100252, 501, 51, 10000, '2011-09-09 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100252, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011628, 100252, 1, 200, 200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1011629, 100252, 50, 10000, 10000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100484, '时光机', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100484, 501, 45, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100484, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013040, 100484, 25, 1200, 1200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100485, '家族VIP对战券', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100485, 501, 2800, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100485, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016360, 100485, 10, 300, 300, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016361, 100485, 100, 2700, 2700, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100547, '大喇叭（绿）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100547, 501, 1000, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100547, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013077, 100547, 1, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013078, 100547, 50, 12000, 12000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100548, '大喇叭（红）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100548, 501, 1001, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100548, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013079, 100548, 1, 250, 250, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013080, 100548, 50, 12000, 12000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100604, '桃心戒指', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100604, 503, 300255, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100604, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013261, 100604, 36500, 5900, 5900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_permanent_stocks (stock_id)
SELECT 1013261
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100622, '合成技能卡', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100622, 504, 300278, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100622, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013374, 100622, 10, 4500, 4500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013382, 100622, 1, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100623, '分解技能卡', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100623, 504, 300279, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100623, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013375, 100623, 10, 900, 900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013383, 100623, 1, 100, 100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100624, '永久魔法剂（大量）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100624, 504, 300280, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100624, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013376, 100624, 5, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013384, 100624, 1, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100625, '永久魔法剂（中量）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100625, 504, 300281, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100625, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013377, 100625, 5, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013385, 100625, 1, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100626, '永久魔法剂（少量）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100626, 504, 300282, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100626, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013378, 100626, 5, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013386, 100626, 1, 200, 200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100627, '极品魔法剂（大量）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100627, 504, 300283, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100627, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013379, 100627, 5, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013387, 100627, 1, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100628, '极品魔法剂（中量）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100628, 504, 300284, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100628, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013380, 100628, 5, 2500, 2500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013388, 100628, 1, 500, 500, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100629, '极品魔法剂（少量）', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100629, 504, 300285, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100629, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013381, 100629, 5, 1000, 1000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013389, 100629, 1, 200, 200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100641, '武器切换', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100641, 501, 46, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100641, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013410, 100641, 10, 100, 100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013411, 100641, 50, 490, 490, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100643, '普通宠物粮', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100643, 505, 400000, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'lucci'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100643, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013584, 100643, 1, 5000, 5000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100644, '高级宠物粮', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100644, 505, 400001, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100644, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013585, 100644, 1, 3990, 3990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100652, '宠物重生剂', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100652, 505, 400002, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100652, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013599, 100652, 1, 990, 990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100819, '家族创建卡', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100819, 501, 2900012, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100819, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015324, 100819, 1, 100, 100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 100984, '天使情侣钻戒', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 100984, 503, 2900015, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 100984, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015581, 100984, 36500, 19900, 19900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_permanent_stocks (stock_id)
SELECT 1015581
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 101034, '绿野仙踪戒指', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 101034, 503, 1999, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 101034, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015703, 101034, 36500, 1990, 1990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_permanent_stocks (stock_id)
SELECT 1015703
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 101050, '家族徽章兑换券', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 101050, 501, 2801, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 101050, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016362, 101050, 1, 2900, 2900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 101062, '家族扩张券', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 101062, 501, 2802, 10, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 101062, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016363, 101062, 1, 990, 990, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 101164, '赏金卡', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 101164, 501, 2803, 10000, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 101164, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016444, 101164, 1, 100, 100, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1016445, 101164, 10, 900, 900, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 190074, '红橙喇叭', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 190074, 501, 2900015, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 190074, 'count'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015457, 190074, 1, 200, 200, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015458, 190074, 10, 2000, 2000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1015459, 190074, 50, 10000, 10000, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 301001, '黑衣绅士猫', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 301001, 505, 400000, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 301001, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013586, 301001, 6, 4800, 4800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 301003, '羞羞蛇', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 301003, 505, 400002, 360, '2013-02-08 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 301003, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1013695, 301003, 6, 6800, 6800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 301004, '沙皮狗', 0, 0
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_items (item_id, sub_category_id, display_order, max_duration_days, sale_starts_at, sale_ends_at, is_on_sale, currency)
SELECT 301004, 505, 400003, 360, '2009-01-01 00:00:00.000', '2060-01-01 00:00:00.000', 1, 'cash_cn'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_preview_items (item_id, quantity_unit)
SELECT 301004, 'day'
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO shop_stocks (id, item_id, duration_days, show_price, sell_price, is_on_sale)
SELECT 1014418, 301004, 6, 5800, 5800, 1
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
INSERT INTO schema_migrations (version, name, applied_at)
SELECT 14, 'ExpandShopOther', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 14);
COMMIT;
