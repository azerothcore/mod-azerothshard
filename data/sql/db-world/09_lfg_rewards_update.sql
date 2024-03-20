UPDATE quest_template SET RewardFactionID1 = 948, RewardFactionValue1 = 3, RewardItem2 = 37711, RewardAmount2 = 3 WHERE id IN (SELECT firstQuestId FROM lfg_dungeon_rewards);

UPDATE quest_template SET RewardFactionID1 = 948, RewardFactionValue1 = 1 WHERE id IN (SELECT otherQuestId FROM lfg_dungeon_rewards);
