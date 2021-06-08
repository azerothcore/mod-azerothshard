#
# !!!!!RUN IT WHEN SERVER IS NOT RUNNING!!!!!
#

 
# clean achievement stats info of not existant players
SET @var := (SELECT GROUP_CONCAT(DISTINCT(playerGuid)) FROM azth_achievement_stats WHERE playerGuid NOT IN (SELECT guid FROM characters));
DELETE FROM azth_achievement_stats WHERE FIND_IN_SET(playerGuid, @var);
