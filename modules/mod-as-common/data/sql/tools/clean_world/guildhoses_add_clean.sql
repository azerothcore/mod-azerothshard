# remove unexistant objects from guildhouses_add

DELETE FROM guildhouses_add WHERE TYPE = 1 AND guid NOT IN ( SELECT guid FROM gameobject );

DELETE FROM guildhouses_add WHERE TYPE = 0 AND guid NOT IN ( SELECT guid FROM creature );
