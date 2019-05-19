# Troubleshooting


## If you see this

```shell
 OpenCV: Cannot use FaceTime HD Camera
OpenCV: camera failed to properly initialize!
Camera not operational
```

## Do this

```shell
sudo killall VDCAssistant
```

## Assertion failed: (side*side*((1 + l.coords)*l.n + l.classes) == inputs), function make_detection_layer, file ./src/detection_layer.c, line 28

* the formula from the assertion is populated with values from the `.cfg` file passed to the training step. 