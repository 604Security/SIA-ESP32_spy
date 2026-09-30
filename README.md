# SIA — Secret Intelligence Agency

A spy game for kids, running on the [Seeed XIAO ESP32S3 Sense](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
([XIAOML Kit](https://www.seeedstudio.com/The-XIAOML-Kit.html)). Forked from **AI petbot**, it turns the board's camera, microphone,
light, screen and memory card into spy gadgets.

Agents log in on a web page served by the board, go on **missions** with **spy gadgets**, earn **points** and rank up:
🥚 Recruit → 🕶️ Agent → ⭐ Special Agent → 👑 Master Spy → 🏆 Legendary Spy.

| Mission | Gadget | Points |
|---|---|---|
| 🔦 Signal Rookie: decode a Morse word flashed on the LED | Signal lamp | 50 (25 with a hint) |
| 👻 Ghost Walk: stay silent for 20 seconds | Listening bug | 30 |
| 🔐 Code Breaker: crack a Caesar-cipher message shown on the OLED | Code machine | 40 |
| ✉️ Secret Courier: send a coded message to the other agent's screen | Code machine | 10 (every 2 min) |
| 🚨 Trap Master: catch an intruder with the sound trap | Sound trap | 40 |
| 📸 Evidence Hunt: photograph a secret target | Spy cam | 20 |

**Sound trap:** when armed, a noise louder than the chosen sensitivity wakes the spy cam, which saves a burst of 3 evidence photos
to the microSD card (with a timestamp), flashes an alert on the OLED and sounds a siren on the web page.

## Run it

```sh
cp firmware/sia_spy/secrets.h.example firmware/sia_spy/secrets.h   # add your 2.4 GHz Wi-Fi
make spy                                                          # build + flash
```

Then open the link printed on serial, or **http://sia.local**. Bookmark `http://sia.local/?agent=<name>` to log straight in.

Points are stored in the board's flash. Evidence photos and the mission log live in `/sia/` on the microSD card.

## Also in this repo (from AI petbot)
- `firmware/phase0/`: bring-up tests (blink, mic, SD, Wi-Fi, camera server)
- `firmware/petbot_test/`: the AI petbot test menu (`make petbot-test`)

See [CLAUDE.md](CLAUDE.md) for hardware details, toolchain setup, and gotchas.
