CREATE TABLE `guildhouses` (
  `id` int(16) unsigned NOT NULL AUTO_INCREMENT,
  `guildId` bigint(20) NOT NULL DEFAULT '0',
  `x` double NOT NULL,
  `y` double NOT NULL,
  `z` double NOT NULL,
  `map` int(16) NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  `price` bigint(20) NOT NULL DEFAULT '0',
  `faction` int(8) unsigned NOT NULL DEFAULT '3',
  `minguildsize` int(16) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
