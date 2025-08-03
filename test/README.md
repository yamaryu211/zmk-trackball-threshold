# ZMK Trackball Threshold Module - Test Configuration

This directory contains test configurations for the zmk-trackball-threshold module.

## Test Files

### `config/test.dtsi`

Device Tree configuration with multiple threshold processor instances:

- `zip_trackball_threshold`: Default threshold (uses Kconfig value)
- `zip_trackball_threshold_custom`: Custom threshold (10)
- `zip_trackball_threshold_low`: Low threshold (2) for testing

### `config/test.conf`

Kconfig settings for testing:

- Enables the trackball threshold processor
- Sets default threshold to 5
- Enables debug logging
- Enables required dependencies

### `config/test.keymap`

Simple test keymap with:

- Basic key layout
- Auto mouse layer (AML)
- Test input processor chain: threshold → temp layer

## Testing Scenarios

### 1. Threshold Filtering Test

Test that movements below threshold are filtered out:

```
Input: x=2, y=1 (magnitude=3, below threshold=5)
Expected: No output to downstream processors

Input: x=3, y=3 (magnitude=6, above threshold=5)
Expected: Pass through to downstream processors
```

### 2. Accumulation Test

Test that small movements accumulate until threshold is reached:

```
Input sequence: x=1,y=1 → x=1,y=1 → x=2,y=1
Accumulated: x=4,y=3 (magnitude=7, above threshold=5)
Expected: Output accumulated movement (x=4,y=3)
```

### 3. Different Threshold Values

Test with different threshold configurations:

- Default (5): Normal filtering
- Custom (10): Stricter filtering
- Low (2): More sensitive filtering

## Usage in Real Configuration

To integrate with actual zmk-config:

1. Add module to `west.yml`
2. Include threshold processor in trackball listener:

```c
&trackball_listener {
    input-processors = <&zip_trackball_threshold>, <&zip_temp_layer AML 30000>;
};
```

3. Configure threshold value as needed:

```c
&zip_trackball_threshold {
    movement-threshold = <5>; // Adjust as needed
};
```

## Expected Behavior

- Small trackball vibrations (< threshold) should not trigger AML
- Intentional trackball movements (> threshold) should trigger AML normally
- No impact on non-movement events (clicks, scrolls)
- Transparent operation when movement exceeds threshold
