DELETE FROM npc_text WHERE ID IN (32001, 32010);
INSERT INTO npc_text (ID, text0_0, text0_1, lang0, em0_0) VALUES 
(32001, "Qui si paga in Marks of Azeroth!", 0, 0, 0),
(32010, "Ti serve più reputazione!", 0, 0, 0);

-- Quartermaster Ozorg (Dk start set vendor)
-- FRIENDLY
DELETE FROM `creature_template` WHERE (entry = 100100);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(100100, 0, 0, 0, 0, 0, 16214, 0, 0, 0, 'Quartermaster Ozorg', 'Acherus Quartermaster', '', 0, 72, 72, 1, 35, 128, 1, 1.14286, 1, 0, 0, 2000, 2000, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 4, 1, 1, 0, 0, 1, 0, 128, '', 12340);

-- requires friendly with azerothshard (3000 rep)
UPDATE creature_template SET `npcflag` = 129, `unit_flags` = 768, `type_flags` = 134217728, `flags_extra` = 2, scriptname = "npc_azth_vendor", `type` = 6  WHERE entry = 100100; 

DELETE FROM npc_vendor WHERE entry = 100100;
INSERT INTO npc_vendor (entry, item, extendedcost) VALUES 
(100100, 38632, 3023), -- weapon
(100100, 38633, 3023), -- weapon
(100100, 38661, 3022), -- head
(100100, 38662, 3020), -- neck
(100100, 38663, 3022), -- shoulder
(100100, 38664, 3021), -- back
(100100, 38665, 3022), -- chest
(100100, 38666, 3021), -- bracer
(100100, 38667, 3022), -- hands
(100100, 38668, 3021), -- waist
(100100, 38669, 3022), -- leg
(100100, 38670, 3021), -- feet
(100100, 38671, 3020), -- ring
(100100, 38672, 3020), -- ring
(100100, 38674, 3020), -- trinket
(100100, 38675, 3020), -- trinket
(100100, 39320, 3021), -- back
(100100, 39322, 3021), -- back
(100100, 38707, 3023), -- weapon
(100100, 34648, 3021), -- feet
(100100, 34649, 3021), -- hands
(100100, 34650, 3022), -- chest
(100100, 34651, 3021), -- waist
(100100, 34652, 3022), -- head
(100100, 34653, 3021), -- wrist
(100100, 34655, 3021), -- shoulder
(100100, 34656, 3022), -- leg
(100100, 34657, 3020), -- neck
(100100, 34658, 3020), -- ring
(100100, 34659, 3021), -- back
(100100, 34661, 3023), -- weapon
(100100, 38147, 3020); -- ring 

DELETE FROM npc_text WHERE ID = 32000;
INSERT INTO npc_text (ID, text0_0, text0_1, lang0, em0_0) VALUES 
(32000, "Se tu vuoi comprare, tu dare me Marks of Azeroth!", 0, 0, 0);

UPDATE creature_template SET MovementType = 1 WHERE entry = 100100; -- set some random movement

-- T3 Vendor
-- HONORED
DELETE FROM `creature_template` WHERE (entry = 100101);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(100101, 0, 0, 0, 0, 0, 10478, 0, 0, 0, 'Lord Raymond George', 'The Argent Dawn', '', 0, 72, 72, 1, 35, 128, 1, 1.14286, 1, 0, 0, 2000, 2000, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 4, 1, 1, 0, 0, 1, 0, 128, '', 12340);

UPDATE creature_template SET `npcflag` = 129, `unit_flags` = 768, `type_flags` = 134217728, `flags_extra` = 2, scriptname = "npc_azth_vendor", `type` = 6  WHERE entry = 100101; 

DELETE FROM npc_vendor WHERE entry = 100101;
INSERT INTO npc_vendor (entry, item, extendedcost) VALUES 
(100101, 22349, 3025),
(100101, 22350, 3025),
(100101, 22351, 3025),
(100101, 22352, 3025),
(100101, 22353, 3025),
(100101, 22354, 3024),
(100101, 22355, 3024),
(100101, 22356, 3024),
(100101, 22357, 3024),
(100101, 22358, 3024),
(100101, 22359, 3025),
(100101, 22360, 3025),
(100101, 22361, 3024),
(100101, 22362, 3024),
(100101, 22363, 3024),
(100101, 22364, 3024),
(100101, 22365, 3024),
(100101, 22366, 3025),
(100101, 22367, 3025),
(100101, 22368, 3024),
(100101, 22369, 3024),
(100101, 22370, 3024),
(100101, 22371, 3024),
(100101, 22372, 3024),
(100101, 22373, 3021),
(100101, 22374, 3021),
(100101, 22375, 3021),
(100101, 22376, 3021);

DELETE FROM npc_text WHERE ID = 32002;
INSERT INTO npc_text (ID, text0_0, text0_1, lang0, em0_0) VALUES 
(32002, "Salve, $N. Sei qui per aiutare gli Argent Dawn? I tuoi Marks of Azeroth saranno estremamente utili per le nostre spese...", 0, 0, 0);

-- Heirloom Vendor
-- Neutral
DELETE FROM `creature_template` WHERE (entry = 100102);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(100102, 0, 0, 0, 0, 0, 3709, 0, 0, 0, 'Arsenio', '', '', 0, 80, 80, 1, 35, 128, 1, 1.14286, 1, 0, 0, 2000, 2000, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 4, 1, 1, 0, 0, 1, 0, 128, '', 12340);

UPDATE creature_template SET `npcflag` = 129, `unit_flags` = 768, `type_flags` = 134217728, 
`flags_extra` = 2, scriptname = "npc_azth_vendor", `type` = 6 WHERE entry = 100102; 

DELETE FROM npc_vendor WHERE entry = 100102;
INSERT INTO npc_vendor (entry, item, extendedcost) VALUES 
(100102,'42943', 3023),
(100102,'42944', 3023),
(100102,'42945', 3023),
(100102,'42946', 3023),
(100102,'42947', 3023),
(100102,'42948', 3023),
(100102,'42949', 3023),
(100102,'42950', 3023),
(100102,'42951', 3023),
(100102,'42952', 3023),
(100102,'42984', 3023),
(100102,'42985', 3023),
(100102,'42991', 3023),
(100102,'42992', 3023),
(100102,'44091', 3023),
(100102,'44092', 3023),
(100102,'44093', 3023),
(100102,'44094', 3023),
(100102,'44095', 3023),
(100102,'44096', 3023),
(100102,'44097', 3023),
(100102,'44098', 3023),
(100102,'44099', 3023),
(100102,'44100', 3023),
(100102,'44101', 3023),
(100102,'44102', 3023),
(100102,'44103', 3023),
(100102,'44105', 3023),
(100102,'44107', 3023),
(100102,'48685', 3023),
(100102,'48691', 3023),
(100102,'48677', 3023),
(100102,'48687', 3023),
(100102,'48689', 3023),
(100102,'48683', 3023),
(100102,'48716', 3023),
(100102,'48718', 3023),
(100102,'50255', 3023);

DELETE FROM npc_text WHERE ID = 32003;
INSERT INTO npc_text (ID, text0_0, text0_1, lang0, em0_0) VALUES 
(32003, "Hey, tu! Vuoi un po' di roba? L'ho rubat.. ricevuta giusto ieri... In cambio ti chiedo solo qualche Mark of Azeroth per delle... ehm.. ricerche...", 0, 0, 0);