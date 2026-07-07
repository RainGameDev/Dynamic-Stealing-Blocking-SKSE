# Watched Thieves Can't Steal

Stealing right in front of someone should be hard. Now it is.

In vanilla Skyrim, you can walk into a shop, look the owner dead in the eye, and pocket everything they own, the only consequence is a bounty. It never made sense that you could physically pick things up while being stared at.

This lightweight SKSE plugin fixes that. If an NPC can see you, you cannot take owned items at all. No stuffing your pockets in broad daylight. You'll get a quick message in the corner:

▎ "I can't steal this, I'm being watched"

Wait until nobody's looking, turn a corner, sneak into the shadows, or clear the room, and everything works exactly like vanilla again. It turns casual thievery into something that actually requires a moment of stealth, without adding menus, scripts, or bloat.

---

Features

- Watched = hands off. While any nearby NPC detects you, owned items can't be picked up.
- Unseen = business as usual. If no one can see you, stealing works exactly like vanilla.
- Shift to override. Need that item right now, consequences be damned? Hold Shift while activating to force the steal through.
- Only affects theft. Regular, unowned objects, loot, your own belongings, containers you're allowed to use — are never touched.
- Zero footprint. No new scripts, no ESP, no MCM, no save bloat. Just a single plugin.
- Fully SE / AE / VR compatible from one file.

---

Requirements

- SKSE64
- Address Library for SKSE Plugins

Installation

Install with your mod manager (MO2 / Vortex) and enable it, or drop the SKSE/Plugins folder into your Data directory manually. That's it, no configuration needed.

---

Q&A

Q: What exactly counts as "being watched"?
A: If any living NPC in your immediate area currently detects you (the same detection the game uses for sneaking), you're being watched. Break line of sight or get out of detection range and you're clear.

Q: Does sneaking help?
A: Yes. If you're sneaking and undetected, no one is watching you, so you can steal freely. That's the intended playstyle: actually be sneaky.

Q: I really need this item and someone's staring at me. What do I do?
A: Hold Shift and activate. That deliberately overrides the block and lets you take it (you'll still get the normal stealing bounty, of course).

Q: Will it block me from taking my own stuff, or looting bodies/containers?
A: No. It only blocks actions the game already flags as stealing. Anything that isn't a crime to take behaves normally.

Q: Does this stop pickpocketing?
A: No. This only affects picking up owned world items. Pickpocketing is its own mechanic and is untouched.

Q: Is it safe to add or remove mid-playthrough?
A: Yes. It doesn't run scripts or write anything to your save, so you can install or uninstall whenever you like.

Q: Any performance impact?
A: None you'll notice. The check only runs the moment you press the activate button on an owned item — never in the background.

Q: Compatibility?
A: Very high. It doesn't edit any records, cells, or NPCs, so it won't conflict with city overhauls, AI mods, or other gameplay tweaks.

---

Known behavior

"Being watched" currently counts any nearby NPC who can see you, including guards and your own followers. If there's demand, a future update could narrow this to only the item's owner or hostile witnesses. Let me know in the comments.
