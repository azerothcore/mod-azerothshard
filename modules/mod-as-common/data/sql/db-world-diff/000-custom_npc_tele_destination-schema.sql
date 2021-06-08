





DROP TABLE IF EXISTS `custom_npc_tele_destination`;


CREATE TABLE `custom_npc_tele_destination` (
  `id` double DEFAULT NULL,
  `name` varchar(900) DEFAULT NULL,
  `pos_X` float DEFAULT NULL,
  `pos_Y` float DEFAULT NULL,
  `pos_Z` float DEFAULT NULL,
  `map` double DEFAULT NULL,
  `orientation` float DEFAULT NULL,
  `level` tinyint(3) DEFAULT NULL,
  `cost` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;








