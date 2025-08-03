# ZMK Trackball Threshold Module

A ZMK input processor module that filters out small trackball movements below a configurable threshold, preventing accidental auto mouse layer activation from minor vibrations.

## Features

- Configurable movement threshold filtering
- Transparent pass-through for movements exceeding threshold
- Compatible with existing ZMK input processor chains
- Lightweight implementation with minimal performance impact

## Installation

Add this module to your `west.yml`:

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    # Add your remote here
    - name: your-remote
      url-base: https://github.com/your-username
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: main
      import: app/west.yml
    - name: zmk-trackball-threshold
      remote: your-remote
      revision: main
  self:
    path: config
```

## Usage

### Basic Configuration

```c
/ {
    zip_trackball_threshold: zip_trackball_threshold {
        compatible = "zmk,input-processor-trackball-threshold";
        movement-threshold = <5>; // Optional, defaults to Kconfig value
    };
};

&trackball_listener {
    input-processors = <&zip_trackball_threshold>, <&zip_temp_layer AML 30000>;
};
```

### Configuration Options

- `movement-threshold`: Minimum movement threshold (default: 5)
  - Movement is calculated as `abs(x) + abs(y)`
  - Values below this threshold are discarded

### Kconfig Options

```
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD=y
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD_DEFAULT_THRESHOLD=5
```

## How It Works

The module accumulates trackball movement events and only passes them through to downstream processors when the total movement magnitude exceeds the configured threshold. This prevents minor vibrations or accidental touches from triggering auto mouse layer activation.

## License

MIT License
