





DROP TABLE IF EXISTS `season`;


CREATE TABLE `season` (
  `itemLevel` int(11) NOT NULL,
  `startingFrom` int(11) NOT NULL,
  `endOn` int(11) NOT NULL,
  `id` tinyint(8) NOT NULL AUTO_INCREMENT,
  `season_num` tinyint(8) NOT NULL,
  `comment` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;








