-- character inventory

DELETE FROM `character_inventory` WHERE `item` NOT IN (SELECT `guid` FROM `item_instance`);


-- [AZTH] character saved position

DELETE FROM character_saved_position WHERE charGuid NOT IN (
	SELECT guid FROM characters
)

-- [AZTH] smartstone commands

DELETE FROM character_smartstone_commands WHERE playerGuid NOT IN (
	SELECT guid FROM characters
)
