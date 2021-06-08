





DROP TABLE IF EXISTS `gameobject_alias`;


CREATE TABLE `gameobject_alias` (
  `guid` bigint(15) NOT NULL COMMENT 'guid from creature',
  `alias` char(15) DEFAULT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;








