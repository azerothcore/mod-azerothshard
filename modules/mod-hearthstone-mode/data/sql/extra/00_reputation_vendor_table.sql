DROP TABLE IF EXISTS `reputation_vendor`;
CREATE TABLE `reputation_vendor`(  
  `vendorId` INT(10) NOT NULL COMMENT 'vendor id',
  `reputation` INT(10) NOT NULL DEFAULT 948 COMMENT 'faction required',
  `value` INT(10) NOT NULL DEFAULT 3000 COMMENT 'how much reputation is required',
  `gossipSatisfied` INT(10) NOT NULL COMMENT 'gossip id when enough reputation',
  `gossipUnsatisfied` INT(10) NOT NULL COMMENT 'gossip id when not enough reputation',
  PRIMARY KEY (`vendorId`)
);

-- vendor pvp only
ALTER TABLE `reputation_vendor`
	ADD COLUMN `PvPVendor` INT(1) NOT NULL DEFAULT '0' AFTER `gossipUnsatisfied`;
