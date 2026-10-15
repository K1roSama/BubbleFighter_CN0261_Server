USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS characters
(
    id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    account_id INT UNSIGNED NOT NULL,
    nickname VARCHAR(12) NOT NULL,
    character_type INT UNSIGNED NOT NULL,
    starter_selection INT UNSIGNED NOT NULL,
    created_at DATETIME(6) NOT NULL,
    updated_at DATETIME(6) NOT NULL,
    PRIMARY KEY (id),
    UNIQUE KEY uk_characters_account_id (account_id),
    UNIQUE KEY uk_characters_nickname (nickname),
    CONSTRAINT fk_characters_account_id FOREIGN KEY (account_id) REFERENCES accounts (id),
    CONSTRAINT ck_characters_starter_selection CHECK
        (character_type IN (1, 2) AND starter_selection = character_type)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 3, 'CreateCharacters', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 3);
