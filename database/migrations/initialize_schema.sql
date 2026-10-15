CREATE DATABASE IF NOT EXISTS bubblefighter_cn0261
    CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;

USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS schema_migrations
(
    version INT UNSIGNED NOT NULL,
    name VARCHAR(128) NOT NULL,
    applied_at DATETIME(6) NOT NULL,
    PRIMARY KEY (version)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 1, 'InitializeSchema', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 1);
