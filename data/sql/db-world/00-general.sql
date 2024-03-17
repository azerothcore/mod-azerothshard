DELETE FROM `creature_template` WHERE `entry` BETWEEN 100000 AND 100006;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `speed_swim`, `speed_flight`, `detection_range`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(100000, 0, 0, 0, 0, 0, 23055, 0, 0, 0, "Han'al", 'Master Lorekeeper', '', 30000, 83, 83, 0, 2007, 3, 1.1, 1.5, 1, 1, 20, 1.2, 1, 0, 1, 0, 0, 1, 1, 8, 33536, 2048, 0, 0, 0, 0, 0, 0, 7, 32, 0, 0, 0, 0, 0, 0, 0, 'SmartAI', 0, 0, 10, 10, 1, 1, 0, 0, 1, 0, 0, 2, 'npc_han_al', 12341),
(100001, 0, 0, 0, 0, 0, 10215, 0, 0, 0, 'Manaben', "Han'al Apprentice", '', 0, 80, 80, 0, 2007, 3, 1.1, 1.5, 1, 1, 20, 1.2, 1, 0, 1, 0, 0, 1, 1, 8, 33536, 2048, 0, 0, 0, 0, 0, 0, 7, 32, 0, 0, 0, 0, 0, 0, 0, '', 0, 0, 10, 10, 1, 1, 0, 0, 1, 0, 0, 2, '', 12341),
(100002, 0, 0, 0, 0, 0, 22439, 0, 0, 0, 'Azth Neutral Vendor', 'AzerothShard', '', 0, 70, 70, 1, 35, 128, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 4, 1, 1, 1, 0, 0, 1, 0, 0, 128, '', 12340),
(100003, 0, 0, 0, 0, 0, 22439, 0, 0, 0, 'Azth Friendly Vendor', 'AzerothShard', '', 0, 70, 70, 1, 35, 128, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 4, 1, 1, 1, 0, 0, 1, 0, 0, 128, '', 12340),
(100004, 0, 0, 0, 0, 0, 22439, 0, 0, 0, 'Azth Honored Vendor', 'AzerothShard', '', 0, 70, 70, 1, 35, 128, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 4, 1, 1, 1, 0, 0, 1, 0, 0, 128, '', 12340),
(100005, 0, 0, 0, 0, 0, 22439, 0, 0, 0, 'Azth Revered Vendor', 'AzerothShard', '', 0, 70, 70, 1, 35, 128, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 4, 1, 1, 1, 0, 0, 1, 0, 0, 128, '', 12340),
(100006, 0, 0, 0, 0, 0, 22439, 0, 0, 0, 'Azth Exalted Vendor', 'AzerothShard', '', 0, 70, 70, 1, 35, 128, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33555202, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 4, 1, 1, 1, 0, 0, 1, 0, 0, 128, '', 12340);

DELETE FROM `npc_text` WHERE `ID` IN (100000, 100001, 100002, 100003);
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `em0_0`) VALUES
(100000, "È bello che tu voglia aiutarmi $n, la storia dev'essere scrupolosamente registrata... nulla deve andar perso!", 0, 0, 0),
(100001, "Hai già delle missioni da completare, ritorna quando sarai riuscito a portarle a termine o quando avrai rinunciato...\nNonostante il fallimento potrei avere ancora bisogno di te, $r", 0, 0, 0),
(100002, "Grazie del tuo aiuto, $c, per oggi mi hai aiutato a sufficienza!\nRitorna domani.", 0, 0, 0),
(100003, "Hai già molto lavoro da fare, $n. Non vorrei che ti affaticassi troppo... torna da me quando avrai concluso un po' del tuo lavoro.", 0, 0, 0);

UPDATE `item_template` SET `Flags`=`Flags`|64, `ScriptName`='item_azth_hearthstone_loot_sack', `stackable`=1, `spellid_1`=36177, `maxcount`=5 WHERE `entry`=32558;
UPDATE `creature_template` SET `npcflag`=`npcflag`|129, `unit_flags`=`unit_flags`|768, `type_flags`=`type_flags`|134217728, `flags_extra`=`flags_extra`|2, `scriptname`="npc_azth_vendor", `gossip_menu_id`=32001 WHERE `entry` IN (100002, 100003, 100004, 100005, 100006);

DELETE FROM `creature_template_resistance` WHERE `CreatureID` IN (100002, 100003, 100004, 100005, 100006);
INSERT INTO `creature_template_resistance` (`CreatureID`, `School`, `Resistance`) VALUES
(100002, 1, 32001),
(100003, 1, 32001),
(100004, 1, 32001),
(100005, 1, 32001),
(100006, 1, 32001),
(100003, 2, 3000),
(100004, 2, 6000),
(100005, 2, 12000),
(100006, 2, 21000);

DELETE FROM `npc_vendor` WHERE `entry` IN (100002, 100003, 100004, 100005, 100006, 100007);
INSERT INTO `npc_vendor` (`entry`, `item`, `extendedcost`) VALUES
(100002, 32558, 100010),
(100003, 32558, 100009),
(100004, 32558, 100008),
(100004, 32558, 100007),
(100005, 32558, 100006),
(100006, 32558, 100005),
(100003, 49426, 100005),
(100003, 47241, 100004),
(100003, 45624, 100003),
(100003, 40753, 100002),
(100003, 49752, 100001);
