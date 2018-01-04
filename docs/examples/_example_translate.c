/* Create table for English entries */
const GUI_Char *
languageEnglishEntries[] = {
    _GT("Button"),
    _GT("Dropdown"),
    _GT("Listview"),
    _GT("Your string here without translate."),
};

/* Create table for German entries */
const GUI_Char *
languageGermanEntries[] = {
    _GT("Taste"),           /* Translate for "Button" */
    _GT("Dropdown-Liste"),  /* Translate for "Dropdown" */
    _GT("Listenansicht"),   /* Translate for "Listview" */
    /* Missing translate for "Your string here without translate." */
};

/* Create English language table */
GUI_TRANSLATE_Language_t languageEnglish = {
    .Entries = languageEnglishEntries,
    .Count = GUI_COUNT_OF(languageEnglishEntries)
};

/* Create German language structure */
GUI_TRANSLATE_Language_t languageGerman = {
    .Entries = languageGermanEntries,
    .Count = GUI_COUNT_OF(languageGermanEntries)
};

/* Set source and active languages */
gui_translate_setsourcelanguage(&languageEnglish); /* Translate from this language */
gui_translate_setactivelanguage(&languageGerman);  /* Translate to this language */

/* Now try to translate */

/* Prints: "Button: Taste\n" */
printf("Button: %s\n", gui_translate_get(_GT("Button")));

/* Prints: "Button: Dropdown-Liste\n" */
printf("Dropdown: %s\n", gui_translate_get(_GT("Dropdown")));

/* Prints: "Button: Listenansicht\n" */
printf("Listview: %s\n", gui_translate_get(_GT("Listview")));

/* Prints: "Your string here without translate.: Your string here without translate.\n" */
printf("Your string here without translate.: %s\n", gui_translate_get(_GT("Your string here without translate.")));