# SteamInput

SteamInput is an input system provided by the Steamworks API that, through GDSteamHL, can be used as an *alternative* to Godot's controller input system.

## Considerations
### Benefits over Godot's Input
- **Support for 8 XInput controllers** (typically limited to 4)
- **APIs to retrieve glyphs (images) for controller buttons**
- **Theoretical support for future devices**
- Steam-standardized input remapping
- Required for "Steam Input API" tag on your Steam store page

### Disadvantages
*(As of June 2025)*
- **Outdated documentation**
- **Cumbersome developer UX**
- **Some APIs just don't work reliably** (Editorial: Or Hunt's implementation is bad)
    - `ISteamUtils::ShowGamepadTextInput` & `ISteamUtils::ShowFloatingGamepadTextInput`
    - `ISteamInput::GetAnalogActionHandle` (for `absolute_mouse` actions only)
- A popup when the user launches your game for the first time
- Dependency on Steam running for input

## Setting Up SteamInput
*See the [SteamInput documentation](https://partner.steamgames.com/doc/features/steam_controller) for the most up-to-date information*

### Basic Concepts
Input from Steam consists of developer-defined **actions** that have one or more inputs mapped to them (e.g, the left joystick for the "Move" action).

These inputs are grouped under **action sets**. During gameplay, the developer will set the active action set with `ISteamInput::ActivateActionSet(InputHandle_t)`. This can be done for all or individual controllers. Additionally, **action set layers** can be defined that partially override the action set they exist under.

### Defining Actions
This consists of three steps:
1. Create the **In-Game Actions File** that lists all your game's actions.
2. Create one or more Action Sets (i.e., controller layouts) via the SteamInput API. 
3. Distribute your Action Sets via Steamworks.

All files relating to actions and controller configurations follow Valve's "KeyValue format." This format is just JSON with colons and only string values.


#### 1. Creating the In-Game Action File (Text Editor)
The In-Game Actions (IGA) File defines all the possible actions for your game during development. Unlike Action Sets, this file merely contains the actions and NOT controller mappings. IGA Files consist of two sections: `"actions"` and `"localization"`.

The `"actions"` section a list of action sets. Within each action set has:
- A `"title"`. Instead a direct string, the title is `#` followed by a key in the `"localization"` section (e.g., `"#Set_FPSControls"`), from which it will get its value.
- Subsections for each input type the set uses:
    - `"StickPadGyro"`: Actions with one or more axes of motion (joysticks, trackpads, gyroscopes), excluding analog triggers. Each of these actions has:
        - A `"title"` following the same rules as the action set title described above.
        - An `"input_mode"` of either `"joystick_move"` (small, continous input) or `"absolute_mouse"` (large input deltas between polls).
        - Optionally, `"os_mouse"` with a value of `"1"` to send the action as OS mouse movement.
    - `"AnalogTrigger"`: Actions with one axis of motion (triggers). Each of these actions is the internal name for the axis as the key and its title (following the rules for `"title"` described above) as the value (e.g., `"AccelerationAxis" "#Action_Accelerate"`).
    - `"Button"`: Actions that are either pressed or not. Similar to `"AnalogTrigger"`, each of these actions is the internal name for the axis as the key and its title (following the rules for `"title"` described above) as the value (e.g., `"jump" "#Action_Jump"`). However, unlike `"AnalogTrigger"` actions, you can also define an OS input event to be generated for the action. For more information, see the [IGA File documentation](https://partner.steamgames.com/doc/features/steam_controller/iga_file).

<details>
<summary>Editorial: joystick_move vs. absolute_mouse</summary>

As of the time of writing (June 2025), Steam strongly recommends using `absolute_mouse` for "anything cursor driven" and "when in doubt".

However, when working on *Sumo Tanks*, I ran into severe issues with `absolute_mouse` for cursors where `ISteamInput::GetAnalogActionData` would unexpectedly return no input when called from GDScript. While I never ironed out exactly what it was, I suspect that `ISteamInput::GetAnalogActionData` was zeroing out the input after it was called since `absolute_mouse` is supposed to be a delta. This would be a major problem since GDSteamHL polls for input every frame, which would cause `SteamHLInput.get_analog_action_data` for `absolute_mouse` actions to always return no input. Even if that issue was addressed in GDSteamHL, it would still be a problem for games that call `ISteamInput::GetAnalogActionData` for the same `absolute_mouse` action multiple times in the same frame.

It's is **ENTIRELY** possible that this was a developer-error on my part, but, if you run into the same issue that I did, I recommend just switching to `joystick_move` and saving yourself the headache.

*Hunt (Mountain Peke Games)*
</details>

<br/>

The `"localization"` section consists of a series of language strings ("english", "spanish", etc.), each with key-value pairs for each localization key (**without the # before it**) and its translation for that language.

Valve provides several [examples](https://partner.steamgames.com/doc/features/steam_controller/iga_examples). These are a great starting point if you are new to making a VDF.

<details>
<summary>Sample IGA from Sumo Tanks</summary>

```VDF
"In Game Action"
{
    "actions"
	{
		"Controls"
		{
			"title"					"#Set_Controls"
			"StickPadGyro"
			{
				"Move"
				{
					"title"			"#Action_Move"
					"input_mode"	"joystick_move"
				}
				"Aim"
				{
					"title"			"#Action_Aim"
					"input_mode"	"joystick_move"
				}
			}
			"AnalogTrigger"
			{
			}
			"Button"
			{
				"MenuSelect"		"#Action_Menu_Select"
				"Pause"				"#Action_Pause"
				
				"Shoot"				"#Action_Shoot"
				"Dash"				"#Action_Dash"
				"Multiplayer"		"#Action_Multiplayer"
			}
		}
	}
	"localization"
	{
		"english"
		{
			"Set_Controls"			"Controls"
			
			"Action_Move"			"Move"
			"Action_Aim"			"Aim"
			
			"Action_Menu_Select"	"Select Menu Item"
			"Action_Pause"			"Pause"
			
			"Action_Shoot"			"Shoot"
			"Action_Dash"			"Charge Dash"
			"Action_Multiplayer"	"Multiplayer"
		}
	}
}
```
</details>

#### 2. Creating Action Sets (Steam Client)
1. Place your VDF in a `controller_config` directory within your Steam installation directory, creating the directory if it does not already exist. On Windows, this is typically `C:/Program Files (x86)/Steam/controller_config`.
2. Plug in the controller you want to make an action set for.
3. Navigate to your game within Steam and open the "controller settings" or "Controller Configurator".
    - **Regular Steam:** Select your game from your Steam library and click the "View controller settings" button. Alternatively, open the games properties by right-clicking the title or pressing the cog button, navigate to the "controller" section, and click the "Controller Configurator Link."
    - **Big Picture Mode:** Select your game from your Steam library and click the controller-icon button. Alternatively, open the games properties by pressing the cog button, navigate to the "controller" section, and click the "Controller Configurator Link" with the mouse cursor (controllers cannot select it).
4. Map the buttons to actions within Steam's UI.

**At this point, you can start implementing and testing SteamInput in your game.**

#### 3. Distribution (Steamworks Site)
<details>
<summary>Editorial: Steamworks Distribution vs. Game Files</summary>

Steam gives you two options to distribute your controller layouts/configurations. The Steamworks documentation promotes distributing the game via Steamworks. However, I would actually recommend the second method: bundling the layouts with your game.

The reason for this is simple: the Steam UI to upload and manage layouts in Steamworks is awful. I have never been confident that a layout has been uploaded correctly or that old layout have been removed. It really is that bad in 2025.

In contrast, bundling layouts is WYSIWYG: your layouts are just files included with the game.

Bunding the layouts with your game also has other benefits:
- It is much easier to sync layouts (by copying files) between the main, demo, and playtest versions of your game, which are separate apps within Steam.
- You can easily test new layouts and new actions/sets before publishing by modifying the files in the game install directory.
- Your layouts and actions are linked to the installed build of your game instead of to all versions on the same branch.

For me, these benefits saved me a lot of time despite a more complex (and less documented) initial setup. Until Steam introduces a better UI to for *developers* to manage layouts, I would highly recommend using this.

*Hunt (Mountain Peke Games)*
</details>

</br>

1. Log into the Steamworks site and navigate to the Steamworks Settings for you game. Then, open the "Steam Input" settings under the Application Tab.
2. Enable Steam Input for your desired controllers.
3. Select if your configuration will be distributed through Steamworks or as a file with the game.
    - **Steamworks:**
        1. On the "controller settings"/"Controller Configurator" screen, press the cog button and select "Share Layout with Community." Fill out the title and description and publish it.
        2. Click the cog button again, click "Layout Details", and write down the Configuration ID after the Link URI.
        3. In Steamworks, use the Configuration ID to add the configuration.
        4. *(Optional)* If you want to remove a published configuration, remove it in Steamworks and the return to the "controller settings"/"Controller Configurator" screen, click the layout to see all available layouts, navigate to "Your Layouts", use the keyboard or gamepad to hover over configuration you want to delete, and using the button displayed at the bottom of the screen.
    - **File:**
        1. On the "controller settings"/"Controller Configurator" screen, press the cog button and select "Export Layout." Fill out the title and description, and set the "Export Type" to "New Personal Save." Confirm. Repeat this for each layout.
        2. Find your layout files under `steamapps/common/Steam Controller Configs`. The easiest way to do this is to search for your configuration title in lowercase with the spaces replaced by underscores (e.g., "test_export" for "Test Export"). Otherwise, you'll need to check within the randomly numbered directories and subdirectories manually. Copy the files to somewhere convenient.
        3. Create an "Action Manifest," which is the same as as the IGA VDF, but starting with `"Action Manifest"` instead of `"In Game Action"` and having an additional `"configuration"` section. This section contains a list of controller strings (`"controller_xboxone"`, `"controller_ps5"`, see the [Action Manifest File documentation](https://partner.steamgames.com/doc/features/steam_controller/action_manifest_file) for a full list). Under each controller string is a index for each layout for that specific controller (starting from `"0"`), with `"path"` followed by a path relative to Action Manifest where the configuration(s) from step 2 will be located. The simplest setup is to have your Action Manifest and layouts all in the same directory. Sumo Tank's Action Manifest does this and is provided as a reference below.
        4. In Steamworks, specify where your Action Manifest will be located relative to the game install.
        5. When uploading your game to Steamworks, include your Action Manifest and associated layout files along the relative paths you specified in Steamworks and in the Action Manifest.
        6. Save and publish your changes.
        7. *(Optional)* Once you've specified the Action Manifest in Steamworks and published that change for the first time, you can test out changes by modifying, adding, and/or deleting the layout files and Action Manifest directly in your game's install location. This allows you to test changes before uploading them as part of a built to Steamworks.
4. Save and publish your changes.

<details>
<summary>Sample Action Manifest from Sumo Tanks</summary>

```VDF
"Action Manifest"
{
	"configurations"
	{
		"controller_xboxone"
		{
			"0"
			{
				"path" "controller_xboxone.vdf"
			}
		}
		"controller_ps5"
		{
			"0"
			{
				"path" "controller_ps5.vdf"
			}
		}
		"controller_switch_pro"
		{
			"0"
			{
				"path" "controller_switch_pro.vdf"
			}
		}
	}
	"actions"
	{
		"Controls"
		{
			"title"					"#Set_Controls"
			"StickPadGyro"
			{
				"Move"
				{
					"title"			"#Action_Move"
					"input_mode"	"joystick_move"
				}
				"Aim"
				{
					"title"			"#Action_Aim"
					"input_mode"	"joystick_move"
				}
			}
			"AnalogTrigger"
			{
			}
			"Button"
			{
				"MenuSelect"		"#Action_Menu_Select"
				"Pause"				"#Action_Pause"
				
				"Shoot"				"#Action_Shoot"
				"Dash"				"#Action_Dash"
				"Multiplayer"		"#Action_Multiplayer"
			}
		}
	}
	"localization"
	{
		"english"
		{
			"Set_Controls"			"Controls"
			
			"Action_Move"			"Move"
			"Action_Aim"			"Aim"
			
			"Action_Menu_Select"	"Select Menu Item"
			"Action_Pause"			"Pause"
			
			"Action_Shoot"			"Shoot"
			"Action_Dash"			"Charge Dash"
			"Action_Multiplayer"	"Multiplayer"
		}
	}
}
```
</details>

## GDSteamHL's Integration
GDSteamHL's SteamInput integration is under the SteamHLInput class.

### Interactions with Godot's Controller Input
Using SteamInput will make Godot's controller input handling behave unexpectedly. Because of this, it is recommended that you branch your input handling logic based on `SteamHL.is_initialized()`.

Keyboard and mouse input can continue to be used without issue.

<details>
<summary>Editorial: Input Software Architecture</summary>

I recommend creating your own input manager that lightly wraps Godot's input system and SteamInput, and keep all your branching in there. By doing this, you can code the rest of your game without having to worry about your input implementation details.

*Hunt (Mountain Peke Games)*
</details>

### Connection & Disconnection Signals
In the Steamworks API, you are expected to poll the connected controllers with `ISteamInput::GetConnectedControllers`. GDSteamHL simplifies this by exposing `steam_controller_connected` and `steam_controller_disconnected` signals that can be treated like Godot's `Input.joy_connection_changed` signal. These signals emit the controller ID.


### SteamInput InputEvents
SteamInput has two types of actions: "digital" and "analog". Digital actions have a single binary state (pressed or not) while analog actions have two continuous states (e.g., controller axis strengths). If you register your SteamInput actions, GDSteamHL will create a Godot `InputActionEvent` for SteamInput that you can listen for in `_input(event)`.

#### How to register SteamInput actions
1. Ensure the Steamworks API is initialized with `SteamHL.is_initialized()`.
2. Retrieve the SteamInput action set; register and retrieve the action handles:
    - `SteamHLInput.get_action_set_handle(str)`
    - `SteamHLInput.register_analog_action(str)`
    - `SteamHLInput.register_digital_action(str)`
3. Verify the handles successfully returned a non-zero value.

<details>
<summary>Sample Implementation</summary>

```GDScript
func _process(_delta: float) -> void:
    if SteamHL.is_initialized():
        if not self._loaded_actions:
            self._register_actions()
    else:
        SteamHL.initialize()


func _register_actions() -> void:
    self.controls_set_handle = SteamHLInput.get_action_set_handle("MyControls")
    self.move_action_handle = SteamHLInput.register_analog_action("Move")
    self.jump_action_handle = SteamHLInput.register_digital_action("Jump")

    if self.controls_set_handle != 0:
        self._loaded_actions = true
```
</details>

#### InputEventSteamHLDigitalAction
A "digital" action in Steam is an action with a binary (on-off) state, such as a button.

The action pressed can be determiend by comparing the ID stored in `action_handle` against the ID returned by `SteamHLInput.register_digital_action(str)` during registration, or by the name stored in `action`.

It consists of:
- Joypad Information
    - `action_handle`: The SteamInput action handle of the action pressed.
    - `controller_handle`: The SteamInput ID of the controller.

- Action Information (inherited from `InputActionEvent`)
    - `pressed`: If the button is pressed.
    - `strength`: 1.0 if the button is pressed, otherwise 0.0.
    - `action`: If you registered an action with `SteamHLInput.register_digital_action(str)`, this will match the string you passed.

#### InputEvenSteamHLAnalogAction
An "analog" action in Steam is an action with a continuous state, such as a control stick or trigger. Unlike all other actions, SteamInput analog actions consist of two values (x and y), but the second is not always used (e.g., bumpers only have 1 axis).

The action pressed can be determiend by comparing the ID stored in `action_handle` against the ID returned by `SteamHLInput.register_digital_action(str)` during registration, or by the name stored in `action`.

It consists of:
- Joypad Information
    - `action_handle`: The SteamInput action handle of the action pressed.
    - `controller_handle`: The SteamInput ID of the controller.

- Action Information (inherited from `InputActionEvent`)
    - `x`: The value of the x-axis of the action, typically between -1.0 and 1.0.
    - `y`: The value of the y-axis of the action, typically between -1.0 and 1.0 for joysticks and 0.0 for triggers.
    - `pressed`: If either axis is non-zero.
    - `strength`: The magnitude of the vector formed by `x` and `y` (e.g., the joystick's distance from center).