DROP TABLE IF EXISTS `creature_alias`;
CREATE TABLE `creature_alias` (
  `guid` BIGINT(15) NOT NULL COMMENT 'guid from creature',
  `alias` CHAR(15) DEFAULT NULL,
  PRIMARY KEY (`guid`)
  );
  
  DROP TABLE IF EXISTS `gameobject_alias`;
CREATE TABLE `gameobject_alias` (
  `guid` BIGINT(15) NOT NULL COMMENT 'guid from creature',
  `alias` CHAR(15) DEFAULT NULL,
  PRIMARY KEY (`guid`)
  );