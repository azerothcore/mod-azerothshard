CREATE TABLE IF NOT EXISTS `pvp_accounts` (
  `id` int(11) DEFAULT NULL,
  `isPvP` int(11) DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `pvp_characters` (
  `id` int(11) DEFAULT NULL,
  `isPvP` int(11) DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
