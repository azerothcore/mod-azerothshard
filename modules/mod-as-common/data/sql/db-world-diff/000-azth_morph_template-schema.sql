





DROP TABLE IF EXISTS `azth_morph_template`;


CREATE TABLE `azth_morph_template` (
  `entry` int(11) NOT NULL,
  `flags` int(11) DEFAULT '0',
  `default_name` text,
  `scale` float DEFAULT '1',
  `aura` int(11) DEFAULT '-1',
  `comment` text,
  UNIQUE KEY `entry` (`entry`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;








