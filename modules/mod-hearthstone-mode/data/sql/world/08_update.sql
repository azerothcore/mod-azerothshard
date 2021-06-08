-- update daily quests reward to 75
UPDATE quest_template SET RewardFactionValue1 = 3 WHERE (id <= 110095 AND id >= 110040) OR (id >= 100000 AND id <= 100100);