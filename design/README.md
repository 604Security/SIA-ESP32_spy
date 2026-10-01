# SIA design choices

Options pages (open in a browser; each is a numbered set of live previews):

| Page | What | Pick |
|---|---|---|
| `hq-options.html` | HQ main screen look (10) | **#2 Night-Vision Cam** |
| `oled-options.html` | What petbot's OLED shows (20) | **#17 Code rain** |
| `login-options.html` | Agent login (10) | **#1 Fingerprint scanner** |
| `outfit-options.html` | Mascot outfits (10, per agent) | **megaspy #6 Ninja mask, spyhunter #7 Tuxedo** |

Generators: `firmware/sia_spy/outfits.py` (outfit pixel art; `python3 outfits.py page` refreshes `outfit-options.html`). The mascot art in all pages comes from
`firmware/sia_spy/page.h` (`spy_art.py page`). The OLED page uses the X11 misc-fixed bitmap fonts U8g2 ships
(4x6, 5x8, 6x10, 7x13B, 10x20), so its previews match the device pixel for pixel.

**Built into `firmware/sia_spy` on 2026-09-30** (all four picks).
