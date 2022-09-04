extends Panel

@onready var ocean = $"../Ocean"

var wind_direction = Vector2()


func _process(_delta):
	for i in $HFlowContainer/Settings.get_children():
		match i.name:
			"Seed":
				ocean.set_seed(hash($HFlowContainer/Settings/Seed/LineEdit.text))
			"Amplitude":
				ocean.set_amplitude($HFlowContainer/Settings/Amplitude/HSlider.value)
			"Wavelength":
				ocean.set_wavelength($HFlowContainer/Settings/Wavelength/HSlider.value)
			"Steepness":
				ocean.set_steepness($HFlowContainer/Settings/Steepness/HSlider.value)
			"WindDirectionX":
				wind_direction.x = $HFlowContainer/Settings/WindDirectionX/HSlider.value
			"WindDirectionY":
				wind_direction.y = $HFlowContainer/Settings/WindDirectionY/HSlider.value
			"WindAlign":
				ocean.set_wind_align($HFlowContainer/Settings/WindAlign/HSlider.value)
			"Speed":
				ocean.set_speed($HFlowContainer/Settings/Speed/HSlider.value)
			"Noise":
				ocean.set_noise_enabled($HFlowContainer/Settings/Noise/CheckBox.pressed)
			"NoiseAmp":
				ocean.set_noise_amplitude($HFlowContainer/Settings/NoiseAmp/HSlider.value)
			"NoiseFreq":
				ocean.set_noise_frequency($HFlowContainer/Settings/NoiseFreq/HSlider.value)
			"NoiseSpeed":
				ocean.set_noise_speed($HFlowContainer/Settings/NoiseSpeed/HSlider.value)


func set_free_look(status):
	if status:
		$HFlowContainer/EditMode.hide()
		$HFlowContainer/FreeLook.show()
	else:
		$HFlowContainer/FreeLook.hide()
		$HFlowContainer/EditMode.show()
