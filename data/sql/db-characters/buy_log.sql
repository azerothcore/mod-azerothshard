DROP TABLE IF EXISTS `buy_log`;
CREATE TABLE IF NOT EXISTS `buy_log` (
	`playerGuid` bigint (10),
	`item` int (10),
	`vendor` bigint (10),
	`price` bigint (10),
	`date` timestamp 
); 
