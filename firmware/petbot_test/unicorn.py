# Regenerate unicorn.h:  python3 unicorn.py xbm > unicorn.h   (preview: python3 unicorn.py)
# Web page copy:        python3 unicorn.py js   -> paste over the UNICORN rows in page.h
# Draws the petbot unicorn at 4x, downsamples to W x H, previews as text, emits XBM bytes.
import sys
from PIL import Image, ImageDraw
W, H, K = 36, 40, 8
def P(*pts): return [(x * K, y * K) for x, y in pts]
def E(x0, y0, x1, y1): return [x0 * K, y0 * K, x1 * K, y1 * K]

def draw(blink=False):
    import math
    im = Image.new("L", (W * K, H * K), 0)
    d = ImageDraw.Draw(im)
    # mane (behind): solid wavy blob down the back of the head and neck
    spine = [(13.5, 11), (11, 15), (9.5, 19), (8.5, 23), (7.5, 27), (6.5, 31), (5.5, 35), (5, 40)]
    for cx, cy in spine:
        d.ellipse(E(cx - 4.5, cy - 4.5, cx + 4.5, cy + 4.5), fill=255)
    # hair lines flowing down the mane
    for off in (-0.8,):
        pts = []
        for i in range(len(spine) - 1):
            (x0, y0), (x1, y1) = spine[i], spine[i + 1]
            for k in range(4):
                t = k / 4
                x = x0 + (x1 - x0) * t + off + 0.9 * math.sin((y0 + (y1 - y0) * t) / 2.2)
                pts.append((x, y0 + (y1 - y0) * t))
        d.line(P(*pts[3:]), fill=0, width=int(0.7 * K))
    # gap between mane and head
    d.polygon(P((12.5, 40), (14, 23), (16.5, 14), (22, 11), (25, 40)), fill=0)
    # neck + head + muzzle
    d.polygon(P((14, 40), (15.5, 24), (22, 21), (25, 40)), fill=255)
    d.ellipse(E(15.5, 11, 30, 26), fill=255)
    d.ellipse(E(22, 16, 35.5, 29.5), fill=255)
    # ear
    d.polygon(P((16, 14.5), (17, 3.5), (22.5, 12)), fill=255)
    d.polygon(P((17.8, 12.2), (18.1, 8), (20.4, 11.8)), fill=0)
    # horn with one stripe
    d.polygon(P((20.5, 13), (27, 10), (34, 0)), fill=255)
    d.line(P((26.5, 5.5), (29.5, 7.5)), fill=0, width=int(0.8 * K))
    # eye (big and cute) or closed
    if blink:
        d.arc(E(19.5, 13, 25.5, 19.5), 20, 160, fill=0, width=int(1.3 * K))
    else:
        d.ellipse(E(19.4, 13.2, 25.2, 19.8), fill=0)
    # nostril + smile
    d.ellipse(E(31, 21.5, 33, 23.5), fill=0)
    d.arc(E(25.5, 21.5, 33, 27.8), 30, 150, fill=0, width=int(1.0 * K))
    out = im.resize((W, H), Image.BOX).point(lambda v: 255 if v >= 128 else 0)
    if not blink:
        out.putpixel((21, 15), 255)  # sparkle in the eye
    return out

def preview(im):
    for y in range(H):
        print("".join("#" if im.getpixel((x, y)) else "." for x in range(W)))

def xbm(im, name):
    bpr = (W + 7) // 8
    out = []
    for y in range(H):
        for b in range(bpr):
            v = 0
            for i in range(8):
                x = b * 8 + i
                if x < W and im.getpixel((x, y)): v |= 1 << i
            out.append(v)
    s = f"static const unsigned char {name}[] U8X8_PROGMEM = {{\n"
    for i in range(0, len(out), 12):
        s += "  " + ", ".join(f"0x{v:02x}" for v in out[i:i+12]) + ",\n"
    return s + "};\n"

if __name__ == "__main__":
    if sys.argv[1:] == ["js"]:
        for name, im in (("UNICORN", draw(False)), ("UNICORN_BLINK", draw(True))):
            rows = ["".join("1" if im.getpixel((x, y)) else "0" for x in range(W)) for y in range(H)]
            print(f"const {name} = [" + ",".join(f"'{int(r, 2):09x}'" for r in rows) + "];")
    elif sys.argv[1:] == ["xbm"]:
        print(f"#define UNICORN_W {W}\n#define UNICORN_H {H}\n")
        print(xbm(draw(False), "UNICORN_BITS"))
        print(xbm(draw(True), "UNICORN_BLINK_BITS"))
    else:
        preview(draw(False)); print(); preview(draw(True))
