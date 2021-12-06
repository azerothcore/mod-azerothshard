DROP TABLE IF EXISTS `character_saved_position`;
CREATE TABLE `character_saved_position`(  
  `charGuid` INT(10) NOT NULL,
  `type` INT(10) NOT NULL,
  `posX` FLOAT(10) NOT NULL,
  `posY` FLOAT(10) NOT NULL,
  `posZ` FLOAT(10) NOT NULL,
  `mapId` FLOAT(10) NOT NULL
);

ALTER TABLE `character_saved_position`
  ADD CONSTRAINT `character_saved_position` UNIQUE(`charGuid`, `type`);