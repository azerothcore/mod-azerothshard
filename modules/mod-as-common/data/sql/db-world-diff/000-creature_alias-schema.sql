





DROP TABLE IF EXISTS `creature_alias`;


CREATE TABLE `creature_alias` (
  `guid` bigint(15) NOT NULL COMMENT 'guid from creature',
  `alias` char(15) DEFAULT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;








