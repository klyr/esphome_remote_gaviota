# ESPHome Gaviota Protocol (external component)

This repository adds a `gaviota` protocol integration for ESPHome `remote_base`.

## What it provides

- `remote_receiver` dumper: `dump: [gaviota]`
- `remote_receiver` trigger: `on_gaviota:`
- `remote_receiver` binary sensor matcher: `gaviota:`
- `remote_transmitter` action: `remote_transmitter.transmit_gaviota`

## Gaviota command list

- `0x11`: Up
- `0x1E`: Release Up
- `0x33`: Down
- `0x3C`: Release Down
- `0x55`: Stop
- `0x80`: Stop + Up
- `0x81`: Stop + Down
- `0xCC`: P2
- `0xC1`: M

## Minimal setup

```yaml
external_components:
  - source: github://YOUR_GITHUB_USER/esphome_remote_gaviota
    components: [gaviota_protocol]

gaviota_protocol:
```

See `example.yaml` for a complete configuration.
