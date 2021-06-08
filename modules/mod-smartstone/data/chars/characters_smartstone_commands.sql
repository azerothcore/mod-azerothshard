DROP TABLE IF EXISTS `character_smartstone_commands`;
CREATE TABLE `character_smartstone_commands`
( `playerGuid` INT(10) NOT NULL COMMENT 'player guid', 
`command` INT(10) NOT NULL COMMENT 'player command id',
  `dateExpired` BIGINT(10) NOT NULL DEFAULT '0',
  `charges` INT(10) NOT NULL DEFAULT '-1'); 

ALTER TABLE `character_smartstone_commands`
  ADD CONSTRAINT `character_smartstone_commands` UNIQUE(playerGuid, command);