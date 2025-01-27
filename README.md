# AzerothShard module

> [!IMPORTANT]  
> AzerothShard has released their collection of modules to the public and no longer being maintained.

## Modules that have been exported to AzerothCore

- `mod-arena-solo-3v3` => [mod-arena-3v3-solo-queue](https://github.com/azerothcore/mod-arena-3v3-solo-queue) 
- `mod-xp-rates` => [mod-chromie-xp](https://github.com/azerothcore/mod-chromie-xp)
- `mod-smartstone` => [mod-chromiecraft-smartstone](https://github.com/chromiecraft/mod-chromiecraft-smartstone)
- `mod-as-common` (Mythic) => [mod-zone-difficulty](https://github.com/azerothcore/mod-zone-difficulty)


> [!NOTE]  
> This module is a collection of custom features that have been implemented privately on AzerothShard project.
> Finally, most of them have be released open-source using an all-in-one module since they are coupled between eachother
> Although possiblity in future this modules being refactored or being moved on [AzerothCore](https://github.com/azerothcore) as many have from the list above.

## Installation

In your AzerothCore folder, run the following command in a bash shell:

`./acore.sh module install mod-azerothshard`

To uninstall:

`./acore.sh module uninstall mod-azerothshard`

## Configure

Create a copy of the `azth_mod.conf.dist` and rename it as `azth_mod.conf` under your etc folder
Then you can change configurations as you whish

## Features

List of features that will be published open-source:

- `Challenge Mode`
- `Mythic+`
- `PlayerStats`
- `Timewalking` (libraries only)
- `Guild house`
- Some useful `SQL` files

> [!NOTE]  
> The list above isn't completed. You can look into the source folders of this repository to see more details.
> List below we may or not one day be release on this repository or be ported to [AzerothCore](https://github.com/azerothcore)

- `Timewalking` (Full, not just libraries)
- `Multiple-dimensions`
