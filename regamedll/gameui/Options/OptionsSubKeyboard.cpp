/*

Copied Date: May 25 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
static int ConvertVGUIToEngine(ButtonCode_t code)
{
	switch (code)
	{
	case KEY_0: return '0';
	case KEY_1: return '1';
	case KEY_2: return '2';
	case KEY_3: return '3';
	case KEY_4: return '4';
	case KEY_5: return '5';
	case KEY_6: return '6';
	case KEY_7: return '7';
	case KEY_8: return '8';
	case KEY_9: return '9';
	case KEY_A: return 'a';
	case KEY_B: return 'b';
	case KEY_C: return 'c';
	case KEY_D: return 'd';
	case KEY_E: return 'e';
	case KEY_F: return 'f';
	case KEY_G: return 'g';
	case KEY_H: return 'h';
	case KEY_I: return 'i';
	case KEY_J: return 'j';
	case KEY_K: return 'k';
	case KEY_L: return 'l';
	case KEY_M: return 'm';
	case KEY_N: return 'n';
	case KEY_O: return 'o';
	case KEY_P: return 'p';
	case KEY_Q: return 'q';
	case KEY_R: return 'r';
	case KEY_S: return 's';
	case KEY_T: return 't';
	case KEY_U: return 'u';
	case KEY_V: return 'v';
	case KEY_W: return 'w';
	case KEY_X: return 'x';
	case KEY_Y: return 'y';
	case KEY_Z: return 'z';
	case KEY_PAD_0: return K_KP_INS;
	case KEY_PAD_1: return K_KP_END;
	case KEY_PAD_2: return K_KP_DOWNARROW;
	case KEY_PAD_3: return K_KP_PGDN;
	case KEY_PAD_4: return K_KP_LEFTARROW;
	case KEY_PAD_5: return K_KP_5;
	case KEY_PAD_6: return K_KP_RIGHTARROW;
	case KEY_PAD_7: return K_KP_HOME;
	case KEY_PAD_8: return K_KP_UPARROW;
	case KEY_PAD_9: return K_KP_PGUP;
	case KEY_PAD_DIVIDE: return K_KP_SLASH;
	case KEY_PAD_MINUS: return K_KP_MINUS;
	case KEY_PAD_PLUS: return K_KP_PLUS;
	case KEY_PAD_ENTER: return K_KP_ENTER;
	case KEY_PAD_DECIMAL: return K_KP_DEL;
	case KEY_PAD_MULTIPLY: return '*';
	case KEY_LBRACKET: return '[';
	case KEY_RBRACKET: return ']';
	case KEY_SEMICOLON: return ';';
	case KEY_APOSTROPHE: return '\'';
	case KEY_BACKQUOTE: return '`';
	case KEY_COMMA: return ',';
	case KEY_PERIOD: return '.';
	case KEY_SLASH: return '/';
	case KEY_BACKSLASH: return '\\';
	case KEY_MINUS: return '-';
	case KEY_EQUAL: return '=';
	case KEY_ENTER: return K_ENTER;
	case KEY_SPACE: return K_SPACE;
	case KEY_BACKSPACE: return K_BACKSPACE;
	case KEY_TAB: return K_TAB;
	case KEY_CAPSLOCK: return K_CAPSLOCK;
	case KEY_ESCAPE: return K_ESCAPE;
	case KEY_INSERT: return K_INS;
	case KEY_DELETE: return K_DEL;
	case KEY_HOME: return K_HOME;
	case KEY_END: return K_END;
	case KEY_PAGEUP: return K_PGUP;
	case KEY_PAGEDOWN: return K_PGDN;
	case KEY_BREAK: return K_PAUSE;
	case KEY_LSHIFT: return K_SHIFT;
	case KEY_RSHIFT: return K_SHIFT;
	case KEY_LALT: return K_ALT;
	case KEY_RALT: return K_ALT;
	case KEY_LCONTROL: return K_CTRL;
	case KEY_RCONTROL: return K_CTRL;
	case KEY_UP: return K_UPARROW;
	case KEY_LEFT: return K_LEFTARROW;
	case KEY_DOWN: return K_DOWNARROW;
	case KEY_RIGHT: return K_RIGHTARROW;
	case KEY_F1: return K_F1;
	case KEY_F2: return K_F2;
	case KEY_F3: return K_F3;
	case KEY_F4: return K_F4;
	case KEY_F5: return K_F5;
	case KEY_F6: return K_F6;
	case KEY_F7: return K_F7;
	case KEY_F8: return K_F8;
	case KEY_F9: return K_F9;
	case KEY_F10: return K_F10;
	case KEY_F11: return K_F11;
	case KEY_F12: return K_F12;

	case KEY_NUMLOCK:
	case KEY_LWIN:
	case KEY_RWIN:
	case KEY_APP:
	case KEY_SCROLLLOCK:
	case KEY_CAPSLOCKTOGGLE:
	case KEY_NUMLOCKTOGGLE:
	case KEY_SCROLLLOCKTOGGLE: return 0;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
static const char* GetButtonName(ButtonCode_t button)
{
	if (button & 1)
		return "MOUSE1";

	else if (button & 2)
		return "MOUSE2";

	else if (button & 4)
		return "MOUSE3";

	else if (button & 8)
		return "MOUSE4";

	else if (button & 16)
		return "MOUSE5";

	return "MOUSE1";
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
COptionsSubKeyboard::COptionsSubKeyboard(vgui::Panel* parent) : PropertyPage(parent, NULL)
{
	memset(m_Bindings, 0, sizeof(m_Bindings));

	// create the key bindings list
	CreateKeyBindingList();
	// Store all current key bindings
	SaveCurrentBindings();
	// Parse default descriptions
	ParseActionDescriptions();

	m_pSetBindingButton = new Button(this, "ChangeKeyButton", "");
	m_pClearBindingButton = new Button(this, "ClearKeyButton", "");

	LoadControlSettings("Resource/OptionsSubKeyboard.res");

	m_pSetBindingButton->SetEnabled(false);
	m_pClearBindingButton->SetEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
COptionsSubKeyboard::~COptionsSubKeyboard()
{
	DeleteSavedBindings();
}

//-----------------------------------------------------------------------------
// Purpose: reloads current keybinding
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnResetData()
{
	// Populate list based on current settings
	FillInCurrentBindings();
	if (IsVisible())
	{
		m_pKeyBindList->SetSelectedItem(0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: saves out keybinding changes
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnApplyChanges()
{
	ApplyAllBindings();
}

//-----------------------------------------------------------------------------
// Purpose: Create key bindings list control
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::CreateKeyBindingList()
{
	// Create the control
	m_pKeyBindList = new VControlsListPanel(this, "listpanel_keybindlist");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnMousePressed(vgui::MouseCode code)
{
	if (m_pKeyBindList->IsCapturing())
	{
		switch (code)
		{
		case MOUSE_LEFT:
		{
			Finish(0, 1);
			break;
		}
		case MOUSE_RIGHT:
		{
			Finish(0, 2);
			break;
		}
		case MOUSE_MIDDLE:
		{
			Finish(0, 4);
			break;
		}
		case MOUSE_4:
		{
			Finish(0, 8);
			break;
		}
		case MOUSE_5:
		{
			Finish(0, 16);
			break;
		}
		}

		return;
	}

	BaseClass::OnMousePressed(code);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnMouseWheeled(int delta)
{
	if (m_pKeyBindList->IsCapturing())
	{
		if (delta > 0)
		{
			for (int i = 0; i < 256; i++)
			{
				const char* pchKeyName = gameuifuncs->Key_BindingForKey(i);

				if (pchKeyName)
				{
					if (pchKeyName[0] && !Q_stricmp(pchKeyName, "MWHEELUP"))
					{
						Finish(i, 0, "MWHEELUP");
						break;
					}
				}
			}

			Finish(-1, 0, "MWHEELUP");
		}
		else
		{
			for (int i = 0; i < 256; i++)
			{
				const char* pchKeyName = gameuifuncs->Key_BindingForKey(i);

				if (pchKeyName)
				{
					if (pchKeyName[0] && Q_stricmp(pchKeyName, "MWHEELDOWN"))
					{
						Finish(i, 0, "MWHEELDOWN");
						break;
					}
				}
			}

			Finish(-1, 0, "MWHEELDOWN");
		}

		return;
	}

	BaseClass::OnMouseWheeled(delta);
}

//-----------------------------------------------------------------------------
// Purpose: binds double-clicking or hitting enter in the keybind list to changing the key
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnKeyCodeTyped(vgui::KeyCode code)
{
	if (m_pKeyBindList->IsCapturing())
	{
		int key = ConvertVGUIToEngine(code);
		Finish(key);

		return;
	}
	else
	{
		if (code == KEY_ENTER)
		{
			OnCommand("ChangeKey");
			return;
		}
	}

	BaseClass::OnKeyCodeTyped(code);
}

//-----------------------------------------------------------------------------
// Purpose: command handler
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "Defaults"))
	{
		// open a box asking if we want to restore defaults
		QueryBox* box = new QueryBox("#GameUI_KeyboardSettings", "#GameUI_KeyboardSettingsText");
		box->AddActionSignalTarget(this);
		box->SetOKCommand(new KeyValues("Command", "command", "DefaultsOK"));
		box->DoModal();
	}
	else if (!Q_stricmp(command, "DefaultsOK"))
	{
		// Restore defaults from default keybindings file
		FillInDefaultBindings();
		m_pKeyBindList->RequestFocus();
	}
	else if (!m_pKeyBindList->IsCapturing() && !Q_stricmp(command, "ChangeKey"))
	{
		m_pKeyBindList->StartCaptureMode(dc_blank);
	}
	else if (!m_pKeyBindList->IsCapturing() && !Q_stricmp(command, "ClearKey"))
	{
		OnKeyCodePressed(KEY_DELETE);
		m_pKeyBindList->RequestFocus();
	}
	else if (!Q_stricmp(command, "Advanced"))
	{
		OpenKeyboardAdvancedDialog();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

const char* UTIL_Parse(const char* data, char* token, int sizeofToken)
{
	data = gEngfuncs.COM_ParseFile(const_cast<char*>(data), token);
	return data;
}

static char* UTIL_CopyString(const char* in)
{
	int len = strlen(in) + 1;
	char* out = new char[len];
	Q_strncpy(out, in, len);
	return out;
}

#ifndef _XBOX
char* UTIL_va(const char* format, ...)
{
	va_list		argptr;
	static char	string[4][1024];
	static int	curstring = 0;

	curstring = (curstring + 1) % 4;

	va_start(argptr, format);
	Q_vsnprintf(string[curstring], 1024, format, argptr);
	va_end(argptr);

	return string[curstring];
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::ParseActionDescriptions(void)
{
	char szBinding[256];
	char szDescription[256];

	KeyValues* item;

	// Load the default keys list
	FileHandle_t fh = g_pFullFileSystem->Open("gfx/shell/kb_act.lst", "rb");
	if (fh == FILESYSTEM_INVALID_HANDLE)
		return;

	int size = g_pFullFileSystem->Size(fh);
	CUtlBuffer buf(0, size, CUtlBuffer::TEXT_BUFFER);
	g_pFullFileSystem->Read(buf.Base(), size, fh);
	g_pFullFileSystem->Close(fh);

	const char* data = (const char*)buf.Base();

	int sectionIndex = 0;
	char token[512];
	while (1)
	{
		data = UTIL_Parse(data, token, sizeof(token));
		// Done.
		if (strlen(token) <= 0)
			break;

		Q_strncpy(szBinding, token, sizeof(szBinding));

		data = UTIL_Parse(data, token, sizeof(token));
		if (strlen(token) <= 0)
		{
			break;
		}

		Q_strncpy(szDescription, token, sizeof(szDescription));

		// Skip '======' rows
		if (szDescription[0] != '=')
		{
			// Flag as special header row if binding is "blank"
			if (!Q_stricmp(szBinding, "blank"))
			{
				// add header item
				m_pKeyBindList->AddSection(++sectionIndex, szDescription);
				m_pKeyBindList->AddColumnToSection(sectionIndex, "Action", szDescription, SectionedListPanel::COLUMN_BRIGHT, 286);
				m_pKeyBindList->AddColumnToSection(sectionIndex, "Key", "#GameUI_KeyButton", SectionedListPanel::COLUMN_BRIGHT, 128);
			}
			else
			{
				// Create a new: blank item
				item = new KeyValues("Item");

				// fill in data
				item->SetString("Action", szDescription);
				item->SetString("Binding", szBinding);
				item->SetString("Key", "");

				// Add to list
				m_pKeyBindList->AddItem(sectionIndex, item);
				item->deleteThis();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Search current data set for item which has the specified binding string
// Input  : *binding - string to find
// Output : KeyValues or NULL on failure
//-----------------------------------------------------------------------------
KeyValues* COptionsSubKeyboard::GetItemForBinding(const char* binding)
{
	static int bindingSymbol = KeyValuesSystem()->GetSymbolForString("Binding");

	// Loop through all items
	for (int i = 0; i < m_pKeyBindList->GetItemCount(); i++)
	{
		KeyValues* item = m_pKeyBindList->GetItemData(m_pKeyBindList->GetItemIDFromRow(i));
		if (!item)
			continue;

		KeyValues* bindingItem = item->FindKey(bindingSymbol);
		const char* bindString = bindingItem->GetString();

		// Check the "Binding" key
		if (!Q_stricmp(bindString, binding))
			return item;
	}
	// Didn't find it
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static ButtonCode_t FindKeyForName(const char* pchKeyName)
{
	for (int i = 0; i < BUTTON_CODE_LAST; i++)
	{
		const char* name = gameuifuncs->Key_NameForKey(i);

		if (!name || !*name)
			continue;

		if (strlen(name) == 1)
		{
			if (!strcmp(pchKeyName, name))
				return i;
		}
		else
		{
			if (!Q_stricmp(pchKeyName, name))
				return i;
		}
	}

	return BUTTON_CODE_INVALID;
}

//-----------------------------------------------------------------------------
// Purpose: Bind the specified keyname to the specified item
// Input  : *item - Item to which to add the key
//			*keyname - The key to be added
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::AddBinding(KeyValues* item, const char* keyname)
{
	// See if it's already there as a binding
	if (!Q_stricmp(item->GetString("Key", ""), keyname))
		return;

	// Make sure it doesn't live anywhere
	RemoveKeyFromBindItems(item, keyname);

	const char* binding = item->GetString("Binding", "");

	// Loop through all the key bindings and set all entries that have
	// the same binding. This allows us to have multiple entries pointing 
	// to the same binding.
	for (int i = 0; i < m_pKeyBindList->GetItemCount(); i++)
	{
		KeyValues* curitem = m_pKeyBindList->GetItemData(m_pKeyBindList->GetItemIDFromRow(i));
		if (!curitem)
			continue;

		const char* curbinding = curitem->GetString("Binding", "");

		if (!Q_stricmp(curbinding, binding))
		{
			curitem->SetString("Key", keyname);
			m_pKeyBindList->InvalidateItem(i);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Remove all keys from list
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::ClearBindItems(void)
{
	for (int i = 0; i < m_pKeyBindList->GetItemCount(); i++)
	{
		KeyValues* item = m_pKeyBindList->GetItemData(m_pKeyBindList->GetItemIDFromRow(i));
		if (!item)
			continue;

		// Reset keys
		item->SetString("Key", "");

		m_pKeyBindList->InvalidateItem(i);
	}

	m_pKeyBindList->InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Remove all instances of the specified key from bindings
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::RemoveKeyFromBindItems(KeyValues* org_item, const char* key)
{
	Assert(key && key[0]);
	if (!key || !key[0])
		return;

	int len = Q_strlen(key);
	char* pszKey = new char[len + 1];

	if (!pszKey)
		return;

	Q_memcpy(pszKey, key, len + 1);

	for (int i = 0; i < m_pKeyBindList->GetItemCount(); i++)
	{
		KeyValues* item = m_pKeyBindList->GetItemData(m_pKeyBindList->GetItemIDFromRow(i));
		if (!item)
			continue;

		// If it's bound to the primary: then remove it
		if (!Q_stricmp(pszKey, item->GetString("Key", "")))
		{
			bool bClearEntry = true;

			if (org_item)
			{
				// Only clear it out if the actual binding isn't the same. This allows
				// us to have the same key bound to multiple entries in the keybinding list
				// if they point to the same command.
				const char* org_binding = org_item->GetString("Binding", "");
				const char* binding = item->GetString("Binding", "");
				if (!Q_stricmp(org_binding, binding))
				{
					bClearEntry = false;
				}
			}

			if (bClearEntry)
			{
				item->SetString("Key", "");
				m_pKeyBindList->InvalidateItem(i);
			}
		}
	}

	delete[] pszKey;

	// Make sure the display is up to date
	m_pKeyBindList->InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Ask the engine for all bindings and set up the list
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::FillInCurrentBindings(void)
{
	// reset the unbind list
	// we only unbind keys used by the normal config items (not custom binds)
	m_KeysToUnbind.RemoveAll();

	// Clear any current settings
	ClearBindItems();

	bool bJoystick = false;

	for (int i = 0; i < BUTTON_CODE_LAST; i++)
	{
		// Look up binding
		const char* binding = gameuifuncs->Key_BindingForKey(i);
		if (!binding)
			continue;

		// See if there is an item for this one?
		KeyValues* item = GetItemForBinding(binding);
		if (item)
		{
			// Bind it by name
			const char* keyName = gameuifuncs->Key_NameForKey((ButtonCode_t)i);

			// Already in list, means user had two keys bound to this item.  We'll only note the first one we encounter
			char const* currentKey = item->GetString("Key", "");
			if (currentKey && currentKey[0])
			{
				ButtonCode_t currentBC = (ButtonCode_t)gameuifuncs->GetVGUI2KeyCodeForBind(currentKey);

				// If we're using a joystick, joystick bindings override keyboard ones
				bool bShouldOverride = bJoystick;

				if (!bShouldOverride)
					continue;

				// Remove the key we're about to override from the unbinding list
				m_KeysToUnbind.FindAndRemove(currentKey);
			}

			AddBinding(item, keyName);

			// remember to apply unbinding of this key when we apply
			m_KeysToUnbind.AddToTail(keyName);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Clean up memory used by saved bindings
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::DeleteSavedBindings(void)
{
	for (int i = 0; i < BUTTON_CODE_LAST; i++)
	{
		if (m_Bindings[i].binding)
		{
			delete[] m_Bindings[i].binding;
			m_Bindings[i].binding = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Copy all bindings into save array
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::SaveCurrentBindings(void)
{
	DeleteSavedBindings();
	for (int i = 0; i < BUTTON_CODE_LAST; i++)
	{
		const char* binding = gameuifuncs->Key_BindingForKey((ButtonCode_t)i);
		if (!binding || !binding[0])
			continue;

		// Copy the binding string
		m_Bindings[i].binding = UTIL_CopyString(binding);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Tells the engine to bind a key
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::BindKey(const char* key, const char* binding)
{
#ifndef _XBOX
	gEngfuncs.pfnClientCmd(UTIL_va("bind \"%s\" \"%s\"\n", key, binding));
#else
	char buff[256];
	Q_snprintf(buff, sizeof(buff), "bind \"%s\" \"%s\"\n", key, binding);
	gEngfuncs.ClientCmd_Unrestricted(buff);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Tells the engine to unbind a key
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::UnbindKey(const char* key)
{
#ifndef _XBOX
	gEngfuncs.pfnClientCmd(UTIL_va("unbind \"%s\"\n", key));
#else
	char buff[256];
	Q_snprintf(buff, sizeof(buff), "unbind \"%s\"\n", key);
	gEngfuncs.ClientCmd_Unrestricted(buff);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Go through list and bind specified keys to actions
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::ApplyAllBindings(void)
{
	// unbind everything that the user unbound
	{for (int i = 0; i < m_KeysToUnbind.Count(); i++)
	{
		UnbindKey(m_KeysToUnbind[i].String());
	}}
	m_KeysToUnbind.RemoveAll();

	// free binding memory
	DeleteSavedBindings();

	for (int i = 0; i < m_pKeyBindList->GetItemCount(); i++)
	{
		KeyValues* item = m_pKeyBindList->GetItemData(m_pKeyBindList->GetItemIDFromRow(i));
		if (!item)
			continue;

		// See if it has a binding
		const char* binding = item->GetString("Binding", "");
		if (!binding || !binding[0])
			continue;

		const char* keyname;

		// Check main binding
		keyname = item->GetString("Key", "");
		if (keyname && keyname[0])
		{
			// Tell the engine
			BindKey(keyname, binding);
			ButtonCode_t code = FindKeyForName(keyname);
			if (code != BUTTON_CODE_INVALID)
			{
				m_Bindings[code].binding = UTIL_CopyString(binding);
			}
		}
	}

	// Now exec their custom bindings
	gEngfuncs.pfnClientCmd("exec userconfig.cfg\n");
}

//-----------------------------------------------------------------------------
// Purpose: Read in defaults from game's default config file and populate list 
//			using those defaults
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::FillInDefaultBindings(void)
{
	FileHandle_t fh = g_pFullFileSystem->Open("cfg/config_default.cfg", "rb");
	if (fh == FILESYSTEM_INVALID_HANDLE)
		return;

	int size = g_pFullFileSystem->Size(fh);
	CUtlBuffer buf(0, size, CUtlBuffer::TEXT_BUFFER);
	g_pFullFileSystem->Read(buf.Base(), size, fh);
	g_pFullFileSystem->Close(fh);

	// Clear out all current bindings
	ClearBindItems();

	const char* data = (const char*)buf.Base();

	// loop through all the binding
	while (data != NULL)
	{
		char cmd[64];
		data = UTIL_Parse(data, cmd, sizeof(cmd));
		if (strlen(cmd) <= 0)
			break;

		if (!Q_stricmp(cmd, "bind"))
		{
			// Key name
			char szKeyName[256];
			data = UTIL_Parse(data, szKeyName, sizeof(szKeyName));
			if (strlen(szKeyName) <= 0)
				break; // Error

			char szBinding[256];
			data = UTIL_Parse(data, szBinding, sizeof(szBinding));
			if (strlen(szKeyName) <= 0)
				break; // Error

			// Find item
			KeyValues* item = GetItemForBinding(szBinding);
			if (item)
			{
				// Bind it
				AddBinding(item, szKeyName);
			}
		}
	}

	PostActionSignal(new KeyValues("ApplyButtonEnable"));

	// Make sure console and escape key are always valid
	KeyValues* item = GetItemForBinding("toggleconsole");
	if (item)
	{
		// Bind it
		AddBinding(item, "`");
	}
	item = GetItemForBinding("cancelselect");
	if (item)
	{
		// Bind it
		AddBinding(item, "ESCAPE");
	}
}

//-----------------------------------------------------------------------------
// Purpose: User clicked on item: remember where last active row/column was
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::ItemSelected(int itemID)
{
	m_pKeyBindList->SetItemOfInterest(itemID);

	if (m_pKeyBindList->IsItemIDValid(itemID))
	{
		// find the details, see if we should be enabled/clear/whatever
		m_pSetBindingButton->SetEnabled(true);

		KeyValues* kv = m_pKeyBindList->GetItemData(itemID);
		if (kv)
		{
			const char* key = kv->GetString("Key", NULL);
			if (key && *key)
			{
				m_pClearBindingButton->SetEnabled(true);
			}
			else
			{
				m_pClearBindingButton->SetEnabled(false);
			}

			if (kv->GetInt("Header"))
			{
				m_pSetBindingButton->SetEnabled(false);
			}
		}
	}
	else
	{
		m_pSetBindingButton->SetEnabled(false);
		m_pClearBindingButton->SetEnabled(false);
	}
}

//-----------------------------------------------------------------------------
// Purpose: called when the capture has finished
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::Finish(ButtonCode_t code, ButtonCode_t button, const char* pchKeyName)
{
	int r = m_pKeyBindList->GetItemOfInterest();

	// Retrieve clicked row and column
	m_pKeyBindList->EndCaptureMode(dc_arrow);

	// Find item for this row
	KeyValues* item = m_pKeyBindList->GetItemData(r);
	if (item)
	{
		if (pchKeyName)
		{
			AddBinding(item, pchKeyName);
			PostActionSignal(new KeyValues("ApplyButtonEnable"));
		}
		else if (button)
		{
			AddBinding(item, GetButtonName(button));
			PostActionSignal(new KeyValues("ApplyButtonEnable"));
		}
		else if (code && code != K_ESCAPE)
		{
			AddBinding(item, gameuifuncs->Key_NameForKey(code));
			PostActionSignal(new KeyValues("ApplyButtonEnable"));
		}

		m_pKeyBindList->InvalidateItem(r);
	}

	m_pSetBindingButton->SetEnabled(true);
	m_pClearBindingButton->SetEnabled(true);
}

//-----------------------------------------------------------------------------
// Purpose: Scans for captured key presses
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnThink()
{
	BaseClass::OnThink();

	if (m_pKeyBindList->IsCapturing())
	{
	}
}

//-----------------------------------------------------------------------------
// Purpose: Check for enter key and go into keybinding mode if it was pressed
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OnKeyCodePressed(vgui::KeyCode code)
{
	// Enter key pressed and not already trapping next key/button press
	if (!m_pKeyBindList->IsCapturing())
	{
		// Grab which item was set as interesting
		int r = m_pKeyBindList->GetItemOfInterest();

		// Check that it's visible
		int x, y, w, h;
		bool visible = m_pKeyBindList->GetCellBounds(r, 1, x, y, w, h);
		if (visible)
		{
			if (KEY_DELETE == code)
			{
				// find the current binding and remove it
				KeyValues* kv = m_pKeyBindList->GetItemData(r);

				const char* key = kv->GetString("Key", NULL);
				if (key && *key)
				{
					RemoveKeyFromBindItems(NULL, key);
				}

				m_pClearBindingButton->SetEnabled(false);
				m_pKeyBindList->InvalidateItem(r);
				PostActionSignal(new KeyValues("ApplyButtonEnable"));

				// message handled, don't pass on
				return;
			}
		}
	}

	// Allow base class to process message instead
	BaseClass::OnKeyCodePressed(code);
}


//-----------------------------------------------------------------------------
// Purpose: advanced keyboard settings dialog
//-----------------------------------------------------------------------------
class COptionsSubKeyboardAdvancedDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsSubKeyboardAdvancedDlg, vgui::Frame);
public:
	COptionsSubKeyboardAdvancedDlg(vgui::VPANEL hParent) : BaseClass(NULL, NULL)
	{
		// parent is ignored, since we want look like we're steal focus from the parent (we'll become modal below)

		SetTitle("#GameUI_KeyboardAdvanced_Title", true);
		SetSize(280, 140);
		LoadControlSettings("resource/OptionsSubKeyboardAdvancedDlg.res");
		MoveToCenterOfScreen();
		SetSizeable(false);
		SetDeleteSelfOnClose(true);
	}

	virtual void Activate()
	{
		BaseClass::Activate();

		input()->SetAppModalSurface(GetVPanel());

		// reset the data
	}

	virtual void OnApplyData()
	{
		// apply data
	}

	virtual void OnCommand(const char* command)
	{
		if (!Q_stricmp(command, "OK"))
		{
			// apply the data
			OnApplyData();
			Close();
		}
		else
		{
			BaseClass::OnCommand(command);
		}
	}

	void OnKeyCodeTyped(KeyCode code)
	{
		// force ourselves to be closed if the escape key it pressed
		if (code == KEY_ESCAPE)
		{
			Close();
		}
		else
		{
			BaseClass::OnKeyCodeTyped(code);
		}
	}
};

//-----------------------------------------------------------------------------
// Purpose: Open advanced keyboard options
//-----------------------------------------------------------------------------
void COptionsSubKeyboard::OpenKeyboardAdvancedDialog()
{
	if (!m_OptionsSubKeyboardAdvancedDlg.Get())
	{
		m_OptionsSubKeyboardAdvancedDlg = new COptionsSubKeyboardAdvancedDlg(GetVParent());
	}
	m_OptionsSubKeyboardAdvancedDlg->Activate();
}
