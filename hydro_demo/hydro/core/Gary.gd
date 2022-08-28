extends CharacterBody3D

var camera_angle = 0
var mouse_sensitivity = 0.3
var camera_change = Vector2()

var direction = Vector3()

#fly variables
const FLY_SPEED = 20
const FLY_ACCEL = 4

var mouse_captured = true

signal free_look_toggled

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _physics_process(delta):
	
	if mouse_captured:
		aim()
		fly(delta)
	
	if Input.is_action_just_pressed('toggle_mouse'):
		if mouse_captured:
			mouse_captured = false
			Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			emit_signal("free_look_toggled", false)
		else:
			mouse_captured = true
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
			emit_signal("free_look_toggled", true)

func _input(event):
	if event is InputEventMouseMotion:
		camera_change = event.relative

func fly(delta):
	# reset the direction of the player
	direction = Vector3()
	
	# get the rotation of the camera
	var _aim = $Head/Camera.get_global_transform().basis
	
	# check input and change direction
	if Input.is_action_pressed("move_forward"):
		direction -= _aim.z
	if Input.is_action_pressed("move_backward"):
		direction += _aim.z
	if Input.is_action_pressed("move_left"):
		direction -= _aim.x
	if Input.is_action_pressed("move_right"):
		direction += _aim.x
	
	direction = direction.normalized()
	
	# where would the player go at max speed
	var target = direction * FLY_SPEED
	
	# calculate a portion of the distance to go
	velocity = velocity.lerp(target, FLY_ACCEL * delta)
	
	# move
	move_and_slide()
	
func aim():
	if camera_change.length() > 0:
		$Head.rotate_y(deg2rad(-camera_change.x * mouse_sensitivity))

		var change = -camera_change.y * mouse_sensitivity
		if change + camera_angle < 90 and change + camera_angle > -90:
			$Head/Camera.rotate_x(deg2rad(change))
			camera_angle += change
		camera_change = Vector2()
