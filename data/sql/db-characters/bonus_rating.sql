DROP TABLE IF EXISTS `rating_bonuses`;
CREATE TABLE `rating_bonuses` (
  `id` INT(10) NOT NULL,
  `type` INT(10) NOT NULL COMMENT '2 = 2v2, 3 = 3v3, 5 = 5v5, 1 = 1v1',
  `day` INT(10) DEFAULT '0' COMMENT '0 to 6, 0 = sunday',
  `startHour` INT(10) DEFAULT '12' COMMENT '0 to 23',
  `endHour` INT(10) DEFAULT '12' COMMENT '0 to 23',
  `multiplier` INT(10) DEFAULT '2',
  PRIMARY KEY (`id`)
);

-- domenica, dalle 21 alle 23 bonus 3x alle 5v5
INSERT INTO `rating_bonuses` VALUES (1, 5, 0, 21, 23, 3);
-- domenica, dalle 15 alle 17 bonus 3x alle 3v3
INSERT INTO `rating_bonuses` VALUES (2, 3, 0, 15, 17, 3);

-- giovedì, dalle 21 alle 23 bonus 3x alle 3v3
INSERT INTO `rating_bonuses` VALUES (3, 3, 4, 21, 23, 3);
-- giovedì, dalle 21 alle 23 bonus 3x alle 5v5
INSERT INTO `rating_bonuses` VALUES (4, 5, 4, 15, 17, 3);