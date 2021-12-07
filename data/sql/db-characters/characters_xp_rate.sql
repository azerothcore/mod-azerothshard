DROP TABLE IF EXISTS `character_xp_rate`;
CREATE TABLE IF NOT EXISTS `character_xp_rate` (
  `guid` int(10) UNSIGNED NOT NULL,
  `xp_rate` float UNSIGNED NOT NULL DEFAULT '1',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Player XP Rate System';