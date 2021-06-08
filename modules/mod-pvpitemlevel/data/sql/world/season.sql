-- --------------------------------------------------------
-- Host:                         server.wownewage.com
-- Versione server:              5.7.16-0ubuntu0.16.04.1 - (Ubuntu)
-- S.O. server:                  Linux
-- HeidiSQL Versione:            9.3.0.4984
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dump della struttura di tabella world.season
CREATE TABLE IF NOT EXISTS `season` (
  `itemLevel` int(11) NOT NULL,
  `startingFrom` int(11) NOT NULL,
  `endOn` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella world.season: ~1 rows (circa)
/*!40000 ALTER TABLE `season` DISABLE KEYS */;
/*!40000 ALTER TABLE `season` ENABLE KEYS */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
