-- First query is the daily quest
-- RewardFactionValue1 = 3 -> 75
-- RewardFactionValue1 = 1 -> 25
UPDATE quest_template SET RewardFactionID1 = 948, RewardFactionValue1 = 3, RewardItem2 = 37711, RewardAmount2 = 3 WHERE id IN (SELECT firstQuestId FROM lfg_dungeon_rewards);

-- this second query is always completed at each dungeon (except the first)
-- 
UPDATE quest_template SET RewardFactionID1 = 948, RewardFactionValue1 = 1 WHERE id IN (SELECT otherQuestId FROM lfg_dungeon_rewards);

-- revert
-- update quest_template set RewardFactionID1 = 0, RewardFactionValue1 = 0, RewardItem2 = 0, RewardAmount2 = 0 where id IN (SELECT firstQuestId FROM lfg_dungeon_rewards ) OR id IN (SELECT otherQuestId FROM lfg_dungeon_rewards);
