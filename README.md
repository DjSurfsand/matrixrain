# Matrix Rain

![Digital rain falling across the desktop](docs/rain.png)

A small Omarchy app that brings your wallpaper to life: green code rains down
the screen behind your windows, and a single switch turns it on or off.

![The control window](docs/control.png)

## What it does

Matrix Rain puts a transparent, full-screen surface on Hyprland's **bottom**
layer. It sits above the wallpaper and below your apps, so the code falls
behind your work instead of covering it. The surface has no keyboard
interactivity, so it never steals focus or clicks.

Each column carries its own speed, trail length and stream of katakana,
digits and letters. The leading glyph is bright, the tail fades, and a few
characters shimmer each tick. The accent follows the current Omarchy theme
live — change the theme and the rain changes with it.

## Controls

| Key | Action |
| --- | --- |
| `Space` | Toggle the rain on or off |
| `Q` | Quit |
| `?` | Show or hide the shortcut overlay |

The switch in the window does the same thing as `Space`. Speed and density
are adjustable, and all three settings are remembered between launches.

## Development

```sh
bin/build      # qmake6 + make into build/matrixrain
bin/test       # Qt Test suite, offscreen
bin/install    # build and install as an Arch package
```

The C++ side owns state, settings, theme following and the layer-shell
surface; QML owns the layout and the Canvas renderer. Everything is compiled
into one binary by `qmake6`.

## Attribution

Theme and translator patterns follow Omacut and Monologue (MIT, David
Heinemeier Hansson). Licensed under MIT; see `LICENSE`.
