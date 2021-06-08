#include "AzthLanguageStrings.h"

void AzthLang::loadStrings() {
    this->add(AZTH_LANG_REMOVED_ITEMS,
              "Items removed from npc",
              "Item rimossi dall'npc");
    this->add(AZTH_LANG_PVP_NPC_SET_ADVICE,
              "Current equipped items will be moved in bag, make sure you have enough space. Do you want continue?",
              "Gli item attualmente equippati saranno spostati nella borsa, assicurati di avere abbastanza spazio. Vuoi proseguire?");
    this->add(AZTH_LANG_PVP_NPC_CANNOT_EQUIP,
              "Item |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r cannot be equipped, maybe it's unique and you already got or you don't have the requirements",
              "L'item |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r non può essere equippato, forse è unico e già lo possiedi oppure non hai i requisiti");
    this->add(AZTH_LANG_RESET_AURAS_ADVICE,
              "Reset aura can only be used while in rest state and not in combat! Go to an Inn",
              "Il comando reset aura può essere utilizzato solo mentre si è in rest e non in combat. Recati in una locanda.");
    this->add(AZTH_LANG_SHOW_BANK_ITEMS,
              "Show my deposited items, from %u to %u",
              "Mostra gli items depositati, da %u a %u");
    
    this->add(AZTH_LANG_INSTANCE_ONLY, "Must be in instance!", "Devi essere in un instance!");
    this->add(AZTH_LANG_RAID_COMBAT, "One of your raid member is in combat!", "Uno dei membri del raid è in combat!");
    
    this->add(AZTH_LANG_GROUP_LEVEL_REG, "|cffff0000 %s level has been registered in group with value: %s. Group size: %u|r", "|cffff0000 Il livello di %s è stato registrato all'interno del gruppo con il valore: %s. Dimensione gruppo: %u|r");
    this->add(AZTH_LANG_INSTANCE_LEVEL_REG, "|cffff0000 %s level has been registered in this instance with value: %s. Players in instance: %u|r", "|cffff0000 Il livello di %s è stato registrato all'interno dell'instance con il valore: %s. Players in instance: %u|r");
    
    //common
    this->add(AZTH_LANG_COMMON_TEMPORARILY, "temporarily", "temporaneamente");
    this->add(AZTH_LANG_COMMON_NOTVALIDPARAMTER, "Parameter not valid", "Parametro non valido");
    this->add(AZTH_LANG_CHECK_EMAIL, "Check your mailbox", "Controlla la tua mail!");
    this->add(AZTH_LANG_BAG_FULL_SENT_TO_MAIL, "Hai le borse piene! Controlla la mailbox!", "Hai le borse piene! Controlla la mailbox!");

    //collection npc
    this->add(AZTH_LANG_COLLECTION_DEPOSIT_ITEMS, "Store Items from your main backpack", "Deposita un item dalla tua borsa principale");
    this->add(AZTH_LANG_COLLECTION_SEARCH_ITEMS, "Search an item by name", "Ricerca un item per nome");
    this->add(AZTH_LANG_COLLECTION_SEARCH_ITEMS_CODE, "Insert name, and press accept button", "Inserisci il nome e premi sul bottone accept");
    this->add(AZTH_LANG_COLLECTION_ITEM_NOT_RECOVERED, "Item not recovered, unique? bag full?", "Item non recuperato, unique? borse piene?");
    this->add(AZTH_LANG_COLLECTION_ITEM_NOT_VALID, "This item is not valid", "Questo item non è valido");
    this->add(AZTH_LANG_COLLECTION_ITEM_WITH_DURATION, "Unable to store items with a duration", "Non è possibile depositare items con una durata temporanea!");
    this->add(AZTH_LANG_COLLECTION_ITEM_NOT_EMPTY_BAG, "Unable to store bag not empty", "Non è possibile depositare borse non vuote!");
    this->add(AZTH_LANG_COLLECTION_ITEM_NOT_STACKED, "Unable to store not stacked item", "Non possono essere depositati item raggruppati in un unico slot (stacked)");
    this->add(AZTH_LANG_COLLECTION_ITEM_ASHEN_BAND, "Unable to store the Ashen Band", "Non è possibile depositare l'Ashen Band!");
    this->add(AZTH_LANG_COLLECTION_ITEM_ALREADY_STORED, "You have already deposited this item", "Hai già depositato questo item!");
    this->add(AZTH_LANG_COLLECTION_ITEM_STORED, "Item stored", "Item depositato");

    //multidimension
    this->add(AZTH_LANG_MULTIDIMENSION_CLASSIC_EXPCHECK, "|cffff0000 You are in the Classic dimension, you can not access the maps of a most recent expansions|r", "|cffff0000 Sei nella dimensione Classic, non è possibile accedere a mappe di espansioni più recenti|r");
    this->add(AZTH_LANG_MULTIDIMENSION_TBC_EXPCHECK, "|cffff0000 You are in the TBC dimension, you can not access the maps of a most recent expansions|r", "|cffff0000 Sei nella dimensione TBC, non è possibile accedere a mappe di espansioni più recenti|r");
    this->add(AZTH_LANG_MULTIDIMENSION_GUILD_GROUPCHECK, "|cffff0000 You can not access in a group with characters of other guilds that are in Guild dimension|r", "|cffff0000 Non è possibile entrare in gruppo con personaggi di altre gilde presenti nella dimensione Guild World|r");
    this->add(AZTH_LANG_MULTIDIMENSION_TW_CHECK60, "You must be at level 60 or lower to access this dimension, try using Timewalking", "E' necessario essere di livello 60 o inferiore per entrare in questa dimensione, prova ad usare il Timewalking");
    this->add(AZTH_LANG_MULTIDIMENSION_TW_CHECK70, "You must be at level 70 or lower to access this dimension, try using Timewalking", "E' necessario essere di livello 70 o inferiore per entrare in questa dimensione, prova ad usare il Timewalking");
    this->add(AZTH_LANG_MULTIDIMENSION_GUILD_ACCESSCHECK, "You must be in a guild to access this dimension", "E' necessario essere in una gilda per entrare in questa dimensione!");
    this->add(AZTH_LANG_MULTIDIMENSION_GM_ACCESSCHECK, "You must be a Game Master to access this dimension", "E' necessario essere un Game Master per accedere a questa dimensione!");
    this->add(AZTH_LANG_MULTIDIMENSION_TEMP_TELEPORT, "|cffff0000 You have been teleported into the dimension ", "|cffff0000 Sei stato trasportato nella dimensione ");
    this->add(AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED, "|cffff0000 You cannot be summoned in this dimension!|r", "|cffff0000 Non puoi essere summonato in questa dimensione!|r");
    this->add(AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED_SUMMONER, "|cffff0000 You can not summon a player from dimension %s to dimension %s |r", "|cffff0000 Non puoi summonare il player dalla dimensione %s alla dimensione %s |r");

    //PvP instant Set/Items
    this->add(AZTH_LANG_PVPITEMS_LEVEL_CHECK, "|cffff0000|Hitem:%u::::::::::::|h[%s]|h|r has too high level!", "|cffff0000|Hitem:%u::::::::::::|h[%s]|h|r ha un livello troppo alto!");
    this->add(AZTH_LANG_PVPITEMS_MLEVEL_CHECK, "|cffff0000 Your medium item level is too high: %u|r", "|cffff0000 Il tuo item level medio è troppo alto: %u|r");
    this->add(AZTH_LANG_PVPITEMS_REMOVED, "The tournament PvP set has been removed", "Il set pvp tournament e' stato rimosso");
    this->add(AZTH_LANG_PVPITEMS_FORCE_REMOVED, "Your PVP set has been removed, you can not change zone with a temporary set equipped!", "Il tuo set PVP e' stato rimosso, non puoi cambiare zona con un set temporaneo!");

    //full pvp accounts
    this->add(AZTH_LANG_PVPACCOUNT_DUNGEON, "You are using a Full-PvP characters, you can not join raid or dungeons.", "Stai usando un personaggio Full-PvP, non puoi partecipare a raid/dungeons.");

    //custom commands
    this->add(AZTH_LANG_CCOMMANDS_QUEST, "Syntax: .qc $quest\n\nCheck if $quest has a bug.", "Sintassi: .qc $quest\n\nControlla se la $quest è buggata.");
    this->add(AZTH_LANG_CCOMANDS_QUEST_HASBUG, "[%s] has a bug!", "[%s] è buggata!");
    this->add(AZTH_LANG_CCOMANDS_QUEST_HASBUGANDCOMPLETED, "[%s] has a bug so it has been completed!", "[%s] è buggata ed è stata completata!");
    this->add(AZTH_LANG_CCOMANDS_QUEST_NOBUG, "[%s] not seems to has a bug, if you think it has, report it in bugtracker, thanks.", "[%s] non risulta essere buggata, se ritieni che lo sia ti preghiamo di segnalarcelo utilizzando il bugtracker.");

    //timewalking
    this->add(AZTH_LANG_TW_LEVEL_MAX, "This mode has maximum item level: %u", "Questa modalità ha l'item level massimo: %u");
    this->add(AZTH_LANG_TW_MODE_ON, "You are in |cffff0000 Timewalking level: %s|r mode", "Sei in modalità |cffff0000 Timewalking livello: %s|r");
    this->add(AZTH_LANG_TW_MODE_OFF, "You exited from |cffff0000 Timewalking|r mode!", "Sei uscito dalla modalità |cffff0000 Timewalking!|r");
    this->add(AZTH_LANG_TW_NEW_ITEM_OBTAINED, "Congrats! you got |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r x%d|r ", "Complimenti! hai ottenuto |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r x%d|r ");
    this->add(AZTH_LANG_TW_GROUP_CHECK_LEADER, "|cffff0000 Invited Player is not at your same Timewalking Level!|r", "|cffff0000 Il Player invitato non è al tuo stesso livello di Timewalking!|r");
    this->add(AZTH_LANG_TW_GROUP_CHECK_PLAYER, "|cffff0000 The Leader is in Timewalking, you must set Timewalking at same level of the leader party to join|r", "|cffff0000 Il Leader del party è in Timewalking. Devi settare il suo stesso livello per poter entrare nel party!|r");
    this->add(AZTH_LANG_TW_MOA_BONUS, "Marks of Azeroth bonus gained for level %s!", "Ottenuto bonus Marks of Azeroth per il livello %s!");
    this->add(AZTH_LANG_TW_ASREP_BONUS, "AzerothShard reputation bonus gained for level: %s!", "Ottenuto bonus reputazione con AzerothShard per il livello %s!");
    this->add(AZTH_LANG_TW_BOSS_KILLED, "Defeated %u of %u Bosses in this instance in %u minutes", "Sconfitti %u su %u Bosses in questa incursione. Tempo impiegato: %u minuti.");
    this->add(AZTH_LANG_TW_LAST_BOSS_KILLED, "Congratulations! Final Boss Defeated!", "Congratulazioni! Boss finale SCONFITTO!");
    this->add(AZTH_LANG_TW_INSTANCE_COMPLETED, "AMAZING! All bosses has been defeated!", "SBALORDITIVO! Tutti i boss sono stati sconfitti!");
    this->add(AZTH_LANG_TW_REP_REMOVED_KILL, "|cffff0000 AzerothShard Reputation Lost:|r you've killed a low level boss, shame on you!", "|cffff0000 Reputazione con AzerothShard persa:|r vergognati! hai ucciso un boss di basso livello!");
    this->add(AZTH_LANG_TW_REP_REMOVED_LOGOUT, "|cffff0000 AzerothShard Reputation Lost.|r last Adventure activity: %d days", "|cffff0000 Reputazione con AzerothShard persa.|r Ultima attività dell'Adventure: %d giorni.");

    //season tournament
    this->add(AZTH_LANG_TOURNAMENT_LEVEL_TOOHIGH, "|cffff0000|Hitem:%u::::::::::::|h[%s]|h|r has a too high level! Remove it to play this season.", "|cffff0000|Hitem:%u::::::::::::|h[%s]|h|r ha un livello troppo alto! Rimuovilo per poter giocare questa season.");
    this->add(AZTH_LANG_TOURNAMENT_LEVEL_ACTUAL, "Current season only allows items with |cffff0000%d|r maximum level", "L'attuale Season ha livello massimo |cffff0000%d|r");

    //ss
    this->add(AZTH_LANG_SS_README, "THINGS TO KNOW!", "COSE DA SAPERE!");
    this->add(AZTH_LANG_SS_TELEPORT_BACK, "Teleport me back", "Riportami indietro");
    this->add(AZTH_LANG_SS_TELEPORT_DISABLED, "Teleport has been currently disabled", "Il teleport è attualmente disabilitato");
    this->add(AZTH_LANG_SS_BACK, "Back", "Indietro");
    this->add(AZTH_LANG_SS_VALUE, "Insert a value" , "Inserisci un valore");
    this->add(AZTH_LANG_SS_POSSES_CHECK, "|CFF7BBEF7[SmartStone]|r: You already have a smartstone!", "|CFF7BBEF7[SmartStone]|r: Hai già una smartstone!");
    this->add(AZTH_LANG_SS_NEWAPP, "You have just unlocked a new app for your SmartStone", "Hai sbloccato una nuova app per la tua SmartStone!");
    this->add(AZTH_LANG_SS_CHANGE_RACE, "Relog to change race", "Rilogga per cambiare razza");
    this->add(AZTH_LANG_SS_CHANGE_FACTION, "Relog to change faction", "Rilogga per cambiare fazione");
    this->add(AZTH_LANG_SS_CHANGE_NAME, "Relog to change name!", "Rilogga per cambiare nome!");

    //hs
    this->add(AZTH_LANG_HS_QUESTS,"===== Quests: =====", "===== Missioni: =====");
    this->add(AZTH_LANG_HS_PVP_QUEST,"PvP Daily Quest: %s","Missione PvP giornaliera: %s");
    this->add(AZTH_LANG_HS_DAILY_QUEST,"PvE Daily Quest: %s","Missione PvE giornaliera: %s");
    this->add(AZTH_LANG_HS_WEEKLY_QUEST,"Weekly Quest: %s","Missione settimanale: %s");
    this->add(AZTH_LANG_HS_TW_QUESTS,"===== Adventure Quests: =====","===== Missioni Adventure: =====");
    this->add(AZTH_LANG_HS_TW_WEEKLY_QUEST,"Raid: %s","Incursione: %s");
    this->add(AZTH_LANG_HS_TW_WEEKLY_RANDOM_QUEST,"Random Raid: %s","Incursione casuale: %s");
    this->add(AZTH_LANG_HS_TW_DAILY_QUEST,"Daily quest: %s","Missione giornaliera: %s");
    this->add(AZTH_LANG_HS_TW_DAILY_RANDOM_QUEST,"Random Daily quest: %s","Missione giornaliera casuale: %s");
    
    this->add(AZTH_LANG_HS_QUEST_LIMIT_REACHED,"You reached the limit of this quest category in your quest log, remove or complete one before.","Hai raggiunto il limite massimo di quest di tale categoria nel tuo quest log, rimuovine o completane una prima.");
    this->add(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX,"[LIMIT REACHED]","[LIMITE RAGGIUNTO]");
    
    //bg item level
    this->add(AZTH_LANG_BG_ITEM_LEVEL_RESET, "Max item level set to its default value.","L'item level massimo e' stato riportato al valore predefinito.");
    this->add(AZTH_LANG_BG_ITEM_LEVEL_CHANGED, "Max item level changed.", "L'item level massimo e' stato cambiato.");
    
    //rating bonus
    this->add(AZTH_LANG_BR_ACTIVE_BONUS,"|CFF7BBEF7[Rating Bonus]|r: Active bonus on %s %.2fx.","|CFF7BBEF7[Rating Bonus]|r: Bonus attivo in %s %.2fx.");
    this->add(AZTH_LANG_BR_NO_BONUS,"|CFF7BBEF7[Rating Bonus]|r: There are no bonuses right now!.","|CFF7BBEF7[Rating Bonus]|r: Non ci sono bonus attivi!.");
    
    //donation vendor
    this->add(AZTH_LANG_DONATION_UPDATE_MENU, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tUpdate menu", "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tAggiorna");
    this->add(AZTH_LANG_DONATION_BACK, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tIndietro..");
    this->add(AZTH_LANG_DONATION_ITEM_RECEIVED, "Item received: |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r", "Item ricevuto: |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r");
    this->add(AZTH_LANG_DONATION_ITEM_NO_DB, "Error: item doesn't exist in database!", "Errore: l'item non esiste nel database!");
    
    //dimension npc
    this->add(AZTH_LANG_DIMENSION_CURRENT, "Currenlty you are on dimension: ", "Attualmente sei nella dimensione: ");
    this->add(AZTH_LANG_DIMENSION_ILLUSORY, "Illusory World: Standard Dimension", "[PRINCIPALE] Illusory World: Dimensione standard.");
    this->add(AZTH_LANG_DIMENSION_GUILD, "Guild World: Guild Houses dimension", "Guild World: Dimensione delle case di gilda.");
    this->add(AZTH_LANG_DIMENSION_PVP, "Corrupted World: PvP Dimension", "Corrupted World: Dimensione riservata al pvp");
    this->add(AZTH_LANG_DIMENSION_ENTERTAIN, "Toys Land: Entertaining World events", "Toys Land: Dimensione dedicata agli eventi di intrattenimento");
    this->add(AZTH_LANG_DIMENSION_GDR, "Warcraft Tales: RPG Dimension", "Warcraft Tales: Dimensione dedicata al GDR");
    this->add(AZTH_LANG_DIMENSION_LVL60, "Classic World: Level 60 Dimension", "Classic World: Dimensione con livello limite 60");
    this->add(AZTH_LANG_DIMENSION_LVL70, "TBC World: Level 70 Dimension", "TBC World: Dimensione con livello limite 70");
    this->add(AZTH_LANG_DIMENSION_TEST, "Test World: Dimension reserverd for test (be careful!)", "Test World: Dimensione riservata ai test (siate cauti!)");
    this->add(AZTH_LANG_DIMENSION_GM, "GameMaster World: Dimension for GM and Developers", "GameMaster World: Benvenuti GM e Developers, questo è il vostro mondo!");
}
