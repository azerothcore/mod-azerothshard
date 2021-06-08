





DROP TABLE IF EXISTS `guildhouses_add`;


CREATE TABLE `guildhouses_add` (
  `guid` int(32) unsigned NOT NULL,
  `type` int(16) unsigned NOT NULL,
  `id` int(16) unsigned NOT NULL,
  `add_type` int(16) unsigned NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;








