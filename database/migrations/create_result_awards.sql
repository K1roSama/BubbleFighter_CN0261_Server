USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS game_result_awards
(
    game_result_player_id BIGINT UNSIGNED NOT NULL,
    score INT UNSIGNED NOT NULL,
    grade TINYINT UNSIGNED NOT NULL,
    medal TINYINT UNSIGNED NOT NULL,
    PRIMARY KEY (game_result_player_id),
    CONSTRAINT fk_game_result_awards_game_result_player_id FOREIGN KEY (game_result_player_id)
        REFERENCES game_result_players (id) ON DELETE CASCADE,
    CONSTRAINT chk_game_result_awards_score CHECK (score <= 2147483647),
    CONSTRAINT chk_game_result_awards_grade CHECK (grade <= 8),
    CONSTRAINT chk_game_result_awards_medal CHECK (medal = 0 OR medal BETWEEN 4 AND 9)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

INSERT IGNORE INTO schema_migrations (version, name, applied_at)
VALUES (12, 'CreateResultAwards', UTC_TIMESTAMP(6));
