Status: Active

# Character Library

**Migration note (February 6, 2026):** This roster is engine-agnostic. Gameplay and content integration should now be planned against the Unreal Engine 5 project.

This library defines NPC-ready character entries for **The Nazarene**. Each entry includes a suggested image asset, background summary, biblical references, NPC behavior, and a reusable library of voice lines.

## Asset conventions
- **Image path**: `assets/characters/<slug>.png`
- **Fallback image**: `assets/characters/placeholder.png`
- **Voice line library**: Short, reusable lines in tone-appropriate language for the character.

## Entry template
```
- Name:
  Image:
  Background:
  References:
  NPC Behavior:
  Voice Lines:
    -
    -
```

---

## Recurring & primary characters (expanded entries)

- Name: God (YHWH)
  Image: assets/characters/god.png
  Background: The covenant-making Creator and sovereign ruler, revealed across the Law, Prophets, and Gospel. The game should avoid anthropomorphic depiction and use light, wind, or voice motifs.
  References: Genesis 1; Exodus 3; Deuteronomy 6; Isaiah 45; John 1
  NPC Behavior: Non-combatant. Interacts via divine signs, scripture echoes, and guidance moments. Presence intensifies during covenant or worship scenes.
  Voice Lines:
    - "I am who I am."
    - "Walk in my ways and live."
    - "I will be with you."

- Name: Jesus (Yeshua)
  Image: assets/characters/jesus.png
  Background: Messiah and Son of God; teacher, healer, and redeemer who fulfills the Law and Prophets.
  References: Matthew 1–28; Mark 1–16; Luke 1–24; John 1–21
  NPC Behavior: Friendly mentor/quest-giver; heals, teaches, and guides the player toward mercy and courage.
  Voice Lines:
    - "Take heart; it is I."
    - "Follow me."
    - "Blessed are the merciful."

- Name: Mary (mother of Jesus)
  Image: assets/characters/mary.png
  Background: Faithful servant chosen to bear the Messiah; present in Jesus’ birth, ministry, crucifixion, and early church.
  References: Luke 1–2; John 2; John 19; Acts 1
  NPC Behavior: Compassionate support; offers prayer and encouragement.
  Voice Lines:
    - "Let it be to me according to your word."
    - "He has done great things for me."

- Name: Joseph (husband of Mary)
  Image: assets/characters/joseph_nazareth.png
  Background: Righteous carpenter who protected Mary and the infant Jesus.
  References: Matthew 1–2; Luke 2
  NPC Behavior: Provider and protector; crafts items and offers guidance.
  Voice Lines:
    - "We will obey the Lord’s command."
    - "Rest here; you are safe."

- Name: John the Baptist
  Image: assets/characters/john_baptist.png
  Background: Wilderness prophet and forerunner who calls Israel to repentance.
  References: Matthew 3; Mark 1; Luke 3; John 1
  NPC Behavior: Prophet NPC; offers baptism-themed quests.
  Voice Lines:
    - "Prepare the way of the Lord."
    - "Repent, for the kingdom is near."

- Name: Peter (Simon)
  Image: assets/characters/peter.png
  Background: Leading disciple and early church shepherd; bold but imperfect.
  References: Matthew 4; Matthew 16; Luke 22; John 21; Acts 1–12
  NPC Behavior: Companion fighter; strong frontline support with occasional hesitation.
  Voice Lines:
    - "Lord, to whom shall we go?"
    - "I will follow, even through the storm."

- Name: Paul (Saul of Tarsus)
  Image: assets/characters/paul.png
  Background: Former persecutor turned apostle to the Gentiles; prolific teacher.
  References: Acts 9; Acts 13–28; Romans–Philemon
  NPC Behavior: Strategist NPC; offers doctrinal quests and diplomatic options.
  Voice Lines:
    - "I was shown mercy."
    - "Press on toward the goal."

- Name: Moses
  Image: assets/characters/moses.png
  Background: Liberator and lawgiver; led Israel out of Egypt and received the Law.
  References: Exodus 1–40; Numbers; Deuteronomy
  NPC Behavior: Guidance NPC; triggers covenant or law-related mechanics.
  Voice Lines:
    - "Let my people go."
    - "Stand firm and see the Lord’s deliverance."

- Name: Abraham
  Image: assets/characters/abraham.png
  Background: Patriarch of faith; received the covenant promise.
  References: Genesis 12–25
  NPC Behavior: Wisdom elder; offers faith-test quests.
  Voice Lines:
    - "The Lord will provide."
    - "Walk before God and be blameless."

- Name: Sarah
  Image: assets/characters/sarah.png
  Background: Matriarch; mother of Isaac.
  References: Genesis 11–23
  NPC Behavior: Matronly counsel; encourages perseverance.
  Voice Lines:
    - "God has brought laughter."
    - "His promise stands."

- Name: Isaac
  Image: assets/characters/isaac.png
  Background: Covenant heir; father of Esau and Jacob.
  References: Genesis 21–35
  NPC Behavior: Peaceable elder; mediates conflicts.
  Voice Lines:
    - "The fields are ready."
    - "May the Lord give you peace."

- Name: Jacob (Israel)
  Image: assets/characters/jacob.png
  Background: Patriarch renamed Israel; father of the twelve tribes.
  References: Genesis 25–50
  NPC Behavior: Quest hub for tribal storylines.
  Voice Lines:
    - "The Lord is in this place."
    - "I will not let you go unless you bless me."

- Name: Joseph (son of Jacob)
  Image: assets/characters/joseph_egypt.png
  Background: Dreamer sold into slavery; rose to power in Egypt.
  References: Genesis 37–50
  NPC Behavior: Political ally; resource management quests.
  Voice Lines:
    - "What you meant for harm, God meant for good."
    - "Interpretation belongs to God."

- Name: Miriam
  Image: assets/characters/miriam.png
  Background: Prophetess and sister of Moses; leads worship after the Red Sea.
  References: Exodus 2; Exodus 15; Numbers 12
  NPC Behavior: Worship leader; morale boosting.
  Voice Lines:
    - "Sing to the Lord!"
    - "He has triumphed gloriously."

- Name: Aaron
  Image: assets/characters/aaron.png
  Background: First high priest; spokesman for Moses.
  References: Exodus 4–40; Leviticus; Numbers
  NPC Behavior: Priestly support; ritual blessing and healing.
  Voice Lines:
    - "The Lord bless you and keep you."
    - "Come, let us draw near."

- Name: Joshua
  Image: assets/characters/joshua.png
  Background: Moses’ successor; led Israel into Canaan.
  References: Joshua 1–24
  NPC Behavior: Military commander; tactical missions.
  Voice Lines:
    - "Be strong and courageous."
    - "As for me and my house, we will serve the Lord."

- Name: Deborah
  Image: assets/characters/deborah.png
  Background: Judge and prophetess who led Israel to victory.
  References: Judges 4–5
  NPC Behavior: Strategic advisor; inspires resistance.
  Voice Lines:
    - "Awake, arise."
    - "The Lord marches before you."

- Name: Gideon
  Image: assets/characters/gideon.png
  Background: Judge who delivered Israel with a small army.
  References: Judges 6–8
  NPC Behavior: Stealth-focused leader; ambush missions.
  Voice Lines:
    - "The Lord is with you, mighty warrior."
    - "Stand and watch."

- Name: Samson
  Image: assets/characters/samson.png
  Background: Judge with Nazarite strength; tragic fall.
  References: Judges 13–16
  NPC Behavior: High-damage ally; strength comes with risk.
  Voice Lines:
    - "Strength comes from the Lord."
    - "Let me finish this."

- Name: Ruth
  Image: assets/characters/ruth.png
  Background: Moabite widow who joined Israel and became ancestor of David.
  References: Ruth 1–4
  NPC Behavior: Loyal companion; supports farming and family quests.
  Voice Lines:
    - "Your people will be my people."
    - "I will stay with you."

- Name: Samuel
  Image: assets/characters/samuel.png
  Background: Prophet and judge who anointed Israel’s first kings.
  References: 1 Samuel 1–25
  NPC Behavior: Prophet NPC; anointing or calling sequences.
  Voice Lines:
    - "Speak, Lord, your servant is listening."
    - "Obedience is better than sacrifice."

- Name: Saul (King)
  Image: assets/characters/saul_king.png
  Background: Israel’s first king; tragic decline.
  References: 1 Samuel 9–31
  NPC Behavior: Unstable ally/antagonist; mood-driven behavior.
  Voice Lines:
    - "The Lord has given me a task."
    - "Why do you torment me?"

- Name: David
  Image: assets/characters/david.png
  Background: Shepherd, warrior, and king; ancestor of the Messiah.
  References: 1 Samuel 16–2 Samuel; Psalms
  NPC Behavior: Heroic ally; worship + warrior duality.
  Voice Lines:
    - "The Lord is my shepherd."
    - "You come against me with sword and spear."

- Name: Solomon
  Image: assets/characters/solomon.png
  Background: King known for wisdom; built the temple.
  References: 1 Kings 1–11; Proverbs; Ecclesiastes; Song of Songs
  NPC Behavior: Sage NPC; puzzle and diplomacy quests.
  Voice Lines:
    - "Give me wisdom to lead."
    - "Everything has its season."

- Name: Elijah
  Image: assets/characters/elijah.png
  Background: Prophet who challenged Baal and called down fire.
  References: 1 Kings 17–2 Kings 2
  NPC Behavior: Dramatic miracle moments; confrontational.
  Voice Lines:
    - "How long will you waver?"
    - "The Lord—He is God!"

- Name: Elisha
  Image: assets/characters/elisha.png
  Background: Successor to Elijah; performed many miracles.
  References: 2 Kings 2–13
  NPC Behavior: Healer and miracle NPC; recovery quests.
  Voice Lines:
    - "Let the Lord open your eyes."
    - "Go in peace."

- Name: Isaiah
  Image: assets/characters/isaiah.png
  Background: Major prophet with visions of holiness and Messiah.
  References: Isaiah 1–66
  NPC Behavior: Visionary; prophecy-driven quests.
  Voice Lines:
    - "Here am I; send me."
    - "Comfort, comfort my people."

- Name: Jeremiah
  Image: assets/characters/jeremiah.png
  Background: Weeping prophet; warned of exile.
  References: Jeremiah 1–52; Lamentations
  NPC Behavior: Somber guide; warns of consequences.
  Voice Lines:
    - "Return, while there is time."
    - "His mercies are new every morning."

- Name: Ezekiel
  Image: assets/characters/ezekiel.png
  Background: Prophet of exile with symbolic visions.
  References: Ezekiel 1–48
  NPC Behavior: Vision quests; symbolic encounters.
  Voice Lines:
    - "The glory of the Lord rose."
    - "A new heart will I give you."

- Name: Daniel
  Image: assets/characters/daniel.png
  Background: Exile prophet; interpreted dreams.
  References: Daniel 1–12
  NPC Behavior: Wisdom NPC; dream interpretation events.
  Voice Lines:
    - "God has shown what is to come."
    - "We will not defile ourselves."

- Name: Esther
  Image: assets/characters/esther.png
  Background: Queen who saved her people.
  References: Esther 1–10
  NPC Behavior: Diplomatic quest-giver; stealth court missions.
  Voice Lines:
    - "For such a time as this."
    - "I will go to the king."

- Name: Nehemiah
  Image: assets/characters/nehemiah.png
  Background: Rebuilder of Jerusalem’s walls.
  References: Nehemiah 1–13
  NPC Behavior: Builder NPC; restoration quests.
  Voice Lines:
    - "Let us rise and build."
    - "The joy of the Lord is our strength."

- Name: Ezra
  Image: assets/characters/ezra.png
  Background: Scribe and reformer; restored the Law.
  References: Ezra 7–10
  NPC Behavior: Scholar NPC; teaches scripture mechanics.
  Voice Lines:
    - "The Law must be read clearly."
    - "We return to the covenant."

- Name: Job
  Image: assets/characters/job.png
  Background: Righteous sufferer; faithful amid trials.
  References: Job 1–42
  NPC Behavior: Endurance mentor; resilience quests.
  Voice Lines:
    - "Though he slay me, yet will I hope."
    - "The Lord gave, and the Lord has taken away."

- Name: Jonah
  Image: assets/characters/jonah.png
  Background: Reluctant prophet to Nineveh.
  References: Jonah 1–4
  NPC Behavior: Humorous but sobering; mercy-focused missions.
  Voice Lines:
    - "I knew you are gracious."
    - "From the depths I called."

- Name: Mary Magdalene
  Image: assets/characters/mary_magdalene.png
  Background: Disciple healed by Jesus; first witness of resurrection.
  References: Luke 8; John 20; Matthew 28
  NPC Behavior: Loyal ally; resurrection witness quests.
  Voice Lines:
    - "I have seen the Lord."
    - "He called my name."

- Name: John (apostle)
  Image: assets/characters/john_apostle.png
  Background: Beloved disciple; author of Gospel and Revelation.
  References: John 13–21; 1–3 John; Revelation
  NPC Behavior: Contemplative guide; love and faith themes.
  Voice Lines:
    - "God is love."
    - "We have seen and testify."

- Name: James (brother of Jesus)
  Image: assets/characters/james_brother.png
  Background: Leader in Jerusalem church; emphasizes faithful action.
  References: Acts 15; James 1–5
  NPC Behavior: Practical mentor; moral decision quests.
  Voice Lines:
    - "Faith works through love."
    - "Do not merely listen."

- Name: Judas Iscariot
  Image: assets/characters/judas.png
  Background: Disciple who betrayed Jesus.
  References: Matthew 26–27; Mark 14; Luke 22; John 13
  NPC Behavior: Antagonist; betrayal plotline.
  Voice Lines:
    - "What will you give me?"
    - "Surely not I."

- Name: Mary and Martha
  Image: assets/characters/mary_martha.png
  Background: Sisters of Bethany; close to Jesus.
  References: Luke 10; John 11–12
  NPC Behavior: Hospitality-focused NPCs; support and grief scenes.
  Voice Lines:
    - "Lord, if you had been here..."
    - "I believe you are the Messiah."

- Name: Lazarus
  Image: assets/characters/lazarus.png
  Background: Brother of Mary and Martha; raised from the dead.
  References: John 11–12
  NPC Behavior: Symbol of hope; resurrection-focused quests.
  Voice Lines:
    - "I was in the tomb."
    - "The Lord called me out."

- Name: Pilate
  Image: assets/characters/pilate.png
  Background: Roman governor who presided over Jesus’ trial.
  References: Matthew 27; Mark 15; Luke 23; John 18–19
  NPC Behavior: Political antagonist; moral dilemma choices.
  Voice Lines:
    - "What is truth?"
    - "I find no fault."

- Name: Herod Antipas
  Image: assets/characters/herod_antipas.png
  Background: Tetrarch who beheaded John the Baptist.
  References: Mark 6; Luke 9; Luke 23
  NPC Behavior: Cruel ruler; palace intrigue.
  Voice Lines:
    - "Bring me the prophet."
    - "I will hear him."

- Name: Caiaphas
  Image: assets/characters/caiaphas.png
  Background: High priest who led the charge against Jesus.
  References: John 18; Matthew 26
  NPC Behavior: Religious authority antagonist.
  Voice Lines:
    - "It is better that one die."
    - "We must protect the nation."

- Name: Satan (the Adversary)
  Image: assets/characters/satan.png
  Background: Spiritual adversary; tempter and accuser.
  References: Genesis 3; Job 1–2; Matthew 4; Revelation 12
  NPC Behavior: Antagonist; psychological temptations.
  Voice Lines:
    - "Did God really say...?"
    - "All this I will give you."

- Name: Gabriel
  Image: assets/characters/gabriel.png
  Background: Messenger angel who delivered key announcements.
  References: Daniel 8–9; Luke 1
  NPC Behavior: Herald NPC; delivers divine messages.
  Voice Lines:
    - "I stand in the presence of God."
    - "Do not be afraid."

- Name: Michael
  Image: assets/characters/michael.png
  Background: Archangel and defender in spiritual battles.
  References: Daniel 10; Jude 1; Revelation 12
  NPC Behavior: Guardian ally in climactic scenes.
  Voice Lines:
    - "The Lord rebuke you."
    - "Stand firm in the battle."

---

## Comprehensive character index (condensed entries)

> The entries below cover **every remaining named or explicitly recurring figure** in the Bible, plus a book-level entry for unnamed crowds and roles in each book. Each entry uses a compact format to ensure the library remains manageable while still providing an image, background, references, NPC behavior, and voice lines.

### Genesis
- Name: Adam
  Image: assets/characters/adam.png
  Background: First human created by God.
  References: Genesis 1–5
  NPC Behavior: Reflective mentor on origin and responsibility.
  Voice Lines:
    - "We were made from the dust."
    - "Guard what was entrusted to you."

- Name: Eve
  Image: assets/characters/eve.png
  Background: First woman; partner to Adam.
  References: Genesis 2–4
  NPC Behavior: Wisdom and regret; emphasizes choice.
  Voice Lines:
    - "Life is a gift."
    - "Choose wisely."

- Name: Cain
  Image: assets/characters/cain.png
  Background: Firstborn; murdered Abel.
  References: Genesis 4
  NPC Behavior: Cautionary NPC; consequences of envy.
  Voice Lines:
    - "Sin crouches at the door."
    - "Am I my brother’s keeper?"

- Name: Abel
  Image: assets/characters/abel.png
  Background: Righteous shepherd; faithful offering.
  References: Genesis 4
  NPC Behavior: Quiet witness; sacrificial themes.
  Voice Lines:
    - "Offer your best."
    - "The Lord sees the heart."

- Name: Seth
  Image: assets/characters/seth.png
  Background: Third son of Adam and Eve; line of promise.
  References: Genesis 4–5
  NPC Behavior: Legacy-focused elder.
  Voice Lines:
    - "Call on the name of the Lord."
    - "A new line begins."

- Name: Noah
  Image: assets/characters/noah.png
  Background: Righteous man; built the ark.
  References: Genesis 6–9
  NPC Behavior: Survival mentor; flood-related quests.
  Voice Lines:
    - "Build according to the word."
    - "Grace found me."

- Name: Shem
  Image: assets/characters/shem.png
  Background: Son of Noah; ancestor of Abraham.
  References: Genesis 5–11
  NPC Behavior: Lineage guide.
  Voice Lines:
    - "The line continues."
    - "Blessed be the Lord."

- Name: Ham
  Image: assets/characters/ham.png
  Background: Son of Noah.
  References: Genesis 5–10
  NPC Behavior: Regional ancestor.
  Voice Lines:
    - "Our family scattered."
    - "We learned hard lessons."

- Name: Japheth
  Image: assets/characters/japheth.png
  Background: Son of Noah.
  References: Genesis 5–10
  NPC Behavior: Regional ancestor.
  Voice Lines:
    - "May God enlarge your tents."
    - "We journeyed far."

- Name: Nimrod
  Image: assets/characters/nimrod.png
  Background: Mighty hunter; associated with Babel.
  References: Genesis 10–11
  NPC Behavior: Power-hungry antagonist.
  Voice Lines:
    - "My kingdom will grow."
    - "No one will stop us."

- Name: Terah
  Image: assets/characters/terah.png
  Background: Father of Abram; led family toward Canaan.
  References: Genesis 11
  NPC Behavior: Elder advising migrations.
  Voice Lines:
    - "We left our homeland."
    - "The journey begins."

- Name: Lot
  Image: assets/characters/lot.png
  Background: Abraham’s nephew; rescued from Sodom.
  References: Genesis 13–19
  NPC Behavior: Survivor; warns against compromise.
  Voice Lines:
    - "Do not look back."
    - "We must leave now."

- Name: Hagar
  Image: assets/characters/hagar.png
  Background: Servant of Sarah; mother of Ishmael.
  References: Genesis 16; 21
  NPC Behavior: Outcast turned survivor; resilience quests.
  Voice Lines:
    - "The Lord sees me."
    - "He hears my cry."

- Name: Ishmael
  Image: assets/characters/ishmael.png
  Background: Son of Abraham and Hagar.
  References: Genesis 16–21; 25
  NPC Behavior: Wilderness survivor.
  Voice Lines:
    - "We lived by the well."
    - "God provided."

- Name: Rebekah
  Image: assets/characters/rebekah.png
  Background: Wife of Isaac; mother of Esau and Jacob.
  References: Genesis 24–27
  NPC Behavior: Strategist; family choices.
  Voice Lines:
    - "The younger will prevail."
    - "Be strong, my son."

- Name: Esau
  Image: assets/characters/esau.png
  Background: Elder twin of Jacob; hunter.
  References: Genesis 25–36
  NPC Behavior: Warrior hunter; reconciliation arcs.
  Voice Lines:
    - "I have enough."
    - "Let peace stand between us."

- Name: Rachel
  Image: assets/characters/rachel.png
  Background: Jacob’s beloved wife; mother of Joseph and Benjamin.
  References: Genesis 29–35
  NPC Behavior: Gentle presence; family quests.
  Voice Lines:
    - "The Lord remembers."
    - "My son, my joy."

- Name: Leah
  Image: assets/characters/leah.png
  Background: Jacob’s wife; mother of many tribes.
  References: Genesis 29–35
  NPC Behavior: Grounded matriarch; perseverance.
  Voice Lines:
    - "The Lord has seen my affliction."
    - "This time I will praise the Lord."

- Name: Benjamin
  Image: assets/characters/benjamin.png
  Background: Youngest son of Jacob; tribe founder.
  References: Genesis 35–49
  NPC Behavior: Loyal ally; family unity.
  Voice Lines:
    - "I will stand with you."
    - "Our family endures."

- Name: Judah
  Image: assets/characters/judah.png
  Background: Son of Jacob; ancestor of David.
  References: Genesis 37–49
  NPC Behavior: Leadership arc; redemption quests.
  Voice Lines:
    - "Let me bear the blame."
    - "We will not abandon him."

- Name: Tamar
  Image: assets/characters/tamar.png
  Background: Judah’s daughter-in-law; preserved lineage.
  References: Genesis 38
  NPC Behavior: Survivor; justice themes.
  Voice Lines:
    - "Justice is due."
    - "Truth will stand."

- Name: Asenath
  Image: assets/characters/asenath.png
  Background: Wife of Joseph in Egypt.
  References: Genesis 41
  NPC Behavior: Court ally.
  Voice Lines:
    - "Peace within the palace."
    - "The Lord provides."

- Name: Manasseh
  Image: assets/characters/manasseh.png
  Background: Son of Joseph; tribe founder.
  References: Genesis 41; 48
  NPC Behavior: Tribal elder.
  Voice Lines:
    - "Our inheritance is wide."
    - "May the Lord bless."

- Name: Ephraim
  Image: assets/characters/ephraim.png
  Background: Son of Joseph; tribe founder.
  References: Genesis 41; 48
  NPC Behavior: Tribal elder.
  Voice Lines:
    - "We flourish in the land."
    - "Blessed beyond measure."

- Name: Unnamed Genesis Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Shepherds, kings, servants, and peoples in Genesis narratives.
  References: Genesis 1–50
  NPC Behavior: Ambient villagers, merchants, and travelers.
  Voice Lines:
    - "We heard of the covenant."
    - "The land is changing."

### Exodus
- Name: Pharaoh (Exodus)
  Image: assets/characters/pharaoh_exodus.png
  Background: Ruler who opposed Moses.
  References: Exodus 1–14
  NPC Behavior: Major antagonist; pride-driven decisions.
  Voice Lines:
    - "Who is the Lord?"
    - "I will not let them go."

- Name: Zipporah
  Image: assets/characters/zipporah.png
  Background: Wife of Moses.
  References: Exodus 2–4
  NPC Behavior: Supportive family NPC.
  Voice Lines:
    - "We will go with you."
    - "The Lord protects."

- Name: Jethro
  Image: assets/characters/jethro.png
  Background: Priest of Midian; father-in-law of Moses.
  References: Exodus 2; 18
  NPC Behavior: Advisor; governance quests.
  Voice Lines:
    - "Choose capable leaders."
    - "Do not carry the burden alone."

- Name: Bezalel
  Image: assets/characters/bezalel.png
  Background: Craftsman of the tabernacle.
  References: Exodus 31; 35–38
  NPC Behavior: Artisan NPC; crafting upgrades.
  Voice Lines:
    - "Skill is a gift."
    - "Let us build with care."

- Name: Oholiab
  Image: assets/characters/oholiab.png
  Background: Craftsman of the tabernacle.
  References: Exodus 31; 35–38
  NPC Behavior: Artisan NPC.
  Voice Lines:
    - "Work with excellence."
    - "This is sacred craft."

- Name: Unnamed Exodus Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Israelite families, elders, and Egyptian officers.
  References: Exodus 1–40
  NPC Behavior: Ambient allies and skeptics.
  Voice Lines:
    - "The sea opened before us."
    - "We followed the cloud."

### Leviticus
- Name: Nadab
  Image: assets/characters/nadab.png
  Background: Son of Aaron; priest who offered unauthorized fire.
  References: Leviticus 10
  NPC Behavior: Cautionary priest.
  Voice Lines:
    - "Serve with reverence."
    - "Do not rush the holy."

- Name: Abihu
  Image: assets/characters/abihu.png
  Background: Son of Aaron; priest who offered unauthorized fire.
  References: Leviticus 10
  NPC Behavior: Cautionary priest.
  Voice Lines:
    - "Honor the commands."
    - "Approach with fear."

- Name: Eleazar
  Image: assets/characters/eleazar.png
  Background: Son of Aaron; priestly leader.
  References: Leviticus 10; Numbers 3–4
  NPC Behavior: Ritual guide.
  Voice Lines:
    - "Keep the altar burning."
    - "Let holiness endure."

- Name: Ithamar
  Image: assets/characters/ithamar.png
  Background: Son of Aaron; priestly leader.
  References: Exodus 6; Leviticus 10
  NPC Behavior: Ritual guide.
  Voice Lines:
    - "Order the camp well."
    - "We serve with care."

- Name: Unnamed Leviticus Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Priests and people under the Law.
  References: Leviticus 1–27
  NPC Behavior: Temple attendants.
  Voice Lines:
    - "Bring your offering."
    - "The Law teaches us."

### Numbers
- Name: Korah
  Image: assets/characters/korah.png
  Background: Led rebellion against Moses.
  References: Numbers 16
  NPC Behavior: Rebel antagonist.
  Voice Lines:
    - "You have gone too far."
    - "We also are holy."

- Name: Balaam
  Image: assets/characters/balaam.png
  Background: Diviner hired to curse Israel; blessed instead.
  References: Numbers 22–24
  NPC Behavior: Ambivalent seer; branching dialogue.
  Voice Lines:
    - "How can I curse whom God has blessed?"
    - "I see him, but not now."

- Name: Balak
  Image: assets/characters/balak.png
  Background: King of Moab; hired Balaam.
  References: Numbers 22–24
  NPC Behavior: Political antagonist.
  Voice Lines:
    - "Curse them for me."
    - "My kingdom is threatened."

- Name: Phinehas
  Image: assets/characters/phinehas.png
  Background: Priest noted for zeal.
  References: Numbers 25
  NPC Behavior: Zealot guardian.
  Voice Lines:
    - "Zeal for holiness."
    - "The covenant is secure."

- Name: Unnamed Numbers Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Israelite camp members and scouts.
  References: Numbers 1–36
  NPC Behavior: Camp NPCs; logistics and morale.
  Voice Lines:
    - "The desert is long."
    - "We march by the trumpet."

### Deuteronomy
- Name: Unnamed Deuteronomy Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Israel gathered to hear the Law.
  References: Deuteronomy 1–34
  NPC Behavior: Listeners and recorders.
  Voice Lines:
    - "We will obey the covenant."
    - "The words are life."

### Joshua
- Name: Rahab
  Image: assets/characters/rahab.png
  Background: Jericho innkeeper who protected Israelite spies.
  References: Joshua 2; 6
  NPC Behavior: Courageous ally; shelter quests.
  Voice Lines:
    - "I know the Lord has given you the land."
    - "Hide here until it passes."

- Name: Achan
  Image: assets/characters/achan.png
  Background: Disobeyed by taking devoted items.
  References: Joshua 7
  NPC Behavior: Cautionary NPC about greed.
  Voice Lines:
    - "I coveted and took."
    - "Do not break the command."

- Name: Unnamed Joshua Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Soldiers, priests, and townspeople in conquest narratives.
  References: Joshua 1–24
  NPC Behavior: Battle-ready NPCs.
  Voice Lines:
    - "The walls fell."
    - "We marched with the ark."

### Judges
- Name: Othniel
  Image: assets/characters/othniel.png
  Background: First judge of Israel.
  References: Judges 3
  NPC Behavior: Warrior leader.
  Voice Lines:
    - "The Spirit came upon me."
    - "Deliverance is here."

- Name: Ehud
  Image: assets/characters/ehud.png
  Background: Left-handed deliverer who defeated Eglon.
  References: Judges 3
  NPC Behavior: Stealth assassin NPC.
  Voice Lines:
    - "I have a message for you."
    - "The Lord delivered us."

- Name: Shamgar
  Image: assets/characters/shamgar.png
  Background: Judge who struck down Philistines.
  References: Judges 3
  NPC Behavior: Brief cameo warrior.
  Voice Lines:
    - "The oxgoad is enough."
    - "Stand your ground."

- Name: Jael
  Image: assets/characters/jael.png
  Background: Defeated Sisera.
  References: Judges 4–5
  NPC Behavior: Stealth ally.
  Voice Lines:
    - "Come in, my lord."
    - "The enemy is gone."

- Name: Barak
  Image: assets/characters/barak.png
  Background: Commander under Deborah.
  References: Judges 4–5
  NPC Behavior: Tactical commander.
  Voice Lines:
    - "If you go with me, I will go."
    - "The Lord has gone ahead."

- Name: Jephthah
  Image: assets/characters/jephthah.png
  Background: Judge who made a rash vow.
  References: Judges 11–12
  NPC Behavior: Tragic warrior; moral choice arcs.
  Voice Lines:
    - "I made a vow to the Lord."
    - "Consider the cost."

- Name: Unnamed Judges Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Tribal families, enemies, and villagers.
  References: Judges 1–21
  NPC Behavior: Ambient NPCs; tribal tensions.
  Voice Lines:
    - "Every tribe fights alone."
    - "We need a deliverer."

### Ruth
- Name: Boaz
  Image: assets/characters/boaz.png
  Background: Kinsman-redeemer who married Ruth.
  References: Ruth 2–4
  NPC Behavior: Benevolent landowner.
  Voice Lines:
    - "May the Lord repay you."
    - "You are under his wings."

- Name: Naomi
  Image: assets/characters/naomi.png
  Background: Ruth’s mother-in-law; returned to Bethlehem.
  References: Ruth 1–4
  NPC Behavior: Elder guide; grief and hope.
  Voice Lines:
    - "Call me Mara."
    - "The Lord has not forsaken us."

- Name: Unnamed Ruth Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Bethlehem community.
  References: Ruth 1–4
  NPC Behavior: Village life NPCs.
  Voice Lines:
    - "A redeemer has come."
    - "Blessed be the Lord."

### 1–2 Samuel
- Name: Hannah
  Image: assets/characters/hannah.png
  Background: Mother of Samuel; prayed for a child.
  References: 1 Samuel 1–2
  NPC Behavior: Prayer-focused NPC.
  Voice Lines:
    - "My heart rejoices in the Lord."
    - "He lifted the humble."

- Name: Eli
  Image: assets/characters/eli.png
  Background: Priest at Shiloh; mentor to Samuel.
  References: 1 Samuel 1–4
  NPC Behavior: Elder priest; warns of decline.
  Voice Lines:
    - "It is the Lord; let him do what seems good."
    - "Listen for his call."

- Name: Jonathan
  Image: assets/characters/jonathan.png
  Background: Saul’s son; loyal friend to David.
  References: 1 Samuel 13–31
  NPC Behavior: Loyal companion; friendship quests.
  Voice Lines:
    - "The Lord saves by many or by few."
    - "Our covenant stands."

- Name: Abigail
  Image: assets/characters/abigail.png
  Background: Wise wife of Nabal; later married David.
  References: 1 Samuel 25
  NPC Behavior: Diplomatic peacemaker.
  Voice Lines:
    - "Please accept my gift."
    - "Wisdom turns aside anger."

- Name: Nathan
  Image: assets/characters/nathan.png
  Background: Prophet who confronted David.
  References: 2 Samuel 7; 12
  NPC Behavior: Conscience of the king.
  Voice Lines:
    - "You are the man."
    - "The Lord has spoken."

- Name: Bathsheba
  Image: assets/characters/bathsheba.png
  Background: Wife of David; mother of Solomon.
  References: 2 Samuel 11–12; 1 Kings 1
  NPC Behavior: Court advisor; maternal influence.
  Voice Lines:
    - "Remember the promise."
    - "The kingdom needs wisdom."

- Name: Absalom
  Image: assets/characters/absalom.png
  Background: David’s son; led rebellion.
  References: 2 Samuel 13–18
  NPC Behavior: Charismatic antagonist.
  Voice Lines:
    - "I will be the judge of Israel."
    - "The people are with me."

- Name: Unnamed Samuel Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Soldiers, priests, and citizens of Israel.
  References: 1–2 Samuel
  NPC Behavior: Court and battlefield NPCs.
  Voice Lines:
    - "The king’s trumpet sounds."
    - "We wait for the prophet."

### 1–2 Kings
- Name: Rehoboam
  Image: assets/characters/rehoboam.png
  Background: Son of Solomon; kingdom split under his rule.
  References: 1 Kings 12
  NPC Behavior: Proud ruler; political tension.
  Voice Lines:
    - "My little finger is thicker."
    - "You will serve me."

- Name: Jeroboam
  Image: assets/characters/jeroboam.png
  Background: First king of northern Israel.
  References: 1 Kings 12–14
  NPC Behavior: Idol-prompting antagonist.
  Voice Lines:
    - "Here are your gods."
    - "The kingdom is mine."

- Name: Ahab
  Image: assets/characters/ahab.png
  Background: King influenced by Jezebel.
  References: 1 Kings 16–22
  NPC Behavior: Weak ruler; moral compromise.
  Voice Lines:
    - "Troubler of Israel."
    - "Bring me the prophet."

- Name: Jezebel
  Image: assets/characters/jezebel.png
  Background: Queen who promoted Baal worship.
  References: 1 Kings 16–21; 2 Kings 9
  NPC Behavior: Manipulative antagonist.
  Voice Lines:
    - "Who is the Lord?"
    - "Serve the gods of the land."

- Name: Hezekiah
  Image: assets/characters/hezekiah.png
  Background: King who trusted God during Assyrian threat.
  References: 2 Kings 18–20; Isaiah 36–39
  NPC Behavior: Faithful king; defense quests.
  Voice Lines:
    - "We trust the Lord."
    - "The battle is not ours."

- Name: Josiah
  Image: assets/characters/josiah.png
  Background: Reforming king who rediscovered the Law.
  References: 2 Kings 22–23
  NPC Behavior: Reformer; cleansing missions.
  Voice Lines:
    - "Return to the covenant."
    - "Read the Book."

- Name: Unnamed Kings Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Kings, prophets, soldiers, and citizens.
  References: 1–2 Kings
  NPC Behavior: Court and battlefield NPCs.
  Voice Lines:
    - "The king has spoken."
    - "The prophet warns us."

### 1–2 Chronicles
- Name: Unnamed Chronicles Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Genealogical and temple personnel.
  References: 1–2 Chronicles
  NPC Behavior: Temple administrators.
  Voice Lines:
    - "The records are kept."
    - "Serve with gladness."

### Ezra–Nehemiah
- Name: Zerubbabel
  Image: assets/characters/zerubbabel.png
  Background: Governor leading return from exile.
  References: Ezra 2–6; Haggai 1–2
  NPC Behavior: Restoration leader.
  Voice Lines:
    - "We will rebuild."
    - "The Lord strengthens us."

- Name: Unnamed Ezra-Nehemiah Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Returnees, builders, and officials.
  References: Ezra–Nehemiah
  NPC Behavior: Rebuilding NPCs.
  Voice Lines:
    - "The wall rises."
    - "We read the Law."

### Esther
- Name: Mordecai
  Image: assets/characters/mordecai.png
  Background: Esther’s cousin; saved the king and guided Esther.
  References: Esther 2–10
  NPC Behavior: Wise counselor.
  Voice Lines:
    - "Who knows why you are here?"
    - "Do not keep silent."

- Name: Haman
  Image: assets/characters/haman.png
  Background: Enemy of the Jews in Persia.
  References: Esther 3–9
  NPC Behavior: Antagonist; pride-driven.
  Voice Lines:
    - "All must bow to me."
    - "Prepare the gallows."

- Name: Unnamed Esther Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Persian court and city dwellers.
  References: Esther
  NPC Behavior: Court NPCs.
  Voice Lines:
    - "The decree is sealed."
    - "The feast is prepared."

### Job
- Name: Job’s Friends (Eliphaz, Bildad, Zophar, Elihu)
  Image: assets/characters/job_friends.png
  Background: Friends who debated Job’s suffering.
  References: Job 2–37
  NPC Behavior: Philosophical debate NPCs.
  Voice Lines:
    - "Consider your ways."
    - "Wisdom is hidden."

- Name: Unnamed Job Crowds/Servants
  Image: assets/characters/placeholder.png
  Background: Job’s household and community.
  References: Job
  NPC Behavior: Ambient NPCs in trial scenes.
  Voice Lines:
    - "We mourn with you."
    - "Hold fast to hope."

### Psalms–Song of Songs
- Name: Asaph
  Image: assets/characters/asaph.png
  Background: Psalmist and worship leader.
  References: Psalms 50; 73–83
  NPC Behavior: Worship leader NPC.
  Voice Lines:
    - "Sing to the Lord."
    - "Let the people praise."

- Name: Korahite Worshipers
  Image: assets/characters/korahites.png
  Background: Family of temple musicians.
  References: Psalms 42–49; 84–88
  NPC Behavior: Choir NPCs.
  Voice Lines:
    - "Better one day in your courts."
    - "Our souls thirst."

- Name: Shulammite Bride
  Image: assets/characters/shulammite.png
  Background: Lover in Song of Songs.
  References: Song of Songs 1–8
  NPC Behavior: Poetic encounter NPC.
  Voice Lines:
    - "My beloved is mine."
    - "Let him kiss me."

- Name: Unnamed Psalms/Wisdom Crowds
  Image: assets/characters/placeholder.png
  Background: Worshipers, sages, and poets.
  References: Psalms–Song of Songs
  NPC Behavior: Temple and city ambience.
  Voice Lines:
    - "Let everything praise."
    - "Wisdom is precious."

### Major Prophets
- Name: Hosea
  Image: assets/characters/hosea.png
  Background: Prophet depicting God’s steadfast love.
  References: Hosea 1–14
  NPC Behavior: Mercy-focused prophet.
  Voice Lines:
    - "Love returns."
    - "Come, let us return."

- Name: Amos
  Image: assets/characters/amos.png
  Background: Prophet of justice.
  References: Amos 1–9
  NPC Behavior: Justice advocate.
  Voice Lines:
    - "Let justice roll."
    - "Seek the Lord and live."

- Name: Micah
  Image: assets/characters/micah.png
  Background: Prophet emphasizing justice and humility.
  References: Micah 1–7
  NPC Behavior: Ethical guide.
  Voice Lines:
    - "Do justice, love mercy."
    - "Walk humbly with your God."

- Name: Nahum
  Image: assets/characters/nahum.png
  Background: Prophet announcing Nineveh’s fall.
  References: Nahum 1–3
  NPC Behavior: Judgment herald.
  Voice Lines:
    - "The Lord is a stronghold."
    - "Evil will not endure."

- Name: Habakkuk
  Image: assets/characters/habakkuk.png
  Background: Prophet wrestling with justice.
  References: Habakkuk 1–3
  NPC Behavior: Questioning prophet.
  Voice Lines:
    - "How long, Lord?"
    - "The righteous will live by faith."

- Name: Zephaniah
  Image: assets/characters/zephaniah.png
  Background: Prophet of the Day of the Lord.
  References: Zephaniah 1–3
  NPC Behavior: Warning NPC.
  Voice Lines:
    - "The day is near."
    - "The Lord sings over you."

- Name: Haggai
  Image: assets/characters/haggai.png
  Background: Prophet urging temple rebuilding.
  References: Haggai 1–2
  NPC Behavior: Builder motivator.
  Voice Lines:
    - "Consider your ways."
    - "Build the house."

- Name: Zechariah
  Image: assets/characters/zechariah_prophet.png
  Background: Prophet of restoration visions.
  References: Zechariah 1–14
  NPC Behavior: Visionary guide.
  Voice Lines:
    - "Return to me."
    - "Not by might, but by my Spirit."

- Name: Malachi
  Image: assets/characters/malachi.png
  Background: Final prophet of the Old Testament.
  References: Malachi 1–4
  NPC Behavior: Covenant reminder.
  Voice Lines:
    - "Return to me and I will return."
    - "A sun of righteousness will rise."

- Name: Unnamed Prophets Crowds
  Image: assets/characters/placeholder.png
  Background: Kings, priests, and peoples during prophetic eras.
  References: Isaiah–Malachi
  NPC Behavior: Ambient NPCs.
  Voice Lines:
    - "The prophet speaks."
    - "We must choose."

### Gospels and Acts
- Name: Zechariah (father of John)
  Image: assets/characters/zechariah_priest.png
  Background: Priest and father of John the Baptist.
  References: Luke 1
  NPC Behavior: Temple priest; prophecy events.
  Voice Lines:
    - "Blessed be the Lord."
    - "My tongue is loosed."

- Name: Elizabeth
  Image: assets/characters/elizabeth.png
  Background: Mother of John the Baptist.
  References: Luke 1
  NPC Behavior: Encouraging elder.
  Voice Lines:
    - "The Lord has shown favor."
    - "Blessed are you among women."

- Name: Simeon
  Image: assets/characters/simeon.png
  Background: Elder who blessed infant Jesus.
  References: Luke 2
  NPC Behavior: Temple elder.
  Voice Lines:
    - "My eyes have seen salvation."
    - "Let your servant depart in peace."

- Name: Anna
  Image: assets/characters/anna.png
  Background: Prophetess who recognized Jesus.
  References: Luke 2
  NPC Behavior: Prayerful elder.
  Voice Lines:
    - "He is the redemption."
    - "Give thanks to God."

- Name: Nicodemus
  Image: assets/characters/nicodemus.png
  Background: Pharisee who sought Jesus at night.
  References: John 3; 7; 19
  NPC Behavior: Curious scholar.
  Voice Lines:
    - "How can this be?"
    - "We must listen."

- Name: Zacchaeus
  Image: assets/characters/zacchaeus.png
  Background: Tax collector who repented.
  References: Luke 19
  NPC Behavior: Redemption story NPC.
  Voice Lines:
    - "Half my goods to the poor."
    - "Today salvation has come."

- Name: Bartimaeus
  Image: assets/characters/bartimaeus.png
  Background: Blind man healed by Jesus.
  References: Mark 10
  NPC Behavior: Healing witness.
  Voice Lines:
    - "Jesus, have mercy!"
    - "I can see!"

- Name: Centurion (at the cross)
  Image: assets/characters/centurion.png
  Background: Roman officer who declared Jesus righteous.
  References: Matthew 27; Mark 15; Luke 23
  NPC Behavior: Soldier turned witness.
  Voice Lines:
    - "Truly, this was the Son of God."
    - "I have seen his innocence."

- Name: Cornelius
  Image: assets/characters/cornelius.png
  Background: Roman centurion; first Gentile convert.
  References: Acts 10
  NPC Behavior: Bridge-builder NPC.
  Voice Lines:
    - "God showed me a vision."
    - "We welcome the message."

- Name: Barnabas
  Image: assets/characters/barnabas.png
  Background: Encourager and missionary companion of Paul.
  References: Acts 4–15
  NPC Behavior: Supportive ally.
  Voice Lines:
    - "Take courage, brother."
    - "Let us go again."

- Name: Silas
  Image: assets/characters/silas.png
  Background: Paul’s missionary companion.
  References: Acts 15–18
  NPC Behavior: Companion NPC.
  Voice Lines:
    - "Sing in the night."
    - "The Lord opens doors."

- Name: Timothy
  Image: assets/characters/timothy.png
  Background: Young pastor mentored by Paul.
  References: Acts 16; 1–2 Timothy
  NPC Behavior: Apprentice NPC.
  Voice Lines:
    - "Guard the good deposit."
    - "Be strong in grace."

- Name: Lydia
  Image: assets/characters/lydia.png
  Background: Merchant in Philippi who hosted the church.
  References: Acts 16
  NPC Behavior: Hospitality and trade NPC.
  Voice Lines:
    - "My home is open."
    - "The Lord opened my heart."

- Name: Stephen
  Image: assets/characters/stephen.png
  Background: First martyr of the church.
  References: Acts 6–7
  NPC Behavior: Witness NPC; courage themes.
  Voice Lines:
    - "I see the heavens opened."
    - "Lord, do not hold this against them."

- Name: Philip (evangelist)
  Image: assets/characters/philip_evangelist.png
  Background: Evangelist who baptized the Ethiopian official.
  References: Acts 8
  NPC Behavior: Mission NPC.
  Voice Lines:
    - "Do you understand what you read?"
    - "Rise and go."

- Name: Unnamed Gospel/Acts Crowds
  Image: assets/characters/placeholder.png
  Background: Disciples, crowds, officials, and converts.
  References: Matthew–Acts
  NPC Behavior: Ambient NPCs; crowd reactions.
  Voice Lines:
    - "Hosanna!"
    - "We have heard the good news."

### Epistles and Revelation
- Name: Peter (apostle)
  Image: assets/characters/peter.png
  Background: Apostle; authored 1–2 Peter.
  References: 1–2 Peter
  NPC Behavior: Shepherd NPC.
  Voice Lines:
    - "Feed my sheep."
    - "Stand firm in grace."

- Name: John (apostle)
  Image: assets/characters/john_apostle.png
  Background: Apostle; authored Revelation.
  References: Revelation 1–22
  NPC Behavior: Visionary narrator.
  Voice Lines:
    - "I was in the Spirit."
    - "Behold, he is coming."

- Name: Unnamed Epistles/Revelation Crowds
  Image: assets/characters/placeholder.png
  Background: Churches, elders, and nations.
  References: Romans–Revelation
  NPC Behavior: Worship gatherings and apocalyptic scenes.
  Voice Lines:
    - "Grace and peace to you."
    - "Amen. Come, Lord Jesus."

