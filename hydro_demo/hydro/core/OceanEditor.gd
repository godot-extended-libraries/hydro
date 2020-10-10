extends Panel

onready var ocean = $"../Ocean"

var wind_direction = Vector2()

func _process(delta):
	for i in $Settings.get_children():
		match i.name:
			"Seed": ocean.set_seed(hash($Settings/Seed/LineEdit.text))
			"Amplitude": ocean.set_amplitude($Settings/Amplitude/HSlider.value)
			"Wavelength": ocean.set_wavelength($Settings/Wavelength/HSlider.value)
			"Steepness": ocean.set_steepness($Settings/Steepness/HSlider.value)
			"WindDirectionX": wind_direction.x = $Settings/WindDirectionX/HSlider.value
			"WindDirectionY": wind_direction.y = $Settings/WindDirectionY/HSlider.value
			"WindAlign": ocean.set_wind_align($Settings/WindAlign/HSlider.value)
			"Speed": ocean.set_speed($Settings/Speed/HSlider.value)
			"Noise": ocean.set_noise_enabled($Settings/Noise/CheckBox.pressed)
			"NoiseAmp": ocean.set_noise_amplitude($Settings/NoiseAmp/HSlider.value)
			"NoiseFreq": ocean.set_noise_frequency($Settings/NoiseFreq/HSlider.value)
			"NoiseSpeed": ocean.set_noise_speed($Settings/NoiseSpeed/HSlider.value)
