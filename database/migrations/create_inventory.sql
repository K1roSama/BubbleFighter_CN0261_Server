USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS item_definitions
(
    id INT UNSIGNED NOT NULL,
    name VARCHAR(64) NOT NULL,
    equipment_slot INT UNSIGNED NOT NULL,
    starter_equipped BOOLEAN NOT NULL,
    PRIMARY KEY (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Verified CN0261 ShopItem / SlotLocation identifiers. Grants are the server's starter policy.
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 201001, '练习手枪', 19, TRUE
WHERE NOT EXISTS (SELECT 1 FROM item_definitions WHERE id = 201001);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 202001, '练习步枪', 18, TRUE
WHERE NOT EXISTS (SELECT 1 FROM item_definitions WHERE id = 202001);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 203001, '练习散弹枪', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM item_definitions WHERE id = 203001);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 204001, '练习狙击枪', 18, FALSE
WHERE NOT EXISTS (SELECT 1 FROM item_definitions WHERE id = 204001);
INSERT INTO item_definitions (id, name, equipment_slot, starter_equipped)
SELECT 207001, '练习大锤', 20, TRUE
WHERE NOT EXISTS (SELECT 1 FROM item_definitions WHERE id = 207001);

CREATE TABLE IF NOT EXISTS character_items
(
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    character_id INT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL,
    quantity INT UNSIGNED NOT NULL DEFAULT 1,
    equipped_slot INT UNSIGNED NULL,
    created_at DATETIME(6) NOT NULL,
    updated_at DATETIME(6) NOT NULL,
    PRIMARY KEY (id),
    UNIQUE KEY uk_character_items_character_id_item_id (character_id, item_id),
    UNIQUE KEY uk_character_items_character_id_equipped_slot (character_id, equipped_slot),
    KEY idx_character_items_item_id (item_id),
    CONSTRAINT fk_character_items_character_id FOREIGN KEY (character_id) REFERENCES characters (id),
    CONSTRAINT fk_character_items_item_id FOREIGN KEY (item_id) REFERENCES item_definitions (id),
    CONSTRAINT ck_character_items_quantity CHECK (quantity > 0)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Backfill V3 characters once. Re-running V4 must not restore deleted or unequipped items.
START TRANSACTION;
INSERT INTO character_items (character_id, item_id, quantity, equipped_slot, created_at, updated_at)
SELECT characters.id, item_definitions.id, 1,
    CASE WHEN item_definitions.starter_equipped THEN item_definitions.equipment_slot ELSE NULL END,
    UTC_TIMESTAMP(6), UTC_TIMESTAMP(6)
FROM characters CROSS JOIN item_definitions
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 4);

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 4, 'CreateInventory', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 4);
COMMIT;
