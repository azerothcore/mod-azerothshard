-- TIER 3 QUESTS
-- DRUID
UPDATE quest_template_addon SET AllowableClasses = 1024 WHERE `id` BETWEEN 9086 AND 9093;
DELETE FROM disables WHERE entry BETWEEN 9086 AND 9093;
DELETE FROM creature_questender WHERE quest BETWEEN 9086 AND 9093;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9086 AND 9093;
INSERT INTO creature_questender (id, quest) VALUES 
(16135, 9086),
(16135, 9087),
(16135, 9088),
(16135, 9089),
(16135, 9090),
(16135, 9091),
(16135, 9092),
(16135, 9093);
INSERT INTO creature_queststarter (id, quest) VALUES
(16135, 9086),
(16135, 9087),
(16135, 9088),
(16135, 9089),
(16135, 9090),
(16135, 9091),
(16135, 9092),
(16135, 9093);

-- HUNTER
UPDATE quest_template_addon SET AllowableClasses = 4 WHERE id BETWEEN 9054 AND 9061;
DELETE FROM disables WHERE entry BETWEEN 9054 AND 9061;
DELETE FROM creature_questender WHERE quest BETWEEN 9054 AND 9061;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9054 AND 9061;
INSERT INTO creature_questender (id, quest) VALUES 
(16132, 9054),
(16132, 9055),
(16132, 9056),
(16132, 9057),
(16132, 9058),
(16132, 9059),
(16132, 9060),
(16132, 9061);
INSERT INTO creature_queststarter (id, quest) VALUES
(16132, 9054),
(16132, 9055),
(16132, 9056),
(16132, 9057),
(16132, 9058),
(16132, 9059),
(16132, 9060),
(16132, 9061);

-- MAGE
UPDATE quest_template_addon SET AllowableClasses = 128 WHERE id BETWEEN 9095 AND 9102;
DELETE FROM disables WHERE entry BETWEEN 9095 AND 9102;
DELETE FROM creature_questender WHERE quest BETWEEN 9095 AND 9102;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9095 AND 9102;
INSERT INTO creature_questender (id, quest) VALUES 
(16116, 9095),
(16116, 9096),
(16116, 9097),
(16116, 9098),
(16116, 9099),
(16116, 9100),
(16116, 9101),
(16116, 9102);
INSERT INTO creature_queststarter (id, quest) VALUES
(16116, 9095),
(16116, 9096),
(16116, 9097),
(16116, 9098),
(16116, 9099),
(16116, 9100),
(16116, 9101),
(16116, 9102);

-- PALADIN
UPDATE quest_template_addon SET AllowableClasses = 2 WHERE id BETWEEN 9043 AND 9050;
DELETE FROM disables WHERE entry BETWEEN 9043 AND 9050;
DELETE FROM creature_questender WHERE quest BETWEEN 9043 AND 9050;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9043 AND 9050;
INSERT INTO creature_questender (id, quest) VALUES 
(16115, 9043),
(16115, 9044),
(16115, 9045),
(16115, 9046),
(16115, 9047),
(16115, 9048),
(16115, 9049),
(16115, 9050);
INSERT INTO creature_queststarter (id, quest) VALUES
(16115, 9043),
(16115, 9044),
(16115, 9045),
(16115, 9046),
(16115, 9047),
(16115, 9048),
(16115, 9049),
(16115, 9050);

-- PRIEST
UPDATE quest_template_addon SET AllowableClasses = 16 WHERE id BETWEEN 9111 AND 9118;
DELETE FROM disables WHERE entry BETWEEN 9111 AND 9118;
DELETE FROM creature_questender WHERE quest BETWEEN 9111 AND 9118;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9111 AND 9118;
INSERT INTO creature_questender (id, quest) VALUES 
(16113, 9111),
(16113, 9112),
(16113, 9113),
(16113, 9114),
(16113, 9115),
(16113, 9116),
(16113, 9117),
(16113, 9118);
INSERT INTO creature_queststarter (id, quest) VALUES
(16113, 9043),
(16113, 9044),
(16113, 9045),
(16113, 9046),
(16113, 9047),
(16113, 9048),
(16113, 9049),
(16113, 9050);

-- ROGUE
UPDATE quest_template_addon SET AllowableClasses = 8 WHERE id BETWEEN 9077 AND 9084;
DELETE FROM disables WHERE entry BETWEEN 9077 AND 9084;
DELETE FROM creature_questender WHERE quest BETWEEN 9077 AND 9084;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9077 AND 9084;
INSERT INTO creature_questender (id, quest) VALUES 
(16131, 9077),
(16131, 9078),
(16131, 9079),
(16131, 9080),
(16131, 9081),
(16131, 9082),
(16131, 9083),
(16131, 9084);
INSERT INTO creature_queststarter (id, quest) VALUES
(16131, 9077),
(16131, 9078),
(16131, 9079),
(16131, 9080),
(16131, 9081),
(16131, 9082),
(16131, 9083),
(16131, 9084);

-- SHAMAN
UPDATE quest_template_addon SET AllowableClasses = 64 WHERE id BETWEEN 9068 AND 9075;
DELETE FROM disables WHERE entry BETWEEN 9068 AND 9075;
DELETE FROM creature_questender WHERE quest BETWEEN 9068 AND 9075;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9068 AND 9075;
INSERT INTO creature_questender (id, quest) VALUES 
(16134, 9068),
(16134, 9069),
(16134, 9070),
(16134, 9071),
(16134, 9072),
(16134, 9073),
(16134, 9074),
(16134, 9075);
INSERT INTO creature_queststarter (id, quest) VALUES
(16134, 9068),
(16134, 9069),
(16134, 9070),
(16134, 9071),
(16134, 9072),
(16134, 9073),
(16134, 9074),
(16134, 9075);

-- WARLOCK
UPDATE quest_template_addon SET AllowableClasses = 256 WHERE id BETWEEN 9103 AND 9110;
DELETE FROM disables WHERE entry BETWEEN 9103 AND 9110;
DELETE FROM creature_questender WHERE quest BETWEEN 9103 AND 9110;
DELETE FROM creature_queststarter WHERE quest BETWEEN 9103 AND 9110;
INSERT INTO creature_questender (id, quest) VALUES 
(16133, 9103),
(16133, 9104),
(16133, 9105),
(16133, 9106),
(16133, 9107),
(16133, 9108),
(16133, 9109),
(16133, 9110);
INSERT INTO creature_queststarter (id, quest) VALUES
(16133, 9103),
(16133, 9104),
(16133, 9105),
(16133, 9106),
(16133, 9107),
(16133, 9108),
(16133, 9109),
(16133, 9110);

-- WARRIOR
UPDATE quest_template_addon SET AllowableClasses = 1 WHERE id = 9034 OR id BETWEEN 9036 AND 9042;
DELETE FROM disables WHERE entry = 9034 OR entry BETWEEN 9036 AND 9042;
DELETE FROM creature_questender WHERE quest = 9034 OR quest BETWEEN 9036 AND 9042;
DELETE FROM creature_queststarter WHERE quest = 9034 OR quest BETWEEN 9036 AND 9042;
INSERT INTO creature_questender (id, quest) VALUES 
(16112, 9034),
(16112, 9036),
(16112, 9037),
(16112, 9038),
(16112, 9039),
(16112, 9040),
(16112, 9041),
(16112, 9042);
INSERT INTO creature_queststarter (id, quest) VALUES
(16112, 9034),
(16112, 9036),
(16112, 9037),
(16112, 9038),
(16112, 9039),
(16112, 9040),
(16112, 9041),
(16112, 9042);