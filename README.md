# Valhalla Combat
  Valhalla Combat is an integrated and script-free combat overhaul designed with modern principles in mind. Heavily inspired by modern action games such as AC:Valhalla, God of War, and Elden Ring, Valhalla Combat will fundamentally change the feeling of Skrim's combat both in 1st and 3rd person, rendering a enjoyable and consistent experience.
# **Features**

## **Weightless**
Valhalla Combat brings a hefty amount of changes; however unlike other combat overhauls, all changes and functionalities are implemented through SKSE. Therefore it is neither "heavy-weight" nor "light-weight", it's weightless. You will not experience any script lag, nor save corruption if you ever decide to uninstall this mod mid-save. 

## **Attack**
- Light attack costs a fixed amount of stamina. Heavy attack costs a fixed portion of stamina. (Tweakable)
- The former incentivizes the player to level up. The latter keeps late-game characters and player from being over-powered. Specific values are tweakable.
- Hitting an enemy with light attacks replenishes stamina. (Tweakable)
- Hitting an enemy with heavy attacks costs reduced stamina. (Tweakable)
- Attack stamina consumption can be turned off in MCM while not in combat, so you can experiment with your movesets more freely like in Elden Ring.

_Stamina costs for attack is a common practice in modern action games; however, Skyrim's level design-along with all other additional levels from the modding community-makes it impossible to make a balanced stamina attack system. With one of those other stamina mods, you'd quickly find yourself surrounded by 20 bandits with 0 stamina; this is much more frustrating even than Dark Souls. Hitting enemies regenerate stamina balances this fact: you get stronger on well-timed hit and can keep your combos going, even when you are surrounded by 20 bandits._

## **Block**
- Blocking costs stamina only.
- When blocking, stamina does not regenerate.<br/>

_In vanilla Skyrim, blocking damages your health depending on your block level. This makes it impossible to play as a sword and shield main against higher level enemies. It becomes a stat contest. Making block cost stamina fixes this issue._
- When the blocker has less than enough stamina to block all damage from an incoming attack...
  - The unblocked portion of damage will still damage health.
  - If it's a light attack, the blocker gets staggered.
  - If it's a heavy attack, the blocker gets knocked back.<br/>

_It is extremely frustrating how shielded enemies block all the time. Many don't stop blocking until they are dead, and the recoil upon hitting a shield breaks the flow of combat. Now, actors out of stamina can no longer block.
Unlike any scripted workarounds, Valhalla Combat's stamina blocking implementation is compatible with all perk and magic overhauls._

## **Perfect Blocking/Parrying**
- Once you raise your shield/weapon, if you manage to block an attack in the following 0.25 sec(customizable), the block counts as a perfect block/Parry.
- Parrying reduces stamina damage caused by blocking to minimum.
- Parrying an attack at 0 stamina will neither lead to exhaustion nor knock-back.
- Parrying deflects the attacker's damage back as stun damage. 
- If the attacker has less than 0 stamina, parrying their attack knocks them back.

_Unlike all other perfect blocking mods(Deflection, Strike Obstruction Systems, Ultimate Combat, Inpa Sekiro Combat, Sekiro Combat, Wildcat, Ordinator, etc...), Valhalla Combat's perfect blocking has 0 script lag. Whether you can successfully parry an attack depends entirely on your skill, not how much script lag you have._

## **Exhaustion**
- When an attack/attempt to block an attack depletes stamina, the attacker enters a state of "exhaustion".
- During exhaustion, all attacks deal significantly less damage.
- During exhaustion, stamina meter greys out and blinks in red to help player distinguish. This requires TrueHud
- Actor will only become exhausted once stamina is fully depleted. 
- The exhaustion state goes away as soon as stamina full replenishes.

_Attacking&Blocking costs stamina, so there will be some penalties for not having enough stamina. Vanilla stamina penalty is outright useless, as it merely places a limitation on doing power attacks, which can be easily lifted by drinking a vegetable soup. Souls stamina penalty is way too punishing and limiting, as it leaves you with 0 countermeasures when out of stamina. Some mods implement procedural stamina penalty i.e. actors do more damage with more stamina; that disrupts the flow of combat as you're required to keep your stamina high. Giving actors debuff ONLY on stamina depletion is the perfect middleground._

## **Stamina**
- Stamina regenerates 5x faster for all races by default. (Tweakable)

_This is to offset the stamina cost of attacking&blocking. Unlike "Souls Like Stamina" or many other combat mods, stamina regeneration is not hard-coded into esp records, but multiplied based on the records you already have. This make stamina changes brought by Valhalla Combat consistent with all other race overhauls. 
When performing offensive actions(attacking/blocking/drawing a bow), stamina does not regenerate._

## **Stun/Execution**
- Every actor has their own separate stun value, based on their health and stamina.
- Use TrueHud to enable NPC stun meter, displaying the amount of stun they have on top of their health meter.
- Stun Damage
  - Every hit damages an actor's stun. 
  - Power attack damages significantly more stun than light. 
  - Blunt weapon damages moderately more stun than blades.
  - Parrying an attack also damages the attacker's stun.
  - Blocked attacks will not damage stun. 
- Stun does not regenerate during combat.
- When an actor's stun reaches 0, the actor can be executed.
