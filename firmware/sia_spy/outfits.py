# Mascot outfits for the SIA OLED (36x40 each). spy_art.py uses these for the real mascots.
#   python3 outfits.py          preview every outfit as text
#   python3 outfits.py page     refresh the bitmaps in design/outfit-options.html (the options page)
import math
import os
import sys
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import unicorn  # noqa: E402

W, H, K = 36, 40, 8
P, E = unicorn.P, unicorn.E
LW = lambda w: max(1, int(w * K))


def finish(big):
    return big.resize((W, H), Image.BOX).point(lambda v: 255 if v >= 128 else 0)


# ---------------------------------------------------------------- unicorn (megaspy)
def unicorn_base():
    big = Image.new("L", (W * K, H * K), 0)
    big.paste(unicorn.draw(False).resize((W * K, H * K), Image.NEAREST))
    return big, ImageDraw.Draw(big)


def white_shape(d, pts, edge=0.9):
    """A white accessory with a black outline, so it stands out against the white unicorn."""
    d.polygon(P(*pts), fill=255)
    d.line(P(*pts, pts[0]), fill=0, width=LW(edge), joint="curve")


def u_shades(d):
    d.rounded_rectangle(E(18.6, 13.8, 25.6, 19), radius=2 * K, fill=0)
    d.line(P((25.4, 15.6), (28.6, 16.4)), fill=0, width=LW(1))
    d.line(P((18.8, 15.2), (16.2, 14.6)), fill=0, width=LW(1))


def u_hat_fedora(d):
    # tilted on the back of the head, behind the horn
    white_shape(d, [(9.5, 11), (10.8, 3), (18, 0.8), (20.8, 8.4)])              # crown
    d.line(P((9.9, 9.4), (20.4, 6.4)), fill=0, width=LW(1.4))                    # band
    white_shape(d, [(5.5, 12.2), (22.5, 7), (23.5, 9.2), (6.5, 14.4)])           # brim


def unicorn_outfit(name, wave=0):
    big, d = unicorn_base()
    if name == "shades":
        u_shades(d)
    elif name == "fedora":
        u_hat_fedora(d)
        u_shades(d)
    elif name == "goggles":
        d.rectangle(E(18.5, 13.2, 28.5, 19.6), fill=0)                               # tube
        d.ellipse(E(26, 12.6, 31.4, 20.2), fill=0)                                   # front lens
        d.ellipse(E(27.4, 14.2, 30, 18.6), outline=255, width=LW(0.8))
        d.line(P((18.6, 15.4), (13.5, 14)), fill=0, width=LW(1.3))                   # strap
    elif name == "earpiece":
        u_shades(d)
        d.ellipse(E(15.3, 15.6, 17.8, 18.1), fill=0)                                 # earpiece
        pts = [(16.5, 18)]
        for i in range(1, 30):                                                       # coiled wire down the neck
            y = 18 + i * 0.5
            pts.append((17.2 + 0.9 * math.sin(i * 1.3), y))
        d.line(P(*pts), fill=0, width=LW(0.8))
    elif name == "disguise":
        d.ellipse(E(19, 13.2, 25.8, 20), outline=0, width=LW(1.2))                  # round glasses
        d.line(P((25.8, 16), (29, 16.6)), fill=0, width=LW(0.9))
        d.polygon(P((25.5, 24.8), (28.5, 22.6), (31, 23.6), (33.5, 22.6), (35.8, 25), (33, 26.6), (30.5, 25.4), (28, 26.6)), fill=0)  # mustache
    elif name == "ninja":
        d.rectangle(E(15.8, 13, 32, 19.6), fill=0)                                   # mask band
        d.ellipse(E(20, 14.6, 25.6, 18.2), fill=255)                                 # eye slit
        d.ellipse(E(22.6, 15, 25, 17.8), fill=0)
        w = (0, 0.8, 1.6, 0.8)[wave % 4]                                            # the tails flutter
        d.polygon(P((16, 14), (9, 12.5 - w), (7, 15 - w), (16, 17)), fill=0)         # knot tails over the mane
        d.polygon(P((15.5, 15.5), (8, 18.5 + w), (8.5, 20.5 + w), (16, 18)), fill=0)
    elif name == "tuxedo":
        u_shades(d)
        d.polygon(P((15, 32), (20, 34), (15, 36.5)), fill=0)                         # bow tie
        d.polygon(P((25, 32), (20, 34), (25, 36.5)), fill=0)
        d.ellipse(E(19, 33, 21, 35), fill=0)
        d.line(P((14.5, 30.5), (20, 33), (25.5, 30.5)), fill=0, width=LW(0.7))      # collar
    elif name == "detective":
        white_shape(d, [(4, 12.8), (8, 9.6), (9.6, 12.6)])                            # back peak
        white_shape(d, [(7, 11), (12.6, 2.6), (19.4, 2), (22, 8.6)])                  # deerstalker crown
        white_shape(d, [(20.6, 8), (25.4, 7), (22, 10)])                              # front peak
        d.line(P((13, 3.4), (11, 10.6)), fill=0, width=LW(0.7))                      # seams
        d.line(P((17, 2.6), (17.5, 9.4)), fill=0, width=LW(0.7))
        d.ellipse(E(19.6, 13, 26, 19.6), outline=0, width=LW(1.1))                  # monocle
        d.line(P((22, 19.6), (20.5, 24), (22, 28)), fill=0, width=LW(0.6))          # chain
    elif name == "headset":
        u_shades(d)
        d.arc(E(12.5, 6, 22, 20), 190, 350, fill=0, width=LW(1.1))                  # headband
        d.ellipse(E(14.6, 14.6, 18.4, 19), fill=0)                                  # ear cup
        d.line(P((17.5, 18.5), (22, 24.5), (27.5, 26.3)), fill=0, width=LW(0.9))    # boom
        d.ellipse(E(26.6, 25.2, 29.4, 28), fill=0)                                  # mic
    elif name == "beret":
        u_shades(d)
        beret = [(14.3 + 8 * math.cos(a) , 8.4 + 3.8 * math.sin(a) - 1.2 * math.cos(a)) for a in [k * math.pi / 10 for k in range(20)]]
        white_shape(d, beret, edge=1.0)                                              # beret, tilted
        d.line(P((14.6, 4.6), (15.4, 2.2)), fill=255, width=LW(1.2))                 # stem
        d.line(P((13.8, 2.4), (16.4, 2)), fill=0, width=LW(0.5))
        for i in range(4):                                                          # striped scarf
            d.polygon(P((14.2 + i * 2.6, 29.5 + i * 0.3), (16.4 + i * 2.6, 29.8 + i * 0.3), (16.8 + i * 2.6, 33 + i * 0.3), (14.6 + i * 2.6, 32.7 + i * 0.3)), fill=0)
    return finish(big)


# ---------------------------------------------------------------- spy agent (spyhunter)
def agent_outfit(name):
    im = Image.new("L", (W * K, H * K), 0)
    d = ImageDraw.Draw(im)
    tux = name == "tuxedo"
    # coat / suit with collar
    d.polygon(P((3, 40), (7, 29), (13, 26), (23, 26), (29, 29), (33, 40)), fill=255)
    if tux:
        d.polygon(P((12, 26.5), (18, 34), (24, 26.5)), fill=0)                       # shirt V
        d.polygon(P((14.5, 26.8), (18, 31), (21.5, 26.8)), fill=255)
        d.polygon(P((14.5, 28), (18, 29.5), (14.5, 31)), fill=0)                     # bow tie
        d.polygon(P((21.5, 28), (18, 29.5), (21.5, 31)), fill=0)
    else:
        d.polygon(P((11, 26.5), (18, 36), (25, 26.5), (22.5, 26), (18, 31), (13.5, 26)), fill=0)
        d.polygon(P((8.5, 29), (12, 21.5), (14, 27)), fill=255)
        d.polygon(P((27.5, 29), (24, 21.5), (22, 27)), fill=255)
    if name == "beret":                                                              # scarf
        for i in range(5):
            d.rectangle(E(11 + i * 3, 24.5, 12.6 + i * 3, 27.5), fill=0 if i % 2 else 255)
        d.rectangle(E(10.5, 24.5, 25.5, 27.5), outline=0, width=LW(0.5))
    # face
    d.ellipse(E(11, 10, 25, 27), fill=255)
    # hats
    if name == "shades":                                                             # no hat: slicked-back hair
        d.ellipse(E(10.4, 6.6, 25.6, 22), fill=255)
        d.arc(E(11.4, 9.6, 24.6, 24), 200, 340, fill=0, width=LW(0.9))             # hairline
        d.line(P((15, 7.4), (14, 11.6)), fill=0, width=LW(0.8))                     # side parting
    elif name in ("fedora", "earpiece", "goggles", "headset", "disguise"):
        d.rounded_rectangle(E(4, 10, 32, 13), radius=1.5 * K, fill=255)
        d.polygon(P((9, 11), (10.5, 3), (18, 1.5), (25.5, 3), (27, 11)), fill=255)
        d.line(P((9.4, 8.6), (26.6, 8.6)), fill=0, width=LW(1.4))
        d.line(P((16, 2.2), (18, 4.5), (20, 2.2)), fill=0, width=LW(0.9))
    elif name == "tuxedo":                                                           # top hat
        d.rounded_rectangle(E(6, 10, 30, 12.6), radius=1.2 * K, fill=255)
        d.rectangle(E(10.5, 0, 25.5, 11), fill=255)
        d.rectangle(E(10.5, 7.4, 25.5, 9.2), fill=0)
    elif name == "detective":                                                        # deerstalker
        d.pieslice(E(9, 2, 27, 20), 180, 360, fill=255)
        d.polygon(P((24, 11), (31, 10.5), (26, 13)), fill=255)
        d.polygon(P((12, 11), (5, 10.5), (10, 13)), fill=255)
        d.line(P((18, 2), (18, 10.6)), fill=0, width=LW(0.7))
        d.line(P((9.6, 10.8), (26.4, 10.8)), fill=0, width=LW(0.8))
    elif name == "beret":
        d.ellipse(E(8, 4.5, 28, 12.5), fill=255)
        d.ellipse(E(8, 4.5, 28, 12.5), outline=0, width=LW(0.7))
        d.line(P((18, 4.8), (18.8, 2.4)), fill=255, width=LW(1.2))
    elif name == "ninja":                                                            # hood
        d.ellipse(E(9.5, 5, 26.5, 26), fill=255)
        d.rectangle(E(10.8, 14.6, 25.2, 19.4), fill=0)                                # eye opening
        d.rectangle(E(12, 15.4, 24, 18.6), fill=255)
    # eyes / glasses
    if name in ("shades", "fedora", "tuxedo", "beret", "earpiece", "headset"):
        d.rounded_rectangle(E(12.6, 15.2, 17.4, 18.6), radius=1.4 * K, fill=0)
        d.rounded_rectangle(E(18.6, 15.2, 23.4, 18.6), radius=1.4 * K, fill=0)
    elif name == "goggles":
        d.rectangle(E(11, 14.6, 25, 19.4), fill=0)
        for cx in (14.5, 21.5):
            d.ellipse(E(cx - 2.2, 15, cx + 2.2, 19), outline=255, width=LW(0.7))
    elif name == "disguise":
        for cx in (15, 21):
            d.ellipse(E(cx - 2.6, 14.4, cx + 2.6, 19.4), outline=0, width=LW(0.9))
        d.polygon(P((13, 22.4), (16, 20.8), (18, 21.6), (20, 20.8), (23, 22.4), (20.5, 23.4), (18, 22.6), (15.5, 23.4)), fill=0)
        d.line(P((18, 18.5), (18.8, 20.4)), fill=0, width=LW(0.8))                 # big nose
    elif name == "detective":
        d.ellipse(E(13.2, 15.6, 16.2, 18), fill=0)
        d.ellipse(E(19.2, 14.6, 23.8, 19.2), outline=0, width=LW(0.9))              # monocle
        d.ellipse(E(20.3, 15.8, 22.7, 18), fill=0)
        d.line(P((23.6, 18), (26, 24)), fill=0, width=LW(0.5))
    elif name == "ninja":
        d.rectangle(E(13.5, 16, 16.5, 17.6), fill=0)
        d.rectangle(E(19.5, 16, 22.5, 17.6), fill=0)
    # mouth (not for the ninja or the mustache)
    if name not in ("ninja", "disguise"):
        d.arc(E(15, 19.5, 21.5, 24), 20, 160, fill=0, width=LW(0.9))
    # extras
    if name == "earpiece":
        d.ellipse(E(9.8, 16, 12.2, 18.6), fill=0)
        d.line(P((11, 18.6), (10.6, 22), (11.8, 25), (11, 28)), fill=0, width=LW(0.8))
    if name == "headset":
        d.arc(E(9, 7, 27, 22), 170, 370, fill=0, width=LW(1))
        d.ellipse(E(9.4, 14.8, 12.4, 19), fill=0)
        d.line(P((11.4, 19), (13, 22.8), (16, 23.4)), fill=0, width=LW(0.9))
    return finish(im)


OUTFITS = [
    ("shades", "Classic shades", "Just the dark spy sunglasses (megaspy's look now; spyhunter's current look is #2)."),
    ("fedora", "Fedora & shades", "A classic spy hat, tilted just so."),
    ("goggles", "Night-vision goggles", "Tube goggles for seeing in the dark (matches the Night-Vision HQ)."),
    ("earpiece", "Secret earpiece", "Shades plus a coiled earpiece wire: listening to HQ."),
    ("disguise", "Disguise kit", "Round glasses and a fake mustache. Nobody will ever know!"),
    ("ninja", "Ninja mask", "A black mask band, and the unicorn's mask tails flutter over her mane."),
    ("tuxedo", "Tuxedo", "Bow tie and shades for fancy undercover parties (top hat for spyhunter)."),
    ("detective", "Detective", "Deerstalker hat and a monocle, like a famous detective."),
    ("headset", "Mission control headset", "Headphones and a boom mic for talking to HQ."),
    ("beret", "Undercover in Paris", "A beret, and a striped scarf for the spy."),
]


def preview(im):
    return ["".join("#" if im.getpixel((x, y)) else "." for x in range(W)) for y in range(H)]


def hexrows(im):
    return ",".join(f"'{int(''.join('1' if im.getpixel((x, y)) else '0' for x in range(W)), 2):09x}'" for y in range(H))


if __name__ == "__main__":
    if sys.argv[1:] == ["page"]:
        here = os.path.dirname(os.path.abspath(__file__))
        js = "const OUTFITS = [\n" + ",\n".join(
            f"  {{id:'{i}', name:'{n}', desc:{desc!r}, megaspy:[{hexrows(unicorn_outfit(i))}], spyhunter:[{hexrows(agent_outfit(i))}]}}"
            for i, n, desc in OUTFITS) + "];"
        path = os.path.join(here, "..", "..", "design", "outfit-options.html")
        page = open(path).read()
        a = page.index("// ART-BEGIN") + len("// ART-BEGIN")
        b = page.index("// ART-END")
        open(path, "w").write(page[:a] + "\n" + js + "\n" + page[b:])
        print("outfit-options.html updated")
    else:
        for i, n, _ in OUTFITS:
            u, a = preview(unicorn_outfit(i)), preview(agent_outfit(i))
            print(f"== {n}")
            for y in range(H):
                print(u[y] + "   " + a[y])
