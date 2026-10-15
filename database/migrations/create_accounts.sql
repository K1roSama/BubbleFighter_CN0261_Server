USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS accounts
(
    id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    account_name VARCHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    password_scheme VARCHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    password_salt CHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    password_hash CHAR(64) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    disabled BOOLEAN NOT NULL DEFAULT FALSE,
    created_at DATETIME(6) NOT NULL,
    updated_at DATETIME(6) NOT NULL,
    PRIMARY KEY (id),
    UNIQUE KEY uk_accounts_account_name (account_name)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS login_tickets
(
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    account_id INT UNSIGNED NOT NULL,
    token_hash CHAR(64) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    expires_at DATETIME(6) NOT NULL,
    consumed_at DATETIME(6) NULL,
    created_at DATETIME(6) NOT NULL,
    PRIMARY KEY (id),
    UNIQUE KEY uk_login_tickets_token_hash (token_hash),
    KEY idx_login_tickets_account_id (account_id),
    KEY idx_login_tickets_expires_at (expires_at),
    CONSTRAINT fk_login_tickets_account_id FOREIGN KEY (account_id) REFERENCES accounts (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

INSERT INTO schema_migrations (version, name, applied_at)
SELECT 2, 'CreateAccounts', UTC_TIMESTAMP(6)
WHERE NOT EXISTS (SELECT 1 FROM schema_migrations WHERE version = 2);
