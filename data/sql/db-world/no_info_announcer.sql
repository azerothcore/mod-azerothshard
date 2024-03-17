DELETE FROM `acore_string` WHERE `entry` IN (7000, 7001);
INSERT INTO `acore_string` (`entry`, `content_default`) VALUES
(7000, "|cffff0000[Arena Queue Announcer]:|r A team joined %ux%u! |r"),
(7001, "|cffff0000[Arena Queue Announcer]:|r A team exited %ux%u! |r");
