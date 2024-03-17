UPDATE `quest_template` SET `Flags`=`Flags`|4226, `RewardFactionID1`=948, `RewardFactionValue1`=0, `RewardFactionOverride1`=10 WHERE `ID` BETWEEN 100000 AND 100080;

UPDATE `item_template` SET `name`="Mark of Azeroth", `stackable`=2147483647, `description`="Il bottino di ogni avventuriero!", `maxcount`=0, `spellid_1`=0, `spellcharges_1`=0, `flags`=0 WHERE `entry`=37711;

UPDATE `quest_template` SET `RewardItem1`=37711, `RewardAmount1`=2 WHERE `ID` BETWEEN 100000 AND 100080;

UPDATE `item_template` SET `name`="Garga's Magic Box", `description`="Non sono così egocentrico da chiamare un NPC come me." WHERE `entry`=32558;

DELETE FROM `creature_questender` WHERE `id`=100001 AND `quest` <= 100080 AND `quest` >= 100000;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(100001, 100000),
(100001, 100001),
(100001, 100002),
(100001, 100003),
(100001, 100004),
(100001, 100005),
(100001, 100006),
(100001, 100007),
(100001, 100008),
(100001, 100009),
(100001, 100010),
(100001, 100011),
(100001, 100012),
(100001, 100013),
(100001, 100014),
(100001, 100015),
(100001, 100016),
(100001, 100017),
(100001, 100018),
(100001, 100019),
(100001, 100020),
(100001, 100021),
(100001, 100022),
(100001, 100023),
(100001, 100024),
(100001, 100025),
(100001, 100026),
(100001, 100027),
(100001, 100028),
(100001, 100029),
(100001, 100030),
(100001, 100031),
(100001, 100032),
(100001, 100033),
(100001, 100034),
(100001, 100035),
(100001, 100036),
(100001, 100037),
(100001, 100038),
(100001, 100039),
(100001, 100040),
(100001, 100041),
(100001, 100042),
(100001, 100043),
(100001, 100044),
(100001, 100045),
(100001, 100046),
(100001, 100047),
(100001, 100048),
(100001, 100049),
(100001, 100050),
(100001, 100051),
(100001, 100052),
(100001, 100053),
(100001, 100054),
(100001, 100055),
(100001, 100056),
(100001, 100057),
(100001, 100058),
(100001, 100059),
(100001, 100060),
(100001, 100061),
(100001, 100062),
(100001, 100063),
(100001, 100064),
(100001, 100065),
(100001, 100066),
(100001, 100067),
(100001, 100068),
(100001, 100069),
(100001, 100070),
(100001, 100071),
(100001, 100072),
(100001, 100073),
(100001, 100074),
(100001, 100075),
(100001, 100076),
(100001, 100077),
(100001, 100078),
(100001, 100079),
(100001, 100080);

UPDATE `creature_template` SET `gossip_menu_id`=60000 WHERE `entry`=100001;

DELETE FROM `gossip_menu` WHERE `MenuID`=60000;
INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES
(60000, 100010);

DELETE FROM `npc_text` WHERE `ID` = 100010;
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `em0_0`) VALUES
(100010, "Il Maestro è una persona molto stimata e famosa... aiutalo, e sarà generoso con te, $n!", 0, 0, 0);

UPDATE `quest_template` SET `QuestInfoID`=0, `flags`=`flags`|4098, `RewardFactionID1`=948, `RewardFactionValue1`=1, `RewardFactionOverride1`=0, `unknown0`=1 WHERE `ID` BETWEEN 100000 AND 100080;

DELETE FROM `item_template` WHERE `entry` >= 100017 AND `entry` <= 154806 AND `subclass` IN (0, 5, 7, 8, 9, 10);

UPDATE `quest_template` SET `Flags`=`Flags`|4290, `QuestInfoID`=62 WHERE `ID` BETWEEN 100000 AND 100046;
UPDATE `quest_template` SET `Flags`=`Flags`|4290, `QuestInfoID`=62 WHERE `ID`=100080;
