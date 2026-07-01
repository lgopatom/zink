# Zink

A minimal Z-machine (interactive fiction) reader for the Mudita Kompakt,
styled with Mudita's own [MMD](https://github.com/mudita/MMD) e-ink design
library.

## Status: Milestone 1 (MVP scaffold)

This first version doesn't run real games yet — it's a stub Library screen
with placeholder titles. Its job is to prove the full pipeline works:

1. Push code to `main`
2. GitHub Actions builds a debug APK (no local Android Studio needed)
3. Download the APK from the Actions run's artifacts
4. `adb install -r app-debug.apk` onto the Kompakt
5. Confirm it launches and renders correctly on the e-ink screen

## Roadmap

- **Milestone 2** — wire in a real Z-machine engine (bocfel, MIT licensed,
  via a trimmed-down fork of the JNI bridge already used by
  [Fabularium](https://github.com/davidar/fabularium)/[Garglk](https://github.com/davidar/garglk))
- **Milestone 3** — storage scanning: one-time folder picker (Storage Access
  Framework), recursive scan for `.z3` / `.z4` / `.z5` files
- **Milestone 4** — save/restore support for in-progress games

## Building locally

You don't need to — GitHub Actions does it. See `.github/workflows/build.yml`.

## License

MIT — see `LICENSE`. (bocfel, the engine Milestone 2 will pull in, is also
MIT; MMD is Apache 2.0. No copyleft entanglement either way.)


