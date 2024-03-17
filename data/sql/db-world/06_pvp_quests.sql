DELETE FROM creature_template WHERE entry >= 110000 AND entry <= 120000;
INSERT INTO creature_template (entry, killcredit1, modelid1, `name`, minlevel, maxlevel, faction, npcflag, unit_flags, unit_flags2, dynamicflags, flags_extra) VALUES
-- (110000, 110000, 17519, "Template", 1, 1, 14, 0, 0, 0, 0, 0),
(110001, 110001, 17519, "Alterac Valley Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110002, 110002, 17519, "Warsong Gulch Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110003, 110003, 17519, "Arathi Basin Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110004, 110004, 17519, "Eye of the Storm Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110005, 110005, 17519, "Strand of the Ancients Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110006, 110006, 17519, "Isle of Conquest Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110007, 110007, 17519, "Arathi Basin Flag Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110008, 110008, 17519, "Warsong Gulch Flag Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110009, 110009, 17519, "Warsong Gulch Flag Recover Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110010, 110010, 17519, "Eye of the Storm Flag Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110011, 110011, 17519, "Alterac Valley Graveyard Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110012, 110012, 17519, "Alterac Valley Tower Defense Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110013, 110013, 17519, "Alterac Valley Tower Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110014, 110014, 17519, "Arathi Basin Base Defense Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110015, 110015, 17519, "Alterac Valley Graveyard Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110016, 110016, 17519, "Isle of Conquest Base Capture Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110017, 110017, 17519, "Isle of Conquest Base Defend Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110018, 110018, 17519, "Kill a Warrior Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110019, 110019, 17519, "Kill a Paladin Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110020, 110020, 17519, "Kill a Hunter Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110021, 110021, 17519, "Kill a Rogue Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110022, 110022, 17519, "Kill a Priest Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110023, 110023, 17519, "Kill a Death Knight Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110024, 110024, 17519, "Kill a Shaman Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110025, 110025, 17519, "Kill a Mage Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110026, 110026, 17519, "Kill a Warlock Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110027, 110027, 17519, "Kill a Druid Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110028, 110028, 17519, "Kill a Human Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110029, 110029, 17519, "Kill an Orc Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110030, 110030, 17519, "Kill a Dwarf Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110031, 110031, 17519, "Kill a Night Elf Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110032, 110032, 17519, "Kill an Undead Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110033, 110033, 17519, "Kill a Tauren Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110034, 110034, 17519, "Kill a Gnome Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110035, 110035, 17519, "Kill a Troll Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110036, 110036, 17519, "Kill a Blood Elf Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110037, 110037, 17519, "Kill a Draenei Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110038, 110038, 17519, "Get a Killing Blow Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110039, 110039, 17519, "Earn Honorable Kill Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0),
(110040, 110040, 17519, "Win Duel Kill Credit", 1, 1, 14, 0, 0, 0, 0, 0)
;

-- debug quests
DELETE FROM quest_template WHERE id >= 110000 AND id <= 110039;
-- INSERT INTO `quest_template` (`id`, `QuestInfoID`, `QuestSortID`, `Flags`, `LogTitle`, `LogDescription`, `QuestDescription`, `OfferRewardText`, `RequiredNpcOrGo1`, `RequiredNpcOrGoCount1`, ObjectiveText1) VALUES
-- (110000,'2','3840','12482','Vinci Alterac Valley','Vinci Alterac Valley','Vinci Alterac Valley','Ritorna da Han\'al','110001','1', "Vinci Alterac Valley"),
-- (110001,'2','3840','12482','Vinci Warsong Gulch','Vinci Warsong Gulch','Vinci Warsong Gulch','Ritorna da Han\'al','110002','1', "Vinci Warsong Gulch"),
-- (110002, '2','3840','12482', 'Vinci Arathi Basin', 'Vinci Arathi Basin', 'Vinci Arathi Basin', 'Ritorna da Han\'al', 110003, 1, 'Vinci Arathi Basin'),
-- (110003, '2','3840','12482', 'Vinci Eye of the Storm', 'Vinci Eye of the Storm', 'Vinci Eye of the Storm', 'Ritorna da Han\'al', 110004, 1, 'Vinci Eye of the Storm'),
-- (110004, '2','3840','12482', 'Vinci Strand of the Ancients', 'Vinci Strand of the Ancients', 'Vinci Strand of the Ancients', 'Ritorna da Han\'al', 110005, 1, 'Vinci Strand of the Ancients'),
-- (110005, '2','3840','12482', 'Vinci Isle of Conquest', 'Vinci Isle of Conquest', 'Vinci Isle of Conquest', 'Ritorna da Han\'al', 110006, 1, 'Vinci Isle of Conquest'),
-- (110006, '2','3840','12482', 'Arathi Basin Flag Capture', 'Arathi Basin Flag Capture', 'Arathi Basin Flag Capture', 'Ritorna da Han\'al', 110007, 1, 'Arathi Basin Flag Capture'),
-- (110007, '2','3840','12482', 'Warsong Gulch Flag Capture', 'Warsong Gulch Flag Capture', 'Warsong Gulch Flag Capture', 'Ritorna da Han\'al', 110008, 1, 'Warsong Gulch Flag Capture'),
-- (110008, '2','3840','12482', 'Warsong Gulch Flag Recover', 'Warsong Gulch Flag Recover', 'Warsong Gulch Flag Recover', 'Ritorna da Han\'al', 110009, 1, 'Warsong Gulch Flag Recover'),
-- (110009, '2','3840','12482', 'Eye of the Storm Flag Capture', 'Eye of the Storm Flag Capture', 'Eye of the Storm Flag Capture', 'Ritorna da Han\'al', 110010, 1, 'Eye of the Storm Flag Capture'),
-- (110010, '2','3840','12482', 'Alterac Valley Graveyard Capture', 'Alterac Valley Graveyard Capture', 'Alterac Valley Graveyard Capture', 'Ritorna da Han\'al', 110011, 1, 'Alterac Valley Graveyard Capture'),
-- (110011, '2','3840','12482', 'Alterac Valley Tower Defense', 'Alterac Valley Tower Defense', 'Alterac Valley Tower Defense', 'Ritorna da Han\'al', 110012, 1, 'Alterac Valley Tower Defense'),
-- (110012, '2','3840','12482', 'Alterac Valley Tower Capture', 'Alterac Valley Tower Capture', 'Alterac Valley Tower Capture', 'Ritorna da Han\'al', 110013, 1, 'Alterac Valley Tower Capture'),
-- (110013, '2','3840','12482', 'Arathi Basin Base Defense', 'Arathi Basin Base Defense', 'Arathi Basin Base Defense', 'Ritorna da Han\'al', 110014, 1, 'Arathi Basin Base Defense'),
-- (110014, '2','3840','12482', 'Alterac Valley Graveyard Capture', 'Alterac Valley Graveyard Capture', 'Alterac Valley Graveyard Capture', 'Ritorna da Han\'al', 110015, 1, 'Alterac Valley Graveyard Capture'),
-- (110015, '2','3840','12482', 'Isle of Conquest Base Capture', 'Isle of Conquest Base Capture', 'Isle of Conquest Base Capture', 'Ritorna da Han\'al', 110016, 1, 'Isle of Conquest Base Capture'),
-- (110016, '2','3840','12482', 'Isle of Conquest Base Defend', 'Isle of Conquest Base Defend', 'Isle of Conquest Base Defend', 'Ritorna da Han\'al', 110017, 1, 'Isle of Conquest Base Defend'),
-- (110017, '2','3840','12482', 'Kill a Warrior', 'Kill a Warrior', 'Kill a Warrior', 'Ritorna da Han\'al', 110018, 1, 'Kill a Warrior'),
-- (110018, '2','3840','12482', 'Kill a Paladin', 'Kill a Paladin', 'Kill a Paladin', 'Ritorna da Han\'al', 110019, 1, 'Kill a Paladin'),
-- (110019, '2','3840','12482', 'Kill a Hunter', 'Kill a Hunter', 'Kill a Hunter', 'Ritorna da Han\'al', 110020, 1, 'Kill a Hunter'),
-- (110020, '2','3840','12482', 'Kill a Rogue', 'Kill a Rogue', 'Kill a Rogue', 'Ritorna da Han\'al', 110021, 1, 'Kill a Rogue'),
-- (110021, '2','3840','12482', 'Kill a Priest', 'Kill a Priest', 'Kill a Priest', 'Ritorna da Han\'al', 110022, 1, 'Kill a Priest'),
-- (110022, '2','3840','12482', 'Kill a Death Knight', 'Kill a Death Knight', 'Kill a Death Knight', 'Ritorna da Han\'al', 110023, 1, 'Kill a Death Knight'),
-- (110023, '2','3840','12482', 'Kill a Shaman', 'Kill a Shaman', 'Kill a Shaman', 'Ritorna da Han\'al', 110024, 1, 'Kill a Shaman'),
-- (110024, '2','3840','12482', 'Kill a Mage', 'Kill a Mage', 'Kill a Mage', 'Ritorna da Han\'al', 110025, 1, 'Kill a Mage'),
-- (110025, '2','3840','12482', 'Kill a Warlock', 'Kill a Warlock', 'Kill a Warlock', 'Ritorna da Han\'al', 110026, 1, 'Kill a Warlock'),
-- (110026, '2','3840','12482', 'Kill a Druid', 'Kill a Druid', 'Kill a Druid', 'Ritorna da Han\'al', 110027, 1, 'Kill a Druid'),
-- (110027, '2','3840','12482', 'Kill a Human', 'Kill a Human', 'Kill a Human', 'Ritorna da Han\'al', 110028, 1, 'Kill a Human'),
-- (110028, '2','3840','12482', 'Kill an Orc', 'Kill an Orc', 'Kill an Orc', 'Ritorna da Han\'al', 110029, 1, 'Kill an Orc'),
-- (110029, '2','3840','12482', 'Kill a Dwarf', 'Kill a Dwarf', 'Kill a Dwarf', 'Ritorna da Han\'al', 110030, 1, 'Kill a Dwarf'),
-- (110030, '2','3840','12482', 'Kill a Night Elf', 'Kill a Night Elf', 'Kill a Night Elf', 'Ritorna da Han\'al', 110031, 1, 'Kill a Night Elf'),
-- (110031, '2','3840','12482', 'Kill an Undead', 'Kill an Undead', 'Kill an Undead', 'Ritorna da Han\'al', 110032, 1, 'Kill an Undead'),
-- (110032, '2','3840','12482', 'Kill a Tauren', 'Kill a Tauren', 'Kill a Tauren', 'Ritorna da Han\'al', 110033, 1, 'Kill a Tauren'),
-- (110033, '2','3840','12482', 'Kill a Gnome', 'Kill a Gnome', 'Kill a Gnome', 'Ritorna da Han\'al', 110034, 1, 'Kill a Gnome'),
-- (110034, '2','3840','12482', 'Kill a Troll', 'Kill a Troll', 'Kill a Troll', 'Ritorna da Han\'al', 110035, 1, 'Kill a Troll'),
-- (110035, '2','3840','12482', 'Kill a Blood Elf', 'Kill a Blood Elf', 'Kill a Blood Elf', 'Ritorna da Han\'al', 110036, 1, 'Kill a Blood Elf'),
-- (110036, '2','3840','12482', 'Kill a Draenei', 'Kill a Draenei', 'Kill a Draenei', 'Ritorna da Han\'al', 110037, 1, 'Kill a Draenei'),
-- (110037, '2','3840','12482', 'Get a Killing Blow', 'Get a Killing Blow', 'Get a Killing Blow', 'Ritorna da Han\'al', 110038, 1, 'Get a Killing Blow'),
-- (110038, '2','3840','12482', 'Earn Honorable Kill', 'Earn Honorable Kill', 'Earn Honorable Kill', 'Ritorna da Han\'al', 110039, 1, 'Earn Honorable Kill'),
-- (110039, '2','3840','12482', 'Win Duel', 'Win Duel', 'Win Duel', 'Ritorna da Han\'al', 110040, 1, 'Win Duel');

UPDATE quest_template SET minlevel = 80 WHERE id >= 110000 AND id <= 110300;


-- real quests
DELETE FROM quest_template WHERE id >= 110040 AND id <= 110095;
-- INSERT INTO `quest_template` (`id`, `QuestInfoID`, `QuestSortID`, `Flags`, `LogTitle`, `LogDescription`, `QuestDescription`, `OfferRewardText`, `RequiredNpcOrGo1`, `RequiredNpcOrGoCount1`, ObjectiveText1, RewardItem1, RewardAmount1) VALUES
-- (110040,'2','3840','12482','Vinci 2 Warsong Gulch','Vai, $N, la battaglia di Warsong Gulch ti attende! Da sempre orchi ed elfi della notte si sfidano in quelle terre... Prendi parte al conflitto e torna vittorioso a raccontarmelo!','Vinci Warsong Gulch','Ritorna da Han\'al','110002','2', "Warsong Gulch vinti", 37711, 2),
-- (110041,'2','3840','12482','Vinci 4 Warsong Gulch','Vai, $N, la battaglia di Warsong Gulch ti attende! Da sempre orchi ed elfi della notte si sfidano in quelle terre... Prendi parte al conflitto e torna vittorioso a raccontarmelo!','Vinci Warsong Gulch','Ritorna da Han\'al','110002','4', "Warsong Gulch vinti", 37711, 3),
-- (110042,'2','3840','12482','Vinci 2 Arathi Basin','Vai, $N, la battaglia di Arathi Basin ti attende! I Forsaken e la Lega di Arathor cercano di accumulare le ricchezze del luogo... aiuta una delle due parti e torna vincitore a raccontarmelo!','Vinci Arathi Basin','Ritorna da Han\'al','110003','2', "Arathi Basin vinti", 37711, 2),
-- (110043,'2','3840','12482','Vinci 4 Arathi Basin','Vai, $N, la battaglia di Arathi Basin ti attende! I Forsaken e la Lega di Arathor cercano di accumulare le ricchezze del luogo... aiuta una delle due parti e torna vincitore a raccontarmelo!','Vinci Arathi Basin','Ritorna da Han\'al','110003','4', "Arathi Basin vinti", 37711, 3),
-- (110044,'2','3840','12482','Vinci 2 Eye of the Storm','Vai, $N, la battaglia di Eye of the Storm ti attende! Corri a Netherstorm e prendi parte al conflitto... torna vittorioso a raccontarmelo!','Vinci Eye of the Storm','Ritorna da Han\'al','110004','2', "Eye of the Storm vinti", 37711, 2),
-- (110045,'2','3840','12482','Vinci 4 Eye of the Storm','Vai, $N, la battaglia di Eye of the Storm ti attende! Corri a Netherstorm e prendi parte al conflitto... torna vittorioso a raccontarmelo!','Vinci Eye of the Storm','Ritorna da Han\'al','110004','4', "Eye of the Storm vinti", 37711, 3),
-- (110046,'2','3840','12482','Cattura 3 flag in Arathi Basin','Cattura 3 flag in Arathi Basin!','Cattura 3 flag in Arathi Basin','Ritorna da Han\'al','110007','3', "Arathi Basin flag catturate", 37711, 2),
-- (110047,'2','3840','12482','Cattura 5 flag in Arathi Basin','Cattura 5 flag in Arathi Basin!','Cattura 5 flag in Arathi Basin','Ritorna da Han\'al','110007','5', "Arathi Basin flag catturate", 37711, 3),
-- (110048,'2','3840','12482','Cattura 3 flag in Warsong Gulch','Cattura 3 flag in Warsong Gulch!','Cattura 3 flag in Warsong Gulch','Ritorna da Han\'al','110008','3', "Warsong Gulch flag catturate", 37711, 2),
-- (110049,'2','3840','12482','Cattura 5 flag in Warsong Gulch','Cattura 5 flag in Warsong Gulch!','Cattura 5 flag in Warsong Gulch','Ritorna da Han\'al','110008','5', "Warsong Gulch flag catturate", 37711, 3),
-- (110050,'2','3840','12482','Recupera 2 flag in Warsong Gulch','Recupera 2 flag in Warsong Gulch!','Recupera 2 flag in Warsong Gulch','Ritorna da Han\'al','110009','2', "Warsong Gulch flag recuperate", 37711, 2),
-- (110051,'2','3840','12482','Recupera 3 flag in Warsong Gulch','Recupera 3 flag in Warsong Gulch!','Recupera 3 flag in Warsong Gulch','Ritorna da Han\'al','110009','3', "Warsong Gulch flag recuperate", 37711, 3),
-- (110052,'2','3840','12482','Cattura 2 flag in Eye of the Storm','Cattura 2 flag in Eye of the Storm!','Cattura 2 flag in Eye of the Storm','Ritorna da Han\'al','110010','2', "Eye of the Storm flag catturate", 37711, 2),
-- (110053,'2','3840','12482','Cattura 4 flag in Eye of the Storm','Cattura 4 flag in Eye of the Storm!','Cattura 4 flag in Eye of the Storm','Ritorna da Han\'al','110010','4', "Eye of the Storm flag catturate", 37711, 3),
-- (110054,'2','3840','12482','Difendi 3 basi in Arathi Basin','Difendi 3 basi in Arathi Basin!','Difendi 3 basi in Arathi Basin','Ritorna da Han\'al','110014','3', "Arathi Basin basi difese", 37711, 2),
-- (110055,'2','3840','12482','Difendi 5 basi in Arathi Basin','Difendi 5 basi in Arathi Basin!','Difendi 5 basi in Arathi Basin','Ritorna da Han\'al','110014','5', "Arathi Basin basi difese", 37711, 3),
-- (110056,'2','3840','12482','Nerfate il Bladestorm','Uccidi 5 Warrior','Uccidi 5 Warrior','Ritorna da Han\'al','110018','5', "Warrior Uccisi", 37711, 2),
-- (110057,'2','3840','12482','Mortal Strike op','Uccidi 10 Warrior','Uccidi 10 Warrior','Ritorna da Han\'al','110018','10', "Warrior Uccisi", 37711, 3),
-- (110058,'2','3840','12482','Come li uccido se hanno il Divine Shield?','Uccidi 5 Paladin','Uccidi 5 Paladin','Ritorna da Han\'al','110019','5', "Paladin Uccisi", 37711, 2),
-- (110059,'2','3840','12482','Odio per i pala','Uccidi 10 Paladin','Uccidi 10 Paladin','Ritorna da Han\'al','110019','10', "Paladin Uccisi", 37711, 3),
-- (110060,'2','3840','12482','Uccidili prima che ti uccidano','Uccidi 5 Hunter','Uccidi 5 Hunter','Ritorna da Han\'al','110020','5', "Hunter Uccisi", 37711, 2),
-- (110061,'2','3840','12482','Tanto hanno quaranta Deterrance','Uccidi 10 Hunter','Uccidi 10 Hunter','Ritorna da Han\'al','110020','10', "Warrior Hunter", 37711, 3),
-- (110062,'2','3840','12482','They do it from behind','Uccidi 5 Rogue','Uccidi 5 Rogue','Ritorna da Han\'al','110021','5', "Rogue Uccisi", 37711, 2),
-- (110063,'2','3840','12482','Stai per uccidermi? LOL no, vanish, cos, sprint, bye','Uccidi 10 Rogue','Uccidi 10 Rogue','Ritorna da Han\'al','110021','10', "Rogue Uccisi", 37711, 3),
-- (110064,'2','3840','12482','Ma è Illidan? Ah, no :,(','Uccidi 5 Warlock','Uccidi 5 Warlock','Ritorna da Han\'al','110026','5', "Warlock Uccisi", 37711, 2),
-- (110065,'2','3840','12482','Fear, fear, fear','Uccidi 10 Warlock','Uccidi 10 Warlock','Ritorna da Han\'al','110026','10', "Warlock Uccisi", 37711, 3),
-- (110066,'2','3840','12482','Tanto va in Bear Form e diventa immortale...','Uccidi 5 Druid','Uccidi 5 Druid','Ritorna da Han\'al','110027','5', "Druid Uccisi", 37711, 2),
-- (110067,'2','3840','12482','Orsopollooo','Uccidi 10 Druid','Uccidi 10 Druid','Ritorna da Han\'al','110027','10', "Druid Uccisi", 37711, 3),
-- (110068,'2','3840','12482','Ognuno per sè?','Uccidi 4 Human','Uccidi 4 Human','Ritorna da Han\'al','110028','4', "Human Uccisi", 37711, 2),
-- (110069,'2','3840','12482','Massacro a Stormwind','Uccidi 7 Human','Uccidi 7 Human','Ritorna da Han\'al','110028','7', "Human Uccisi", 37711, 3),
-- (110070,'2','3840','12482','Orgrimmar reputation','Uccidi 4 Orc','Uccidi 4 Orc','Ritorna da Han\'al','110029','4', "Orc Uccisi", 37711, 2),
-- (110071,'2','3840','12482','Ma non assomiglia a Shrek','Uccidi 7 Orc','Uccidi 7 Orc','Ritorna da Han\'al','110029','7', "Orc Uccisi", 37711, 3),
-- (110072,'2','3840','12482','Ma non è uno gnomo?','Uccidi 4 Dwarf','Uccidi 4 Dwarf','Ritorna da Han\'al','110030','4', "Dwarf Uccisi", 37711, 2),
-- (110073,'2','3840','12482','Re Magni in pensione','Uccidi 7 Dwarf','Uccidi 7 Dwarf','Ritorna da Han\'al','110030','7', "Dwarf Uccisi", 37711, 3),
-- (110074,'2','3840','12482','Taglialegna','Uccidi 4 Night Elf','Uccidi 4 Night Elf','Ritorna da Han\'al','110031','4', "Night Elf Uccisi", 37711, 2),
-- (110075,'2','3840','12482','Ma non erano buggati?','Uccidi 7 Night Elf','Uccidi 7 Night Elf','Ritorna da Han\'al','110031','7', "Night Elf Uccisi", 37711, 3),
-- (110076,'2','3840','12482','Ma è già morto...','Uccidi 4 Undead','Uccidi 4 Undead','Ritorna da Han\'al','110032','4', "Undead Uccisi", 37711, 2),
-- (110077,'2','3840','12482','Mmm... che profumino...','Uccidi 7 Undead','Uccidi 7 Undead','Ritorna da Han\'al','110032','7', "Undead Uccisi", 37711, 3),
-- (110078,'2','3840','12482','Mucche Killer','Uccidi 4 Tauren','Uccidi 4 Tauren','Ritorna da Han\'al','110033','4', "Tauren Uccisi", 37711, 2),
-- (110079,'2','3840','12482','Tutti cornuti','Uccidi 7 Tauren','Uccidi 7 Tauren','Ritorna da Han\'al','110033','7', "Tauren Uccisi", 37711, 3),
-- (110080,'2','3840','12482','Non si distinguono','Uccidi 4 Gnome','Uccidi 4 Gnome','Ritorna da Han\'al','110034','4', "Gnome Uccisi", 37711, 2),
-- (110081,'2','3840','12482','Basso è bello','Uccidi 7 Gnome','Uccidi 7 Gnome','Ritorna da Han\'al','110034','7', "Gnome Uccisi", 37711, 3),
-- (110082,'2','3840','12482','Rock and Troll','Uccidi 4 Troll','Uccidi 4 Troll','Ritorna da Han\'al','110035','4', "Troll Uccisi", 37711, 2),
-- (110083,'2','3840','12482','Vodoo is life','Uccidi 7 Troll','Uccidi 7 Troll','Ritorna da Han\'al','110035','7', "Troll Uccisi", 37711, 3),
-- (110084,'2','3840','12482','Sig Nicious I love you','Uccidi 4 Blood Elf','Uccidi 4 Blood Elf','Ritorna da Han\'al','110036','4', "Blood Elf Uccisi", 37711, 2),
-- (110085,'2','3840','12482','Preferivo senza Sunwell','Uccidi 7 Blood Elf','Uccidi 7 Blood Elf','Ritorna da Han\'al','110036','7', "Blood Elf Uccisi", 37711, 3),
-- (110086,'2','3840','12482','Rimandateli sul loro pianeta! Ruspa!','Uccidi 4 Draenei','Uccidi 4 Draenei','Ritorna da Han\'al','110037','4', "Draenei Uccisi", 37711, 2),
-- (110087,'2','3840','12482','Odio per i Draenei','Uccidi 7 Draenei','Uccidi 7 Draenei','Ritorna da Han\'al','110037','7', "Draenei Uccisi", 37711, 3),
-- (110088,'2','3840','12482','Occhio al Mind Control','Uccidi 5 Priest','Uccidi 5 Priest','Ritorna da Han\'al','110022','5', "Priest Uccisi", 37711, 2),
-- (110089,'2','3840','12482','Tanto ti shottano anche da Disci...','Uccidi 10 Priest','Uccidi 10 Priest','Ritorna da Han\'al','110022','10', "Priest Uccisi", 37711, 3),
-- (110090,'2','3840','12482','Incubo per i caster','Uccidi 5 Death Knight','Uccidi 5 Death Knight','Ritorna da Han\'al','110023','5', "Death Knight Uccisi", 37711, 2),
-- (110091,'2','3840','12482','Ah, ma è immune agli stun??','Uccidi 10 Death Knight','Uccidi 10 Death Knight','Ritorna da Han\'al','110023','10', "Death Knight Uccisi", 37711, 3),
-- (110092,'2','3840','12482','Non fanno abbastanza danno','Uccidi 5 Shaman','Uccidi 5 Shaman','Ritorna da Han\'al','110024','5', "Shaman Uccisi", 37711, 2),
-- (110093,'2','3840','12482','Ma quello è un Lava Bur..','Uccidi 10 Shaman','Uccidi 10 Shaman','Ritorna da Han\'al','110024','10', "Shaman Uccisi", 37711, 3),
-- (110094,'2','3840','12482','Ice Barrier indistruttibile','Uccidi 5 Mage','Uccidi 5 Mage','Ritorna da Han\'al','110025','5', "Mage Uccisi", 37711, 2),
-- (110095,'2','3840','12482','Ma quanto scappa col Blink?!','Uccidi 10 Mage','Uccidi 10 Mage','Ritorna da Han\'al','110025','10', "Mage Uccisi", 37711, 3);

-- UPDATE quest_template SET minlevel = 80, questlevel = 80, OfferRewardText = "Bravo, $N", RequestItemsText = "Si, $N?"  WHERE id >= 110040 AND id <= 110095;
-- UPDATE quest_template SET  QuestInfoID = 41, flags = 4098, RewardFactionID1 = 948, RewardFactionValue1 = 1, RewardFactionOverride1 = 0, unknown0 = 1, QuestSortID = -241 WHERE id >= 110040 AND id <= 110095;

DELETE FROM creature_questender WHERE id = 100001 AND quest >= 110040 AND quest <= 110095;
INSERT INTO creature_questender (id, quest) VALUES 
(100001, 110040),
(100001, 110041),
(100001, 110042),
(100001, 110043),
(100001, 110044),
(100001, 110045),
(100001, 110046),
(100001, 110047),
(100001, 110048),
(100001, 110049),
(100001, 110050),
(100001, 110051),
(100001, 110052),
(100001, 110053),
(100001, 110054),
(100001, 110055),
(100001, 110056),
(100001, 110057),
(100001, 110058),
(100001, 110059),
(100001, 110060),
(100001, 110061),
(100001, 110062),
(100001, 110063),
(100001, 110064),
(100001, 110065),
(100001, 110066),
(100001, 110067),
(100001, 110068),
(100001, 110069),
(100001, 110070),
(100001, 110071),
(100001, 110072),
(100001, 110073),
(100001, 110074),
(100001, 110075),
(100001, 110076),
(100001, 110077),
(100001, 110078),
(100001, 110079),
(100001, 110080),
(100001, 110081),
(100001, 110082),
(100001, 110083),
(100001, 110084),
(100001, 110085),
(100001, 110086),
(100001, 110087),
(100001, 110088),
(100001, 110089),
(100001, 110090),
(100001, 110091),
(100001, 110092),
(100001, 110093),
(100001, 110094),
(100001, 110095); 

UPDATE quest_template SET LogDescription = CONCAT(LogDescription, " in Battleground.") WHERE id <= 110095 AND id > 110055;
