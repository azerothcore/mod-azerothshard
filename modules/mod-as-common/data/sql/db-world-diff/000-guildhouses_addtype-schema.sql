





DROP TABLE IF EXISTS `guildhouses_addtype`;


CREATE TABLE `guildhouses_addtype` (
  `add_type` int(16) unsigned NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  `price` bigint(20) NOT NULL DEFAULT '1000',
  `minguildsize` int(16) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`add_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;








