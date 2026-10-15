USE bubblefighter_cn0261;

CREATE TABLE IF NOT EXISTS character_progression
(
    character_id INT UNSIGNED NOT NULL,
    experience INT UNSIGNED NOT NULL DEFAULT 0,
    updated_at DATETIME(6) NOT NULL,
    PRIMARY KEY (character_id),
    CONSTRAINT fk_character_progression_character_id FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE,
    CONSTRAINT chk_character_progression_experience CHECK (experience <= 2147483647)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

INSERT INTO character_progression (character_id, experience, updated_at)
SELECT characters.id, 0, UTC_TIMESTAMP(6)
FROM characters LEFT JOIN character_progression ON character_progression.character_id = characters.id
WHERE character_progression.character_id IS NULL;

CREATE TABLE IF NOT EXISTS game_results
(
    id CHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    room_id SMALLINT UNSIGNED NOT NULL,
    tracking_id INT UNSIGNED NOT NULL,
    mode INT UNSIGNED NOT NULL,
    sub_mode INT UNSIGNED NOT NULL,
    map_name VARCHAR(128) NOT NULL,
    end_reason TINYINT UNSIGNED NOT NULL,
    first_team_score SMALLINT UNSIGNED NOT NULL,
    second_team_score SMALLINT UNSIGNED NOT NULL,
    winner TINYINT UNSIGNED NOT NULL,
    policy_version VARCHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    completed_at DATETIME(6) NOT NULL,
    PRIMARY KEY (id),
    CONSTRAINT chk_game_results_end_reason CHECK (end_reason IN (1, 2)),
    CONSTRAINT chk_game_results_winner CHECK (winner <= 2),
    CONSTRAINT chk_game_results_first_team_score CHECK (first_team_score <= 200),
    CONSTRAINT chk_game_results_second_team_score CHECK (second_team_score <= 200)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE IF NOT EXISTS game_result_players
(
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    game_result_id CHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    slot TINYINT UNSIGNED NOT NULL,
    character_id INT UNSIGNED NOT NULL,
    account_id INT UNSIGNED NOT NULL,
    nickname VARCHAR(12) NOT NULL,
    team TINYINT UNSIGNED NOT NULL,
    bubble_count SMALLINT UNSIGNED NOT NULL,
    pang_count SMALLINT UNSIGNED NOT NULL,
    rescue_count SMALLINT UNSIGNED NOT NULL,
    experience_before INT UNSIGNED NOT NULL,
    experience_after INT UNSIGNED NOT NULL,
    lucci_before INT UNSIGNED NOT NULL,
    lucci_after INT UNSIGNED NOT NULL,
    cash INT UNSIGNED NOT NULL,
    experience_gain INT UNSIGNED NOT NULL,
    lucci_gain INT UNSIGNED NOT NULL,
    level INT UNSIGNED NOT NULL,
    level_start INT UNSIGNED NOT NULL,
    next_level INT UNSIGNED NOT NULL,
    level_before INT UNSIGNED NOT NULL,
    PRIMARY KEY (id),
    UNIQUE KEY uk_game_result_players_game_result_id_slot (game_result_id, slot),
    UNIQUE KEY uk_game_result_players_game_result_id_character_id (game_result_id, character_id),
    KEY idx_game_result_players_character_id (character_id),
    CONSTRAINT fk_game_result_players_game_result_id FOREIGN KEY (game_result_id) REFERENCES game_results (id)
        ON DELETE CASCADE,
    CONSTRAINT fk_game_result_players_character_id FOREIGN KEY (character_id) REFERENCES characters (id),
    CONSTRAINT chk_game_result_players_slot CHECK (slot < 8),
    CONSTRAINT chk_game_result_players_team CHECK (team IN (1, 2)),
    CONSTRAINT chk_game_result_players_experience_after CHECK (experience_after <= 2147483647),
    CONSTRAINT chk_game_result_players_lucci_after CHECK (lucci_after <= 2147483647),
    CONSTRAINT chk_game_result_players_cash CHECK (cash <= 2147483647),
    CONSTRAINT chk_game_result_players_experience_gain CHECK (experience_before + experience_gain = experience_after),
    CONSTRAINT chk_game_result_players_lucci_gain CHECK (lucci_before + lucci_gain = lucci_after),
    CONSTRAINT chk_game_result_players_level CHECK (level BETWEEN 1 AND 200),
    CONSTRAINT chk_game_result_players_level_start CHECK (level_start <= experience_after),
    CONSTRAINT chk_game_result_players_next_level CHECK (next_level >= experience_after)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci;

INSERT IGNORE INTO schema_migrations (version, name, applied_at)
VALUES (7, 'CreateGameResults', UTC_TIMESTAMP(6));
