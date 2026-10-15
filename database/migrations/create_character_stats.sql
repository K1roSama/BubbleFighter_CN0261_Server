USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS character_stats
(
    character_id INT UNSIGNED NOT NULL,
    lucci INT UNSIGNED NOT NULL DEFAULT 10000,
    cash INT UNSIGNED NOT NULL DEFAULT 0,
    updated_at DATETIME(6) NOT NULL,
    PRIMARY KEY (character_id),
    CONSTRAINT fk_character_stats_character_id FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE,
    CONSTRAINT ck_character_stats_lucci CHECK (lucci <= 2147483647),
    CONSTRAINT ck_character_stats_cash CHECK (cash <= 2147483647)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

-- Initial balances are the self-hosted server policy, not original operator rewards.
START TRANSACTION;
INSERT INTO character_stats (character_id, lucci, cash, updated_at)
SELECT characters.id, 10000, 0, UTC_TIMESTAMP(6)
FROM characters
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 5)
    AND NOT EXISTS (SELECT 1 FROM character_stats WHERE character_stats.character_id = characters.id);

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 5, 'CreateCharacterStats', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 5);
COMMIT;
